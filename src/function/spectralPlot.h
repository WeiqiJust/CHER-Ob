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
#ifndef SPECTRALPLOT_H
#define SPECTRALPLOT_H
#include <QWidget>

#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
#include <qwt_plot_barchart.h>

class QwtPlot;
class QwtInterval;
class RectItem;

//class QwtPlotCurve;
class QwtPlotMarker;

#if 0
class SpectralCurve: public QwtPlotCurve
{
public:
    SpectralCurve( const QString &title ):
        QwtPlotCurve( title )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;
        c.setAlpha( 150 );

        setPen( c );
        setBrush( c );
    }
};
#endif
// customized plots
class SpectralPlot: public QwtPlot
{
  Q_OBJECT

public:
  SpectralPlot( QWidget *parent );
  ~SpectralPlot();

  void setRectOfInterest( const QRectF & );

public slots:
  void updateSpectralPlot( std::vector<float> wavelengths,  std::vector<float> hyperPixels, const int* icoords, const std::string* fname);
  void exportPlot();
  void set_normalized_plot(int state);
  void showCurve( QwtPlotItem *item, bool on );

private:
  QwtPlotBarChart *mCurve1;
  int mSpectralCoords[3];
  std::string mSpectralFname;
  bool isValidExport;
  bool isNormalizedPlot;

  void initGradient();
  void normalize_vector_in(QVector<double>& vec);
  void replot_curve();

  QVector<double>  mWavelengths;
  QVector<double>  mHyperPixels;
};


#endif // SPECTRALPLOT_H
