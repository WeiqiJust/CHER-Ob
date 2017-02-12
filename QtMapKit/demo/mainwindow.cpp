#include "mainwindow.h"
#include <QCoreApplication>
 
MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent)
{
 m_pWebView = new QWebView(this);
 //set position and size
 m_pWebView->setGeometry(0,0,800,800);
 //m_pWebView->load(QUrl("http://www.google.com"));
 m_pWebView->load(QUrl("qrc:///gmap_.html"));
}

MainWindow::~MainWindow()
{
}