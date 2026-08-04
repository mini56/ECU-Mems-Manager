#ifndef MEMSINTERFACE_H
#define MEMSINTERFACE_H

#include <QMainWindow>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QSystemTrayIcon>
#include <QMetaType>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

#ifndef MEMS_INTERFACE_VERSION
#define MEMS_INTERFACE_VERSION "0.9.0"
#endif

QT_FORWARD_DECLARE_CLASS(QSerialPort)

namespace Ui { class MEMSInterface; }

/*
 * Runtime data snapshot produced by the worker (MEMSLogic) and consumed by the
 * GUI. Passed across the worker/GUI thread boundary via a queued signal, so it
 * is registered as a Qt meta-type (see Q_DECLARE_METATYPE below).
 */
struct MEMSData
{
    int         engineRPM     = 0;
    double      coolantTemp   = 0.0;
    double      intakeAirTemp = 0.0;
    double      batteryVoltage = 0.0;
    double      mapSensor     = 0.0;
    QStringList faultCodes;
};

/*
 * Worker object that owns the serial connection to the ECU. It lives in its
 * own QThread so the GUI never blocks on serial I/O. All public entry points
 * are slots (invoked via queued connections from the GUI) and all results are
 * reported back through signals.
 */
class MEMSLogic : public QObject
{
    Q_OBJECT

public:
    explicit MEMSLogic(QObject *parent = nullptr);
    ~MEMSLogic();

public slots:
    void connectToPort(const QString &portName);
    void disconnectFromPort();
    void startDataStream();
    void stopDataStream();
    void clearFaults();

    // Actuator tests (activate)
    void testFuelPump();
    void testPTCRelay();
    void testACRelay();
    void testPurgeValve();
    void testO2Heater();
    void testBoostValve();
    void testFan1();
    void testFan2();
    void testFan3();

    // Actuator shut-off
    void turnOffFuelPump();
    void turnOffPTCRelay();
    void turnOffACRelay();
    void turnOffPurgeValve();
    void turnOffO2Heater();
    void turnOffBoostValve();
    void turnOffFan1();
    void turnOffFan2();
    void turnOffFan3();

signals:
    void connectionStatusChanged(bool connected, const QString &message);
    void dataReceived(const MEMSData &data);
    void faultsCleared();

    void fuelPumpTestComplete();
    void ptcRelayTestComplete();
    void acRelayTestComplete();
    void purgeValveTestComplete();
    void o2HeaterTestComplete();
    void boostValveTestComplete();
    void fan1TestComplete();
    void fan2TestComplete();
    void fan3TestComplete();

private slots:
    void pollOnce();

private:
    QSerialPort *m_port    = nullptr;
    QTimer      *m_pollTimer = nullptr;
    bool         m_streaming = false;
};

/*
 * Main application window. Owns the UI (generated from memsinterface.ui),
 * a live QtCharts plot, and the MEMSLogic worker running in m_logicThread.
 */
class MEMSInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit MEMSInterface(QWidget *parent = nullptr);
    ~MEMSInterface();

signals:
    // GUI -> worker commands
    void connectToPort(const QString &portName);
    void disconnectFromPort();
    void startDataStream();
    void stopDataStream();
    void clearFaults();

    void testFuelPump();
    void testPTCRelay();
    void testACRelay();
    void testPurgeValve();
    void testO2Heater();
    void testBoostValve();
    void testFan1();
    void testFan2();
    void testFan3();

    void turnOffFuelPump();
    void turnOffPTCRelay();
    void turnOffACRelay();
    void turnOffPurgeValve();
    void turnOffO2Heater();
    void turnOffBoostValve();
    void turnOffFan1();
    void turnOffFan2();
    void turnOffFan3();

private slots:
    // Auto-connected UI slots (connectSlotsByName)
    void on_m_ConnectButton_clicked();
    void on_m_DisconnectButton_clicked();
    void on_m_StartStreamButton_clicked();
    void on_m_StopStreamButton_clicked();
    void on_m_ClearFaultsButton_clicked();
    void on_m_SnapshotButton_clicked();

    void on_m_FuelPump_TestButton_clicked();
    void on_m_FuelPump_OffButton_clicked();
    void on_m_PTC_Relay_TestButton_clicked();
    void on_m_PTC_Relay_OffButton_clicked();
    void on_m_AC_Relay_TestButton_clicked();
    void on_m_AC_Relay_OffButton_clicked();
    void on_m_Purge_Valve_TestButton_clicked();
    void on_m_Purge_Valve_OffButton_clicked();
    void on_m_O2_Heater_TestButton_clicked();
    void on_m_O2_Heater_OffButton_clicked();
    void on_m_Boost_Valve_TestButton_clicked();
    void on_m_Boost_Valve_OffButton_clicked();
    void on_m_Fan1_TestButton_clicked();
    void on_m_Fan1_OffButton_clicked();
    void on_m_Fan2_TestButton_clicked();
    void on_m_Fan2_OffButton_clicked();
    void on_m_Fan3_TestButton_clicked();
    void on_m_Fan3_OffButton_clicked();

    // Worker -> GUI feedback
    void onConnectionStatusChanged(bool connected, const QString &message);
    void onDataReceived(const MEMSData &data);
    void onFaultsCleared();

    void onFuelPumpTestComplete();
    void onPTCRelayTestComplete();
    void onACRelayTestComplete();
    void onPurgeValveTestComplete();
    void onO2HeaterTestComplete();
    void onBoostValveTestComplete();
    void onFan1TestComplete();
    void onFan2TestComplete();
    void onFan3TestComplete();

private:
    void refreshComPorts();
    void updateUIState(bool connected);
    void setActuatorTestsEnabled(bool enabled);
    void setActuatorsOffEnabled(bool enabled);
    void startAutoLogging();
    void stopAutoLogging();

    Ui::MEMSInterface *ui;

    MEMSLogic  *m_memsLogic;
    QThread     m_logicThread;

    QChart      *m_chart;
    QChartView  *m_chartView;
    QLineSeries *m_rpmSeries;
    QLineSeries *m_tempSeries;

    QTimer      *m_reconnectTimer;
    QString      m_lastPortName;
    QStringList  m_previousFaults;

    QFile       *m_autoLogFile;
    QTextStream *m_autoLogStream;

    QSystemTrayIcon *m_trayIcon;
};

Q_DECLARE_METATYPE(MEMSData)

#endif // MEMSINTERFACE_H
