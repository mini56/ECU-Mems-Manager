#ifndef ECUDATABASE_H
#define ECUDATABASE_H

#include <QString>
#include <QVector>

#include "ecufactory.h"

/*
 * Description d'un calculateur Rover MEMS.
 */

struct ECUDescription
{
    TypeECU type;

    QString version;

    QString reference;

    QString constructeur;

    QString vehicule;

    QString moteur;

    QString protocole;

    QString connecteur;

    bool lectureROM;

    bool ecritureROM;

    bool adaptations;

    bool actionneurs;

    uint8_t id0;
    uint8_t id1;
    uint8_t id2;
    uint8_t id3;
};

class ECUDatabase
{
public:

    static QVector<ECUDescription> liste();

    static const ECUDescription *chercher(uint8_t *id);

};

#endif