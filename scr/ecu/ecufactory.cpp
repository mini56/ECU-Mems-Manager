#include "ecufactory.h"

#include "mems12ecu.h"
#include "mems13ecu.h"
#include "mems16ecu.h"

ECUInterface *ECUFactory::creer(TypeECU type)
{
    switch(type)
    {

    case ECU_MEMS12:
        return new MEMS12ECU();

    case ECU_MEMS13:
        return new MEMS13ECU();

    case ECU_MEMS16:
        return new MEMS16ECU();

    default:
        return nullptr;

    }
}

QString ECUFactory::nom(TypeECU type)
{
    switch(type)
    {

    case ECU_MEMS12:
        return "MEMS 1.2";

    case ECU_MEMS13:
        return "MEMS 1.3";

    case ECU_MEMS16:
        return "MEMS 1.6";

    default:
        return "Inconnu";

    }
}

/*
 * Détection provisoire.
 *
 * La vraie table de correspondance des identifiants Rover sera
 * ajoutée progressivement à partir des ECU réels.
 */

TypeECU ECUFactory::detecter(uint8_t *ecuId)
{
    if(ecuId == nullptr)
        return ECU_INCONNU;

    /*
     * Les identifiants exacts seront renseignés au fur et à mesure
     * de la constitution de la base de données.
     */

    switch(ecuId[0])
    {

        case 0x12:
            return ECU_MEMS12;

        case 0x13:
            return ECU_MEMS13;

        case 0x16:
            return ECU_MEMS16;

        default:
            return ECU_INCONNU;

    }
}