#ifndef MEMS13INTERFACE_H
#define MEMS13INTERFACE_H

#include "ecuinterface.h"

class MEMSInterface;

class MEMS13Interface : public ECUInterface
{
    Q_OBJECT

public:

    explicit MEMS13Interface(QObject *parent = nullptr);
    ~MEMS13Interface();

    bool connectECU() override;
    void disconnectECU() override;
    bool isConnected() const override;

    bool detectECU() override;

    QString ecuName() const override;
    QString ecuVersion() const override;

    bool startPolling() override;
    void stopPolling() override;

    mems_data *liveData() override;

    bool readFaultCodes() override;
    bool clearFaultCodes() override;

    bool fuelPump(bool enable) override;
    bool purgeValve(bool enable) override;
    bool o2Heater(bool enable) override;
    bool coolingFan(int fan,bool enable) override;
    bool moveIAC(int position) override;

    bool resetECU() override;
    bool resetAdaptations() override;

    bool readROM() override;
    bool writeROM() override;

private:

    MEMSInterface *m_mems;
};

#endif