#ifndef MEMS16ECU_H
#define MEMS16ECU_H

#include "ecuinterface.h"

/*
 * Gestion des calculateurs Rover MEMS 1.6
 */

class MEMS16ECU : public ECUInterface
{
public:

    MEMS16ECU();
    virtual ~MEMS16ECU();

    bool initialiser(mems_info *info) override;

    bool lireDonnees(mems_data *data) override;

    bool lireIdentifiant(uint8_t *buffer) override;

    bool effacerDefauts() override;

    bool resetAdaptations() override;

    bool resetECU() override;

    QString nomECU() const override;

    QString versionECU() const override;

    bool supportLectureROM() const override;

private:

    mems_info *m_info;

};

#endif