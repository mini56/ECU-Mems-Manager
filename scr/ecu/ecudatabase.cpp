#include "ecudatabase.h"

QVector<ECUDescription> ECUDatabase::liste()
{
    QVector<ECUDescription> db;

    //=========================================================
    // MINI SPI MEMS 1.3
    //=========================================================

    db.push_back({

        QByteArray(),

        "Lucas",

        "MEMS",

        "1.3",

        "",

        "",

        "",

        "Mini SPI",

        "A-Series",

        "1275",

        "Euro 1",

        "1992-1994",

        PROTOCOLE_ROSCO,

        CABLE_UART3,

        true,

        true,

        true,

        true,

        true,

        true,

        true,

        true

    });

    //=========================================================
    // MINI SPI MEMS 1.6
    //=========================================================

    db.push_back({

        QByteArray(),

        "Lucas",

        "MEMS",

        "1.6",

        "",

        "",

        "",

        "Mini SPI",

        "A-Series",

        "1275",

        "Euro 2",

        "1994-1996",

        PROTOCOLE_ROSCO,

        CABLE_UART3,

        true,

        true,

        true,

        true,

        true,

        true,

        true,

        true

    });

    //=========================================================
    // ROVER K SERIES MEMS 1.2
    //=========================================================

    db.push_back({

        QByteArray(),

        "Lucas",

        "MEMS",

        "1.2",

        "",

        "",

        "",

        "Rover",

        "K-Series",

        "1400",

        "Euro 1",

        "1991-1994",

        PROTOCOLE_ROSCO,

        CABLE_UART3,

        true,

        true,

        true,

        true,

        true,

        true,

        true,

        true

    });

    return db;
}