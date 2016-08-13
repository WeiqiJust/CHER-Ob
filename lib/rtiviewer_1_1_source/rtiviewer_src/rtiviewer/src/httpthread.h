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


#ifndef HTTPGETTER_H
#define HTTPGETTER_H

#include "rti.h"

#include <QThread>
#include <QUrl>
#include <QHttp>
#include <QBuffer>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>

//! Class for a request of tile.
/*!
  The class contains info about a tile request for a remote RTI image.
*/
class TileRequest
{
public:
	int row; /*!< Row of the tile. */
	int col; /*!< Column of the tile. */
	int zIndex; /*!< Z-index of the tile. */
	int level; /*!< Mip-mapping level of the tile. */
	QBuffer* buffer; /*!< Pointer to the tile data. */

	int nTry; /*!< Number of failed request. */

	//! Constructor.
	/*!
	  \param r row.
	  \param c column.
	  \param z z-index.
	  \param l mip-mapping level.
	*/
	TileRequest(int r, int c, int z, int l) :
	  row(r),
	  col(c),
	  zIndex(z),
	  level(l),
	  nTry(0)
	{
		buffer = new QBuffer;
		buffer->open(QIODevice::ReadWrite);
	};


    //! Deconstructor.
	~TileRequest()
	{
		delete buffer;
	};

};


//! Worker for HTTP thread.
/*!
  Worker used by the HTTP thread to get the tiles of the image.
*/
class HttpWorker : public QObject
{
	Q_OBJECT

private:

	Rti* image; /*!< Pointer to RTI image. */

	QHttp* http; /*!< Http getter. */
	QUrl* url; /*!< Base url for the request.*/

	QMutex& mutex; /*!< Mutex to provide a mutual exclusion lock between the GUI thread and the HTTP thread. */
	QWaitCondition& infoReady; /*!< Wait condition to synchronize the GUI thread and the HTTP thread. */

	bool httpError; /*!< Holds whether an HTTP error occurred. */

	bool syncRequest; /*!< Holds whether the request is synchronous with the GUI thread (the GUI thread waits the reply). */ 
	int idReq; /*!< Indentifier of HTTP request. */
	bool received; /*!< Holds whether the reply of the last request is already received. */

	QTimer* timer; /*!< Timer to manage the sending of the requests. */

	int startRow, startCol, endRow, endCol; /*<! Info about the tiles contained in the current view of the browser. */
	QRect subImg; /*!< Sub-image coordinates for the current view of the browser. */ 
	TileRequest* current; /*!< Current tile request. */
	TileRequest* next; /*!< Next tile request. */

	bool aborted; /*!< Holds whether the request is failed. */

	QString filename; /*!< Name of the file to download. */

public:
	
	//! Constructor.
	/*!
	  \param m reference to mutex used for the mutual exclusion with GUI thread.
	  \param w reference to wait condition used for the synchronization with GUI thread.
	  \param parent parent object.
	*/
	HttpWorker(QMutex& m, QWaitCondition& w, QObject* parent = 0);

	//! Deconstructor.
	~HttpWorker();

	/*!
	  Checks if an HTTP error occurred.
	*/
	bool errorState();

	/*!
	  Reset the HTTP error state.
	*/
	void resetErrorState();
	
private:
	
	/*!
	  Computes the info about the next tile request. The three parameters get the info of the current request
	  and return the info of the next request.
	  \param row reference to the row of the tile.
	  \param col reference to the column of the tile.
	  \param level reference to the level of the tile.
	  \return true if there is a next tile to request, false otherwise.
	*/
	bool nextTile(int& row, int& col, int& level);

	/*!
	  Computes the info about the first tile request for a specific level of mip-mapping.
	  \param row reference to the row of the tile.
	  \param col reference to the column of the tile.
	  \param i control parameter.
	  \param level level of the tile.
	*/
	void firstTile(int& row, int& col, int i, int level);

private slots:

	/*!
	  The slot is invoked when HTTP reply is received.
	  \param id identifier of the request.
	  \param error flag to indicate an HTTP error.
	*/
	void httpRequestFinished(int id, bool error);

	/*!
	  The slot is invoked when a HTTP reply header is received.
	  \param responseHeader reply header.
	*/
	void readResponseHeader(const QHttpResponseHeader & responseHeader);

	/*!
	  The slot is invoked to send the current tile request.
	*/
	void sendRequestTiles();

public slots:

	/*!
	  Sets the base url for the request.
	*/
	void setUrl(QUrl* url, QString name);

	/*!
	  Sets the pointer to RTI image.
	*/
	void setRti(Rti* rti);

	/*!
	  Sends the request for the xml file info.
	  \param b pointer to buffer to fill with the xml file info.
	*/
	void requestInfo(QBuffer* b);

	/*!
	  Sends the request for the thumbnail.
	  \param b pointer to buffer to fill with the thumbnail.
	*/
	void requestThumb(QBuffer* b);

	/*!
	  The slot is invoked to begin the sending of requests.
	*/
	void startRequestTiles();


	/*!
	  Updates the info about the sub-image in the current view of the browser.
	*/
	void updateView(QRectF r);


	/*!
	  Stops the sending of requests.
	*/
	void abort();

signals:

	/*!
	  Signals a error.
	*/
	void errorOccurred(QString error);

	/*!
	  Constrains the refresh of the browser.
	*/
	void imageUpdated();

	/*!
	  Signals the end of the downloading of the tiles.
	*/
	void downloadFinished();
};



//! Http thread
/*!
  Thread to download a remote RTI image.
*/
class HttpThread: public QThread 
{

	Q_OBJECT

// private data members
private:
	
	HttpWorker* worker; /*!< Pointer to the worker. */

// construction
public:

	//! Constructor.
	/*!
	  \param m reference to mutex used for the mutual exclusion with GUI thread.
	  \param w reference to wait condition used for the synchronization with GUI thread.
	*/
	HttpThread(QMutex& m, QWaitCondition& w);

	/*!
	  Main function of the thread.
	*/
	void run();

	/*!
	  Sets the base url.
	*/
	void setUrl(QUrl* u, QString name)
	{
		emit urlChanged(u, name);
	}

	/*!
	  Sets the Rti image.
	*/
	void setRti(Rti* rti)
	{
		emit rtiChanged(rti);
	}

	/*!
	  Requests the xml file info.
	  \param b pointer to the buffer to fill with the xlm file info.
	*/
	void getInfo(QBuffer* b)
	{
		emit sendRequestInfo(b);
	}

	/*!
	  Requests the thumbnail.
	  \param b pointer to the buffer to fill with the thumbnail.
	*/
	void getThumb(QBuffer* b)
	{
		emit sendRequestThumb(b);
	}

	/*!
	  Stats the sending of the tile requests.  
	*/
	void getTiles()
	{
		emit sendRequestTiles();
	}

	/*!
	  Stops the sending of the tile requests.
	*/
	void closeConnection()
	{
		emit abort();
	}

	/*!
	  Checks if an HTTP error occurred.
	*/
	bool checkHttpError()
	{
		return worker->errorState();
	}

	/*!
	  Reset the HTTP error state.
	*/
	void resetHttpError()
	{
		worker->resetErrorState();
	}

signals:

	// Signals to comunicate with the HTTP worker.
	void urlChanged(QUrl* u, QString name);
	void rtiChanged(Rti* rti);
	void sendRequestInfo(QBuffer* b);
	void sendRequestThumb(QBuffer* b);
	void sendRequestTiles();
	void errorOccurred(QString error);
	void imageUpdated();
	void downloadFinished();
	void viewUpdated(QRectF r);
	void abort();
};

#endif /* HTTPGETTER_H */
