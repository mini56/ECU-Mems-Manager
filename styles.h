#ifndef STYLES_H
#define STYLES_H

#include <QString>

// Feuille de style moderne - thème clair
static const QString STYLE_LIGHT = R"(
QWidget {
    background-color: #ffffff;
    color: #1f2430;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 9pt;
}
QMainWindow, QDialog {
    background-color: #ffffff;
}
QGroupBox {
    background-color: #ffffff;
    border: 1px solid #dcdfe4;
    border-radius: 8px;
    margin-top: 14px;
    padding: 10px;
    font-weight: 600;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 12px;
    padding: 0 6px;
    color: #2f6fed;
}
QLabel {
    background-color: transparent;
}
QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #7bb8ff, stop:0.49 #2f7dff, stop:0.5 #1f66e6, stop:1 #1653c2);
    color: #ffffff;
    border: 1px solid #14479e;
    border-radius: 8px;
    padding: 6px 16px;
    font-weight: 600;
}
QPushButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #96c8ff, stop:0.49 #4a90ff, stop:0.5 #2f7ded, stop:1 #1f66c8);
}
QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1653c2, stop:0.5 #1f66e6, stop:0.51 #2f7dff, stop:1 #7bb8ff);
    padding-top: 7px;
}
QPushButton:disabled {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #f2f3f5, stop:0.5 #dfe2e8, stop:1 #cdd1d8);
    color: #9aa0ab;
    border: 1px solid #c7cbd4;
}
QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
    background-color: #ffffff;
    border: 1px solid #d3d7de;
    border-radius: 5px;
    padding: 4px 6px;
    selection-background-color: #2f6fed;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 1px solid #2f6fed;
}
QComboBox::drop-down {
    border: none;
    width: 20px;
}
QComboBox QAbstractItemView {
    background-color: #ffffff;
    border: 1px solid #d3d7de;
    selection-background-color: #2f6fed;
    selection-color: #ffffff;
}
QCheckBox, QRadioButton {
    spacing: 6px;
}
QTabWidget::pane {
    border: 1px solid #dcdfe4;
    border-radius: 8px;
    background-color: #ffffff;
    top: -1px;
}
QTabBar::tab {
    background-color: #e7e9ee;
    color: #565d6b;
    border: 1px solid #dcdfe4;
    border-bottom: none;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 8px 18px;
    margin-right: 2px;
    font-weight: 600;
}
QTabBar::tab:selected {
    background-color: #ffffff;
    color: #2f6fed;
}
QTabBar::tab:hover {
    color: #2f6fed;
}
QMenuBar {
    background-color: #ffffff;
    border-bottom: 1px solid #dcdfe4;
}
QMenuBar::item:selected {
    background-color: #e7edfd;
    border-radius: 4px;
}
QMenu {
    background-color: #ffffff;
    border: 1px solid #dcdfe4;
}
QMenu::item:selected {
    background-color: #2f6fed;
    color: #ffffff;
}
QStatusBar {
    background-color: #ffffff;
    border-top: 1px solid #dcdfe4;
}
QTableWidget, QTableView {
    background-color: #ffffff;
    gridline-color: #eceef2;
    border: 1px solid #dcdfe4;
    border-radius: 6px;
}
QHeaderView::section {
    background-color: #eceef2;
    color: #565d6b;
    padding: 4px;
    border: none;
    font-weight: 600;
}
QScrollBar:vertical {
    background: transparent;
    width: 10px;
}
QScrollBar::handle:vertical {
    background: #c7cbd4;
    border-radius: 5px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover {
    background: #a9adb8;
}
QScrollBar:horizontal {
    background: transparent;
    height: 10px;
}
QScrollBar::handle:horizontal {
    background: #c7cbd4;
    border-radius: 5px;
    min-width: 24px;
}
QToolTip {
    background-color: #1f2430;
    color: #ffffff;
    border: none;
    padding: 4px 8px;
    border-radius: 4px;
}
)";

// Feuille de style moderne - thème sombre
static const QString STYLE_DARK = R"(
QWidget {
    background-color: #1e2128;
    color: #e6e8ee;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 9pt;
}
QMainWindow, QDialog {
    background-color: #1e2128;
}
QGroupBox {
    background-color: #262a33;
    border: 1px solid #363b47;
    border-radius: 8px;
    margin-top: 14px;
    padding: 10px;
    font-weight: 600;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 12px;
    padding: 0 6px;
    color: #5b9dff;
}
QLabel {
    background-color: transparent;
}
QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #6fa8f5, stop:0.49 #3d7bff, stop:0.5 #2c63e0, stop:1 #1f4bb8);
    color: #ffffff;
    border: 1px solid #14356e;
    border-radius: 8px;
    padding: 6px 16px;
    font-weight: 600;
}
QPushButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #8ebbff, stop:0.49 #598fff, stop:0.5 #3d78f0, stop:1 #2c5cc4);
}
QPushButton:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1f4bb8, stop:0.5 #2c63e0, stop:0.51 #3d7bff, stop:1 #6fa8f5);
    padding-top: 7px;
}
QPushButton:disabled {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #3a3f4a, stop:0.5 #2f333c, stop:1 #26292f);
    color: #6b7180;
    border: 1px solid #363b47;
}
QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
    background-color: #2b2f39;
    border: 1px solid #40465400;
    border: 1px solid #40465b;
    border-radius: 5px;
    padding: 4px 6px;
    color: #e6e8ee;
    selection-background-color: #3d7bff;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 1px solid #5b9dff;
}
QComboBox::drop-down {
    border: none;
    width: 20px;
}
QComboBox QAbstractItemView {
    background-color: #2b2f39;
    color: #e6e8ee;
    border: 1px solid #40465b;
    selection-background-color: #3d7bff;
    selection-color: #ffffff;
}
QCheckBox, QRadioButton {
    spacing: 6px;
}
QTabWidget::pane {
    border: 1px solid #363b47;
    border-radius: 8px;
    background-color: #262a33;
    top: -1px;
}
QTabBar::tab {
    background-color: #22252d;
    color: #9aa1b0;
    border: 1px solid #363b47;
    border-bottom: none;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 8px 18px;
    margin-right: 2px;
    font-weight: 600;
}
QTabBar::tab:selected {
    background-color: #262a33;
    color: #5b9dff;
}
QTabBar::tab:hover {
    color: #5b9dff;
}
QMenuBar {
    background-color: #22252d;
    border-bottom: 1px solid #363b47;
}
QMenuBar::item:selected {
    background-color: #2b3550;
    border-radius: 4px;
}
QMenu {
    background-color: #262a33;
    border: 1px solid #363b47;
    color: #e6e8ee;
}
QMenu::item:selected {
    background-color: #3d7bff;
    color: #ffffff;
}
QStatusBar {
    background-color: #22252d;
    border-top: 1px solid #363b47;
}
QTableWidget, QTableView {
    background-color: #262a33;
    color: #e6e8ee;
    gridline-color: #363b47;
    border: 1px solid #363b47;
    border-radius: 6px;
}
QHeaderView::section {
    background-color: #2b2f39;
    color: #9aa1b0;
    padding: 4px;
    border: none;
    font-weight: 600;
}
QScrollBar:vertical {
    background: transparent;
    width: 10px;
}
QScrollBar::handle:vertical {
    background: #40465b;
    border-radius: 5px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover {
    background: #565d70;
}
QScrollBar:horizontal {
    background: transparent;
    height: 10px;
}
QScrollBar::handle:horizontal {
    background: #40465b;
    border-radius: 5px;
    min-width: 24px;
}
QToolTip {
    background-color: #e6e8ee;
    color: #1e2128;
    border: none;
    padding: 4px 8px;
    border-radius: 4px;
}
)";

#endif // STYLES_H
