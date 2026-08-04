#ifndef ECUFACTORY_H
#define ECUFACTORY_H

#include "ecuinterface.h"

enum TypeECU
{
    ECU_INCONNU = 0,
    ECU_MEMS12,
    ECU_MEMS13,
    ECU_MEMS16
};

/*
 * Fabrique des calculateurs.
 * Une seule classe est créée suivant le type d'ECU détecté.
 */

class ECUFactory
{
public:

    static ECUInterface *creer(TypeECU type);

    static TypeECU detecter(uint8_t *ecuId);

    static QString nom(TypeECU type);

};

#endif