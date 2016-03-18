#ifndef METADATA_H
#define METADATA_H

#include <QDialog>
#include <QDomDocument>
#include <QFileInfo>
#include <QGroupBox>
#include <QHash>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include <QTreeWidget>
#include <QXmlStreamReader>
#include <QWidget>

#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

#include "bookmarkTreeWidget.h"
#include "bookmarkWidget.h"

class MainWindow;
class VtkWidget;

class DMetadataTreeWidget : public BookmarkTreeWidget
{
    Q_OBJECT

public:
    DMetadataTreeWidget(QWidget* parent = 0);

public slots:
protected:
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

class Metadata : public QDialog
{
  Q_OBJECT

public:
  Metadata(QString path, VtkWidget* w = 0, bool file = false);
  ~Metadata();

  static MainWindow* mw();
  void refreshMetadata();

  QDomDocument xmlFile;
  QPushButton* editButton, *deleteButton;
  QTextEdit* textEdit;


signals:

public slots:
  void updateButtons();

private slots:

  void addCategory();
  void deleteItem();
  void loadText(QTreeWidgetItem* item, int i);
  void showContextMenu(const QPoint&);
  void toggleEdit();

private:

  void keyPressEvent(QKeyEvent* event);

  QTreeWidgetItem* createChildItem(QTreeWidgetItem* item);
  void readXML();
  void readCategory(QTreeWidgetItem *item);
  void readElement(QTreeWidgetItem *item);

  void addElement(QTreeWidgetItem* item, const QString text);
  void buildMetadataHash();
  QString formatFilesize(qint64 bytes);
  void get2DInfo();
  void get3DInfo();
  void pickCategory();
  void saveItem(QTreeWidgetItem* item);

  bool isFile; // DT: true if retrieving metadata for single image, false if project metadata.
  VtkWidget* w; // DT: holds pointer to current VtkWidget if retrieving metadata for single image.

  QDomElement root;
  QString xmlPath;
  QXmlStreamReader xml;

  QFileInfo* finfo;
  QFileInfo* tfinfo;
  QLocale locale;
  QString textureFilename;
  QString mPath;
  vtkImageData* hyperdata;
  vtkSmartPointer<vtkImageViewer2> viewer;
  vtkTexture* texture;
  bool isDICOM;
  int points, polys;

  bool userIsEditing;

  QHash<QString, QStringList> hash;
  QVector<QString> categories;
  QVector<QStringList> elements;

  /* DT: GUI stuff */
  DMetadataTreeWidget* dataTree;
  QGridLayout* grid;
  QGroupBox* groupBox;
  QHBoxLayout* hbox, *hboxButtons;
  QLabel* openGLLabel, *coresLabel, *filenameLabel, *filesizeLabel, *filesizeLabel2, *dimensionsLabel, *dimensionsLabel2;
  QLabel* texFilenameLabel, *texFilesizeLabel, *texFilesizeLabel2, *texDimensionsLabel, *texDimensionsLabel2;
  QLabel* pointLabel, *pointLabel2, *polyLabel, *polyLabel2;
  QLineEdit* filenameLine, *texFilenameLine;
  QPushButton* addButton, *exportButton;
  QVBoxLayout* vbox, *vboxData;
  /* end of GUI stuff... */
};

#endif // METADATA_H
