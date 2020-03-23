#ifndef CIRCLENODE_H
#define CIRCLENODE_H

#include <QtWidgets/QWidget>
#include <gl/glut.h>
#include <QOpenGLWidget>
#include"QOpenGLFunctions"

class circleNode : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit circleNode(QWidget *parent = 0);
	~circleNode();
	void initializeGL();
	void paintGL();
	void resizeGL( int width,int height);
private:
	
};

#endif // CIRCLENODE_H
