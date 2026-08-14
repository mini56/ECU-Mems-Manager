#include <QThread>
#include <QDateTime>
#include <QCoreApplication>
#include <string.h>
#include "memsinterface.h"

/**
 * Constructor. Sets the serial device and measurement units.
 * @param device Name of (or path to) the serial device used to comminucate
 *  with the ECU.
 */
MEMSInterface::MEMSInterface(QString device, QObject * parent):
QObject(parent), m_deviceName(device), m_stopPolling(false), m_shutdownThread(false), m_initComplete(false)
{
  memset(&m_data, 0, sizeof(mems_data));
  memset(m_d0_response_buffer, 0, 4);
}

/**
 * Destructor.
 */
MEMSInterface::~MEMSInterface()
{
}

/**
 * Checks that the ECU link is up, emitting notConnected() when it is not.
 * @return True when a command may be sent to the ECU; false otherwise.
 */
bool MEMSInterface::requireConnection()
{
  if (isConnected())
  {
    return true;
  }

  emit notConnected();
  return false;
}

/**
 * Reports the outcome of a command sent to the ECU.
 * @return The value of commandSucceeded, for convenience.
 */
bool MEMSInterface::reportCommandResult(bool commandSucceeded)
{
  if (!commandSucceeded)
  {
    emit errorSendingCommand();
  }

  return commandSucceeded;
}

/**
 * Sends a single actuator command to the ECU, if it is connected.
 * @return True if the command was sent successfully; false otherwise.
 */
bool MEMSInterface::sendActuatorCommand(actuator_cmd cmd)
{
  if (!isConnected())
  {
    return false;
  }

  return reportCommandResult(mems_test_actuator(&m_memsinfo, cmd, NULL));
}

/**
 * Clears the block of fault codes.
 */
void MEMSInterface::onFaultCodesClearRequested()
{
  if (requireConnection() && reportCommandResult(mems_clear_faults(&m_memsinfo)))
  {
    emit faultCodesClearSuccess();
  }
}

/**
 * Resets all adjustments.
 */
void MEMSInterface::onResetAdjustmentsRequested()
{
  if (requireConnection())
  {
    reportCommandResult(mems_reset_adjustments(&m_memsinfo));
  }
}

/**
 * Resets complete ECU.
 */
void MEMSInterface::onResetECURequested()
{
  if (requireConnection() && reportCommandResult(mems_reset_ECU(&m_memsinfo)))
  {
    emit ECUResetSuccess();
  }
}

/**
 * Responds to a signal requesting that the idle air control valve be moved.
 */
void MEMSInterface::onIdleAirControlMovementRequest(int desiredPos)
{
  if (requireConnection())
  {
    reportCommandResult(mems_move_iac(&m_memsinfo, desiredPos));
  }

  emit moveIACComplete();
}

/**
 * Attempts to open the serial device that is connected to the ECU.
 * @return True if serial device was opened successfully and the
 *  ECU is responding to commands; false otherwise.
 */
bool MEMSInterface::connectToECU()
{
  bool status = mems_connect(&m_memsinfo, m_deviceName.toStdString().c_str()) &&
    mems_init_link(&m_memsinfo, m_d0_response_buffer);
  if (status)
  {
    emit gotEcuId(m_d0_response_buffer);
  }
  return status;
}

/**
 * Sets a flag that will cause us to stop polling and disconnect from the serial device.
 */
void MEMSInterface::disconnectFromECU()
{
  m_stopPolling = true;
}

/**
 * Cleans up and exits the worker thread.
 */
void MEMSInterface::onShutdownThreadRequest()
{
  if (m_serviceLoopRunning)
  {
    m_shutdownThread = true;
  }
  else
  {
    QThread::currentThread()->quit();
  }
}

/**
 * Indicates whether the serial device is currently open/connected.
 * @return True when the device is connected; false otherwise.
 */
bool MEMSInterface::isConnected()
{
  return (m_initComplete && mems_is_connected(&m_memsinfo));
}

/**
 * Responds to the parent thread being started by initializing the library
 * struct and emitting a signal indicating that the interface is ready.
 */
void MEMSInterface::onParentThreadStarted()
{
  // Initialize the interface state info struct here, so that
  // it's in the context of the thread that will use it.
  if (!m_initComplete)
  {
    mems_init(&m_memsinfo);
    m_initComplete = true;
  }

  emit interfaceThreadReady();
}

/**
 * Responds to a signal to start polling the ECU.
 */
void MEMSInterface::onStartPollingRequest()
{
  if (connectToECU())
  {
    emit connected();

    m_stopPolling = false;
    m_shutdownThread = false;
    runServiceLoop();
  }
  else
  {
#ifdef WIN32
    QString simpleDeviceName = m_deviceName;

    if (simpleDeviceName.indexOf("\\\\.\\") == 0)
    {
      simpleDeviceName.remove(0, 4);
    }
    emit failedToConnect(simpleDeviceName);
#else
    emit failedToConnect(m_deviceName);
#endif
  }
}

/**
 * Calls the library in a loop until commanded to stop.
 */
void MEMSInterface::runServiceLoop()
{
  bool connected = mems_is_connected(&m_memsinfo);

  m_serviceLoopRunning = true;
  while (!m_stopPolling && !m_shutdownThread && connected)
  {
    if (mems_read(&m_memsinfo, &m_data))
    {
      emit readSuccess();
      emit dataReady();
    }
    else
    {
      emit readError();
    }
    QCoreApplication::processEvents();
  }
  m_serviceLoopRunning = false;

  if (connected)
  {
    mems_disconnect(&m_memsinfo);
  }
  emit disconnected();

  if (m_shutdownThread)
  {
    QThread::currentThread()->quit();
  }
}

/**
 * Switches an actuator on, waits briefly, and switches it back off.
 * @return True if both commands were sent successfully; false otherwise.
 */
bool MEMSInterface::actuatorOnOffDelayTest(actuator_cmd onCmd, actuator_cmd offCmd)
{
  bool status = false;

  if (requireConnection())
  {
    if (mems_test_actuator(&m_memsinfo, onCmd, NULL))
    {
      QThread::sleep(1);
      status = mems_test_actuator(&m_memsinfo, offCmd, NULL);
    }

    reportCommandResult(status);
  }

  return status;
}

void MEMSInterface::onFuelPumpTest()
{
  actuatorOnOffDelayTest(MEMS_FuelPumpOn, MEMS_FuelPumpOff);
  emit fuelPumpTestComplete();
}

void MEMSInterface::on_m_Purge_Valve_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_PurgeValveOn, MEMS_PurgeValveOff);
  emit PurgeValveTestComplete();
}

void MEMSInterface::onPTCRelayTest()
{
  actuatorOnOffDelayTest(MEMS_PTCRelayOn, MEMS_PTCRelayOff);
  emit ptcRelayTestComplete();
}

void MEMSInterface::onACRelayTest()
{
  actuatorOnOffDelayTest(MEMS_ACRelayOn, MEMS_ACRelayOff);
  emit acRelayTestComplete();
}

void MEMSInterface::on_m_O2Heater_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_O2HeaterOn, MEMS_O2HeaterOff);
  emit O2HeaterTestComplete();
}

void MEMSInterface::on_m_Boost_Valve_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_BoostValveOn, MEMS_BoostValveOff);
  emit BoostValveTestComplete();
}

void MEMSInterface::on_m_Fan1_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_Fan1On, MEMS_Fan1Off);
  emit Fan1TestComplete();
}

void MEMSInterface::on_m_Fan2_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_Fan2On, MEMS_Fan2Off);
  emit Fan2TestComplete();
}

void MEMSInterface::on_m_Fan3_TestButton_clicked()
{
  actuatorOnOffDelayTest(MEMS_Fan3On, MEMS_Fan3Off);
  emit Fan3TestComplete();
}

void MEMSInterface::onFuelPumpOn()
{
  sendActuatorCommand(MEMS_FuelPumpOn);
}

void MEMSInterface::onFuelPumpOff()
{
  sendActuatorCommand(MEMS_FuelPumpOff);
}

void MEMSInterface::onPTCRelayOn()
{
  sendActuatorCommand(MEMS_PTCRelayOn);
}

void MEMSInterface::onPTCRelayOff()
{
  sendActuatorCommand(MEMS_PTCRelayOff);
}

void MEMSInterface::onACRelayOn()
{
  sendActuatorCommand(MEMS_ACRelayOn);
}

void MEMSInterface::onACRelayOff()
{
  sendActuatorCommand(MEMS_ACRelayOff);
}

void MEMSInterface::onIgnitionCoilTest()
{
  sendActuatorCommand(MEMS_FireCoil);
}

void MEMSInterface::onFuelInjectorTest()
{
  sendActuatorCommand(MEMS_TestInjectors);
}

void MEMSInterface::on_m_fuel_trim_plusButton_clicked()
{
  sendActuatorCommand(MEMS_FuelTrimPlus);
}

void MEMSInterface::on_m_fuel_trim_minusButton_clicked()
{
  sendActuatorCommand(MEMS_FuelTrimMinus);
}

void MEMSInterface::on_m_idle_decay_plusButton_clicked()
{
  sendActuatorCommand(MEMS_IdleDecayPlus);
}

void MEMSInterface::on_m_idle_decay_minusButton_clicked()
{
  sendActuatorCommand(MEMS_IdleDecayMinus);
}

void MEMSInterface::on_m_idle_speed_plusButton_clicked()
{
  sendActuatorCommand(MEMS_IdleSpeedPlus);
}

void MEMSInterface::on_m_idle_speed_minusButton_clicked()
{
  sendActuatorCommand(MEMS_IdleSpeedMinus);
}

void MEMSInterface::on_m_ignition_advance_plusButton_clicked()
{
  sendActuatorCommand(MEMS_IgnitionAdvancePlus);
}

void MEMSInterface::on_m_ignition_advance_minusButton_clicked()
{
  sendActuatorCommand(MEMS_IgnitionAdvanceMinus);
}

void MEMSInterface::on_m_Purge_Valve_OnButton_clicked()
{
  sendActuatorCommand(MEMS_PurgeValveOn);
}

void MEMSInterface::on_m_Purge_Valve_OffButton_clicked()
{
  sendActuatorCommand(MEMS_PurgeValveOff);
}

void MEMSInterface::on_m_O2Heater_OnButton_clicked()
{
  sendActuatorCommand(MEMS_O2HeaterOn);
}

void MEMSInterface::on_m_O2Heater_OffButton_clicked()
{
  sendActuatorCommand(MEMS_O2HeaterOff);
}

void MEMSInterface::on_m_Boost_Valve_OnButton_clicked()
{
  sendActuatorCommand(MEMS_BoostValveOn);
}

void MEMSInterface::on_m_Boost_Valve_OffButton_clicked()
{
  sendActuatorCommand(MEMS_BoostValveOff);
}

void MEMSInterface::on_m_Fan1_OnButton_clicked()
{
  sendActuatorCommand(MEMS_Fan1On);
}

void MEMSInterface::on_m_Fan2_OnButton_clicked()
{
  sendActuatorCommand(MEMS_Fan2On);
}

void MEMSInterface::on_m_Fan3_OnButton_clicked()
{
  sendActuatorCommand(MEMS_Fan3On);
}

void MEMSInterface::on_m_Fan1_OffButton_clicked()
{
  sendActuatorCommand(MEMS_Fan1Off);
}

void MEMSInterface::on_m_Fan2_OffButton_clicked()
{
  sendActuatorCommand(MEMS_Fan2Off);
}

void MEMSInterface::on_m_Fan3_OffButton_clicked()
{
  sendActuatorCommand(MEMS_Fan3Off);
}

void MEMSInterface::on_m_IACMinusButton_clicked()
{
  sendActuatorCommand(MEMS_CloseIAC);
}

void MEMSInterface::on_m_IACPlusButton_clicked()
{
  sendActuatorCommand(MEMS_OpenIAC);
}

void MEMSInterface::on_m_AllActuatorsOffButton_clicked()
{
  sendActuatorCommand(MEMS_AllActuatorsOff);
}

void MEMSInterface::on_interactive_push_button_clicked()
{
  if (isConnected())
  {
    emit errorSendingCommand();
  }
}
