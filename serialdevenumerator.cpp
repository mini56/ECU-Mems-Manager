#include <QStringList>
#ifdef linux
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#elif defined(WIN32)
#include <stdio.h>
#include <windows.h>
#endif
#include "serialdevenumerator.h"
#include "splashprogress.h"
#include <QApplication>

/**
 * Constructor.
 */
SerialDevEnumerator::SerialDevEnumerator()
{
}

/**
 * Queries the OS / device filesystem for the list of serial devices likely
 * to be the one that is connected to the ECU.
 * @return List of device names
 */
QStringList SerialDevEnumerator::getSerialDevList(QString savedDevName)
{
  QStringList serialDevices;

  if (savedDevName.isNull() || savedDevName.isEmpty())
  {
    serialDevices.append("");
  }
  else
  {
    serialDevices.append(savedDevName);
  }

#ifdef linux

  // first check to see if this Linux distribution uses the /dev/serial/
  // directory to store symlinks to each serial device
  QDir devSerial("/dev/serial/by-id/", "", QDir::Name, QDir::Files | QDir::NoDotAndDotDot);

  if (devSerial.exists())
  {
    QFileInfoList files = devSerial.entryInfoList();
    foreach(const QFileInfo file, files)
    {
      if (file.exists())
      {
        if (file.isSymLink())
        {
          serialDevices.append(file.symLinkTarget());
        }
        else
        {
          serialDevices.append(file.absoluteFilePath());
        }
      }
    }
  }

  // if nothing was found using the method above, simply return a list of
  // devices that match the pattern "ttyS*"
  if (serialDevices.count() == 0)
  {
    QDir dev("/dev", "ttyUSB* ttyS*", QDir::NoSort, QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList files = dev.entryInfoList();
    foreach(const QFileInfo file, files)
    {
      if (file.exists())
      {
        if (file.isSymLink())
        {
          serialDevices.append(file.symLinkTarget());
        }
        else
        {
          serialDevices.append(file.absoluteFilePath());
        }
      }
    }
  }

#elif defined(__APPLE__)

  QDir dev("/dev", "cu.usbserial*", QDir::NoSort, QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);
  QFileInfoList files = dev.entryInfoList();
  foreach(const QFileInfo file, files)
  {
    if (file.exists())
    {
      if (file.isSymLink())
      {
        serialDevices.append(file.symLinkTarget());
      }
      else
      {
        serialDevices.append(file.absoluteFilePath());
      }
    }
  }

#elif defined(WIN32)

  // compiling with MinGW, so use a WinAPI mechanism to enumerate ports
  char portName[8];
  COMMCONFIG cc;
  DWORD dwSize = sizeof(COMMCONFIG);

  // apparently, COM ports can be numbered from 1 to 255
  // Optimisation : si un port a déjà fonctionné précédemment, on le teste
  // en premier. S'il répond, on évite de scanner les 255 ports un par un,
  // ce qui accélère considérablement le démarrage.
  if (!savedDevName.isEmpty())
  {
    COMMCONFIG ccSaved;
    DWORD dwSizeSaved = sizeof(COMMCONFIG);
    QByteArray savedDevBytes = savedDevName.toLocal8Bit();
    if (GetDefaultCommConfig(savedDevBytes.constData(), &ccSaved, &dwSizeSaved))
    {
      if (g_splashProgressCallback)
      {
        g_splashProgressCallback(100);
      }
      serialDevices.removeDuplicates();
      return serialDevices;
    }
  }

  for (int portNum = 1; portNum < 256; portNum++)
  {
    snprintf(portName, 8, "COM%d", portNum);
    if (GetDefaultCommConfig(portName, &cc, &dwSize))
    {
      serialDevices.append(portName);
    }

    if (g_splashProgressCallback)
    {
      g_splashProgressCallback((portNum * 100) / 255);
    }
    if (qApp)
    {
      qApp->processEvents();
    }
  }

#endif

  serialDevices.removeDuplicates();
  return serialDevices;
}
