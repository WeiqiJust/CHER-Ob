#ifndef GLVIEWLIGHT_H
#define GLVIEWLIGHT_H

//====================================================
// OpenGL Header Setting for QT multi-platform
// (Order matters)
//====================================================
#if ((defined(__MACH__))&&(defined(__APPLE__)))
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#endif
#include <QGLWidget>
//====================================================

#include "scene.h"

class GLViewLight : public QGLWidget
{
  Q_OBJECT

public:
//  GLViewLight(QWidget *parent = 0);
  GLViewLight(const Scene* const s=NULL, QWidget *parent = 0);

protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);

private:
  void draw();
  int faceAtPosition(const QPoint &pos);
  const Scene* const scene_;

  GLfloat rotationX;
  GLfloat rotationY;
  GLfloat rotationZ;
  QColor faceColors[4];
  QPoint lastPos;
};

#endif
