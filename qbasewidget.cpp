#include "qbasewidget.h"
#include <QVBoxLayout>

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QDebug>
#include "systemlog.h"

#define  MARGIN  5		// 四个角的长度
#define  TITLEH  30
#define MINWIDTH 800
#define MINHIGHT 600

QMainFrame::QMainFrame(const QString& strTitle,
                       QMainWindow* pMainWindow,
                       QWidget *parent)
	: QFrame(parent)
{
	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, false, NULL, 0);
	setWindowTitle(strTitle);
	setWindowFlags(Qt::FramelessWindowHint );
	setLineWidth(0);
    m_titleBar = new TitleBar(strTitle,pMainWindow);
    QVBoxLayout *pVLayoutMain = new QVBoxLayout(this);
    pVLayoutMain->setContentsMargins(0,0,1,0);
    pVLayoutMain->setSpacing(0);
    pVLayoutMain->addWidget(m_titleBar);
    pVLayoutMain->addWidget(pMainWindow);


	connect(m_titleBar,SIGNAL(slgRestore()),this,SLOT(slotRestore()));
	connect(m_titleBar,SIGNAL(slgMax()),this,SLOT(slotMax()));
    connect(m_titleBar,SIGNAL(slgExit()),this,SLOT(slotClose()));
	//connect(pMainWindow,SIGNAL(sigLockSystem(bool)),this,SLOT(slotLockSystem(bool)));
	connect(this,SIGNAL(sigCloseCheackSystem()),pMainWindow,SLOT(slotCloseCheackSystem()));
	//connect(pMainWindow,SIGNAL(sigCtrlStatus(bool)),this,SLOT(slotSystemStatus(bool)));
    setLayout(pVLayoutMain);	
	m_ScreencRect =  QApplication::desktop()->availableGeometry();
	m_qRect = QRect((m_ScreencRect.width() - MINWIDTH)/2,(m_ScreencRect.height()- MINHIGHT)/2,MINWIDTH,MINHIGHT);
}

QMainFrame::~QMainFrame()
{

}

void QMainFrame::slotLockSystem(bool bLock)
{
	m_titleBar->setDisabled(bLock);
}
void  QMainFrame::resizeEvent(QResizeEvent *event)
{
	m_nWidth = event->size().width();
	m_nHeight = event->size().height();
	MakeRegions();	
}
void QMainFrame::MakeRegions()
{
	m_regions[Top] = QRect(MARGIN, 0, m_nWidth - MARGIN - MARGIN, MARGIN);
	m_regions[TopRight] = QRect(m_nWidth - MARGIN, 0, MARGIN, MARGIN);
	m_regions[Right] = QRect(m_nWidth - MARGIN, MARGIN, MARGIN, m_nHeight - MARGIN - MARGIN);
	m_regions[RightBottom] = QRect(m_nWidth - MARGIN, m_nHeight - MARGIN, MARGIN, MARGIN);
	m_regions[Bottom] = QRect(MARGIN, m_nHeight - MARGIN, m_nWidth - MARGIN - MARGIN, MARGIN);
	m_regions[LeftBottom] = QRect(0, m_nHeight - MARGIN, MARGIN, MARGIN);
	m_regions[Left] = QRect(0, MARGIN, MARGIN, m_nHeight - MARGIN - MARGIN);
	m_regions[LeftTop] = QRect(0, 0, MARGIN, MARGIN);
	m_regions[Inner] = QRect(MARGIN, MARGIN + TITLEH, m_nWidth - MARGIN - MARGIN, m_nHeight - MARGIN - MARGIN - TITLEH);
	m_regions[Title] = QRect(m_nWidth/3, MARGIN, m_nWidth/2, TITLEH);
}
WidgetRegion QMainFrame::HitTest(const QPoint& pos)
{	
	for (int nPos = 0 ; nPos  < 10; nPos++)
	{
		const QRect rect = m_regions[nPos];
		if (rect.contains(pos))
		{
			return WidgetRegion(nPos);
		}
	}
	return Inner;
}
void QMainFrame::setMaxsizeWindow()
{	
	this->resize(m_ScreencRect.width(),m_ScreencRect.height());					
	QPoint newpos(0,0 );
	this->move(newpos);	
	return;	
}
bool QMainFrame::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = (MSG*)message;
	switch (msg->message) 
	{			
	case WM_NCHITTEST:
		{
			int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
			int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();			
 			WidgetRegion eRegion = HitTest(QPoint(xPos,yPos));		
 			*result = ChangeRegionsToWinHit(eRegion); 	
			return true;			
		}
		break;
	case WM_NCCALCSIZE:
		return false;
	case WM_GETMINMAXINFO:
		{
			if (::IsZoomed(msg->hwnd))
			{
				// 最大化时会超出屏幕，所以填充边框间距
				RECT frame = { 0, 0, 0, 0 };
				AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
				frame.left = abs(frame.left);
				frame.top = abs(frame.bottom);
				this->setContentsMargins(frame.left, frame.top, frame.right, frame.bottom);				
			}
			else 
			{
				this->setContentsMargins(2, 2, 2, 2);
			}
			*result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
			return true;
		}
		break;

	case WM_NCLBUTTONDBLCLK:
		{
			int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
			int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();		
			WidgetRegion eRegion = HitTest(QPoint(xPos,yPos));		
			if (ChangeRegionsToWinHit(eRegion) == HTCAPTION)
			{
				if (m_nWidth == m_ScreencRect.width() && m_nHeight == m_ScreencRect.height())
				{
					slotRestore();
				}
				else
				{
					m_qRect = this->geometry();
					slotMax();	
				}			
			}				
		}
		break;
	case WM_SIZING://窗口缩放
		{					
			LPRECT lprc = (LPRECT)msg->lParam;
			if (lprc->right - lprc->left < MINWIDTH)
			{
				//左边，左上、左下
				if (GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_LEFT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_TOPLEFT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_BOTTOMLEFT )
				{
					lprc->left = this->geometry().right() - MINWIDTH;
				}
				//右边，右上、右下
				else if (GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_RIGHT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_TOPRIGHT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_BOTTOMRIGHT)
				{
					lprc->right = this->geometry().left() + MINWIDTH;
				}
			}
			if (lprc->bottom - lprc->top < MINHIGHT)
			{
				//上边，左上、右上
				if (GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_TOP
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_TOPLEFT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_TOPRIGHT )
				{
					lprc->top = this->geometry().bottom() - MINHIGHT;
				}
				//下边，左下、右下
				else if (GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_BOTTOM
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_BOTTOMLEFT
					|| GET_WPARAM(msg->wParam,msg->lParam) == WMSZ_BOTTOMRIGHT)
				{
					lprc->bottom = this->geometry().top() + MINHIGHT;
				}
			}	
		}
		break;
	default:
		break;

	}
	return QFrame::nativeEvent(eventType, message, result);
}
void QMainFrame::slotRestore()
{
	this->setGeometry(m_qRect);
	m_titleBar->ShowMaxBtn();
}
void QMainFrame::slotMax()
{
	m_qRect = this->geometry();
	this->resize(m_ScreencRect.width(),m_ScreencRect.height());					
	QPoint newpos(0,0 );
	this->move(newpos);	
	m_titleBar->ShowMaxBtn(false);
	return;	
}

void QMainFrame::slotClose()
{

	emit sigCloseCheackSystem();
// 	添加日志
// 		QString strLog = tr("user exit system");
// 		CSystemLogDlg::GetInstance()->WriteLog(strLog,E_LOG_FATAL);	 
// 	
// 		QMessageBox msg(QMessageBox::Question,tr("Tip"),tr("Confirm Exit the program?"),QMessageBox::Yes | QMessageBox::No);
// 		msg.setButtonText(QMessageBox::Yes,tr("Yes"));
// 		msg.setButtonText(QMessageBox::No,tr("No"));
// 		msg.setDefaultButton(QMessageBox::No);
// 		msg.exec();		 	
// 		if (msg.clickedButton() == msg.button(QMessageBox::No)) 
// 		{    		
// 			return;
// 		}  
// 		exit(-1);	
}
long QMainFrame::ChangeRegionsToWinHit(const WidgetRegion eRegion)
{	
	switch (eRegion)
	{
	case Left:
		return HTLEFT;
	case Right:
		return HTRIGHT;
	case Top:
		return HTTOP;
	case Bottom:
		return HTBOTTOM;
	case LeftTop:
		return HTTOPLEFT;
	case TopRight:
		return HTTOPRIGHT;
	case LeftBottom:
		return HTBOTTOMLEFT;
	case RightBottom:
		return HTBOTTOMRIGHT;
	case Title:
		return HTCAPTION;
	default:
		return HTCLIENT;
		break;
	}
	return HTCLIENT;
}

void QMainFrame::slotSystemStatus(bool bCtrl)
{
	if (bCtrl)
	{
		m_titleBar->SetTitle(QObject::tr("Ltech StudioPlay studio broadcast control(Ctrl)"));
	}
	else
	{
		m_titleBar->SetTitle(QObject::tr("Ltech StudioPlay studio broadcast control"));
	}
}