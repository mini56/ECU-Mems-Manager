#ifndef MEMS13ECU_H
#define MEMS13ECU_H

#include "ecuinterface.h"

/*
 * Gestion des calculateurs Rover MEMS 1.3
 * (Mini SPI, Metro, Rover Série 100...)
 */

class MEMS13ECU : public ECUInterface
{
public:

    MEMS13ECU();
    virtual ~MEMS13ECU();

    bool initialiser(mems_info *info) override;

    bool lireDonnees(mems_data *data) override;

    bool lireIdentifiant(uint8_t *buffer) override;

    bool effacerDefauts() override;

    bool resetAdaptations() override;

    bool resetECU() override;

    QString nomECU() const override;

    QString versionECU() const override;

    bool supportLectureROM() const override;

    bool supportEcritureROM() const override;

private:

    mems_info *m_info;

};

#endif