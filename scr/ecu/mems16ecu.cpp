#include "mems16ecu.h"

MEMS16ECU::MEMS16ECU()
{
    m_info = nullptr;
}

MEMS16ECU::~MEMS16ECU()
{
}

bool MEMS16ECU::initialiser(mems_info *info)
{
    m_info = info;
    return (m_info != nullptr);
}

bool MEMS16ECU::lireDonnees(mems_data *data)
{
    if (!m_info)
        return false;

    return mems_read(m_info, data);
}

bool MEMS16ECU::lireIdentifiant(uint8_t *buffer)
{
    if (!m_info)
        return false;

    return mems_init_link(m_info, buffer);
}

bool MEMS16ECU::effacerDefauts()
{
    if (!m_info)
        return false;

    return mems_clear_faults(m_info);
}

bool MEMS16ECU::resetAdaptations()
{
    if (!m_info)
        return false;

    return mems_reset_adjustments(m_info);
}

bool MEMS16ECU::resetECU()
{
    if (!m_info)
        return false;

    return mems_reset_ECU(m_info);
}

QString MEMS16ECU::nomECU() const
{
    return "Rover MEMS";
}

QString MEMS16ECU::versionECU() const
{
    return "1.6";
}

bool MEMS16ECU::supportLectureROM() const
{
    // La lecture de ROM sera ajoutée ultérieurement.
    return false;
}