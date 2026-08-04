#ifndef MEMS12ECU_H
#define MEMS12ECU_H

#include "ecuinterface.h"

/*
 * Gestion des calculateurs Rover MEMS 1.2
 */

class MEMS12ECU : public ECUInterface
{
public:

    MEMS12ECU();
    virtual ~MEMS12ECU();

    bool initialiser(mems_info *info) override;

    bool lireDonnees(mems_data *data) override;

    bool lireIdentifiant(uint8_t *buffer) override;

    bool effacerDefauts() override;

    bool resetAdaptations() override;

    bool resetECU() override;

    QString nomECU() const override;

    QString versionECU() const override;

private:

    mems_info *m_info;

};

#endif