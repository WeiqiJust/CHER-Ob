#include <QtGui>

#include "glviewlight.h"

GLViewLight::GLViewLight(const Scene* const s, QWidget* parent)
  : scene_(s), QGLWidget(parent)
{
}

//GLViewLight::GLViewLight(QWidget *parent)
//    : QGLWidget(parent)
//{
////    rotationX = -21.0;
////    rotationY = -57.0;
////    rotationZ = 0.0;
////    faceColors[0] = Qt::red;
////    faceColors[1] = Qt::green;
////    faceColors[2] = Qt::blue;
////    faceColors[3] = Qt::yellow;
//}

void GLViewLight::initializeGL()
{
  setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
  qglClearColor(Qt::black);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  setAttribute(Qt::WA_DeleteOnClose);
}

void GLViewLight::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLfloat x = GLfloat(width) / height;
  glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
  glMatrixMode(GL_MODELVIEW);
}

void GLViewLight::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw();
}

void GLViewLight::mousePressEvent(QMouseEvent *event)
{
  lastPos = event->pos();
}

void GLViewLight::mouseMoveEvent(QMouseEvent *event)
{
  GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
  GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();

  if (event->buttons() & Qt::LeftButton) {
      rotationX += 180 * dy;
      rotationY += 180 * dx;
      updateGL();
  } else if (event->buttons() & Qt::RightButton) {
      rotationX += 180 * dy;
      rotationZ += 180 * dx;
      updateGL();
  }
  lastPos = event->pos();
}

void GLViewLight::mouseDoubleClickEvent(QMouseEvent *event)
{
  int face = faceAtPosition(event->pos());
  if (face != -1) {
    QColor color = QColorDialog::getColor(faceColors[face], this);
    if (color.isValid()) {
        faceColors[face] = color;
        updateGL();
    }
  }
}

void GLViewLight::draw()
{
//  static const GLfloat P1[3] = { 0.0, -1.0, +2.0 };
//  static const GLfloat P2[3] = { +1.73205081, -1.0, -1.0 };
//  static const GLfloat P3[3] = { -1.73205081, -1.0, -1.0 };
//  static const GLfloat P4[3] = { 0.0, +2.0, 0.0 };

//  static const GLfloat * const coords[4][3] = {
//      { P1, P2, P3 }, { P1, P3, P4 }, { P1, P4, P2 }, { P2, P4, P3 }
//  };

//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//  glTranslatef(0.0, 0.0, -10.0);
//  glRotatef(rotationX, 1.0, 0.0, 0.0);
//  glRotatef(rotationY, 0.0, 1.0, 0.0);
//  glRotatef(rotationZ, 0.0, 0.0, 1.0);

//  for (int i = 0; i < 4; ++i) {
//      glLoadName(i);
//      glBegin(GL_TRIANGLES);
//      qglColor(faceColors[i]);
//      for (int j = 0; j < 3; ++j) {
//          glVertex3f(coords[i][j][0], coords[i][j][1],
//                     coords[i][j][2]);
//      }
//      glEnd();
//  }

    scene_->draw(); // draws a spiral
//  // Draws a spiral
//  const float nbSteps = 200.0;
//  glBegin(GL_QUAD_STRIP);
//  for (float i=0; i<nbSteps; ++i)
//    {
//      float ratio = i/nbSteps;
//      float angle = 21.0*ratio;
//      float c = cos(angle);
//      float s = sin(angle);
//      float r1 = 1.0 - 0.8f*ratio;
//      float r2 = 0.8f - 0.8f*ratio;
//      float alt = ratio - 0.5f;
//      const float nor = 0.5f;
//      const float up = sqrt(1.0-nor*nor);
//      glColor3f(1.0-ratio, 0.2f , ratio);
//      glNormal3f(nor*c, up, nor*s);
//      glVertex3f(r1*c, alt, r1*s);
//      glVertex3f(r2*c, alt+0.05f, r2*s);
//    }
//  glEnd();

}

int GLViewLight::faceAtPosition(const QPoint &pos)
{
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(GLdouble(pos.x()), GLdouble(viewport[3] - pos.y()),
                  5.0, 5.0, viewport);
    GLfloat x = GLfloat(width()) / height();
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    draw();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    if (!glRenderMode(GL_RENDER))
        return -1;
    return buffer[3];
}
