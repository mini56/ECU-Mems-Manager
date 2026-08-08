```cpp
#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent),
      m_connectionName(
          QString("ECU_Mems_Manager_%1")
              .arg(QUuid::createUuid().toString())
      )
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

QString DatabaseManager::defaultDatabasePath() const
{
    const QString directory =
        QCoreApplication::applicationDirPath() + "/database";

    QDir dir;
    dir.mkpath(directory);

    return directory + "/ecu_mems_manager.sqlite";
}

bool DatabaseManager::open(const QString &databasePath)
{
    if (m_database.isOpen())
        return true;

    m_databasePath =
        databasePath.isEmpty()
            ? defaultDatabasePath()
            : databasePath;

    m_database = QSqlDatabase::addDatabase(
        "QSQLITE",
        m_connectionName
    );

    m_database.setDatabaseName(m_databasePath);

    if (!m_database.open())
        return false;

    return initialize();
}

void DatabaseManager::close()
{
    if (!m_database.isValid())
        return;

    if (m_database.isOpen())
        m_database.close();

    const QString connection = m_connectionName;

    m_database = QSqlDatabase();

    QSqlDatabase::removeDatabase(connection);
}

bool DatabaseManager::isOpen() const
{
    return m_database.isValid() && m_database.isOpen();
}

QString DatabaseManager::databasePath() const
{
    return m_databasePath;
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

bool DatabaseManager::initialize()
{
    if (!isOpen())
        return false;

    return createTables();
}

bool DatabaseManager::createTables()
{
    return createMetaTable()
        && createEcuTable()
        && createVehicleTable()
        && createEcuVehicleTable()
        && createDtcTable()
        && createParameterTable()
        && createActuatorTable()
        && createEcuParameterTable()
        && createEcuActuatorTable();
}

bool DatabaseManager::createMetaTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS meta ("
        "id INTEGER PRIMARY KEY CHECK(id = 1),"
        "application_name TEXT NOT NULL,"
        "database_version INTEGER NOT NULL,"
        "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ")"
    );
}

bool DatabaseManager::createEcuTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS ecu ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "manufacturer TEXT,"
        "family TEXT NOT NULL,"
        "model TEXT,"
        "mems_version TEXT NOT NULL,"
        "protocol TEXT,"
        "part_number TEXT,"
        "description TEXT,"
        "notes TEXT,"
        "UNIQUE(mems_version, part_number)"
        ")"
    );
}

bool DatabaseManager::createVehicleTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS vehicle ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "make TEXT NOT NULL,"
        "model TEXT NOT NULL,"
        "variant TEXT,"
        "year_from INTEGER,"
        "year_to INTEGER,"
        "engine TEXT,"
        "displacement TEXT,"
        "fuel_system TEXT,"
        "notes TEXT"
        ")"
    );
}

bool DatabaseManager::createEcuVehicleTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS ecu_vehicle ("
        "ecu_id INTEGER NOT NULL,"
        "vehicle_id INTEGER NOT NULL,"
        "notes TEXT,"
        "PRIMARY KEY(ecu_id, vehicle_id),"
        "FOREIGN KEY(ecu_id) REFERENCES ecu(id),"
        "FOREIGN KEY(vehicle_id) REFERENCES vehicle(id)"
        ")"
    );
}

bool DatabaseManager::createDtcTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS dtc ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "ecu_id INTEGER,"
        "code TEXT NOT NULL,"
        "description TEXT,"
        "system TEXT,"
        "severity INTEGER DEFAULT 0,"
        "possible_causes TEXT,"
        "diagnostic_procedure TEXT,"
        "repair_notes TEXT,"
        "FOREIGN KEY(ecu_id) REFERENCES ecu(id),"
        "UNIQUE(ecu_id, code)"
        ")"
    );
}

bool DatabaseManager::createParameterTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS parameter ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "display_name TEXT,"
        "unit TEXT,"
        "data_type TEXT,"
        "description TEXT,"
        "minimum REAL,"
        "maximum REAL,"
        "nominal_min REAL,"
        "nominal_max REAL"
        ")"
    );
}

bool DatabaseManager::createActuatorTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS actuator ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "display_name TEXT,"
        "description TEXT,"
        "test_available INTEGER NOT NULL DEFAULT 0,"
        "on_available INTEGER NOT NULL DEFAULT 0,"
        "off_available INTEGER NOT NULL DEFAULT 0"
        ")"
    );
}

bool DatabaseManager::createEcuParameterTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS ecu_parameter ("
        "ecu_id INTEGER NOT NULL,"
        "parameter_id INTEGER NOT NULL,"
        "address INTEGER,"
        "scale REAL DEFAULT 1.0,"
        "offset REAL DEFAULT 0.0,"
        "minimum REAL,"
        "maximum REAL,"
        "notes TEXT,"
        "PRIMARY KEY(ecu_id, parameter_id),"
        "FOREIGN KEY(ecu_id) REFERENCES ecu(id),"
        "FOREIGN KEY(parameter_id) REFERENCES parameter(id)"
        ")"
    );
}

bool DatabaseManager::createEcuActuatorTable()
{
    QSqlQuery query(m_database);

    return query.exec(
        "CREATE TABLE IF NOT EXISTS ecu_actuator ("
        "ecu_id INTEGER NOT NULL,"
        "actuator_id INTEGER NOT NULL,"
        "command_code INTEGER,"
        "notes TEXT,"
        "PRIMARY KEY(ecu_id, actuator_id),"
        "FOREIGN KEY(ecu_id) REFERENCES ecu(id),"
        "FOREIGN KEY(actuator_id) REFERENCES actuator(id)"
        ")"
    );
}
```
