#ifndef ECUINTERFACE_H
#define ECUINTERFACE_H

#include <QObject>
#include <QString>

struct mems_data;

class ECUInterface : public QObject
{
    Q_OBJECT

public:

    explicit ECUInterface(QObject *parent = nullptr);
    virtual ~ECUInterface();

    // Connexion
    virtual bool connectECU() = 0;
    virtual void disconnectECU() = 0;
    virtual bool isConnected() const = 0;

    // Détection
    virtual bool detectECU() = 0;
    virtual QString ecuName() const = 0;
    virtual QString ecuVersion() const = 0;

    // Données temps réel
    virtual bool startPolling() = 0;
    virtual void stopPolling() = 0;
    virtual mems_data *liveData() = 0;

    // Défauts
    virtual bool readFaultCodes() = 0;
    virtual bool clearFaultCodes() = 0;

    // Actionneurs
    virtual bool fuelPump(bool enable) = 0;
    virtual bool purgeValve(bool enable) = 0;
    virtual bool o2Heater(bool enable) = 0;
    virtual bool coolingFan(int fan,bool enable) = 0;
    virtual bool moveIAC(int position) = 0;

    // Réinitialisations
    virtual bool resetECU() = 0;
    virtual bool resetAdaptations() = 0;

    // ROM
    virtual bool readROM() = 0;
    virtual bool writeROM() = 0;

signals:

    void connected();
    void disconnected();

    void dataReady();

    void communicationError(QString message);
};

#endif