#ifndef ECUDESCRIPTION_H
#define ECUDESCRIPTION_H

#include <QString>
#include <QByteArray>

enum ECUProtocol
{
    PROTOCOLE_INCONNU,
    PROTOCOLE_ROSCO,
    PROTOCOLE_KLINE
};

enum CableType
{
    CABLE_UART3,
    CABLE_OBD16,
    CABLE_OBD16_UART3
};

struct ECUDescription
{
    QByteArray ecuId;

    QString constructeur;

    QString famille;

    QString version;

    QString reference;

    QString logiciel;

    QString cartographie;

    QString vehicule;

    QString moteur;

    QString cylindree;

    QString norme;

    QString annee;

    ECUProtocol protocole;

    CableType cable;

    bool lectureROM;

    bool ecritureROM;

    bool lectureRAM;

    bool actionneurs;

    bool adaptations;

    bool immobiliseur;

    bool compatibleMEMSDiag;

    bool compatibleECUMemsManager;
};

#endif