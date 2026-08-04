#include "mems13ecu.h"

MEMS13ECU::MEMS13ECU()
{
    m_info = nullptr;
}

MEMS13ECU::~MEMS13ECU()
{
}

bool MEMS13ECU::initialiser(mems_info *info)
{
    m_info = info;
    return (m_info != nullptr);
}

bool MEMS13ECU::lireDonnees(mems_data *data)
{
    if (!m_info)
        return false;

    return mems_read(m_info, data);
}

bool MEMS13ECU::lireIdentifiant(uint8_t *buffer)
{
    if (!m_info)
        return false;

    return mems_init_link(m_info, buffer);
}

bool MEMS13ECU::effacerDefauts()
{
    if (!m_info)
        return false;

    return mems_clear_faults(m_info);
}

bool MEMS13ECU::resetAdaptations()
{
    if (!m_info)
        return false;

    return mems_reset_adjustments(m_info);
}

bool MEMS13ECU::resetECU()
{
    if (!m_info)
        return false;

    return mems_reset_ECU(m_info);
}

QString MEMS13ECU::nomECU() const
{
    return "Rover MEMS";
}

QString MEMS13ECU::versionECU() const
{
    return "1.3";
}

bool MEMS13ECU::supportLectureROM() const
{
    // Prévu pour la version 1.0
    return false;
}

bool MEMS13ECU::supportEcritureROM() const
{
    // Prévu pour la version 1.0
    return false;
}