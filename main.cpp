#include <QApplication>
#include <QMessageBox>

#include "mainwindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("ECU Mems Manager");
    QApplication::setApplicationVersion("0.9.0");
    QApplication::setOrganizationName("ECU Mems Manager");

    /*
     * Base SQLite locale.
     *
     * La base est créée automatiquement dans :
     *
     *    <dossier du programme>/database/
     *
     * Aucun accès Internet n'est nécessaire.
     */
    DatabaseManager database;

    if (!database.open())
    {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Erreur base de données"),
            QObject::tr(
                "Impossible d'ouvrir la base de données SQLite.\n\n"
                "ECU Mems Manager ne peut pas démarrer."
            )
        );

        return 1;
    }

    MainWindow window;
    window.show();

    const int result = app.exec();

    database.close();

    return result;
}
