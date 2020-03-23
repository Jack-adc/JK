#include "picture.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QFile>
int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);//必须添加本行，否则opengl生成的图像无法显示
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
  
    picture w;
    w.show();
    return a.exec();
}
