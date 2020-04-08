#ifndef QBASEWIDGET_H
#define QBASEWIDGET_H

#include <QFrame>
#include <QMainWindow>
#include <windowsx.h>
#include <windows.h>
#include "titlebar.h"

enum WidgetRegion
{
	Top         = 0,
	TopRight    = 1,
	Right       = 2,
	RightBottom = 3,
	Bottom      = 4,
	LeftBottom  = 5,
	Left        = 6,
	LeftTop     = 7,
	Inner       = 8,
	Title       = 9,
};


class QMainFrame : public QFrame
{
	Q_OBJECT

public:
	QMainFrame(const QString& strTile,
        QMainWindow* pMainWindow,
        QWidget *parent = 0);

	~QMainFrame();
	void setMaxsizeWindow();
signals:	void sigCloseCheackSystem();private:
	WidgetRegion HitTest(const QPoint& pos);
	long ChangeRegionsToWinHit(const WidgetRegion eRegion);
	void MakeRegions();

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);	
	virtual void resizeEvent(QResizeEvent *);

protected slots:
    void slotClose();
	void slotRestore();
	void slotMax();
	void slotLockSystem(bool bLock);
	void slotSystemStatus(bool bCtrl);

private:
	int m_nWidth;					//窗口宽度
	int m_nHeight;					//窗口高度
	TitleBar* m_titleBar;
	QRect m_regions[10];
	QRect m_ScreencRect;
	QRect m_qRect;
};

#endif // QBASEWIDGET_H
