#include "ecumanager.h"

ECUManager::ECUManager(QObject *parent)
    : QObject(parent)
{
    m_ecu = nullptr;
}

ECUManager::~ECUManager()
{
}

bool ECUManager::connectECU()
{
    return false;
}

void ECUManager::disconnectECU()
{
}

bool ECUManager::isConnected() const
{
    return false;
}

bool ECUManager::detectECU()
{
    return false;
}

bool ECUManager::startPolling()
{
    return false;
}

void ECUManager::stopPolling()
{
}

bool ECUManager::readFaultCodes()
{
    return false;
}

bool ECUManager::clearFaultCodes()
{
    return false;
}

bool ECUManager::testFuelPump()
{
    return false;
}

bool ECUManager::testCoolingFan()
{
    return false;
}

bool ECUManager::testPurgeValve()
{
    return false;
}

bool ECUManager::testO2Heater()
{
    return false;
}

bool ECUManager::moveIAC(int position)
{
    Q_UNUSED(position)
    return false;
}

bool ECUManager::resetAdaptations()
{
    return false;
}

bool ECUManager::resetECU()
{
    return false;
}

bool ECUManager::readROM()
{
    return false;
}

bool ECUManager::writeROM()
{
    return false;
}