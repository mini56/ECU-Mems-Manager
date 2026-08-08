```cpp
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
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

    bool initialize();

    QSqlDatabase database() const;

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

private:
    QSqlDatabase m_database;
    QString m_databasePath;
    QString m_connectionName;
};

#endif // DATABASEMANAGER_H
```
