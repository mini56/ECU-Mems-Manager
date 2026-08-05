#include "aboutbox.h"
#include <QMessageBox>
#include <QString>

void AboutBox::showAbout(QWidget* parent) {
    QMessageBox::about(
        parent,
        "À propos de ECU Mems Manager",
        QString::fromUtf8(
            "<h2>ECU Mems Manager</h2>"
            "<p>Version 0.9.0</p>"
            "<p><b>Compatible avec :</b></p>"
            "<ul>"
            "<li>Rover MEMS 1.2</li>"
            "<li>Rover MEMS 1.3</li>"
            "<li>Rover MEMS 1.6</li>"
            "</ul>"
            "<p><b>Protocole :</b> ROSCO</p>"
            "<p><b>Développement :</b> Claude Lespagnol</p>"
        )
    );
}
