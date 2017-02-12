#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QMainWindow>
#include <QWebView>
#include <QUrl>
 
namespace Ui {
 class MainWindow;
}
 
class MainWindow : public QMainWindow
{
 Q_OBJECT
public:
 
 explicit MainWindow(QWidget *parent = 0);
 virtual ~MainWindow();
 
private:
 
 QWebView* m_pWebView;
};
 
#endif // MAINWINDOW_H