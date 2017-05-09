/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Zeyu Wang (zeyu.wang@yale.edu)

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
   anonymous help from various software engineering communities.

   This file defines the different classes of bookmark that the user may work
   with (corresponding to the different file types that may be displayed).

*****************************************************************************/
#ifndef VIDEO_GENERATOR_H
#define VIDEO_GENERATOR_H

#include <QtScript>
#include <QPrinter>
#include <QWidget>
#include <QFileDialog>
#include <QDomDocument>
#include <QPainter>
#include <QFont>
#include <QTextDocument>
#include <QRectF>

#include <vtkExtractSelectedFrustum.h>
#include <vtkDataSet.h>

#include "../CHE/CHEInfoDialog.h"
#include "../vtkEnums.h"
#include "../visualization/vtkWidget.h"

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

struct CTSurfaceCornerPoint_
{
	double point[4][3];
};

/**
 * This class is to save the current status of 3D object since when generate a video,
 * 3D object screenshot will change the view status. This class serves as a back-up.
 * After the screenshot, the status will be restored.
 */
class WidgetInfo3D_
{
public:
	bool infoStatus;
	bool isMeasuring;
	bool isAnnotation;
	double pos[3]; // camera position
    double foc[3]; // focal point
    double angle; // view angle
    double view[3]; // view up direction
    double clip[2]; // clipping range
    double scale; // parallel scale
	bool isDirectionalLight;
	double orientation[3];
	double brightness;
	double contrast;
	RenderMode3D renderMode;
	int interpolation;
	int textureOn;
};

/**
 * This class is to maintain the object necessary information.
 * Main frame send the instances to the videoGenerate class.
 */

class VideoObject : public QWidget
{
	Q_OBJECT

public:
	WidgetMode mMode;
	QString mName;
	QString mNotesPath;
	QVector<QPair<QString, NoteType> > mNotes;	// note content and type (2D/3D)
	QVector<int> mNoteReorder; // mNoteReorder.size() == mNotes.size() - 1
	QVector<QString> mPictures;	// image notes
	QVector<int> mCategories;	// selected categories for notes
	QString mCHEName;
	VtkWidget* mGla;
	bool isShowGeneral;
};

/**
 * This class is to generate a video based on user's choice from videoFilter dialog.
 */
class VideoGenerator : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  path     The absolute location of the path.
	 * @param  project  Whether the video is about a project or CHE.
	 */
	VideoGenerator(QString path, bool project = true);
	/**
	 * @brief  Set necessary information for the video if it is a project.
	 */
	void setProjectName(const QString project)	{mProjectName = project;}
	void setUserName(const QString userName)	{mUserName = userName;}
	void setKeyword(const QString keyword)	{mKeyword = keyword;}
	void setAffiliation(const QString affiliation)	{mAffiliation = affiliation;}
	void setDescription(const QString description)	{mDescription = description;}
	void setUpdateSetup(const QString project, const QString userName, const QString affiliation, const int videoFormat, const int resolutionOption,
		const int frameDuration2D, const int transDuration2D, const int frameDuration3D, const int transDuration3D, const int dolly3D, const bool isShowGeneral);

	/**
	 * @brief  Set CHE text information for the video if it is a CHE.
	 */
	void setCHEInfo(const CHEInfoBasic* info);

	/**
	 * @brief  Provide API for main frame to add a video object.
	 * @param  object  The added object.
	 */
	void addObject(VideoObject* object)	{mObjects.push_back(object);}

	/**
	 * @brief  Start to generate video.
	 */
	void generate();

	/**
	 * @brief  Find main frame.
	 */
	static MainWindow* mw();

//////////////////////////////////////////////////
private:
	/**
	 * @brief  Detect whether the points can be seen from current directon.
	 *         There are six directions to generate screenshot for 3D object.
	 *         If the point is visible then transform it from world coordinate to image coordinate.
	 *         This function is used to mark note labels as id numbers on screenshot for 3D images.
	 *         It works for point note and surface note. For surface note, the center point of the surface
	 *         is calculated and tested. If the center point is visible then we mark the note label.
	 * @param  render         The current vtkRenderer that shows the object.
	 * @param  points         The vector of points to be detected in world coordinate.
	 * @param  visiblePoints  The vector of visible points in image coordinate.
	 */
	void detectPointVisibility(vtkSmartPointer<vtkRenderer> render, 
		QVector<double*> points, QVector<QPair<double, double> >& visiblePoints);

	/**
	 * @brief  Detect whether the frustum can be seen from current directon.
	 *         There are six directions to generate screenshot for 3D object.
	 *         If the point is visible then transform it from world coordinate to image coordinate.
	 *         This function is used to mark note labels as id numbers on screenshot for 3D images.
	 *         It works for frustum note. The center point of frustum is calculated and serves as the
	 *         origin point of ray tracing to the opposite of viewing direction. The intersection of
	 *         the ray and the 3D object is used to test whether the frustum is visible.
	 * @param  gla            The vtkWidget of the 3D object to get camera info.
	 * @param  center         The center point of the frustum.
	 * @param  visiblePoints  The vector of visible points in image coordinate.
	 * @param  dataset        The 3D object dataset.
	 * @param  view           The view direction.
	 */
	void detectFrustumVisibility(const VtkWidget* gla, QVector<double*> center,
		QVector<QPair<double, double> >& visiblePoints, QVector<vtkSmartPointer<vtkDataSet> > dataset, OrthogonalView3D view);


	/**
	 * @brief  Detect whether the points can be seen from current directon.
	 *         There are six directions to generate screenshot for 3D object.
	 *         If the point is visible then transform it from world coordinate to image coordinate.
	 *         This function is used to mark note labels as id numbers on screenshot for 3D images.
	 *         It works for point note and surface note. For surface note, the center point of the surface
	 *         is calculated and tested. If the center point is visible then we mark the note label.
	 * @param  render         The current vtkRenderer that shows the object.
	 * @param  points         The vector of points to be detected in world coordinate.
	 * @param  visiblePoints  The vector of visible points in image coordinate.
	 */
	void detectCTSurfaceVisibility(vtkSmartPointer<vtkRenderer> render, 
		QVector<QPair<double*, CTSurfaceCornerPoint_> > points, QVector<QPair<double*, CTSurfaceCornerPoint_> >& visiblePoints);

	/**
	 * @brief  Save the current vtkWidget info to prepare for the screenshot.
	 * @param  gla   The vtkWidget of the 3D object.
	 * @param  info  The saved widget info.
	 */
	void saveWidgetinfo(const VtkWidget* gla, WidgetInfo3D_ &info);

	/**
	 * @brief  Init the vtkWidget for the screenshot.
	 * @param  gla         The vtkWidget of the 3D or CT Volume object.
	 * @param  isCTVolume  If the widget is CT Volume.
	 */
	void initWidget(VtkWidget* gla, bool isCTVolume);

	/**
	 * @brief  recover the vtkWidget after screenshot.
	 * @param  gla         The vtkWidget of the 3D or CT Volume object.
	 * @param  info        The previous saved widget info.
	 * @param  isCTVolume  If the widget is CT Volume.
	 */
	void recoverWidget(VtkWidget* gla, WidgetInfo3D_ info, bool isCTVolume);

	/**
	 * @brief  Compute center point for surface note for 3D object.
	 * @param  gla			The vtkWidget of the 3D object.
	 * @param  cellIds		The selected cell ids in the surface.
	 * @param  center		The center point in world coordinate.
	 * @param  centerCellId	The cell ID that contains the computed center.
	 */
	void computeCenter(const VtkWidget* gla, QVector<int> cellIds, double* center, vtkIdType* centerCellId);

	/**
	 * @brief  Compute center point for frustum note for 3D object.
	 * @param  gla       The vtkWidget of the 3D object.
	 * @param  points    The center points of planes that defines the frustum.
	 * @param  normals   The normal vectors of planes that defines the frustum.
	 * @param  center    The center point in world coordinate.
	 * @param  polyData  The 3D object dataset.
	 */
	void computeCenter(const VtkWidget* gla, vtkSmartPointer<vtkPoints> points,
		vtkSmartPointer<vtkDataArray> normals, double* center, vtkSmartPointer<vtkDataSet>& polyData);

	/**
	 * @brief  Compute center point for surface note for CT Volume data.
	 * @param  cornerPoints  The vector of four corner points that define the surface.
	 * @param  center        The center point in world coordinate.
	 */
	void computeCenter(CTSurfaceCornerPoint_ cornerPoints, double* center);

	/**
	 * @brief  Resize screenshots to the size of video without changing aspect ratio.
	 * @param  src		Source image.
	 * @param  mysize	The size of video.
	 * @return Resized frame for generating videos.
	 */
	cv::Mat resize2Video(cv::Mat& src, cv::Size mysize);

	/**
	 * @brief  Put annotations in the subtitle.
	 * @param  src		Source image.
	 * @param  mystr	The annotation of object.
	 * @param  mysize	The size of video.
	 * @param  img		The path of associated image.
	 * @return New image with annotation in the subtitle.
	 */
	cv::Mat putSubtitle(cv::Mat& src, std::string mystr, cv::Size mysize, std::string img = "");

	/**
	 * @brief  Emphasize the note region (rectangle), other parts blurred and darkened
	 * @param  src		Source image.
	 * @param  roi		The region of interest.
	 * @return New image with the emphasized note.
	 */
	cv::Mat emphasizeNote(cv::Mat& src, cv::Rect roi);

	/**
	 * @brief  Emphasize the note region (circle), other parts blurred and darkened
	 * @param  src		Source image.
	 * @param  center	The center of circle.
	 * @param  radius	The radius of circle.
	 * @return New image with the emphasized note.
	 */
	cv::Mat emphasizeNote(cv::Mat& src, cv::Point center, int radius);

	/**
	 * @brief  Extract note content and the first associated image
	 * @param  content	Note body with header omitted.
	 * @return Note content (first) and the path of associated image (second).
	 */
	QPair<QString, QString> parseTextAndImg(QString content);

	/**
	 * @brief  Blender two images to make smooth transition
	 * @param  img1			The first image.
	 * @param  img2			The second image.
	 * @param  outputVideo	The video writer.
	 */
	void blend2Video(cv::Mat& img1, cv::Mat& img2, cv::VideoWriter& outputVideo);

	void generatePointNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<QPair<int, int>, QString> >& pointNote2D);
	void generateSurfaceNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<int*, QString> >& surfaceNote2D);
	void generatePolygonNote2D(int noteid, cv::VideoWriter& outputVideo, cv::Mat& frame, cv::Mat& prevFrame, cv::Mat& currFrame, QVector<QPair<QVector<QPair<int, int> >, QString> >& polygonNote2D);
	void generatePointNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<QPair<int, double*>, QString> >& pointNote3D);
	void generateSurfaceNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<QPair<int, double*>, QString> >& surfaceNote3D);
	void generateFrustumNote3D(int i, int noteid, cv::VideoWriter& outputVideo, double* prevCam, double* currCam, cv::Mat& currFrame, QString& screenshotObj, QString& screenshotDict, QVector<QPair<double*, QString> >& frustumNote3D);

private:
	bool isProject;
	bool isWmv;
	QString mLocation;
	QString mProjectName;
	QString mUserName;
	QString mKeyword, mAffiliation, mDescription;
	CHEInfoBasic* mCHEInfo;
	QVector<VideoObject*> mObjects;

	// user preferences from videoSetup
	int mVideoFormat; // 0: DIV3, 1: MP42, 2: WMV2
	int mResolutionOption; // 0: 800*600, 1: 1600*900, 2: 1600*1200
	cv::Size mysize;
	int mFrameDuration2D;
	int mTransDuration2D;
	int mFrameDuration3D;
	int mTransDuration3D;
	int mDolly3D;
	bool mShowGeneral;
};

#endif // VIDEO_GENERATOR