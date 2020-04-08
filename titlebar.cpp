#include "titlebar.h"
#include <QPushButton>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSettings>
#include <QPixmap>

#include <QPushButton>
#include <QMenuBar>
#include <QDate>


TitleBar::TitleBar(const QString &strTitle, QMainWindow *pParent)
    : QWidget(pParent)
    , m_bIsMove(false)
	, m_mainWindow(pParent)
{
    Init(strTitle);
}

TitleBar::~TitleBar()
{
   
}

//> 初始化
void TitleBar::Init(const QString &strTitle)
{
    QFont font;
    font.setPixelSize(15);
    QLabel *pLblLogo = new QLabel(this);
    pLblLogo->setObjectName("lblTitle");
    pLblLogo->setPixmap(QPixmap(":/Ampctrl/Recources/sysbutton/logo.png"));
    
    pLblLogo->setFixedSize(65, 20);
    pLblLogo->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pLblTitle = new QLabel(strTitle, this);
    m_pLblTitle->setFont(font);
    m_pLblTitle->setObjectName("lblTitle");
    m_pLblTitle->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pBtnMin = new QPushButton(this);
    m_pBtnMin->setObjectName("btnMin");
    m_pBtnMin->setFixedSize(34, 26);
    m_pBtnMin->setToolTip(tr("Minimize"));
    connect(m_pBtnMin, &QPushButton::clicked, this, &TitleBar::slotShowMin);

    m_pBtnMax = new QPushButton(this);
    m_pBtnMax->setObjectName("btnMax");
    m_pBtnMax->setFixedSize(34, 26);
    m_pBtnMax->setToolTip(tr("Maximize"));
	m_pBtnMax->setHidden(true);
    connect(m_pBtnMax, &QPushButton::clicked, this, &TitleBar::showMax);
	m_pBtnMax->setStyleSheet("QPushButton#btnMax { border-image: url(:/Ampctrl/Recources/sysbutton/max.png); }"
		"QPushButton#btnMax:hover { border-image: url(:/Ampctrl/Recources/sysbutton/maxhov.png); }"
		"QPushButton#btnMax:pressed { border-image: url(:/Ampctrl/Recources/sysbutton/maxpress.png); }");

	m_pBtnRevert = new QPushButton(this);
	m_pBtnRevert->setFixedSize(34, 26);
	m_pBtnRevert->setObjectName("btnMax");
	m_pBtnRevert->setToolTip(tr("Revert"));
	connect(m_pBtnRevert, &QPushButton::clicked, this, &TitleBar::showRestore);
	m_pBtnRevert->setStyleSheet("QPushButton#btnMax { border-image: url(:/Ampctrl/Recources/sysbutton/normal.png); }"
		"QPushButton#btnMax:hover { border-image: url(:/Ampctrl/Recources/sysbutton/normalhov.png); }"
		"QPushButton#btnMax:pressed { border-image: url(:/Ampctrl/Recources/sysbutton/normalpress.png); }");

    m_pBtnClose = new QPushButton(this);
	m_pBtnClose->setShortcut(Qt::Key_Alt|Qt::Key_F4);
	// m_pBtnClose->setShortcut(QKeySequence(Qt::Key_Alt + Qt::Key_F4));
    m_pBtnClose->setObjectName("btnClose");
    m_pBtnClose->setFixedSize(34, 26);
    m_pBtnClose->setToolTip(tr("Close"));

    connect(m_pBtnClose, &QPushButton::clicked, this, &TitleBar::slotClose);

    QMenuBar *pMenuBar = m_mainWindow->menuBar();	
	pMenuBar->setFont(font);
    pMenuBar->setFixedWidth(300);
    m_pDate = new QLabel(this);
 
    QString weekDay = getWeek(QDate::currentDate().toString("yyyy-MM-dd"));
    m_pDate->setText(QDate::currentDate().toString("yyyy-MM-dd") + " " + weekDay);

    QHBoxLayout *pHLayoutMain = new QHBoxLayout();
	pHLayoutMain->setAlignment(Qt::AlignBottom);
    pHLayoutMain->addWidget(pLblLogo);
    pHLayoutMain->addWidget(pMenuBar);
    pHLayoutMain->addStretch();
    pHLayoutMain->addWidget(m_pLblTitle, 0, Qt::AlignCenter);
    pHLayoutMain->addStretch();
    pHLayoutMain->addWidget(m_pDate);
    pHLayoutMain->addSpacing(20);
    pHLayoutMain->addWidget(m_pBtnMin);
    pHLayoutMain->addWidget(m_pBtnMax);
	pHLayoutMain->addWidget(m_pBtnRevert);
    pHLayoutMain->addWidget(m_pBtnClose);
    pHLayoutMain->setSpacing(0);
	pHLayoutMain->setMargin(0);
	pHLayoutMain->setContentsMargins(2, 0, 0, 0);
    setLayout(pHLayoutMain);
    setMaximumHeight(26);

	//setStyleSheet("background-color:rgb(37,37,38);");
}

//> 设置标题接口
void TitleBar::SetTitle(const QString &strTitle)
{
    m_pLblTitle->setText(strTitle);
}

void TitleBar::TranslateLanguage()
{
	m_pBtnMin->setToolTip(tr("Minimize"));
	m_pBtnMax->setToolTip(tr("Maximize/Revert"));
	m_pBtnClose->setToolTip(tr("Close"));
}
void TitleBar::slotShowMin()
{
	parentWidget()->showMinimized();
}
void TitleBar::ShowMaxBtn(bool b)
{
	m_pBtnRevert->setHidden(true);
	m_pBtnMax->setHidden(true);
	if (b)
	{
		m_pBtnMax->setHidden(false);
	}
	else
	{
		m_pBtnRevert->setHidden(false);
	}
}
void TitleBar::showRestore()
{
	emit slgRestore();
}
void TitleBar::showMax()
{
	emit slgMax();	
}
void TitleBar::slotClose()
{
    emit slgExit();
}
void TitleBar::slotSetProjectName( const QString& strProjName )
{
    m_pProjname->setText(strProjName);
}

QString TitleBar::getWeek(QString dataTime)
{
    QDate date;
    dataTime = dataTime.trimmed();
    QString format("yyyy-MM-dd");
    date = QDate::fromString(dataTime,format);

    QString strWeek;
    switch (date.dayOfWeek())
    {
    case  1:
        strWeek = tr("Monday");
        break;
    case  2:
        strWeek = tr("Tuesday");
        break;
    case 3:
        strWeek = tr("Wednesday");
        break;
    case 4:
        strWeek = tr("Thursday");
        break;
    case 5:
        strWeek = tr("Friday");
        break;
    case 6:
        strWeek = tr("Saturday");
        break;
    case 7:
        strWeek = tr("Sunday");
        break;
    default:
        strWeek = tr("Error");
        break;
    }
    return strWeek;
}

void TitleBar::slotSetPlName( const QString& strPLName )
{
    m_pPLname->setText(strPLName);
}

//> 鼠标单击事件
void TitleBar::mousePressEvent(QMouseEvent *pEvent)
{
	m_oldPos = pEvent->pos();
	m_move_pos = pEvent->pos();
	m_mouseDown = pEvent->button() == Qt::LeftButton;
}

//> 鼠标移动事件
void TitleBar::mouseMoveEvent(QMouseEvent *pEvent)
{
	if (m_mouseDown && !parentWidget()->isMaximized())
	{
		QPoint move_global_pos = pEvent->globalPos();
		parentWidget()->move(  move_global_pos -m_move_pos);
		parentWidget()->repaint();
	}
}
//> 鼠标双击事件
void TitleBar::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
	if (m_pBtnMax->isHidden())
	{
		emit slgRestore();
	}
	else if (m_pBtnRevert->isHidden())
	{
		emit slgMax();	
	}
}
//> 鼠标释放事件
void TitleBar::mouseReleaseEvent(QMouseEvent *pEvent)
{
	Q_UNUSED(pEvent);

	if (m_bIsMove) {
		m_bIsMove = false;
	}
	m_mouseDown = false;
}
