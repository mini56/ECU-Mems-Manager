#include "helpviewer.h"
#include <QFile>
#include <QTextDocument>
#include <QDesktopServices>

HelpViewer::HelpViewer(const QString title, QWidget * parent):QDialog(parent), m_vbox(0), m_viewer(0), m_closeButton(0)
{
  this->setWindowTitle(title + " - Aide");
  this->setMinimumWidth(850);
  this->setMinimumHeight(550);
  this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

  m_vbox = new QVBoxLayout(this);
  m_closeButton = new QPushButton("Fermer", this);
  connect(m_closeButton, SIGNAL(clicked()), this, SLOT(onCloseClicked()));
  m_viewer = new QTextBrowser(this);
  m_viewer->setOpenLinks(false);
  connect(m_viewer, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));

  QFile helpFile(":/help/help.html");

  if (helpFile.open(QFile::ReadOnly))
  {
    m_viewer->setHtml(QString::fromUtf8(helpFile.readAll()));
    helpFile.close();
  }
  else
  {
    m_viewer->setHtml(tr("<h3>Impossible de charger l'aide</h3>"
                         "<p>Le fichier d'aide (%1) n'a pas pu être ouvert : %2</p>")
                        .arg(helpFile.fileName(), helpFile.errorString()));
  }

  m_vbox->addWidget(m_viewer);
  m_vbox->addWidget(m_closeButton);
}

void HelpViewer::onCloseClicked()
{
  this->hide();
}

void HelpViewer::onAnchorClicked(QUrl url)
{
  QDesktopServices::openUrl(url);
}
