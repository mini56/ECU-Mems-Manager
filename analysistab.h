#ifndef ANALYSISTAB_H
#define ANALYSISTAB_H

#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QColor>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QMap>

/**
 * Mini-graphique dedie a une seule voie, avec son propre titre, sa
 * propre echelle verticale (valeurs reelles, pas normalisees) et l'axe
 * du temps en bas. Utilise en mode "empile" (une fenetre par courbe).
 */
class SingleChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingleChartWidget(const QString &name, const QColor &color, QWidget *parent = 0);
    void setData(const QVector<double> &time, const QVector<double> &values);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

private:
    QString m_name;
    QColor m_color;
    QVector<double> m_time;
    QVector<double> m_values;
    bool m_hasCursor;
    int m_cursorX;
};

/**
 * Widget de trace "superpose" : plusieurs series sur un seul graphique,
 * chacune normalisee sur sa propre echelle (0-100%). Utilise en mode
 * superposition (bouton en haut de l'onglet).
 */
class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);

    void setData(const QVector<double> &time,
                 const QVector<QVector<double> > &series,
                 const QStringList &names,
                 const QVector<QColor> &colors);
    void setVisible(int index, bool visible);
    void clearData();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

private:
    QVector<double> m_time;
    QVector<QVector<double> > m_series;
    QStringList m_names;
    QVector<QColor> m_colors;
    QVector<bool> m_visible;
    bool m_hasCursor;
    int m_cursorX;
};

/**
 * Onglet "Analyse" : charge un fichier CSV enregistre par MEMS-Scan et
 * permet d'afficher/masquer chaque colonne de donnees. Deux modes
 * d'affichage : une mini-fenetre par voie empilee (par defaut), ou
 * toutes les voies superposees sur un seul graphique.
 */
class AnalysisTab : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisTab(QWidget *parent = 0);
    void loadFile(const QString &path);

private slots:
    void onLoadFileClicked();
    void onCheckboxToggled(bool checked);
    void onSelectAllClicked();
    void onSelectNoneClicked();
    void onOverlayToggled(bool checked);

private:
    void parseCsv(const QString &path);
    void rebuildCheckboxes();
    void updateChartVisibility();
    void addStackedChart(int index);
    void removeStackedChart(int index);
    void rebuildOverlayChart();

    QLabel *m_fileLabel;
    QPushButton *m_loadButton;
    QPushButton *m_selectAllButton;
    QPushButton *m_selectNoneButton;
    QPushButton *m_overlayButton;

    QScrollArea *m_checkboxScrollArea;
    QWidget *m_checkboxContainer;
    QVBoxLayout *m_checkboxLayout;

    QScrollArea *m_stackScrollArea;
    QWidget *m_stackContainer;
    QVBoxLayout *m_stackLayout;
    QMap<int, SingleChartWidget*> m_stackedCharts;

    ChartWidget *m_overlayChart;
    bool m_overlayMode;

    QVector<double> m_time;
    QVector<QVector<double> > m_columns;
    QStringList m_columnNames;
    QVector<QCheckBox*> m_checkboxes;
    QVector<QColor> m_colors;
};

#endif // ANALYSISTAB_H
