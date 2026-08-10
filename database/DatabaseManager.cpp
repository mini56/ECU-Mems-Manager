#include "Databasemanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>
#include <QSqlRecord>

QVariantList DatabaseManager::queryList(
    const QString &sql,
    const QVariantList &values) const
{
    QVariantList result;

    if (!isOpen())
        return result;

    QSqlQuery query(m_database);

    if (!query.prepare(sql))
        return result;

    for (int i = 0; i < values.size(); ++i)
        query.bindValue(i, values.at(i));

    if (!query.exec())
        return result;

    while (query.next())
    {
        QVariantMap row;

        for (int i = 0; i < query.record().count(); ++i)
            row.insert(
                query.record().fieldName(i),
                query.value(i)
            );

        result.append(row);
    }

    return result;
}

QVariantMap DatabaseManager::queryOne(
    const QString &sql,
    const QVariantList &values) const
{
    QVariantMap result;

    const QVariantList rows = queryList(sql, values);

    if (!rows.isEmpty())
        result = rows.first().toMap();

    return result;
}


// ============================================================
// ECU
// ============================================================

int DatabaseManager::addEcu(
    const QString &manufacturer,
    const QString &family,
    const QString &model,
    const QString &memsVersion,
    const QString &protocol,
    const QString &partNumber,
    const QString &description,
    const QString &notes)
{
    if (!isOpen())
        return -1;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR IGNORE INTO ecu "
        "(manufacturer, family, model, mems_version, protocol, "
        "part_number, description, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(manufacturer);
    query.addBindValue(family);
    query.addBindValue(model);
    query.addBindValue(memsVersion);
    query.addBindValue(protocol);
    query.addBindValue(partNumber);
    query.addBindValue(description);
    query.addBindValue(notes);

    if (!query.exec())
        return -1;

    return query.lastInsertId().toInt();
}

QVariantList DatabaseManager::getEcus() const
{
    return queryList(
        "SELECT * FROM ecu ORDER BY mems_version, model"
    );
}

QVariantMap DatabaseManager::getEcu(int id) const
{
    return queryOne(
        "SELECT * FROM ecu WHERE id = ?",
        QVariantList() << id
    );
}


// ============================================================
// VEHICULES
// ============================================================

int DatabaseManager::addVehicle(
    const QString &make,
    const QString &model,
    const QString &variant,
    int yearFrom,
    int yearTo,
    const QString &engine,
    const QString &displacement,
    const QString &fuelSystem,
    const QString &notes)
{
    if (!isOpen())
        return -1;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT INTO vehicle "
        "(make, model, variant, year_from, year_to, engine, "
        "displacement, fuel_system, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(make);
    query.addBindValue(model);
    query.addBindValue(variant);
    query.addBindValue(yearFrom);
    query.addBindValue(yearTo);
    query.addBindValue(engine);
    query.addBindValue(displacement);
    query.addBindValue(fuelSystem);
    query.addBindValue(notes);

    if (!query.exec())
        return -1;

    return query.lastInsertId().toInt();
}

QVariantList DatabaseManager::getVehicles() const
{
    return queryList(
        "SELECT * FROM vehicle "
        "ORDER BY make, model, year_from"
    );
}

QVariantMap DatabaseManager::getVehicle(int id) const
{
    return queryOne(
        "SELECT * FROM vehicle WHERE id = ?",
        QVariantList() << id
    );
}


// ============================================================
// ASSOCIATION ECU / VEHICULE
// ============================================================

bool DatabaseManager::linkEcuVehicle(
    int ecuId,
    int vehicleId,
    const QString &notes)
{
    if (!isOpen())
        return false;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR REPLACE INTO ecu_vehicle "
        "(ecu_id, vehicle_id, notes) "
        "VALUES (?, ?, ?)"
    );

    query.addBindValue(ecuId);
    query.addBindValue(vehicleId);
    query.addBindValue(notes);

    return query.exec();
}

QVariantList DatabaseManager::getVehiclesForEcu(int ecuId) const
{
    return queryList(
        "SELECT v.* "
        "FROM vehicle v "
        "INNER JOIN ecu_vehicle ev "
        "ON ev.vehicle_id = v.id "
        "WHERE ev.ecu_id = ? "
        "ORDER BY v.make, v.model, v.year_from",
        QVariantList() << ecuId
    );
}

QVariantList DatabaseManager::getEcusForVehicle(int vehicleId) const
{
    return queryList(
        "SELECT e.* "
        "FROM ecu e "
        "INNER JOIN ecu_vehicle ev "
        "ON ev.ecu_id = e.id "
        "WHERE ev.vehicle_id = ? "
        "ORDER BY e.mems_version, e.model",
        QVariantList() << vehicleId
    );
}


// ============================================================
// DTC
// ============================================================

int DatabaseManager::addDtc(
    int ecuId,
    const QString &code,
    const QString &description,
    const QString &system,
    int severity,
    const QString &possibleCauses,
    const QString &diagnosticProcedure,
    const QString &repairNotes)
{
    if (!isOpen())
        return -1;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR REPLACE INTO dtc "
        "(ecu_id, code, description, system, severity, "
        "possible_causes, diagnostic_procedure, repair_notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(ecuId);
    query.addBindValue(code);
    query.addBindValue(description);
    query.addBindValue(system);
    query.addBindValue(severity);
    query.addBindValue(possibleCauses);
    query.addBindValue(diagnosticProcedure);
    query.addBindValue(repairNotes);

    if (!query.exec())
        return -1;

    return query.lastInsertId().toInt();
}

QVariantList DatabaseManager::getDtcs(int ecuId) const
{
    if (ecuId < 0)
    {
        return queryList(
            "SELECT * FROM dtc ORDER BY code"
        );
    }

    return queryList(
        "SELECT * FROM dtc "
        "WHERE ecu_id = ? "
        "ORDER BY code",
        QVariantList() << ecuId
    );
}

QVariantMap DatabaseManager::getDtc(int id) const
{
    return queryOne(
        "SELECT * FROM dtc WHERE id = ?",
        QVariantList() << id
    );
}

QVariantMap DatabaseManager::getDtcByCode(
    int ecuId,
    const QString &code) const
{
    return queryOne(
        "SELECT * FROM dtc "
        "WHERE ecu_id = ? AND code = ?",
        QVariantList() << ecuId << code
    );
}


// ============================================================
// PARAMETRES
// ============================================================

int DatabaseManager::addParameter(
    const QString &name,
    const QString &displayName,
    const QString &unit,
    const QString &dataType,
    const QString &description,
    double minimum,
    double maximum,
    double nominalMin,
    double nominalMax)
{
    if (!isOpen())
        return -1;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR IGNORE INTO parameter "
        "(name, display_name, unit, data_type, description, "
        "minimum, maximum, nominal_min, nominal_max) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(name);
    query.addBindValue(displayName);
    query.addBindValue(unit);
    query.addBindValue(dataType);
    query.addBindValue(description);
    query.addBindValue(minimum);
    query.addBindValue(maximum);
    query.addBindValue(nominalMin);
    query.addBindValue(nominalMax);

    if (!query.exec())
        return -1;

    return query.lastInsertId().toInt();
}

QVariantList DatabaseManager::getParameters() const
{
    return queryList(
        "SELECT * FROM parameter ORDER BY name"
    );
}

QVariantList DatabaseManager::getParametersForEcu(int ecuId) const
{
    return queryList(
        "SELECT p.*, ep.address, ep.scale, ep.offset, "
        "ep.minimum AS ecu_minimum, "
        "ep.maximum AS ecu_maximum, "
        "ep.notes AS ecu_notes "
        "FROM parameter p "
        "INNER JOIN ecu_parameter ep "
        "ON ep.parameter_id = p.id "
        "WHERE ep.ecu_id = ? "
        "ORDER BY p.name",
        QVariantList() << ecuId
    );
}

bool DatabaseManager::linkEcuParameter(
    int ecuId,
    int parameterId,
    int address,
    double scale,
    double offset,
    double minimum,
    double maximum,
    const QString &notes)
{
    if (!isOpen())
        return false;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR REPLACE INTO ecu_parameter "
        "(ecu_id, parameter_id, address, scale, offset, "
        "minimum, maximum, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(ecuId);
    query.addBindValue(parameterId);
    query.addBindValue(address);
    query.addBindValue(scale);
    query.addBindValue(offset);
    query.addBindValue(minimum);
    query.addBindValue(maximum);
    query.addBindValue(notes);

    return query.exec();
}


// ============================================================
// ACTIONNEURS
// ============================================================

int DatabaseManager::addActuator(
    const QString &name,
    const QString &displayName,
    const QString &description,
    bool testAvailable,
    bool onAvailable,
    bool offAvailable)
{
    if (!isOpen())
        return -1;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR IGNORE INTO actuator "
        "(name, display_name, description, "
        "test_available, on_available, off_available) "
        "VALUES (?, ?, ?, ?, ?, ?)"
    );

    query.addBindValue(name);
    query.addBindValue(displayName);
    query.addBindValue(description);
    query.addBindValue(testAvailable ? 1 : 0);
    query.addBindValue(onAvailable ? 1 : 0);
    query.addBindValue(offAvailable ? 1 : 0);

    if (!query.exec())
        return -1;

    return query.lastInsertId().toInt();
}

QVariantList DatabaseManager::getActuators() const
{
    return queryList(
        "SELECT * FROM actuator ORDER BY name"
    );
}

QVariantList DatabaseManager::getActuatorsForEcu(int ecuId) const
{
    return queryList(
        "SELECT a.*, ea.command_code, ea.notes AS ecu_notes "
        "FROM actuator a "
        "INNER JOIN ecu_actuator ea "
        "ON ea.actuator_id = a.id "
        "WHERE ea.ecu_id = ? "
        "ORDER BY a.name",
        QVariantList() << ecuId
    );
}

bool DatabaseManager::linkEcuActuator(
    int ecuId,
    int actuatorId,
    int commandCode,
    const QString &notes)
{
    if (!isOpen())
        return false;

    QSqlQuery query(m_database);

    query.prepare(
        "INSERT OR REPLACE INTO ecu_actuator "
        "(ecu_id, actuator_id, command_code, notes) "
        "VALUES (?, ?, ?, ?)"
    );

    query.addBindValue(ecuId);
    query.addBindValue(actuatorId);
    query.addBindValue(commandCode);
    query.addBindValue(notes);

    return query.exec();
}

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

