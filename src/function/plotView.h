/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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
#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QtGui>
#include <QWidget>
#include <QPushButton>

class SpectralPlot;
class MainWindow;

// main doc
class PlotView: public QWidget
{
  Q_OBJECT

public:
  PlotView( QWidget *parent = NULL );
  ~PlotView();

signals:
  void currentHyperPixelsChanged(std::vector<float> wavelengths, std::vector<float> hyperPixels, const int* icoords, const std::string* fname);

public slots:
  void updateSpectralPlot( std::vector<float> wavelengths,  std::vector<float> hyperPixels, const int* icoords, const std::string* fname);

private:
  MainWindow* mw();
  SpectralPlot *mSpectralPlot;
  QPushButton  *exportButton;
  QCheckBox    *normalizeCheckBox;
  std::vector<float> mWavelengths;
  std::vector<float> mHyperPixels;
};

#endif // PLOTVIEW_H
