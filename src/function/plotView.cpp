/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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

#include <QLayout>

#include "plotView.h"
#include "../mainWindow.h"
#include "spectralPlot.h"

PlotView::PlotView( QWidget *parent )
  : QWidget( parent )
{
  mSpectralPlot = new SpectralPlot(this);
  connect(this, SIGNAL(currentHyperPixelsChanged(std::vector<float>, std::vector<float>, const int*, const double*, const std::string*, Information*)), mSpectralPlot, SLOT(updateSpectralPlot(std::vector<float>, std::vector<float>, const int*, const double*, const std::string*, Information*)) );

  exportButton = new QPushButton(this);
  exportButton->setText(QString("Export"));
  connect(exportButton, SIGNAL(clicked(bool)), mSpectralPlot, SLOT(exportPlot()));

  normalizeCheckBox = new QCheckBox(this);
  normalizeCheckBox->setText(QString("Normalized"));
  normalizeCheckBox->setCheckState(Qt::Unchecked);
  connect(normalizeCheckBox, SIGNAL(stateChanged(int)), mSpectralPlot, SLOT(set_normalized_plot(int)));
  //  int width = this->frameGeometry().width();
  //  int height = this->frameGeometry().height();
  //  mSpectralPlot->resize(QSize(width, height));
  //  mSpectralPlot->replot();

  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setMargin( 0 );
  layout->addWidget( mSpectralPlot );

  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget( exportButton );
  hlayout->addWidget( normalizeCheckBox );
  layout->addLayout( hlayout );
  
  this->setLayout(layout);
}

void PlotView::updateSpectralPlot(std::vector<float> wavelengths, std::vector<float> hyperPixels, const int* icoords, const double* dcoords, const std::string* fname)
{
  mWavelengths = wavelengths;
  mHyperPixels = hyperPixels;

  emit currentHyperPixelsChanged(wavelengths, hyperPixels, icoords, dcoords, fname, mw()->mInformation);
}

MainWindow * PlotView::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}

PlotView::~PlotView()
{
}
