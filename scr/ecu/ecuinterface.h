#ifndef ECUINTERFACE_H
#define ECUINTERFACE_H

#include <QString>
#include "../rosco.h"

/*
 * Interface commune à tous les calculateurs Rover MEMS.
 * Toutes les versions (1.2, 1.3, 1.6...) devront hériter de cette classe.
 */

class ECUInterface
{
public:

    virtual ~ECUInterface() {}

    // Nom de la famille MEMS
    virtual QString nomECU() const = 0;

    // Version
    virtual QString versionECU() const = 0;

    // Initialisation
    virtual bool initialiser(mems_info *info) = 0;

    // Lecture des données temps réel
    virtual bool lireDonnees(mems_data *data) = 0;

    // Lecture de l'identifiant ECU
    virtual bool lireIdentifiant(uint8_t *buffer) = 0;

    // Effacement des défauts
    virtual bool effacerDefauts() = 0;

    // Remise à zéro des adaptations
    virtual bool resetAdaptations() = 0;

    // Réinitialisation ECU
    virtual bool resetECU() = 0;

    // Présence de la lecture ROM
    virtual bool supportLectureROM() const
    {
        return false;
    }

    // Présence de l'écriture ROM
    virtual bool supportEcritureROM() const
    {
        return false;
    }

};

#endif