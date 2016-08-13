/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)

 - License:  GNU General Public License Usage
   Alternatively, this file may be used under the terms of the GNU General
   Public License version 3.0 as published by the Free Software Foundation
   and appearing in the file LICENSE.GPL included in the packaging of this
   file. Please review the following information to ensure the GNU General
   Public License version 3.0 requirements will be met:
   http://www.gnu.org/copyleft/gpl.html.

 - Warranty: This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4138)
#endif

#include "mainWindow.h"

#include <QApplication>
#include <iostream>

// Required when building with the static Qt
#if QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qico)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qmng)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
#endif // QT_STATICPLUGIN

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(CHEROb);
  QCoreApplication::setOrganizationName("Yale Graphics Lab");
  QCoreApplication::setApplicationName("CHEROb");

  QApplication app(argc, argv);

  //------------------------------------------------------------------------------
  // for new library places (This will solve misbehavior in the deploy mode)
#ifdef __APPLE__
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  dir.cd("Frameworks");
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
  //------------------------------------------------------------------------------

  if (!QGLFormat::hasOpenGL()) {
    cerr << "This system has no OpenGL support" << endl;
    return 1;
  }

  vtkObject::GlobalWarningDisplayOff(); // Disabling vtkOutputWindow

  MainWindow mainWin;

  mainWin.show();

  // MK: automatically maximize the main window
  // However, this function has a bug in the current QT version.
  // Don't use this!!!
  //mainWin.showMaximized();
  return app.exec();
}
