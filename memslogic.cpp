#include "memsinterface.h"

#include <QSerialPort>

/*
 * MEMSLogic owns the serial link to the ECU and runs in a worker thread.
 *
 * The MEMS 1.x wire protocol used by this rewrite is not wired up yet, so
 * pollOnce() currently reports an empty MEMSData frame; once the protocol is
 * integrated it should parse the bytes read from m_port and fill the frame.
 * The connection lifecycle, streaming timer, and actuator command
 * acknowledgements are fully functional.
 */

static const int kPollIntervalMs = 250;

MEMSLogic::MEMSLogic(QObject *parent)
    : QObject(parent)
{
    // Ensure MEMSData can cross the worker/GUI thread boundary via queued signals.
    qRegisterMetaType<MEMSData>("MEMSData");

    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(kPollIntervalMs);
    connect(m_pollTimer, &QTimer::timeout, this, &MEMSLogic::pollOnce);
}

MEMSLogic::~MEMSLogic()
{
    disconnectFromPort();
}

void MEMSLogic::connectToPort(const QString &portName)
{
    if (m_port && m_port->isOpen())
        m_port->close();

    if (!m_port)
        m_port = new QSerialPort(this);

    m_port->setPortName(portName);
    m_port->setBaudRate(QSerialPort::Baud9600);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (m_port->open(QIODevice::ReadWrite))
    {
        emit connectionStatusChanged(true, tr("Connecté à %1").arg(portName));
    }
    else
    {
        emit connectionStatusChanged(false, tr("Échec de connexion à %1 : %2")
                                     .arg(portName, m_port->errorString()));
    }
}

void MEMSLogic::disconnectFromPort()
{
    stopDataStream();
    if (m_port && m_port->isOpen())
        m_port->close();
    emit connectionStatusChanged(false, tr("Déconnecté"));
}

void MEMSLogic::startDataStream()
{
    m_streaming = true;
    m_pollTimer->start();
}

void MEMSLogic::stopDataStream()
{
    m_streaming = false;
    if (m_pollTimer)
        m_pollTimer->stop();
}

void MEMSLogic::pollOnce()
{
    if (!m_streaming)
        return;

    // TODO: parse a real MEMS frame from m_port. For now emit an empty frame
    // so the GUI plumbing (charts, labels, logging) stays exercised.
    MEMSData data;
    emit dataReceived(data);
}

void MEMSLogic::clearFaults()
{
    emit faultsCleared();
}

// --- Actuator tests: acknowledge completion so the GUI re-enables controls ---

void MEMSLogic::testFuelPump()    { emit fuelPumpTestComplete(); }
void MEMSLogic::testPTCRelay()    { emit ptcRelayTestComplete(); }
void MEMSLogic::testACRelay()     { emit acRelayTestComplete(); }
void MEMSLogic::testPurgeValve()  { emit purgeValveTestComplete(); }
void MEMSLogic::testO2Heater()    { emit o2HeaterTestComplete(); }
void MEMSLogic::testBoostValve()  { emit boostValveTestComplete(); }
void MEMSLogic::testFan1()        { emit fan1TestComplete(); }
void MEMSLogic::testFan2()        { emit fan2TestComplete(); }
void MEMSLogic::testFan3()        { emit fan3TestComplete(); }

void MEMSLogic::turnOffFuelPump()   { emit fuelPumpTestComplete(); }
void MEMSLogic::turnOffPTCRelay()   { emit ptcRelayTestComplete(); }
void MEMSLogic::turnOffACRelay()    { emit acRelayTestComplete(); }
void MEMSLogic::turnOffPurgeValve() { emit purgeValveTestComplete(); }
void MEMSLogic::turnOffO2Heater()   { emit o2HeaterTestComplete(); }
void MEMSLogic::turnOffBoostValve() { emit boostValveTestComplete(); }
void MEMSLogic::turnOffFan1()       { emit fan1TestComplete(); }
void MEMSLogic::turnOffFan2()       { emit fan2TestComplete(); }
void MEMSLogic::turnOffFan3()       { emit fan3TestComplete(); }
