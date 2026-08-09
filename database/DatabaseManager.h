#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool open(const QString &databasePath = QString());
    void close();

    bool isOpen() const;
    QString databasePath() const;
    QSqlDatabase database() const;

    // Initialisation
    bool initialize();

    // ECU
    int addEcu(const QString &manufacturer,
               const QString &family,
               const QString &model,
               const QString &memsVersion,
               const QString &protocol,
               const QString &partNumber,
               const QString &description,
               const QString &notes = QString());

    QVariantList getEcus() const;
    QVariantMap getEcu(int id) const;

    // Véhicules
    int addVehicle(const QString &make,
                   const QString &model,
                   const QString &variant,
                   int yearFrom,
                   int yearTo,
                   const QString &engine,
                   const QString &displacement,
                   const QString &fuelSystem,
                   const QString &notes = QString());

    QVariantList getVehicles() const;
    QVariantMap getVehicle(int id) const;

    // Association ECU / véhicule
    bool linkEcuVehicle(int ecuId,
                        int vehicleId,
                        const QString &notes = QString());

    QVariantList getVehiclesForEcu(int ecuId) const;
    QVariantList getEcusForVehicle(int vehicleId) const;

    // DTC
    int addDtc(int ecuId,
               const QString &code,
               const QString &description,
               const QString &system = QString(),
               int severity = 0,
               const QString &possibleCauses = QString(),
               const QString &diagnosticProcedure = QString(),
               const QString &repairNotes = QString());

    QVariantList getDtcs(int ecuId = -1) const;
    QVariantMap getDtc(int id) const;
    QVariantMap getDtcByCode(int ecuId,
                             const QString &code) const;

    // Paramètres
    int addParameter(const QString &name,
                     const QString &displayName,
                     const QString &unit,
                     const QString &dataType,
                     const QString &description,
                     double minimum,
                     double maximum,
                     double nominalMin,
                     double nominalMax);

    QVariantList getParameters() const;
    QVariantList getParametersForEcu(int ecuId) const;

    bool linkEcuParameter(int ecuId,
                          int parameterId,
                          int address,
                          double scale = 1.0,
                          double offset = 0.0,
                          double minimum = 0.0,
                          double maximum = 0.0,
                          const QString &notes = QString());

    // Actionneurs
    int addActuator(const QString &name,
                    const QString &displayName,
                    const QString &description,
                    bool testAvailable,
                    bool onAvailable,
                    bool offAvailable);

    QVariantList getActuators() const;
    QVariantList getActuatorsForEcu(int ecuId) const;

    bool linkEcuActuator(int ecuId,
                         int actuatorId,
                         int commandCode,
                         const QString &notes = QString());

private:
    bool createTables();

    bool createMetaTable();
    bool createEcuTable();
    bool createVehicleTable();
    bool createEcuVehicleTable();
    bool createDtcTable();
    bool createParameterTable();
    bool createActuatorTable();
    bool createEcuParameterTable();
    bool createEcuActuatorTable();

    QString defaultDatabasePath() const;

    QVariantList queryList(const QString &sql,
                           const QVariantList &values = QVariantList()) const;

    QVariantMap queryOne(const QString &sql,
                         const QVariantList &values = QVariantList()) const;

private:
    QSqlDatabase m_database;
    QString m_databasePath;
    QString m_connectionName;
};

#endif // DATABASEMANAGER_H
