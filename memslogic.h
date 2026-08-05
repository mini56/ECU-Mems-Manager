// memslogic.h
#pragma once

#include <QObject>
#include <QThread>

class MEMSLogic : public QObject {
    Q_OBJECT

public:
    explicit MEMSLogic(QObject *parent = nullptr);
    ~MEMSLogic();

public slots:
    void connectToPort();
    void disconnectFromPort();
    void startDataStream();
    void stopDataStream();
    void clearFaults();
    void testFuelPump();
    void testPTCRelay();
    void testACRelay();

signals:
    // Ajoute ici les signaux si nécessaire
};
