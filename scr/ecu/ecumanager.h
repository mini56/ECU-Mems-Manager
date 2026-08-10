#ifndef ECUMANAGER_H
#define ECUMANAGER_H

#include <QObject>

class ECUInterface;

class ECUManager : public QObject
{
    Q_OBJECT

public:
    explicit ECUManager(QObject *parent = nullptr);
    ~ECUManager();

    // Connexion au calculateur
    bool connectECU();
    void disconnectECU();
    bool isConnected() const;

    // Détection automatique
    bool detectECU();

    // Lecture des données
    bool startPolling();
    void stopPolling();

    // Défauts
    bool readFaultCodes();
    bool clearFaultCodes();

    // Actionneurs
    bool testFuelPump();
    bool testCoolingFan();
    bool testPurgeValve();
    bool testO2Heater();
    bool moveIAC(int position);

    // Réinitialisations
    bool resetAdaptations();
    bool resetECU();

    // ROM
    bool readROM();
    bool writeROM();

signals:
    void connected();
    void disconnected();

    void liveDataUpdated();
    void faultCodesUpdated();

    void ecuDetected(QString ecuName);

    void error(QString message);

private:
    ECUInterface *m_ecu;
};

#endif // ECUMANAGER_H
