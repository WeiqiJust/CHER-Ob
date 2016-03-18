/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


#ifndef LIGHT_CONTROL_RTI_H
#define LIGHT_CONTROL_RTI_H

#include <QWidget>
#include <QTimer>

#include <vcg/space/point3.h>
#include <vcg/math/base.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif

class MainWindow;

//! Light Control class.
/*!
  The class defines the widget to set the light direction.
*/
class LightControlRTI: public QWidget
{
	Q_OBJECT

public:

	//! Constructor.
	/*!
	  \param parent 
	  \param size size of the widget.
	*/
	LightControlRTI(QWidget *parent, int size);
	
	//! Deconstructor.
	~LightControlRTI();


protected:

	/*!
	  Paint event handler.
	*/
	void paintEvent(QPaintEvent *event);

	/*!
	  Mouse press event handler.
	*/
	void mousePressEvent(QMouseEvent *event);
	
	/*!
	  Mouse move event handler.
	*/
	void mouseMoveEvent(QMouseEvent *event);
	
	/*!
	  Mouse release event handler.
	*/
	void mouseReleaseEvent(QMouseEvent *event);


// private data members
private:

	int height; /*!< Height of the widget. */
	int width; /*!< Width of the widget. */

	int xFocus; /*!< x-coordinate of the gradient focus. */
	int yFocus; /*!< y-corrdinate of the gradient focus. */

        float dx; /*!< Maximum shift of the light position from the center on x-axis. */
        float dy; /*!< Maximum shift of the light position from the center on y-axis. */

	vcg::Point3f light; /*!< Light vector. */
	
	bool rightPressed; /*!< Holds whether the rigth mouse button is pressed in the last mouse press event. */  
	bool dragging; /*!< Holds whether the left mouse button is pressed in the last mouse press event. */

	QTimer *timer; /*!< Timer to manage the paint event. */
	bool interactive; /*!< Holds whether the widget can update the light vector throughout the mouse interaction or when a release event occurred. */
	QGradientStops stops; /*!< Defines the stop points for the gradient. */

	bool enabled; /*!< Holds whether the widget is enabled. */ 

	/*!
	  Checks if the rectangle defined by maximun shifts contains the point (x,y)
	  \param  x, y coordinates of the point.
	  \return true if the rectangle contains the point, false otherwise.
    */
	bool isValidPoint(int x, int y)
	{
		QRect r(width/2 - dx, height/2 - dy, dx * 2 , dy * 2);
		return r.contains(x, y);
	}


	/*!
	  Returns the intersection between the line that pass through points (x1,y1) (width/2, height/2), and the 
	  rectangle defined by maximum shifts.
	  \param x1, y1 coordinate of the input point.
	  \param xout, yout coordinate of the output point.
	*/
	void intersect(int x1, int y1, int& xout, int& yout)
	{
		if (y1 == height/2) //horizontal line
		{
			yout = y1;
			if (x1 > width/2)
				xout = width/2 + dx;
			else
				xout = width/2 - dx;

		}
		else if(x1 == width/2) //vertical line
		{
			xout = x1;
			if (y1 > height/2)
				yout = height/2 + dy;
			else
				yout = height/2 - dy;
		}
		else //general case
		{
                        float m = (height/2.0 - y1)/(width/2.0 - x1);
                        float q = x1 * m - y1;
                        float temp = 0;

			int x = x1 < width/2? width/2 - dx: width/2 + dx;
			int y = y1 < height/2? height/2 - dy: height/2 + dy;
			temp = x*m - q;
			if (temp < (height/2 + dy) && temp > (height/2 - dy))
			{
				xout = x;
				yout = temp;
			}
			else
			{
				xout = (y + q)/m;
				yout = y;
			}
		}
	}


	/*!
	  Computes projections of the normalized light vector.
	*/
	void calcLightVector()
	{
		//Projects the light vector on uv space
                float distance2 = (xFocus - width/2)*(xFocus - width/2) + (yFocus - height/2)*(yFocus - height/2);
                float projectedDist = sqrt(distance2)/ (width/2);
		if (projectedDist > 1) projectedDist = 1;
		//Computes the u and v components
                float alpha = M_PI /2;
		if (width/2 != xFocus)
		{
                        float m = (height/2.0 - yFocus)/(width/2.0 - xFocus);
			alpha = atan(m);
		}
		alpha = vcg::math::Abs(alpha);
		light[0] = xFocus > width/2? projectedDist * cos(alpha): -(projectedDist * cos(alpha));
		light[1] = yFocus > height/2? -(projectedDist * sin(alpha)): projectedDist * sin(alpha);
                float inner = 1 - light[0]*light[0] - light[1]*light[1];
		light[2] = inner > 0 ? sqrt(inner): 0.0;
		light.Normalize();
	}


signals:

	/*!
	  Emitted when the user changes the light position. If \a refresh is true the image in the browser is updated.
	  \param l new light vector.
	  \param refresh flag
    */
	void lightChanged(vcg::Point3f l, bool refresh = true);

private slots:

	/*!
	  Invoked when the timer times out.
	*/
	void fired();

public slots:
	
	
	void setInteractive(bool value) {interactive = value;}
	
	void setEnabled(bool value);

	/*!
	  Moves the light position.
	  \param offx, offy offset.
	*/
	void moveLightPosition(float offx, float offy);

	/*!
	  Sets the light direction.
	  \param l light vector.
	*/
	void setLight(const vcg::Point3f& l, bool refresh);

//accessor
public:
	
    /*!
      Gets the light direction.
    */
    vcg::Point3f getLight();

    /*!
	  Sets the maximum shift on x-axis.
	*/
	void setMaxXAngle(int degree)
	{
                float angle = degree/180.0 * M_PI;
		dx = sin(angle) * width/2; 
	}

	/*!
	  Sets the maximum shift on y-axis.
	*/
	void setMaxYAngle(int degree)
	{
                float angle = degree/180.0 * M_PI;
		dy = sin(angle) * height/2;
	}

};

#endif /*LIGHT_CONTROL_RTI_H */
