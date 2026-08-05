#include "mems13interface.h"
#include "../memsinterface.h"

MEMS13Interface::MEMS13Interface(QObject *parent)
    : ECUInterface(parent)
{
    m_mems = nullptr;
}

MEMS13Interface::~MEMS13Interface()
{
}

bool MEMS13Interface::connectECU()
{
    return false;
}

void MEMS13Interface::disconnectECU()
{
}

bool MEMS13Interface::isConnected() const
{
    return false;
}

bool MEMS13Interface::detectECU()
{
    return true;
}

QString MEMS13Interface::ecuName() const
{
    return "Lucas MEMS";
}

QString MEMS13Interface::ecuVersion() const
{
    return "1.3";
}

bool MEMS13Interface::startPolling()
{
    return false;
}

void MEMS13Interface::stopPolling()
{
}

mems_data *MEMS13Interface::liveData()
{
    return nullptr;
}

bool MEMS13Interface::readFaultCodes()
{
    return false;
}

bool MEMS13Interface::clearFaultCodes()
{
    return false;
}

bool MEMS13Interface::fuelPump(bool)
{
    return false;
}

bool MEMS13Interface::purgeValve(bool)
{
    return false;
}

bool MEMS13Interface::o2Heater(bool)
{
    return false;
}

bool MEMS13Interface::coolingFan(int,bool)
{
    return false;
}

bool MEMS13Interface::moveIAC(int)
{
    return false;
}

bool MEMS13Interface::resetECU()
{
    return false;
}

bool MEMS13Interface::resetAdaptations()
{
    return false;
}

bool MEMS13Interface::readROM()
{
    return false;
}

bool MEMS13Interface::writeROM()
{
    return false;
}