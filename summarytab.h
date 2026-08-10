#ifndef SUMMARYTAB_H
#define SUMMARYTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "rosco.h"

/**
 * Onglet "toutes les mesures" : liste complete de toutes les valeurs
 * actuellement lues sur l'ECU (y compris les champs non documentes),
 * repartie sur trois tableaux cote a cote pour tenir a l'ecran. Les
 * lignes ayant une info-bulle explicative sont reperables par une
 * icone (bulle) devant leur nom.
 *
 * Cet onglet a ete reconstruit integralement par le code : dans le
 * fichier .ui d'origine, tout son contenu avait ete laisse commente
 * par l'auteur du logiciel.
 */
class SummaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryTab(QWidget *parent = 0);
    void updateData(mems_data *data);

private:
    int addRow(const QString &label);
    void setValue(int row, const QString &text);
    void setTooltip(int row, const QString &text);

    QTableWidget *m_table0;
    QTableWidget *m_table1;
    QTableWidget *m_table2;
    int m_rowCount;
    int m_rowsPerTable;

    int m_rowEngineRpm, m_rowCoolantTemp, m_rowAmbientTemp, m_rowIntakeAirTemp, m_rowFuelTemp,
        m_rowMapKpa, m_rowBatteryVoltage, m_rowThrottlePot, m_rowIdleSwitch, m_rowUk1,
        m_rowParkNeutralSwitch, m_rowFaultCodes, m_rowIdleSetPoint, m_rowIdleHot, m_rowUk2,
        m_rowIacPosition, m_rowIdleError, m_rowIgnitionAdvanceOffset, m_rowIgnitionAdvance,
        m_rowCoilTime, m_rowUk3, m_rowUk4, m_rowUk5, m_rowIgnitionSwitch, m_rowThrottleAngle,
        m_rowUk6, m_rowAirFuelRatio, m_rowDtc2, m_rowLambdaVoltage, m_rowLambdaFrequency,
        m_rowLambdaDutycycle, m_rowLambdaStatus, m_rowClosedLoop, m_rowLongTermFuelTrim,
        m_rowShortTermFuelTrim, m_rowCarbonCanisterDutycycle, m_rowDtc3, m_rowIdleBasePos,
        m_rowUk7, m_rowDtc4, m_rowIgnitionAdvance2, m_rowIdleSpeedOffset, m_rowIdleError2,
        m_rowUk10, m_rowDtc5, m_rowUk11, m_rowUk12, m_rowUk13, m_rowUk14, m_rowUk15, m_rowUk16,
        m_rowUk1A, m_rowUk1B, m_rowUk1C, m_rowDtc0, m_rowDtc1;
};

#endif // SUMMARYTAB_H
