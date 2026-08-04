#include "memsinterface.h"
#include "ui_memsinterface.h"

#include <QDateTime>
#include <QSerialPortInfo>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

MEMSInterface::MEMSInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MEMSInterface)
    , m_memsLogic(nullptr)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_rpmSeries(nullptr)
    , m_tempSeries(nullptr)
    , m_reconnectTimer(new QTimer(this))
    , m_autoLogFile(nullptr)
    , m_autoLogStream(nullptr)
    , m_trayIcon(nullptr)
{
    ui->setupUi(this);

    // Titre dynamique de la fenêtre
    setWindowTitle(QString("MEMS Diagnostic Interface - Version %1").arg(MEMS_INTERFACE_VERSION));

    // Configuration de l'icône dans la zone de notification (System Tray)
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/app_icon.png"), this);
    QMenu *trayMenu = new QMenu(this);
    QAction *restoreAction = trayMenu->addAction(tr("Restaurer"));
    QAction *quitAction = trayMenu->addAction(tr("Quitter"));
    
    connect(restoreAction, &QAction::triggered, this, &MEMSInterface::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    // Option de menu "Toujours au premier plan"
    QAction *alwaysOnTopAction = new QAction(tr("Toujours au premier plan"), this);
    alwaysOnTopAction->setCheckable(true);
    connect(alwaysOnTopAction, &QAction::toggled, this, [this](bool checked) {
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        show(); // Réaffiche la fenêtre après la mise à jour des flags
    });
    ui->menuOption->addAction(alwaysOnTopAction);

    // Configuration du Graphique (QtCharts)
    m_rpmSeries = new QLineSeries();
    m_rpmSeries->setName("RPM");
    m_tempSeries = new QLineSeries();
    m_tempSeries->setName("Coolant Temp (°C)");

    m_chart = new QChart();
    m_chart->addSeries(m_rpmSeries);
    m_chart->addSeries(m_tempSeries);
    m_chart->createDefaultAxes();
    m_chart->setTitle("ECU Live Data");

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // Intégration du graphique dans le widget dédié
    QVBoxLayout *layout = new QVBoxLayout(ui->graphWidget);
    layout->addWidget(m_chartView);
    ui->graphWidget->setLayout(layout);

    // Création et isolation du Worker Logic dans un QThread séparé
    m_memsLogic = new MEMSLogic();
    m_memsLogic->moveToThread(&m_logicThread);

    // Connexions pour le nettoyage du thread
    connect(&m_logicThread, &QThread::finished, m_memsLogic, &QObject::deleteLater);
    
    // Connexions Interface -> Logic (Ordres)
    connect(this, &MEMSInterface::connectToPort, m_memsLogic, &MEMSLogic::connectToPort);
    connect(this, &MEMSInterface::disconnectFromPort, m_memsLogic, &MEMSLogic::disconnectFromPort);
    connect(this, &MEMSInterface::startDataStream, m_memsLogic, &MEMSLogic::startDataStream);
    connect(this, &MEMSInterface::stopDataStream, m_memsLogic, &MEMSLogic::stopDataStream);
    connect(this, &MEMSInterface::clearFaults, m_memsLogic, &MEMSLogic::clearFaults);

    // Actionneurs (ON)
    connect(this, &MEMSInterface::testFuelPump, m_memsLogic, &MEMSLogic::testFuelPump);
    connect(this, &MEMSInterface::testPTCRelay, m_memsLogic, &MEMSLogic::testPTCRelay);
    connect(this, &MEMSInterface::testACRelay, m_memsLogic, &MEMSLogic::testACRelay);
    connect(this, &MEMSInterface::testPurgeValve, m_memsLogic, &MEMSLogic::testPurgeValve);
    connect(this, &MEMSInterface::testO2Heater, m_memsLogic, &MEMSLogic::testO2Heater);
    connect(this, &MEMSInterface::testBoostValve, m_memsLogic, &MEMSLogic::testBoostValve);
    connect(this, &MEMSInterface::testFan1, m_memsLogic, &MEMSLogic::testFan1);
    connect(this, &MEMSInterface::testFan2, m_memsLogic, &MEMSLogic::testFan2);
    connect(this, &MEMSInterface::testFan3, m_memsLogic, &MEMSLogic::testFan3);

    // Actionneurs (OFF)
    connect(this, &MEMSInterface::turnOffFuelPump, m_memsLogic, &MEMSLogic::turnOffFuelPump);
    connect(this, &MEMSInterface::turnOffPTCRelay, m_memsLogic, &MEMSLogic::turnOffPTCRelay);
    connect(this, &MEMSInterface::turnOffACRelay, m_memsLogic, &MEMSLogic::turnOffACRelay);
    connect(this, &MEMSInterface::turnOffPurgeValve, m_memsLogic, &MEMSLogic::turnOffPurgeValve);
    connect(this, &MEMSInterface::turnOffO2Heater, m_memsLogic, &MEMSLogic::turnOffO2Heater);
    connect(this, &MEMSInterface::turnOffBoostValve, m_memsLogic, &MEMSLogic::turnOffBoostValve);
    connect(this, &MEMSInterface::turnOffFan1, m_memsLogic, &MEMSLogic::turnOffFan1);
    connect(this, &MEMSInterface::turnOffFan2, m_memsLogic, &MEMSLogic::turnOffFan2);
    connect(this, &MEMSInterface::turnOffFan3, m_memsLogic, &MEMSLogic::turnOffFan3);

    // Connexions Logic -> Interface (Retours de données)
    connect(m_memsLogic, &MEMSLogic::connectionStatusChanged, this, &MEMSInterface::onConnectionStatusChanged);
    connect(m_memsLogic, &MEMSLogic::dataReceived, this, &MEMSInterface::onDataReceived);
    connect(m_memsLogic, &MEMSLogic::faultsCleared, this, &MEMSInterface::onFaultsCleared);

    // Retours de fin de test d'actionneur
    connect(m_memsLogic, &MEMSLogic::fuelPumpTestComplete, this, &MEMSInterface::onFuelPumpTestComplete);
    connect(m_memsLogic, &MEMSLogic::ptcRelayTestComplete, this, &MEMSInterface::onPTCRelayTestComplete);
    connect(m_memsLogic, &MEMSLogic::acRelayTestComplete, this, &MEMSInterface::onACRelayTestComplete);
    connect(m_memsLogic, &MEMSLogic::purgeValveTestComplete, this, &MEMSInterface::onPurgeValveTestComplete);
    connect(m_memsLogic, &MEMSLogic::o2HeaterTestComplete, this, &MEMSInterface::onO2HeaterTestComplete);
    connect(m_memsLogic, &MEMSLogic::boostValveTestComplete, this, &MEMSInterface::onBoostValveTestComplete);
    connect(m_memsLogic, &MEMSLogic::fan1TestComplete, this, &MEMSInterface::onFan1TestComplete);
    connect(m_memsLogic, &MEMSLogic::fan2TestComplete, this, &MEMSInterface::onFan2TestComplete);
    connect(m_memsLogic, &MEMSLogic::fan3TestComplete, this, &MEMSInterface::onFan3TestComplete);

    // Configuration de la reconnexion automatique (3 secondes)
    m_reconnectTimer->setInterval(3000);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        if (!ui->m_ConnectButton->isEnabled() && !m_lastPortName.isEmpty()) {
            emit connectToPort(m_lastPortName);
        }
    });

    // Démarrage du thread de calcul/communication
    m_logicThread.start();

    // Remplissage initial de la liste des ports COM disponibles
    refreshComPorts();

    // État par défaut de l'interface graphique
    updateUIState(false);
}

MEMSInterface::~MEMSInterface()
{
    stopAutoLogging();

    m_logicThread.quit();
    m_logicThread.wait();
    delete ui;
}

void MEMSInterface::refreshComPorts()
{
    ui->m_ComPortComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        ui->m_ComPortComboBox->addItem(port.portName());
    }
}

void MEMSInterface::updateUIState(bool connected)
{
    ui->m_ConnectButton->setEnabled(!connected);
    ui->m_DisconnectButton->setEnabled(connected);
    ui->m_ComPortComboBox->setEnabled(!connected);

    ui->m_StartStreamButton->setEnabled(connected);
    ui->m_StopStreamButton->setEnabled(false);
    ui->m_ClearFaultsButton->setEnabled(connected);

    setActuatorTestsEnabled(connected);
    setActuatorsOffEnabled(false);

    if (!connected) {
        ui->m_StatusLabel->setText("Déconnecté");
        ui->m_StatusLabel->setStyleSheet("QLabel { color : red; }");
    } else {
        ui->m_StatusLabel->setText("Connecté");
        ui->m_StatusLabel->setStyleSheet("QLabel { color : green; }");
    }
}

void MEMSInterface::setActuatorTestsEnabled(bool enabled)
{
    ui->m_FuelPump_TestButton->setEnabled(enabled);
    ui->m_PTC_Relay_TestButton->setEnabled(enabled);
    ui->m_AC_Relay_TestButton->setEnabled(enabled);
    ui->m_Purge_Valve_TestButton->setEnabled(enabled);
    ui->m_O2_Heater_TestButton->setEnabled(enabled);
    ui->m_Boost_Valve_TestButton->setEnabled(enabled);
    ui->m_Fan1_TestButton->setEnabled(enabled);
    ui->m_Fan2_TestButton->setEnabled(enabled);
    ui->m_Fan3_TestButton->setEnabled(enabled);
}

void MEMSInterface::setActuatorsOffEnabled(bool enabled)
{
    ui->m_FuelPump_OffButton->setEnabled(enabled);
    ui->m_PTC_Relay_OffButton->setEnabled(enabled);
    ui->m_AC_Relay_OffButton->setEnabled(enabled);
    ui->m_Purge_Valve_OffButton->setEnabled(enabled);
    ui->m_O2_Heater_OffButton->setEnabled(enabled);
    ui->m_Boost_Valve_OffButton->setEnabled(enabled);
    ui->m_Fan1_OffButton->setEnabled(enabled);
    ui->m_Fan2_OffButton->setEnabled(enabled);
    ui->m_Fan3_OffButton->setEnabled(enabled);
}

// -------------------------------------------------------------
// Slots de Connexion & Flux
// -------------------------------------------------------------

void MEMSInterface::on_m_ConnectButton_clicked()
{
    QString portName = ui->m_ComPortComboBox->currentText();
    if (!portName.isEmpty()) {
        m_lastPortName = portName;
        emit connectToPort(portName);
    }
}

void MEMSInterface::on_m_DisconnectButton_clicked()
{
    m_reconnectTimer->stop();
    emit disconnectFromPort();
}

void MEMSInterface::on_m_StartStreamButton_clicked()
{
    emit startDataStream();
    ui->m_StartStreamButton->setEnabled(false);
    ui->m_StopStreamButton->setEnabled(true);
    startAutoLogging();
}

void MEMSInterface::on_m_StopStreamButton_clicked()
{
    emit stopDataStream();
    ui->m_StartStreamButton->setEnabled(true);
    ui->m_StopStreamButton->setEnabled(false);
    stopAutoLogging();
}

void MEMSInterface::on_m_ClearFaultsButton_clicked()
{
    emit clearFaults();
}

// -------------------------------------------------------------
// Réception & Traitement des Données ECU
// -------------------------------------------------------------

void MEMSInterface::onConnectionStatusChanged(bool connected, const QString &message)
{
    updateUIState(connected);
    ui->m_StatusLabel->setText(message);

    if (connected) {
        m_reconnectTimer->stop();
    } else if (!m_lastPortName.isEmpty()) {
        m_reconnectTimer->start();
    }
}

void MEMSInterface::onDataReceived(const MEMSData &data)
{
    // Mise à jour des libellés de données
    ui->m_RpmLabel->setText(QString::number(data.engineRPM));
    ui->m_CoolantTempLabel->setText(QString("%1 °C").arg(data.coolantTemp));
    ui->m_IntakeTempLabel->setText(QString("%1 °C").arg(data.intakeAirTemp));
    ui->m_BatteryVoltLabel->setText(QString("%1 V").arg(data.batteryVoltage, 0, 'f', 2));
    ui->m_MapLabel->setText(QString("%1 KPa").arg(data.mapSensor));

    // Mise à jour du graphique temporel
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_rpmSeries->append(now, data.engineRPM);
    m_tempSeries->append(now, data.coolantTemp);

    // Fenêtre glissante de 60 secondes sur l'axe X du graphique
    m_chart->axisX()->setRange(QDateTime::currentDateTime().addSecs(-60), QDateTime::currentDateTime());

    // Notification sonore et visuelle en cas d'apparition de nouveaux défauts
    if (!data.faultCodes.isEmpty() && data.faultCodes != m_previousFaults) {
        QApplication::beep();
        QApplication::alert(this, 3000);
        m_previousFaults = data.faultCodes;
    }

    // Affichage de la liste des défauts
    ui->m_FaultsListWidget->clear();
    for (const QString &fault : data.faultCodes) {
        ui->m_FaultsListWidget->addItem(fault);
    }

    // Enregistrement continu dans le fichier journal CSV
    if (m_autoLogStream) {
        *m_autoLogStream << QDateTime::currentDateTime().toString(Qt::ISODate) << ","
                         << data.engineRPM << ","
                         << data.coolantTemp << ","
                         << data.intakeAirTemp << ","
                         << data.batteryVoltage << ","
                         << data.mapSensor << "\n";
    }
}

void MEMSInterface::onFaultsCleared()
{
    ui->m_FaultsListWidget->clear();
    m_previousFaults.clear();
    ui->statusbar->showMessage("Défauts effacés avec succès.", 5000);
}

// -------------------------------------------------------------
// Capture d'écran instantanée
// -------------------------------------------------------------

void MEMSInterface::on_m_SnapshotButton_clicked()
{
    QPixmap pixmap = this->grab();
    QString dirPath = QCoreApplication::applicationDirPath() + "/captures";
    QDir().mkpath(dirPath);

    QString fileName = QString("%1/snapshot_%2.png")
                           .arg(dirPath)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

    if (pixmap.save(fileName, "PNG")) {
        ui->statusbar->showMessage("Capture d'écran sauvegardée : " + fileName, 5000);
    } else {
        ui->statusbar->showMessage("Échec de la capture d'écran.", 5000);
    }
}

// -------------------------------------------------------------
// Auto-Logging CSV
// -------------------------------------------------------------

void MEMSInterface::startAutoLogging()
{
    if (m_autoLogFile) return;

    QString logsFolder = QCoreApplication::applicationDirPath() + "/logs";
    QDir().mkpath(logsFolder);
    
    QString filePath = QString("%1/mems_log_%2.csv")
                           .arg(logsFolder)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

    m_autoLogFile = new QFile(filePath, this);
    if (m_autoLogFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_autoLogStream = new QTextStream(m_autoLogFile);
        *m_autoLogStream << "Timestamp,RPM,CoolantTemp,IntakeTemp,BatteryVoltage,MAP\n";
    } else {
        // Report the failure and reset state so we don't leave a half-open
        // QFile behind that makes it look as if logging is active.
        ui->statusbar->showMessage(
            tr("Impossible d'ouvrir le fichier journal (%1) : %2")
                .arg(filePath, m_autoLogFile->errorString()),
            5000);
        delete m_autoLogFile;
        m_autoLogFile = nullptr;
    }
}

void MEMSInterface::stopAutoLogging()
{
    if (m_autoLogFile) {
        m_autoLogFile->close();
        delete m_autoLogStream;
        delete m_autoLogFile;
        m_autoLogStream = nullptr;
        m_autoLogFile = nullptr;
    }
}

// -------------------------------------------------------------
// Actionneurs : Pompes, Relais & Valves
// -------------------------------------------------------------

// Fuel Pump
void MEMSInterface::on_m_FuelPump_TestButton_clicked() {
    emit testFuelPump();
    ui->m_FuelPump_TestButton->setEnabled(false);
    ui->m_FuelPump_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_FuelPump_OffButton_clicked() {
    emit turnOffFuelPump();
    ui->m_FuelPump_OffButton->setEnabled(false);
    ui->m_FuelPump_TestButton->setEnabled(true);
}
void MEMSInterface::onFuelPumpTestComplete() {
    ui->m_FuelPump_OffButton->setEnabled(false);
    ui->m_FuelPump_TestButton->setEnabled(true);
}

// PTC Relay
void MEMSInterface::on_m_PTC_Relay_TestButton_clicked() {
    emit testPTCRelay();
    ui->m_PTC_Relay_TestButton->setEnabled(false);
    ui->m_PTC_Relay_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_PTC_Relay_OffButton_clicked() {
    emit turnOffPTCRelay();
    ui->m_PTC_Relay_OffButton->setEnabled(false);
    ui->m_PTC_Relay_TestButton->setEnabled(true);
}
void MEMSInterface::onPTCRelayTestComplete() {
    ui->m_PTC_Relay_OffButton->setEnabled(false);
    ui->m_PTC_Relay_TestButton->setEnabled(true);
}

// AC Relay
void MEMSInterface::on_m_AC_Relay_TestButton_clicked() {
    emit testACRelay();
    ui->m_AC_Relay_TestButton->setEnabled(false);
    ui->m_AC_Relay_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_AC_Relay_OffButton_clicked() {
    emit turnOffACRelay();
    ui->m_AC_Relay_OffButton->setEnabled(false);
    ui->m_AC_Relay_TestButton->setEnabled(true);
}
void MEMSInterface::onACRelayTestComplete() {
    ui->m_AC_Relay_OffButton->setEnabled(false);
    ui->m_AC_Relay_TestButton->setEnabled(true);
}

// Purge Valve
void MEMSInterface::on_m_Purge_Valve_TestButton_clicked() {
    emit testPurgeValve();
    ui->m_Purge_Valve_TestButton->setEnabled(false);
    ui->m_Purge_Valve_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_Purge_Valve_OffButton_clicked() {
    emit turnOffPurgeValve();
    ui->m_Purge_Valve_OffButton->setEnabled(false);
    ui->m_Purge_Valve_TestButton->setEnabled(true);
}
void MEMSInterface::onPurgeValveTestComplete() {
    ui->m_Purge_Valve_OffButton->setEnabled(false);
    ui->m_Purge_Valve_TestButton->setEnabled(true);
}

// O2 Heater
void MEMSInterface::on_m_O2_Heater_TestButton_clicked() {
    emit testO2Heater();
    ui->m_O2_Heater_TestButton->setEnabled(false);
    ui->m_O2_Heater_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_O2_Heater_OffButton_clicked() {
    emit turnOffO2Heater();
    ui->m_O2_Heater_OffButton->setEnabled(false);
    ui->m_O2_Heater_TestButton->setEnabled(true);
}
void MEMSInterface::onO2HeaterTestComplete() {
    ui->m_O2_Heater_OffButton->setEnabled(false);
    ui->m_O2_Heater_TestButton->setEnabled(true);
}

// Boost Valve
void MEMSInterface::on_m_Boost_Valve_TestButton_clicked() {
    emit testBoostValve();
    ui->m_Boost_Valve_TestButton->setEnabled(false);
    ui->m_Boost_Valve_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_Boost_Valve_OffButton_clicked() {
    emit turnOffBoostValve();
    ui->m_Boost_Valve_OffButton->setEnabled(false);
    ui->m_Boost_Valve_TestButton->setEnabled(true);
}
void MEMSInterface::onBoostValveTestComplete() {
    ui->m_Boost_Valve_OffButton->setEnabled(false);
    ui->m_Boost_Valve_TestButton->setEnabled(true);
}

// Fan 1
void MEMSInterface::on_m_Fan1_TestButton_clicked() {
    emit testFan1();
    ui->m_Fan1_TestButton->setEnabled(false);
    ui->m_Fan1_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_Fan1_OffButton_clicked() {
    emit turnOffFan1();
    ui->m_Fan1_OffButton->setEnabled(false);
    ui->m_Fan1_TestButton->setEnabled(true);
}
void MEMSInterface::onFan1TestComplete() {
    ui->m_Fan1_OffButton->setEnabled(false);
    ui->m_Fan1_TestButton->setEnabled(true);
}

// Fan 2
void MEMSInterface::on_m_Fan2_TestButton_clicked() {
    emit testFan2();
    ui->m_Fan2_TestButton->setEnabled(false);
    ui->m_Fan2_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_Fan2_OffButton_clicked() {
    emit turnOffFan2();
    ui->m_Fan2_OffButton->setEnabled(false);
    ui->m_Fan2_TestButton->setEnabled(true);
}
void MEMSInterface::onFan2TestComplete() {
    ui->m_Fan2_OffButton->setEnabled(false);
    ui->m_Fan2_TestButton->setEnabled(true);
}

// Fan 3
void MEMSInterface::on_m_Fan3_TestButton_clicked() {
    emit testFan3();
    ui->m_Fan3_TestButton->setEnabled(false);
    ui->m_Fan3_OffButton->setEnabled(true);
}
void MEMSInterface::on_m_Fan3_OffButton_clicked() {
    emit turnOffFan3();
    ui->m_Fan3_OffButton->setEnabled(false);
    ui->m_Fan3_TestButton->setEnabled(true);
}
void MEMSInterface::onFan3TestComplete() {
    ui->m_Fan3_OffButton->setEnabled(false);
    ui->m_Fan3_TestButton->setEnabled(true);
}
