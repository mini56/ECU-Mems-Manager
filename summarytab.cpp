#include "summarytab.h"
#include <QHeaderView>

static const QString TOOLTIP_ICON = QString::fromUtf8("\xF0\x9F\x92\xAC ");

SummaryTab::SummaryTab(QWidget *parent) : QWidget(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  m_rowCount = 0;
  m_rowsPerTable = 19;

  m_table0 = new QTableWidget(0, 2, this);
  m_table1 = new QTableWidget(0, 2, this);
  m_table2 = new QTableWidget(0, 2, this);

  QTableWidget *tables[3] = { m_table0, m_table1, m_table2 };
  for (int i = 0; i < 3; i++)
  {
    tables[i]->setHorizontalHeaderLabels(QStringList() << "Paramètre" << "Valeur");
    tables[i]->horizontalHeader()->setStretchLastSection(true);
    tables[i]->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tables[i]->verticalHeader()->setVisible(false);
    tables[i]->setEditTriggers(QTableWidget::NoEditTriggers);
    tables[i]->setSelectionMode(QTableWidget::NoSelection);
    tables[i]->setAlternatingRowColors(true);
    layout->addWidget(tables[i]);
  }

  m_rowEngineRpm             = addRow("Régime moteur (tr/min)");
  m_rowCoolantTemp           = addRow("Température liquide refroid. (°C)");
  m_rowAmbientTemp           = addRow("Température ambiante (°C)");
  m_rowIntakeAirTemp         = addRow("Température air admission (°C)");
  m_rowFuelTemp              = addRow("Température carburant (°C)");
  m_rowMapKpa                = addRow("Pression collecteur (kPa)");
  m_rowBatteryVoltage        = addRow("Tension batterie (V)");
  m_rowThrottlePot           = addRow("Position papillon (%)");
  m_rowIdleSwitch            = addRow("Contact ralenti");
  m_rowUk1                   = addRow("Non documenté 1");
  m_rowParkNeutralSwitch     = addRow("Contact point mort");
  m_rowFaultCodes            = addRow("Codes défaut (brut)");
  m_rowIdleSetPoint          = addRow("Consigne de ralenti");
  m_rowIdleHot               = addRow("Ralenti chaud");
  m_rowUk2                   = addRow("Non documenté 2");
  m_rowIacPosition           = addRow("Position moteur pas-à-pas (%)");
  m_rowIdleError             = addRow("Erreur de ralenti");
  m_rowIgnitionAdvanceOffset = addRow("Décalage d'avance");
  m_rowIgnitionAdvance       = addRow("Avance à l'allumage (°)");
  m_rowCoilTime              = addRow("Temps bobine (ms)");
  m_rowUk3                   = addRow("Non documenté 3");
  m_rowUk4                   = addRow("Non documenté 4");
  m_rowUk5                   = addRow("Non documenté 5");
  m_rowIgnitionSwitch        = addRow("Contact allumage");
  m_rowThrottleAngle         = addRow("Angle papillon (°)");
  m_rowUk6                   = addRow("Non documenté 6");
  m_rowAirFuelRatio          = addRow("Ratio air/carburant");
  m_rowDtc2                  = addRow("DTC 2");
  m_rowLambdaVoltage         = addRow("Tension sonde lambda (mV)");
  m_rowLambdaFrequency       = addRow("Fréquence lambda");
  m_rowLambdaDutycycle       = addRow("Cycle lambda (%)");
  m_rowLambdaStatus          = addRow("État lambda");
  m_rowClosedLoop            = addRow("Boucle fermée");
  m_rowLongTermFuelTrim      = addRow("Correction carburant long terme (%)");
  m_rowShortTermFuelTrim     = addRow("Correction carburant court terme (%)");
  m_rowCarbonCanisterDutycycle = addRow("Cycle purge canister (%)");
  m_rowDtc3                  = addRow("DTC 3");
  m_rowIdleBasePos           = addRow("Position de base ralenti");
  m_rowUk7                   = addRow("Non documenté 7");
  m_rowDtc4                  = addRow("DTC 4");
  m_rowIgnitionAdvance2      = addRow("Avance à l'allumage 2");
  m_rowIdleSpeedOffset       = addRow("Décalage régime de ralenti");
  m_rowIdleError2            = addRow("Erreur de ralenti 2");
  m_rowUk10                  = addRow("Non documenté 10");
  m_rowDtc5                  = addRow("DTC 5");
  m_rowUk11                  = addRow("Non documenté 11");
  m_rowUk12                  = addRow("Non documenté 12");
  m_rowUk13                  = addRow("Non documenté 13");
  m_rowUk14                  = addRow("Non documenté 14");
  m_rowUk15                  = addRow("Non documenté 15");
  m_rowUk16                  = addRow("Non documenté 16");
  m_rowUk1A                  = addRow("Non documenté 1A");
  m_rowUk1B                  = addRow("Non documenté 1B");
  m_rowUk1C                  = addRow("Non documenté 1C");
  m_rowDtc0                  = addRow("DTC 0");
  m_rowDtc1                  = addRow("DTC 1");

  // Info-bulles explicatives, reprises des textes déjà présents ailleurs
  // dans le logiciel (onglets Aperçu et Réglages). Les lignes concernées
  // sont repérables par l'icône en forme de bulle devant leur nom.
  setTooltip(m_rowCoolantTemp,
    "Affiche la température du liquide de refroidissement mesurée par l'ECU. Si le capteur est en circuit ouvert, "
    "une valeur par défaut d'environ 60°C sera affichée. Pendant la montée en température, la valeur doit augmenter "
    "progressivement de la température ambiante à environ 90°C. Un capteur défectueux peut causer un démarrage "
    "difficile, un ralenti trop élevé, une surconsommation et des ventilateurs tournant en continu.");
  setTooltip(m_rowAmbientTemp,
    "Affiche la température mesurée par l'ECU via la sonde de température d'air ambiant (si équipée). Si le capteur "
    "est en circuit ouvert, une valeur par défaut fixe sera affichée. Ce capteur sert à l'ECU pour mesurer la "
    "température dans le compartiment moteur, généralement pour piloter un ventilateur supplémentaire. Sur les ECU "
    "MEMS qui ne supportent pas ce capteur, la valeur affichera N/S.");
  setTooltip(m_rowIntakeAirTemp,
    "Affiche la température mesurée par l'ECU via la sonde de température d'air admission (si équipée). Si le "
    "capteur est en circuit ouvert, une valeur par défaut fixe sera affichée. Cette température sert à l'ECU pour "
    "retarder l'allumage afin d'éviter le cliquetis et ajuster la richesse à chaud.");
  setTooltip(m_rowMapKpa,
    "Affiche la pression mesurée par le capteur de pression d'air interne du MEMS. Cette valeur doit indiquer la "
    "pression atmosphérique de 100 kPa moteur à l'arrêt, et une valeur plus basse entre 25 et 40 kPa au ralenti. Des "
    "valeurs très élevées peuvent indiquer un problème du capteur interne au MEMS, ou plus probablement une durite "
    "de dépression bouchée ou débranchée.");
  setTooltip(m_rowBatteryVoltage,
    "Affiche la tension d'alimentation du véhicule mesurée en interne par l'ECU. De grands écarts sur cette mesure "
    "peuvent entraîner des difficultés de démarrage et des erreurs de CO au ralenti.");
  setTooltip(m_rowThrottlePot,
    "Affiche la position du papillon obtenue par l'ECU MEMS via le potentiomètre de papillon. Cette valeur doit "
    "passer d'une valeur basse à une valeur haute lorsque la pédale d'accélérateur est enfoncée.");
  setTooltip(m_rowIdleSwitch,
    "Affiche l'état du contacteur papillon (si équipé). Si le contact indique 'ON' alors que le papillon est fermé, "
    "le véhicule ne tournera pas correctement au ralenti et la position papillon fermé devra peut-être être "
    "réinitialisée (enfoncer/relâcher complètement l'accélérateur 5 fois en 10 secondes après la mise du contact, "
    "puis attendre 20 secondes).");
  setTooltip(m_rowParkNeutralSwitch,
    "Affiche l'état du contacteur point mort/parking mesuré par l'ECU MEMS. Ce contacteur améliore la régulation du "
    "ralenti sur les véhicules à boîte automatique ou CVT. Ne fonctionne pas sur les véhicules à boîte manuelle.");
  setTooltip(m_rowIdleSpeedOffset,
    "Si un décalage de service du ralenti a été configuré dans cet ECU, l'écart par rapport au ralenti normal est "
    "affiché ici. Normalement, seules deux valeurs sont possibles : 0 tr/min ou 49 tr/min.");
  setTooltip(m_rowIacPosition,
    "C'est le nombre de pas du moteur pas-à-pas depuis la fermeture complète (0) que l'ECU a appris comme position "
    "correcte pour maintenir le régime de ralenti visé, moteur bien chaud. Si cette valeur sort de la plage 10-50 "
    "pas, c'est le signe possible d'un défaut ou d'un mauvais réglage.");
  setTooltip(m_rowIdleError,
    "C'est l'écart actuel entre le régime de ralenti visé par l'ECU MEMS et le régime moteur réel. Une valeur "
    "supérieure à 100 tr/min indique que l'ECU ne maîtrise pas le ralenti, signe possible d'un défaut.");
  setTooltip(m_rowIgnitionAdvanceOffset,
    "Affiche le décalage d'avance de service actuellement utilisé par l'ECU MEMS. C'est un réglage spécial pour les "
    "pays utilisant un carburant à faible indice d'octane, configurable dans l'onglet Réglages.");
  setTooltip(m_rowCoilTime,
    "C'est le temps de charge de la bobine d'allumage jusqu'à son courant nominal, mesuré par l'ECU MEMS. Avec une "
    "tension batterie d'environ 14V, cette valeur doit être d'environ 2-3ms. Une valeur élevée peut indiquer un "
    "problème du circuit primaire de la bobine.");
  setTooltip(m_rowThrottleAngle,
    "Affiche la position du papillon obtenue par l'ECU MEMS via le potentiomètre de papillon. Cette valeur doit "
    "passer d'une valeur basse à une valeur haute lorsque la pédale d'accélérateur est enfoncée.");
  setTooltip(m_rowLambdaVoltage,
    "Affiche la tension de la sonde à oxygène lue par l'ECU MEMS. Moteur bien chaud, en conditions normales de "
    "ralenti ou de conduite, cette tension oscille rapidement entre 0,0-0,2V et 0,7-1,0V.");
  setTooltip(m_rowLambdaStatus,
    "Affiche l'état du diagnostic interne MEMS sur la sonde à oxygène et son câblage. La valeur ON indique aucun "
    "défaut, OFF indique un problème possible.");
  setTooltip(m_rowClosedLoop,
    "Affiche si l'injection est régulée par rétroaction des sondes à oxygène. Sur un véhicule bien chaud, l'état de "
    "boucle doit indiquer boucle fermée dans la plupart des conditions de conduite et de ralenti.");
  setTooltip(m_rowLongTermFuelTrim,
    "Affiche la correction carburant actuelle par rétroaction, en pourcentage par rapport à la valeur cartographiée. "
    "Des valeurs élevées (ex. 120%) indiquent une compensation d'un mélange trop pauvre, des valeurs basses (ex. "
    "80%) une compensation d'un mélange trop riche.");
  setTooltip(m_rowShortTermFuelTrim,
    "La correction carburant court terme n'est active qu'en boucle fermée. Des valeurs jusqu'à 10% sont normales, "
    "occasionnellement jusqu'à 15%. Des valeurs supérieures à 15% sont anormales.");
  setTooltip(m_rowIgnitionAdvance2,
    "Fonction intégrée à l'ECU MEMS pour pallier certaines situations pendant la vie du véhicule (carburant à "
    "faible indice d'octane, usure moteur), en avançant légèrement l'allumage au ralenti.");
}

int SummaryTab::addRow(const QString &label)
{
  int tableIdx = m_rowCount / m_rowsPerTable;
  if (tableIdx > 2) tableIdx = 2;
  QTableWidget *table = (tableIdx == 0) ? m_table0 : (tableIdx == 1) ? m_table1 : m_table2;

  int row = table->rowCount();
  table->insertRow(row);
  QTableWidgetItem *labelItem = new QTableWidgetItem(label);
  labelItem->setFlags(labelItem->flags() & ~Qt::ItemIsEditable);
  table->setItem(row, 0, labelItem);

  QTableWidgetItem *valueItem = new QTableWidgetItem("--");
  valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
  valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
  table->setItem(row, 1, valueItem);

  int index = m_rowCount;
  m_rowCount++;
  return index;
}

void SummaryTab::setTooltip(int globalRow, const QString &text)
{
  if (globalRow < 0) return;
  int tableIdx = globalRow / m_rowsPerTable;
  if (tableIdx > 2) tableIdx = 2;
  int localRow = globalRow % m_rowsPerTable;
  QTableWidget *table = (tableIdx == 0) ? m_table0 : (tableIdx == 1) ? m_table1 : m_table2;
  if (localRow >= 0 && localRow < table->rowCount())
  {
    table->item(localRow, 0)->setToolTip(text);
    table->item(localRow, 1)->setToolTip(text);
    // Icône "bulle" pour signaler visuellement qu'une info-bulle existe
    table->item(localRow, 0)->setText(TOOLTIP_ICON + table->item(localRow, 0)->text());
  }
}

void SummaryTab::setValue(int globalRow, const QString &text)
{
  if (globalRow < 0) return;
  int tableIdx = globalRow / m_rowsPerTable;
  if (tableIdx > 2) tableIdx = 2;
  int localRow = globalRow % m_rowsPerTable;
  QTableWidget *table = (tableIdx == 0) ? m_table0 : (tableIdx == 1) ? m_table1 : m_table2;
  if (localRow >= 0 && localRow < table->rowCount())
  {
    table->item(localRow, 1)->setText(text);
  }
}

void SummaryTab::updateData(mems_data *data)
{
  if (!data) return;

  setValue(m_rowEngineRpm, QString::number(data->engine_rpm));
  setValue(m_rowCoolantTemp, QString::number(data->coolant_temp));
  setValue(m_rowAmbientTemp, QString::number(data->ambient_temp));
  setValue(m_rowIntakeAirTemp, QString::number(data->intake_air_temp));
  setValue(m_rowFuelTemp, QString::number(data->fuel_temp));
  setValue(m_rowMapKpa, QString::number(data->map_kpa));
  setValue(m_rowBatteryVoltage, QString::number(data->battery_voltage / 10.0, 'f', 1));
  setValue(m_rowThrottlePot, QString::number(data->throttle_pot));
  setValue(m_rowIdleSwitch, data->idle_switch ? "Activé" : "Désactivé");
  setValue(m_rowUk1, QString::number(data->uk1));
  setValue(m_rowParkNeutralSwitch, data->park_neutral_switch ? "Activé" : "Désactivé");
  setValue(m_rowFaultCodes, QString::number(data->fault_codes));
  setValue(m_rowIdleSetPoint, QString::number(data->idle_set_point));
  setValue(m_rowIdleHot, QString::number(data->idle_hot));
  setValue(m_rowUk2, QString::number(data->uk2));
  setValue(m_rowIacPosition, QString::number(data->iac_position));
  setValue(m_rowIdleError, QString::number(data->idle_error));
  setValue(m_rowIgnitionAdvanceOffset, QString::number(data->ignition_advance_offset));
  setValue(m_rowIgnitionAdvance, QString::number(data->ignition_advance));
  setValue(m_rowCoilTime, QString::number(data->coil_time));
  setValue(m_rowUk3, QString::number(data->uk3));
  setValue(m_rowUk4, QString::number(data->uk4));
  setValue(m_rowUk5, QString::number(data->uk5));
  setValue(m_rowIgnitionSwitch, data->ignition_switch ? "Activé" : "Désactivé");
  setValue(m_rowThrottleAngle, QString::number(data->throttle_angle));
  setValue(m_rowUk6, QString::number(data->uk6));
  setValue(m_rowAirFuelRatio, QString::number(data->air_fuel_ratio));
  setValue(m_rowDtc2, QString::number(data->dtc2));
  setValue(m_rowLambdaVoltage, QString::number(data->lambda_voltage));
  setValue(m_rowLambdaFrequency, QString::number(data->lambda_sensor_frequency));
  setValue(m_rowLambdaDutycycle, QString::number(data->lambda_sensor_dutycycle));
  setValue(m_rowLambdaStatus, data->lambda_sensor_status ? "OK" : "Défaut");
  setValue(m_rowClosedLoop, data->closed_loop ? "Fermée" : "Ouverte");
  setValue(m_rowLongTermFuelTrim, QString::number(data->long_term_fuel_trim));
  setValue(m_rowShortTermFuelTrim, QString::number(data->short_term_fuel_trim));
  setValue(m_rowCarbonCanisterDutycycle, QString::number(data->carbon_canister_dutycycle));
  setValue(m_rowDtc3, QString::number(data->dtc3));
  setValue(m_rowIdleBasePos, QString::number(data->idle_base_pos));
  setValue(m_rowUk7, QString::number(data->uk7));
  setValue(m_rowDtc4, QString::number(data->dtc4));
  setValue(m_rowIgnitionAdvance2, QString::number(data->ignition_advance2));
  setValue(m_rowIdleSpeedOffset, QString::number(data->idle_speed_offset));
  setValue(m_rowIdleError2, QString::number(data->idle_error2));
  setValue(m_rowUk10, QString::number(data->uk10));
  setValue(m_rowDtc5, QString::number(data->dtc5));
  setValue(m_rowUk11, QString::number(data->uk11));
  setValue(m_rowUk12, QString::number(data->uk12));
  setValue(m_rowUk13, QString::number(data->uk13));
  setValue(m_rowUk14, QString::number(data->uk14));
  setValue(m_rowUk15, QString::number(data->uk15));
  setValue(m_rowUk16, QString::number(data->uk16));
  setValue(m_rowUk1A, QString::number(data->uk1A));
  setValue(m_rowUk1B, QString::number(data->uk1B));
  setValue(m_rowUk1C, QString::number(data->uk1C));
  setValue(m_rowDtc0, QString::number(data->dtc0));
  setValue(m_rowDtc1, QString::number(data->dtc1));
}
