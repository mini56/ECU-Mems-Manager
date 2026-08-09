#include "DatabaseSeed.h"

namespace DatabaseSeed
{

bool populate(DatabaseManager &database)
{
    if (!database.isOpen())
        return false;

    /*
     * ============================================================
     * ACTIONNEURS
     * ============================================================
     *
     * Les noms correspondent aux fonctions déjà présentes
     * dans MEMSInterface / MainWindow.
     */

    database.addActuator(
        "fuel_pump",
        "Fuel Pump",
        "Fuel pump relay",
        true,
        true,
        true
    );

    database.addActuator(
        "ptc_relay",
        "PTC Relay",
        "PTC relay",
        true,
        true,
        true
    );

    database.addActuator(
        "ac_relay",
        "A/C Relay",
        "Air conditioning relay",
        true,
        true,
        true
    );

    database.addActuator(
        "injector",
        "Injector",
        "Fuel injector test",
        true,
        false,
        false
    );

    database.addActuator(
        "ignition_coil",
        "Ignition Coil",
        "Ignition coil test",
        true,
        false,
        false
    );

    database.addActuator(
        "iac",
        "Idle Air Control",
        "Idle air control valve",
        true,
        false,
        false
    );

    database.addActuator(
        "purge_valve",
        "Purge Valve",
        "Evaporative purge valve",
        true,
        true,
        true
    );

    database.addActuator(
        "o2_heater",
        "O2 Heater",
        "Oxygen sensor heater",
        true,
        true,
        true
    );

    database.addActuator(
        "boost_valve",
        "Boost Valve",
        "Boost control valve",
        true,
        true,
        true
    );

    database.addActuator(
        "fan1",
        "Fan 1",
        "Cooling fan 1",
        true,
        true,
        true
    );

    database.addActuator(
        "fan2",
        "Fan 2",
        "Cooling fan 2",
        true,
        true,
        true
    );

    database.addActuator(
        "fan3",
        "Fan 3",
        "Cooling fan 3",
        true,
        true,
        true
    );

    return true;
}

}
