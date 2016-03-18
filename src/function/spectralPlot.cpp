/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

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
#include <vector>
#include <fstream>
#include <cmath>

#include <QtGui>
#include <QFileDialog>
#include <QString>

#include <qwt_plot_item.h>
#include <qwt_plot.h>
#include <qwt_plot_rescaler.h>
#include <qwt_scale_div.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_interval.h>
#include <qwt_painter.h>
#include <qwt_legend.h>
//#include <qwt_legend_item.h>
#include <qwt_legend_data.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_column_symbol.h>

#include "spectralPlot.h"
#include <stdio.h>
#include <iostream>

//#define ADD_LEGEND

SpectralPlot::~SpectralPlot()
{
//      delete d_directPainter;
}


void SpectralPlot::updateSpectralPlot( std::vector<float> wavelengths,  std::vector<float> hyperPixels, const int* icoords, const std::string* fname)
{
#ifdef _DEBUG
  // normalizing
  for (std::vector<float>::iterator it = wavelengths.begin(); it!=wavelengths.end(); ++it) {
      std::cerr << *it << " ";
  }
  for (std::vector<float>::iterator it = hyperPixels.begin(); it!=hyperPixels.end(); ++it) {
      std::cerr << *it << " ";
  }
#endif

  std::vector<double> dwavelengths (wavelengths.begin(), wavelengths.end());
  std::vector<double> dhyperPixels (hyperPixels.begin(), hyperPixels.end());

  mWavelengths = QVector<double>::fromStdVector(dwavelengths);
  mHyperPixels = QVector<double>::fromStdVector(dhyperPixels);
  
  replot_curve();

  isValidExport = (icoords != NULL) && (fname != NULL) && (fname->size() > 0); 
  if (isValidExport) {
    mSpectralCoords[0] = icoords[0];
    mSpectralCoords[1] = icoords[1];
    mSpectralCoords[2] = icoords[2];
    mSpectralFname = *fname;
  }
}

void SpectralPlot::exportPlot() {
  if (!isValidExport) { return; }
  QString annotation_label = QInputDialog::getText(this, tr("Annotation Text Label"),
						   tr("Comment:"), QLineEdit::Normal);
  QFileInfo fi(QString(mSpectralFname.c_str()));
  QString dir = fi.absolutePath();
  QString basename = fi.baseName();
  //std::cerr << "Image filename - " << basename.toStdString() << std::endl;
  //std::cerr << "Dir - " << dir.toStdString() << std::endl;
  QString normalized_fname = (isNormalizedPlot ? "_norm" : "");
  //----- Export Image -----//
  QPixmap px(this->frameRect().size());
  this->render(&px,QPoint(),this->frameRect());
  QString spectral_image_fname = basename + "_" + QString::number(mSpectralCoords[0]) + "_" + QString::number(mSpectralCoords[1]) + normalized_fname + ".png";
  QString image_fname = dir + "/" + spectral_image_fname;
  px.save(image_fname);
  //----- Export Annotation -----//
  QString annotation_fname = dir + "/" + basename + "_" + QString::number(mSpectralCoords[0]) + "_" + QString::number(mSpectralCoords[1]) + normalized_fname + ".msa";
  std::fstream of(annotation_fname.toStdString().c_str(),std::fstream::out);
  of << "{" << std::endl;
  of << "\"originalImage\" :  \"" << fi.fileName().toStdString() << "\"," << std::endl;
  of << "\"spectralImage\" :  \"" << spectral_image_fname.toStdString() << "\"," << std::endl;
  of << "\"sampleLocation\" : {" << std::endl;
  of << "     \"x\":" <<  mSpectralCoords[0] << "," << std::endl;
  of << "     \"y\":" <<  mSpectralCoords[1] << "." << std::endl;
  of << "}," << std::endl;
  of << "\"samples\" : {" << std::endl;
  of << "     \"wavelength\" :  \"nm\"," << std::endl;
  of << "     \"reflectance\" :  \"\"," << std::endl;
  of << "     \"normalized\" :  " << (isNormalizedPlot ? "true" : "false") << "," << std::endl;
  of << "     \"values\" : {" << std::endl;
  int spectralData_size = mWavelengths.size();
  for (int i = 0; i < spectralData_size-1; ++i) {
    of << "          \"" << mWavelengths[i] << "\" : " << mHyperPixels[i] << "," << std::endl;
  }
  of << "          \"" << mWavelengths[spectralData_size-1] << "\" : " << mHyperPixels[spectralData_size-1] << std::endl;
  of << "     }" << std::endl;
  of << "}," << std::endl;
  of << "\"creatingApplication\" : {" << std::endl;
  of << "     \"name\" :  \"Hyper3D\"," << std::endl;
  of << "     \"version\" :  \".....\"." << std::endl;
  of << "}," << std::endl;
  of << "\"comment\" :  \"" << annotation_label.toStdString() << "\"." << std::endl;
  of << "}" << std::endl;
  
  of.close();
}

void SpectralPlot::set_normalized_plot(int state) {
  switch(state) {
  case Qt::Unchecked: {
    isNormalizedPlot = false;
  } break;
  case Qt::Checked: {
    isNormalizedPlot = true;
  } break;
  default:
    isNormalizedPlot = false;
  }
  replot_curve();
  std::cerr << "isNormalizedPlot - " << isNormalizedPlot << std::endl;
}

void SpectralPlot::showCurve( QwtPlotItem *item, bool on )
{
#ifdef ADD_LEGEND
    item->setVisible( on );

    QwtLegendItem *legendItem = qobject_cast<QwtLegendItem *>( legend()->find( item ) );

    if ( legendItem )
        legendItem->setChecked( on );

    replot();
#endif
}

// this is for background of the plot
void SpectralPlot::initGradient()
{
    QPalette pal = canvas()->palette();

    QLinearGradient gradient( 0.0, 0.0, 1.0, 0.0 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
    //MK: two dark blue gradient
    //    gradient.setColorAt( 0.0, QColor( 0, 49, 110 ) );
    //    gradient.setColorAt( 1.0, QColor( 0, 87, 174 ) );

    //---------------------------------------------
    // Nikon D70 Measurement
    //          start   	end	  bandwidth	  average wave	 peak
    //  Blue:	    380	  580	      200	         480 	       470
    //  Green:	  390	  700	      310	         545        540
    //  Red:	    480	  750	      270	         615	       590

    //MK: color
    gradient.setColorAt( 0.0, QColor( 0, 0, 0 ) ); // black
    gradient.setColorAt( 1.0/8., QColor( 0, 0, 0 ) ); // black
    //==============================================================
    gradient.setColorAt( 1.5/8., QColor( 142, 112, 178 ) ); // purple
    gradient.setColorAt( 1.8/8., QColor(   0, 181, 236 ) ); // blue
    gradient.setColorAt( 2.4/8., QColor(   0, 173, 104 ) ); // green
    gradient.setColorAt( 3.0/8., QColor( 255, 198, 10 ) ); // yellow
    gradient.setColorAt( 4.5/8., QColor( 210,  30,  71 ) ); // red
    //==============================================================
    gradient.setColorAt( 5/8., QColor( 0, 0, 0 ) ); // black
    gradient.setColorAt( 1.0, QColor( 0, 0, 0 ) ); // black

    pal.setBrush( QPalette::Window, QBrush( gradient ) );

    canvas()->setPalette( pal );
}

void SpectralPlot::normalize_vector_in(QVector<double>& vec) {
  std::size_t vec_size = vec.size();
  double length = 0.0;
  for (std::size_t i = 0; i < vec_size; ++i) {
    double value = vec[i];
    length += (value * value);
  }
  length = std::sqrt(length);
  for (std::size_t i = 0; i < vec_size; ++i) {
    vec[i] /= length;
  }
}

void SpectralPlot::replot_curve() {
  QVector<double>  curr_mHyperPixels = mHyperPixels;
  if (isNormalizedPlot) { normalize_vector_in(curr_mHyperPixels); }
  QVector<QPointF> spectralData;
  int spectralData_size = mWavelengths.size();
  for (int i = 0; i < spectralData_size; ++i) {
    spectralData.push_back(QPointF(mWavelengths[i],curr_mHyperPixels[i]));
  }
  mCurve1->setSamples(spectralData);

  this->replot();
}

SpectralPlot::SpectralPlot( QWidget *parent ):
    QwtPlot( parent )
{
  setAutoReplot( false );

  QwtPlotCanvas* qp_canvas = (QwtPlotCanvas*)canvas();
  qp_canvas->setPaintAttribute( QwtPlotCanvas::BackingStore, false );
//  setCanvasBackground( QColor( Qt::darkBlue ) );
  initGradient();//background

  plotLayout()->setAlignCanvasToScales( true );

  //MK: draw grid
  QwtPlotGrid *grid = new QwtPlotGrid;
  //grid->enableXMin(true);
  grid->setMajorPen( QPen( Qt::white, 0, Qt::DotLine ) );
  grid->setMinorPen( QPen( Qt::gray, 0 , Qt::DotLine ) );
  grid->attach( this );

  setAutoReplot( false );

  plotLayout()->setAlignCanvasToScales( true );

  // Legend
#ifdef ADD_LEGEND
  QwtLegend *legend = new QwtLegend;
  legend->setItemMode( QwtLegend::CheckableItem );
  insertLegend( legend, QwtPlot::RightLegend );
#endif

  //MK: set X-axis
  QString xlabel(" Wavelength [nm] ");
  QwtText xtext( xlabel );
  xtext.setFont( QFont( "Helvetica", 9)); //, QFont::Bold ) );
  //xtext.setColor( QColor( 200, 150, 0 ) );
  setAxisScale( QwtPlot::xBottom, 300, 1000, 100 );
  setAxisTitle( QwtPlot::xBottom, xtext );
  setAxisFont( QwtPlot::xBottom, QFont( "Helvetica", 9 )  ); // QFont( "Helvetica", 10, QFont::Bold )
//  setAxisLabelRotation( QwtPlot::xBottom, 0.0 );
//  setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

  //MK: set Y-axis
  QString ylabel(" Reflectance ");
  QwtText ytext( ylabel );
  ytext.setFont( QFont( "Helvetica", 9)); //, QFont::Bold ) );
  setAxisScale( QwtPlot::yLeft, 0, 1, 0.2 );
  setAxisTitle( QwtPlot::yLeft, ytext );
  setAxisFont( QwtPlot::yLeft, QFont( "Helvetica", 9 )  ); // QFont( "Helvetica", 10, QFont::Bold )


  // Wavebands (QSI): 370, 417, 475, 525, 628, 675, 755nm

  mCurve1 = new QwtPlotBarChart(tr("On/Off"));
  QwtColumnSymbol* symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
  symbol->setFrameStyle(QwtColumnSymbol::Raised);
  symbol->setPalette(QPalette(QColor(255,255,255,255),QColor(255,255,255,100)));
  mCurve1->setSymbol(symbol);
  symbol = 0;
  mCurve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );//
  QVector<QPointF> spectralData;
  mCurve1->setSamples(spectralData);
  mCurve1->attach( this );

  showCurve( mCurve1, true );

  mSpectralCoords[0] = 0;
  mSpectralCoords[1] = 0;
  mSpectralCoords[2] = 0;

  mSpectralFname = "";
  isValidExport = false;
  isNormalizedPlot = false;  

#ifdef ADD_LEGEND
  connect( this, SIGNAL( legendChecked( QwtPlotItem *, bool ) ), SLOT( showCurve( QwtPlotItem *, bool ) ) );
#endif
}
