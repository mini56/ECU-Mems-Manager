/*
 * Unit tests for the scr/ecu module: ECUFactory and the MEMS 1.2 / 1.3 / 1.6
 * ECU implementations. These classes had no test coverage previously.
 *
 * The librosco backend is replaced by the stub in librosco_stub.cpp, so the
 * tests can drive the return values of the hardware calls and confirm that the
 * ECU classes forward their arguments and guard against a missing connection.
 */

#include <QtTest/QtTest>
#include <memory>

#include "../scr/ecu/ecufactory.h"
#include "../scr/ecu/ecuinterface.h"
#include "../scr/ecu/mems12ecu.h"
#include "../scr/ecu/mems13ecu.h"
#include "../scr/ecu/mems16ecu.h"
#include "librosco_stub.h"

class TestEcu : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // ECUFactory
    void factory_creer_returnsMatchingType();
    void factory_creer_unknownReturnsNull();
    void factory_nom();
    void factory_nom_data();
    void factory_detecter();
    void factory_detecter_data();
    void factory_detecter_nullReturnsUnknown();

    // Common ECU identity / capabilities
    void ecu_identity();
    void ecu_identity_data();
    void ecu_romSupport();

    // Behaviour without an initialised connection
    void ecu_callsFailWhenNotInitialised();
    void ecu_callsFailWhenNotInitialised_data();

    // Behaviour once initialised (forwarding to librosco)
    void ecu_initialiser();
    void ecu_forwarding();
    void ecu_forwarding_data();
    void ecu_initialiserNullFails();

private:
    static ECUInterface *make(TypeECU type) { return ECUFactory::creer(type); }
};

void TestEcu::init()
{
    rosco_stub::reset();
}

// ---------------------------------------------------------------- ECUFactory

void TestEcu::factory_creer_returnsMatchingType()
{
    std::unique_ptr<ECUInterface> e12(ECUFactory::creer(ECU_MEMS12));
    std::unique_ptr<ECUInterface> e13(ECUFactory::creer(ECU_MEMS13));
    std::unique_ptr<ECUInterface> e16(ECUFactory::creer(ECU_MEMS16));

    QVERIFY(e12 != nullptr);
    QVERIFY(e13 != nullptr);
    QVERIFY(e16 != nullptr);

    QCOMPARE(e12->versionECU(), QString("1.2"));
    QCOMPARE(e13->versionECU(), QString("1.3"));
    QCOMPARE(e16->versionECU(), QString("1.6"));
}

void TestEcu::factory_creer_unknownReturnsNull()
{
    QCOMPARE(ECUFactory::creer(ECU_INCONNU), static_cast<ECUInterface *>(nullptr));
    QCOMPARE(ECUFactory::creer(static_cast<TypeECU>(999)),
             static_cast<ECUInterface *>(nullptr));
}

void TestEcu::factory_nom_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<QString>("expected");

    QTest::newRow("mems12") << int(ECU_MEMS12) << QString("MEMS 1.2");
    QTest::newRow("mems13") << int(ECU_MEMS13) << QString("MEMS 1.3");
    QTest::newRow("mems16") << int(ECU_MEMS16) << QString("MEMS 1.6");
    QTest::newRow("unknown") << int(ECU_INCONNU) << QString("Inconnu");
    QTest::newRow("out-of-range") << 999 << QString("Inconnu");
}

void TestEcu::factory_nom()
{
    QFETCH(int, type);
    QFETCH(QString, expected);
    QCOMPARE(ECUFactory::nom(static_cast<TypeECU>(type)), expected);
}

void TestEcu::factory_detecter_data()
{
    QTest::addColumn<int>("firstByte");
    QTest::addColumn<int>("expected");

    QTest::newRow("0x12 -> MEMS12") << 0x12 << int(ECU_MEMS12);
    QTest::newRow("0x13 -> MEMS13") << 0x13 << int(ECU_MEMS13);
    QTest::newRow("0x16 -> MEMS16") << 0x16 << int(ECU_MEMS16);
    QTest::newRow("0x00 -> unknown") << 0x00 << int(ECU_INCONNU);
    QTest::newRow("0xFF -> unknown") << 0xFF << int(ECU_INCONNU);
}

void TestEcu::factory_detecter()
{
    QFETCH(int, firstByte);
    QFETCH(int, expected);

    uint8_t id[4] = { static_cast<uint8_t>(firstByte), 0, 0, 0 };
    QCOMPARE(int(ECUFactory::detecter(id)), expected);
}

void TestEcu::factory_detecter_nullReturnsUnknown()
{
    QCOMPARE(int(ECUFactory::detecter(nullptr)), int(ECU_INCONNU));
}

// ------------------------------------------------------------ ECU identity

void TestEcu::ecu_identity_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<QString>("version");

    QTest::newRow("mems12") << int(ECU_MEMS12) << QString("1.2");
    QTest::newRow("mems13") << int(ECU_MEMS13) << QString("1.3");
    QTest::newRow("mems16") << int(ECU_MEMS16) << QString("1.6");
}

void TestEcu::ecu_identity()
{
    QFETCH(int, type);
    QFETCH(QString, version);

    std::unique_ptr<ECUInterface> ecu(make(static_cast<TypeECU>(type)));
    QVERIFY(ecu != nullptr);
    QCOMPARE(ecu->nomECU(), QString("Rover MEMS"));
    QCOMPARE(ecu->versionECU(), version);
}

void TestEcu::ecu_romSupport()
{
    // None of the current implementations support ROM read/write.
    std::unique_ptr<ECUInterface> e12(make(ECU_MEMS12));
    std::unique_ptr<ECUInterface> e13(make(ECU_MEMS13));
    std::unique_ptr<ECUInterface> e16(make(ECU_MEMS16));

    QCOMPARE(e12->supportLectureROM(), false);
    QCOMPARE(e12->supportEcritureROM(), false);
    QCOMPARE(e13->supportLectureROM(), false);
    QCOMPARE(e13->supportEcritureROM(), false);
    QCOMPARE(e16->supportLectureROM(), false);
    QCOMPARE(e16->supportEcritureROM(), false);
}

// ---------------------------------------------- Behaviour when not connected

void TestEcu::ecu_callsFailWhenNotInitialised_data()
{
    QTest::addColumn<int>("type");
    QTest::newRow("mems12") << int(ECU_MEMS12);
    QTest::newRow("mems13") << int(ECU_MEMS13);
    QTest::newRow("mems16") << int(ECU_MEMS16);
}

void TestEcu::ecu_callsFailWhenNotInitialised()
{
    QFETCH(int, type);
    std::unique_ptr<ECUInterface> ecu(make(static_cast<TypeECU>(type)));

    mems_data data;
    uint8_t buffer[4] = {0};

    // Without initialiser() every hardware call must fail early and must not
    // reach the (stubbed) librosco layer.
    QCOMPARE(ecu->lireDonnees(&data), false);
    QCOMPARE(ecu->lireIdentifiant(buffer), false);
    QCOMPARE(ecu->effacerDefauts(), false);
    QCOMPARE(ecu->resetAdaptations(), false);
    QCOMPARE(ecu->resetECU(), false);

    QCOMPARE(rosco_stub::lastInfo, static_cast<mems_info *>(nullptr));
}

// -------------------------------------------------- Behaviour once connected

void TestEcu::ecu_initialiser()
{
    std::unique_ptr<ECUInterface> ecu(make(ECU_MEMS16));
    mems_info info;
    QVERIFY(ecu->initialiser(&info));
}

void TestEcu::ecu_initialiserNullFails()
{
    std::unique_ptr<ECUInterface> ecu(make(ECU_MEMS16));
    QCOMPARE(ecu->initialiser(nullptr), false);

    // A failed initialisation must leave the ECU disconnected.
    mems_data data;
    QCOMPARE(ecu->lireDonnees(&data), false);
    QCOMPARE(rosco_stub::lastInfo, static_cast<mems_info *>(nullptr));
}

void TestEcu::ecu_forwarding_data()
{
    QTest::addColumn<int>("type");
    QTest::newRow("mems12") << int(ECU_MEMS12);
    QTest::newRow("mems13") << int(ECU_MEMS13);
    QTest::newRow("mems16") << int(ECU_MEMS16);
}

void TestEcu::ecu_forwarding()
{
    QFETCH(int, type);
    std::unique_ptr<ECUInterface> ecu(make(static_cast<TypeECU>(type)));

    mems_info info;
    QVERIFY(ecu->initialiser(&info));

    mems_data data;
    uint8_t buffer[4] = {0};

    // Return value is passed straight through from the librosco layer, and the
    // ECU forwards the same mems_info pointer / buffers it was initialised with.
    rosco_stub::readReturn = true;
    QCOMPARE(ecu->lireDonnees(&data), true);
    QCOMPARE(rosco_stub::lastInfo, &info);
    QCOMPARE(rosco_stub::lastData, &data);

    rosco_stub::readReturn = false;
    QCOMPARE(ecu->lireDonnees(&data), false);

    rosco_stub::initLinkReturn = true;
    QCOMPARE(ecu->lireIdentifiant(buffer), true);
    QCOMPARE(rosco_stub::lastBuffer, buffer);
    rosco_stub::initLinkReturn = false;
    QCOMPARE(ecu->lireIdentifiant(buffer), false);

    rosco_stub::clearFaultsReturn = true;
    QCOMPARE(ecu->effacerDefauts(), true);
    rosco_stub::clearFaultsReturn = false;
    QCOMPARE(ecu->effacerDefauts(), false);

    rosco_stub::resetAdjustmentsReturn = true;
    QCOMPARE(ecu->resetAdaptations(), true);
    rosco_stub::resetAdjustmentsReturn = false;
    QCOMPARE(ecu->resetAdaptations(), false);

    rosco_stub::resetECUReturn = true;
    QCOMPARE(ecu->resetECU(), true);
    rosco_stub::resetECUReturn = false;
    QCOMPARE(ecu->resetECU(), false);
}

QTEST_APPLESS_MAIN(TestEcu)
#include "test_ecu.moc"
