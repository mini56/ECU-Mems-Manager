#include "analysistab.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QTime>
#include <QMessageBox>
#include <QScrollBar>
#include <QFontMetrics>
#include <QMap>

// Correspondance entre les noms techniques des colonnes du CSV et des
// libellés clairs en français, affichés dans la liste des voies.
static QString friendlyColumnName(const QString &rawName)
{
  static QMap<QString, QString> names;
  if (names.isEmpty())
  {
    names["80x01-02_engine-rpm"] = "Régime moteur (tr/min)";
    names["80x03_coolant_temp"] = "Température liquide refroid. (°C)";
    names["80x04_ambient_temp"] = "Température ambiante (°C)";
    names["80x05_intake_air_temp"] = "Température air admission (°C)";
    names["80x06_fuel_temp"] = "Température carburant (°C)";
    names["80x07_map_kpa"] = "Pression collecteur (kPa)";
    names["80x08_battery_voltage"] = "Tension batterie (V)";
    names["80x09_throttle_pot"] = "Position papillon (%)";
    names["80x0A_idle_switch"] = "Contact ralenti";
    names["80x0C_park_neutral_switch"] = "Contact point mort";
    names["80x0D-0E_fault_codes"] = "Codes défaut (brut)";
    names["80x0F_idle_set_point"] = "Consigne de ralenti";
    names["80x10_idle_hot"] = "Ralenti chaud";
    names["80x12_iac_position"] = "Position moteur pas-à-pas (%)";
    names["80x13-14_idle_error"] = "Erreur de ralenti";
    names["80x15_ignition_advance_offset"] = "Décalage d'avance";
    names["80x16_ignition_advance"] = "Avance à l'allumage (°)";
    names["80x17-18_coil_time"] = "Temps bobine (ms)";
    names["80x19_crankshaft_position_sensor"] = "Capteur position vilebrequin";
    names["7dx01_ignition_switch"] = "Contact allumage";
    names["7dx02_throttle_angle"] = "Angle papillon (°)";
    names["7dx04_air_fuel_ratio"] = "Ratio air/carburant";
    names["7dx05_dtc2"] = "DTC 2";
    names["7dx06_lambda_voltage"] = "Tension sonde lambda (mV)";
    names["7dx07_lambda_sensor_frequency"] = "Fréquence lambda";
    names["7dx08_lambda_sensor_dutycycle"] = "Cycle lambda (%)";
    names["7dx09_lambda_sensor_status"] = "État lambda";
    names["7dx0A_closed_loop"] = "Boucle fermée";
    names["7dx0B_long_term_fuel_trim"] = "Correction carburant long terme (%)";
    names["7dx0C_short_term_fuel_trim"] = "Correction carburant court terme (%)";
    names["7dx0D_carbon_canister_dutycycle"] = "Cycle purge canister (%)";
    names["7dx0E_dtc3"] = "DTC 3";
    names["7dx0F_idle_base_pos"] = "Position de base ralenti";
    names["7dx11_dtc4"] = "DTC 4";
    names["7dx12_ignition_advance2"] = "Avance à l'allumage 2";
    names["7dx13_idle_speed_offset"] = "Décalage régime de ralenti";
    names["7dx14_idle_error2"] = "Erreur de ralenti 2";
    names["7dx16_dtc5"] = "DTC 5";
  }

  if (names.contains(rawName))
  {
    return names[rawName];
  }
  if (rawName.contains("_uk") || rawName.contains("uk1") || rawName.contains("uk2"))
  {
    return "Non documenté (" + rawName + ")";
  }
  return rawName;
}

//=============================================================================
// SingleChartWidget : un graphique dedie a une seule voie
//=============================================================================

SingleChartWidget::SingleChartWidget(const QString &name, const QColor &color, QWidget *parent)
  : QWidget(parent), m_name(name), m_color(color), m_hasCursor(false), m_cursorX(0)
{
  setMinimumHeight(190);
  setMaximumHeight(190);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMouseTracking(true);
  setAutoFillBackground(true);
  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor("#ffffff"));
  setPalette(pal);
}

void SingleChartWidget::setData(const QVector<double> &time, const QVector<double> &values)
{
  m_time = time;
  m_values = values;
  update();
}

void SingleChartWidget::mouseMoveEvent(QMouseEvent *event)
{
  m_hasCursor = true;
  m_cursorX = event->pos().x();
  update();
}

void SingleChartWidget::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  m_hasCursor = false;
  update();
}

void SingleChartWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Titre de la courbe
  painter.setPen(QColor("#1f2430"));
  QFont titleFont = painter.font();
  titleFont.setBold(true);
  titleFont.setPointSize(9);
  painter.setFont(titleFont);
  painter.drawText(QRect(0, 4, width(), 18), Qt::AlignHCenter, m_name);

  const int leftMargin = 55;
  const int rightMargin = 10;
  const int topMargin = 26;
  const int bottomMargin = 20;
  QRect plotRect(leftMargin, topMargin, width() - leftMargin - rightMargin,
                  height() - topMargin - bottomMargin);

  painter.setPen(QPen(QColor("#e5e8ee"), 1));
  painter.setBrush(QColor("#fbfbfd"));
  painter.drawRect(plotRect);

  if (m_time.count() < 2 || m_values.count() < 2)
  {
    painter.setPen(QColor("#9aa0ab"));
    painter.drawText(plotRect, Qt::AlignCenter, "Pas de données");
    return;
  }

  double tMin = m_time.first();
  double tMax = m_time.last();
  double tSpan = (tMax - tMin) > 0.0001 ? (tMax - tMin) : 1.0;

  double vMin = m_values[0];
  double vMax = m_values[0];
  for (int i = 1; i < m_values.count(); i++)
  {
    if (m_values[i] < vMin) vMin = m_values[i];
    if (m_values[i] > vMax) vMax = m_values[i];
  }
  if (vMax - vMin < 0.0001)
  {
    vMax += 1.0;
    vMin -= 1.0;
  }
  double margin = (vMax - vMin) * 0.08;
  vMax += margin;
  vMin -= margin;
  double vSpan = vMax - vMin;

  // Grille horizontale + graduation de l'échelle réelle à gauche
  painter.setPen(QColor("#eef0f4"));
  QFont axisFont = painter.font();
  axisFont.setBold(false);
  axisFont.setPointSize(7);
  painter.setFont(axisFont);
  for (int i = 0; i <= 4; i++)
  {
    int y = plotRect.top() + (plotRect.height() * i) / 4;
    painter.drawLine(plotRect.left(), y, plotRect.right(), y);
    double val = vMax - (vSpan * i) / 4.0;
    painter.setPen(QColor("#6b7280"));
    painter.drawText(QRect(0, y - 8, leftMargin - 6, 16), Qt::AlignRight | Qt::AlignVCenter,
                      QString::number(val, 'f', val < 10 ? 1 : 0));
    painter.setPen(QColor("#eef0f4"));
  }

  // Tracé de la courbe
  painter.setPen(QPen(m_color, 2));
  QPolygonF poly;
  int n = qMin(m_time.count(), m_values.count());
  for (int i = 0; i < n; i++)
  {
    double xFrac = (m_time[i] - tMin) / tSpan;
    double yFrac = (m_values[i] - vMin) / vSpan;
    double x = plotRect.left() + xFrac * plotRect.width();
    double y = plotRect.bottom() - yFrac * plotRect.height();
    poly << QPointF(x, y);
  }
  painter.drawPolyline(poly);

  // Axe du temps en bas
  painter.setPen(QColor("#6b7280"));
  for (int i = 0; i <= 5; i++)
  {
    double t = tMin + (tSpan * i) / 5.0;
    int x = plotRect.left() + (plotRect.width() * i) / 5;
    painter.drawText(QRect(x - 30, plotRect.bottom() + 2, 60, 16), Qt::AlignCenter,
                      QString("%1 s").arg(t - tMin, 0, 'f', 0));
  }

  // Curseur de survol avec valeur instantanée
  if (m_hasCursor && m_cursorX >= plotRect.left() && m_cursorX <= plotRect.right())
  {
    painter.setPen(QPen(QColor("#9aa0ab"), 1, Qt::DashLine));
    painter.drawLine(m_cursorX, plotRect.top(), m_cursorX, plotRect.bottom());

    double xFrac = double(m_cursorX - plotRect.left()) / double(plotRect.width());
    double tAtCursor = tMin + xFrac * tSpan;
    int idx = 0;
    double best = 1e18;
    for (int i = 0; i < m_time.count(); i++)
    {
      double d = qAbs(m_time[i] - tAtCursor);
      if (d < best) { best = d; idx = i; }
    }
    if (idx < m_values.count())
    {
      QString label = QString::number(m_values[idx], 'f', 1);
      QFontMetrics fm(axisFont);
      int textW = fm.horizontalAdvance(label) + 10;
      int boxX = m_cursorX + 6;
      if (boxX + textW > plotRect.right()) boxX = m_cursorX - 6 - textW;

      painter.setPen(Qt::NoPen);
      painter.setBrush(QColor(255, 255, 255, 230));
      painter.drawRect(boxX, plotRect.top() + 4, textW, 16);
      painter.setPen(m_color);
      painter.drawText(QRect(boxX + 5, plotRect.top() + 4, textW, 16),
                        Qt::AlignVCenter | Qt::AlignLeft, label);
    }
  }
}

//=============================================================================
// ChartWidget : mode superpose (toutes les voies sur un seul graphique)
//=============================================================================

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent), m_hasCursor(false), m_cursorX(0)
{
  setMinimumHeight(150);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMouseTracking(true);
  setAutoFillBackground(true);
  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor("#0e1420"));
  setPalette(pal);
}

void ChartWidget::setData(const QVector<double> &time,
                           const QVector<QVector<double> > &series,
                           const QStringList &names,
                           const QVector<QColor> &colors)
{
  m_time = time;
  m_series = series;
  m_names = names;
  m_colors = colors;
  m_visible = QVector<bool>(names.count(), false);
  update();
}

void ChartWidget::setVisible(int index, bool vis)
{
  if (index >= 0 && index < m_visible.count())
  {
    m_visible[index] = vis;
    update();
  }
}

void ChartWidget::clearData()
{
  m_time.clear();
  m_series.clear();
  m_names.clear();
  m_colors.clear();
  m_visible.clear();
  update();
}

void ChartWidget::mouseMoveEvent(QMouseEvent *event)
{
  m_hasCursor = true;
  m_cursorX = event->pos().x();
  update();
}

void ChartWidget::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  m_hasCursor = false;
  update();
}

void ChartWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const int leftMargin = 8;
  const int rightMargin = 8;
  const int topMargin = 10;
  const int bottomMargin = 26;
  QRect plotRect(leftMargin, topMargin, width() - leftMargin - rightMargin,
                  height() - topMargin - bottomMargin);

  painter.setPen(QPen(QColor("#233047"), 1));
  for (int i = 0; i <= 4; i++)
  {
    int y = plotRect.top() + (plotRect.height() * i) / 4;
    painter.drawLine(plotRect.left(), y, plotRect.right(), y);
  }
  for (int i = 0; i <= 10; i++)
  {
    int x = plotRect.left() + (plotRect.width() * i) / 10;
    painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
  }

  if (m_time.count() < 2)
  {
    painter.setPen(QColor("#8a93a6"));
    painter.drawText(rect(), Qt::AlignCenter,
                      "Chargez un fichier CSV et cochez une ou plusieurs voies ci-contre");
    return;
  }

  double tMin = m_time.first();
  double tMax = m_time.last();
  double tSpan = (tMax - tMin) > 0.0001 ? (tMax - tMin) : 1.0;

  int anyVisible = 0;
  for (int s = 0; s < m_series.count(); s++)
  {
    if (s >= m_visible.count() || !m_visible[s]) continue;
    anyVisible++;
    const QVector<double> &values = m_series[s];
    if (values.isEmpty()) continue;

    double vMin = values[0];
    double vMax = values[0];
    for (int i = 1; i < values.count(); i++)
    {
      if (values[i] < vMin) vMin = values[i];
      if (values[i] > vMax) vMax = values[i];
    }
    double vSpan = (vMax - vMin) > 0.0001 ? (vMax - vMin) : 1.0;

    painter.setPen(QPen(m_colors[s % m_colors.count()], 2));
    QPolygonF poly;
    int n = qMin(m_time.count(), values.count());
    for (int i = 0; i < n; i++)
    {
      double xFrac = (m_time[i] - tMin) / tSpan;
      double yFrac = (values[i] - vMin) / vSpan;
      poly << QPointF(plotRect.left() + xFrac * plotRect.width(),
                       plotRect.bottom() - yFrac * plotRect.height());
    }
    painter.drawPolyline(poly);
  }

  if (anyVisible == 0)
  {
    painter.setPen(QColor("#8a93a6"));
    painter.drawText(rect(), Qt::AlignCenter,
                      "Cochez une ou plusieurs voies dans la liste à gauche pour les afficher");
  }

  painter.setPen(QColor("#8a93a6"));
  for (int i = 0; i <= 5; i++)
  {
    double t = tMin + (tSpan * i) / 5.0;
    int x = plotRect.left() + (plotRect.width() * i) / 5;
    painter.drawText(QRect(x - 30, plotRect.bottom() + 4, 60, 18), Qt::AlignCenter,
                      QString("%1 s").arg(t - tMin, 0, 'f', 0));
  }

  if (m_hasCursor && m_cursorX >= plotRect.left() && m_cursorX <= plotRect.right() && anyVisible > 0)
  {
    painter.setPen(QPen(QColor("#e6e8ee"), 1, Qt::DashLine));
    painter.drawLine(m_cursorX, plotRect.top(), m_cursorX, plotRect.bottom());

    double xFrac = double(m_cursorX - plotRect.left()) / double(plotRect.width());
    double tAtCursor = tMin + xFrac * tSpan;
    int idx = 0;
    double best = 1e18;
    for (int i = 0; i < m_time.count(); i++)
    {
      double d = qAbs(m_time[i] - tAtCursor);
      if (d < best) { best = d; idx = i; }
    }

    int legendY = plotRect.top() + 6;
    QFont f = painter.font();
    f.setPointSize(8);
    painter.setFont(f);
    for (int s = 0; s < m_series.count(); s++)
    {
      if (s >= m_visible.count() || !m_visible[s]) continue;
      if (idx >= m_series[s].count()) continue;
      QString label = QString("%1 : %2").arg(m_names[s]).arg(m_series[s][idx], 0, 'f', 1);
      QFontMetrics fm(f);
      int textW = fm.horizontalAdvance(label) + 10;
      int boxX = m_cursorX + 8;
      if (boxX + textW > plotRect.right()) boxX = m_cursorX - 8 - textW;
      painter.setPen(Qt::NoPen);
      painter.setBrush(QColor(14, 20, 32, 210));
      painter.drawRect(boxX, legendY, textW, 16);
      painter.setPen(m_colors[s % m_colors.count()]);
      painter.drawText(QRect(boxX + 5, legendY, textW, 16), Qt::AlignVCenter | Qt::AlignLeft, label);
      legendY += 18;
    }
  }
}

//=============================================================================
// AnalysisTab
//=============================================================================

AnalysisTab::AnalysisTab(QWidget *parent) : QWidget(parent), m_overlayMode(false)
{
  m_colors << QColor("#4fc3f7") << QColor("#ff7043") << QColor("#66bb6a")
           << QColor("#ffca28") << QColor("#ba68c8") << QColor("#26c6da")
           << QColor("#ef5350") << QColor("#9ccc65") << QColor("#ec407a")
           << QColor("#7e57c2") << QColor("#8d6e63") << QColor("#5c6bc0");

  QVBoxLayout *rootLayout = new QVBoxLayout(this);

  QHBoxLayout *mainLayout = new QHBoxLayout();
  rootLayout->addLayout(mainLayout, 1);

  // --- Colonne de gauche : chargement + liste des voies (défilement propre) ---
  QWidget *leftPanel = new QWidget(this);
  leftPanel->setMaximumWidth(320);
  leftPanel->setMinimumWidth(300);
  QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

  m_loadButton = new QPushButton("Charger un fichier CSV...", leftPanel);
  connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadFileClicked()));
  leftLayout->addWidget(m_loadButton);

  m_fileLabel = new QLabel("Aucun fichier chargé", leftPanel);
  m_fileLabel->setWordWrap(true);
  m_fileLabel->setStyleSheet("color: #565d6b; font-style: italic;");
  leftLayout->addWidget(m_fileLabel);

  QHBoxLayout *selectRow = new QHBoxLayout();
  m_selectAllButton = new QPushButton("Tout cocher", leftPanel);
  m_selectNoneButton = new QPushButton("Tout décocher", leftPanel);
  connect(m_selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAllClicked()));
  connect(m_selectNoneButton, SIGNAL(clicked()), this, SLOT(onSelectNoneClicked()));
  selectRow->addWidget(m_selectAllButton);
  selectRow->addWidget(m_selectNoneButton);
  leftLayout->addLayout(selectRow);

  // Bouton de superposition : sous les deux boutons ci-dessus, largeur fixe
  // pour qu'il ne change pas de taille selon le texte affiché
  m_overlayButton = new QPushButton("Superposer toutes\nles courbes sélectionnées", leftPanel);
  m_overlayButton->setCheckable(true);
  m_overlayButton->setMinimumWidth(290);
  m_overlayButton->setFixedWidth(290);
  m_overlayButton->setMinimumHeight(44);
  connect(m_overlayButton, SIGNAL(toggled(bool)), this, SLOT(onOverlayToggled(bool)));
  leftLayout->addWidget(m_overlayButton);

  QLabel *voiesLabel = new QLabel("Voies disponibles :", leftPanel);
  voiesLabel->setStyleSheet("font-weight: 600; margin-top: 6px;");
  leftLayout->addWidget(voiesLabel);

  m_checkboxScrollArea = new QScrollArea(leftPanel);
  m_checkboxScrollArea->setWidgetResizable(true);
  m_checkboxContainer = new QWidget();
  m_checkboxLayout = new QVBoxLayout(m_checkboxContainer);
  m_checkboxLayout->addStretch();
  m_checkboxScrollArea->setWidget(m_checkboxContainer);
  leftLayout->addWidget(m_checkboxScrollArea, 1);

  mainLayout->addWidget(leftPanel);

  // --- Colonne de droite : pile de mini-graphiques, avec son propre défilement ---
  m_stackScrollArea = new QScrollArea(this);
  m_stackScrollArea->setWidgetResizable(true);
  m_stackContainer = new QWidget();
  m_stackLayout = new QVBoxLayout(m_stackContainer);
  m_stackLayout->addStretch();
  m_stackScrollArea->setWidget(m_stackContainer);
  mainLayout->addWidget(m_stackScrollArea, 1);

  // Graphique superposé (mode alternatif, masqué par défaut)
  m_overlayChart = new ChartWidget(this);
  m_overlayChart->hide();
  mainLayout->addWidget(m_overlayChart, 1);
}

void AnalysisTab::onLoadFileClicked()
{
  QString path = QFileDialog::getOpenFileName(this, "Charger un fichier journal",
                                                "logs", "Fichiers CSV (*.csv);;Tous les fichiers (*.*)");
  if (path.isEmpty()) return;
  loadFile(path);
}

void AnalysisTab::loadFile(const QString &path)
{
  parseCsv(path);
}

void AnalysisTab::parseCsv(const QString &path)
{
  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier :\n" + path, QMessageBox::Ok);
    return;
  }

  QTextStream stream(&file);
  if (!stream.atEnd()) stream.readLine(); // ID ECU, ignoré

  if (stream.atEnd())
  {
    QMessageBox::warning(this, "Erreur", "Fichier CSV vide ou incomplet.", QMessageBox::Ok);
    return;
  }
  QString headerLine = stream.readLine();
  QStringList headers = headerLine.split(",");
  if (!headers.isEmpty() && headers[0].startsWith("#"))
  {
    headers[0] = headers[0].mid(1);
  }

  int columnCount = headers.count() - 1;
  if (columnCount <= 0)
  {
    QMessageBox::warning(this, "Erreur", "Format de fichier CSV non reconnu.", QMessageBox::Ok);
    return;
  }

  m_columnNames.clear();
  for (int i = 1; i < headers.count(); i++)
  {
    m_columnNames << friendlyColumnName(headers[i].trimmed());
  }

  m_time.clear();
  m_columns.clear();
  m_columns.resize(columnCount);

  QTime firstTime;
  int lineCount = 0;

  while (!stream.atEnd())
  {
    QString line = stream.readLine();
    if (line.trimmed().isEmpty()) continue;
    QStringList fields = line.split(",");
    if (fields.count() < 2) continue;

    QTime t = QTime::fromString(fields[0], "hh:mm:ss");
    double seconds;
    if (t.isValid())
    {
      if (!firstTime.isValid()) firstTime = t;
      seconds = firstTime.secsTo(t);
      if (seconds < 0) seconds += 24 * 3600;
    }
    else
    {
      seconds = lineCount;
    }
    m_time.append(seconds);

    for (int c = 0; c < columnCount; c++)
    {
      double value = 0.0;
      if (c + 1 < fields.count())
      {
        value = fields[c + 1].trimmed().toDouble();
      }
      m_columns[c].append(value);
    }
    lineCount++;
  }
  file.close();

  if (lineCount == 0)
  {
    QMessageBox::warning(this, "Erreur", "Aucune donnée exploitable dans ce fichier.", QMessageBox::Ok);
    return;
  }

  m_fileLabel->setText(QFileInfo(path).fileName() + " (" + QString::number(lineCount) + " points)");

  m_overlayChart->setData(m_time, m_columns, m_columnNames, m_colors);
  rebuildCheckboxes();
}

void AnalysisTab::rebuildCheckboxes()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxLayout->removeWidget(m_checkboxes[i]);
    delete m_checkboxes[i];
  }
  m_checkboxes.clear();

  qDeleteAll(m_stackedCharts);
  m_stackedCharts.clear();

  QLayoutItem *stretchItem = m_checkboxLayout->takeAt(m_checkboxLayout->count() - 1);

  for (int i = 0; i < m_columnNames.count(); i++)
  {
    QCheckBox *cb = new QCheckBox(m_columnNames[i], m_checkboxContainer);
    QColor c = m_colors[i % m_colors.count()];
    cb->setStyleSheet(QString(
      "QCheckBox { padding: 3px 0; }"
      "QCheckBox::indicator { width: 14px; height: 14px; border: 2px solid %1; border-radius: 3px; background-color: #ffffff; }"
      "QCheckBox::indicator:checked { background-color: %1; }"
      "QCheckBox::indicator:hover { border: 2px solid %1; }"
      ).arg(c.name()));

    if (m_columnNames[i].contains("Régime moteur") || m_columnNames[i].contains("liquide refroid") ||
        m_columnNames[i].contains("Tension batterie"))
    {
      cb->setChecked(true);
    }

    connect(cb, SIGNAL(toggled(bool)), this, SLOT(onCheckboxToggled(bool)));
    m_checkboxLayout->addWidget(cb);
    m_checkboxes.append(cb);
  }

  if (stretchItem) m_checkboxLayout->addItem(stretchItem);
  else m_checkboxLayout->addStretch();

  updateChartVisibility();
}

void AnalysisTab::onCheckboxToggled(bool checked)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  if (!cb) return;
  int index = m_checkboxes.indexOf(cb);
  if (index < 0) return;

  if (m_overlayMode)
  {
    m_overlayChart->setVisible(index, checked);
  }
  else
  {
    if (checked) addStackedChart(index);
    else removeStackedChart(index);
  }
}

void AnalysisTab::addStackedChart(int index)
{
  if (m_stackedCharts.contains(index)) return;
  if (index < 0 || index >= m_columns.count()) return;

  SingleChartWidget *chart = new SingleChartWidget(m_columnNames[index],
                                                     m_colors[index % m_colors.count()],
                                                     m_stackContainer);
  chart->setData(m_time, m_columns[index]);

  int insertPos = m_stackLayout->count() - 1; // avant le stretch final
  m_stackLayout->insertWidget(insertPos, chart);
  m_stackedCharts[index] = chart;
}

void AnalysisTab::removeStackedChart(int index)
{
  if (!m_stackedCharts.contains(index)) return;
  SingleChartWidget *chart = m_stackedCharts.take(index);
  m_stackLayout->removeWidget(chart);
  delete chart;
}

void AnalysisTab::onSelectAllClicked()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxes[i]->setChecked(true);
  }
}

void AnalysisTab::onSelectNoneClicked()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_checkboxes[i]->setChecked(false);
  }
}

void AnalysisTab::updateChartVisibility()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    bool checked = m_checkboxes[i]->isChecked();
    m_overlayChart->setVisible(i, checked);
    if (!m_overlayMode)
    {
      if (checked) addStackedChart(i);
      else removeStackedChart(i);
    }
  }
}

void AnalysisTab::onOverlayToggled(bool checked)
{
  m_overlayMode = checked;
  if (checked)
  {
    m_overlayButton->setText("Revenir à l'affichage empilé");
    m_stackScrollArea->hide();
    m_overlayChart->show();
    rebuildOverlayChart();
  }
  else
  {
    m_overlayButton->setText("Superposer toutes les courbes sélectionnées");
    m_overlayChart->hide();
    m_stackScrollArea->show();
    updateChartVisibility();
  }
}

void AnalysisTab::rebuildOverlayChart()
{
  for (int i = 0; i < m_checkboxes.count(); i++)
  {
    m_overlayChart->setVisible(i, m_checkboxes[i]->isChecked());
  }
}
