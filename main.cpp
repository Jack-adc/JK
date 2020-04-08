#include "picture.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QFile>
#include "qbasewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/picture/Resources/Magic.png"));
    QTranslator qttranslator1;
    qttranslator1.load(":/picture/picture_zh.qm");
    a.installTranslator(&qttranslator1);
//     QFile file(":/picture/BlackSkin.qss");
//     if (!file.open(QFile::ReadOnly))
//         return -1;
// 
//     a.setStyleSheet(file.readAll());
//   file.close();
  
    picture *w = new picture;
    QMainFrame* mainFrame = NULL;
    mainFrame = new QMainFrame(QObject::tr("Picture"), w);
    mainFrame->showNormal();
    mainFrame->setMaxsizeWindow();
    int res = a.exec();
    w->deleteLater();
    delete mainFrame;
    mainFrame = nullptr;
    return a.exec();
}
