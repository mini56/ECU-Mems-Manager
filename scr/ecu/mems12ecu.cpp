#include "mems12ecu.h"

MEMS12ECU::MEMS12ECU()
{
    m_info = nullptr;
}

MEMS12ECU::~MEMS12ECU()
{
}

bool MEMS12ECU::initialiser(mems_info *info)
{
    m_info = info;
    return (m_info != nullptr);
}

bool MEMS12ECU::lireDonnees(mems_data *data)
{
    if (!m_info)
        return false;

    return mems_read(m_info, data);
}

bool MEMS12ECU::lireIdentifiant(uint8_t *buffer)
{
    if (!m_info)
        return false;

    return mems_init_link(m_info, buffer);
}

bool MEMS12ECU::effacerDefauts()
{
    if (!m_info)
        return false;

    return mems_clear_faults(m_info);
}

bool MEMS12ECU::resetAdaptations()
{
    if (!m_info)
        return false;

    return mems_reset_adjustments(m_info);
}

bool MEMS12ECU::resetECU()
{
    if (!m_info)
        return false;

    return mems_reset_ECU(m_info);
}

QString MEMS12ECU::nomECU() const
{
    return "Rover MEMS";
}

QString MEMS12ECU::versionECU() const
{
    return "1.2";
}