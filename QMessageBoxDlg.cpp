#include "QMessageBoxDlg.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <QTimer>

#define WIDTH 60
#define HEIGHT 30

#define MINW 300
#define MINH 180

CMessage::CMessage(E_MSGTYPE eType, QString &strInfo,QWidget *parent)
	:QWidget(parent)
{
    m_eType = eType;
    m_strInfo = strInfo;
	m_tFont.setPointSize(13);
	m_nWaitTime = 5;
}

CMessage::~CMessage()
{

}

 int CMessage::exec()
 {
     bool bRet = false;
     switch (m_eType)
     {
		case E_MSGTYPE_INFO:
			{
				QMessageBox msg(QMessageBox::Information,tr("Information"),m_strInfo,QMessageBox::Yes);
				msg.setMinimumWidth(MINW);
				msg.setMinimumHeight(MINH);

				msg.setButtonText(QMessageBox::Yes,tr("OK"));
				msg.button(QMessageBox::Yes)->setFixedSize(WIDTH, HEIGHT);
				msg.setFont(m_tFont);
				msg.exec();
				bRet = true;
				break;
			}
		case E_MSGTYPE_QUES:
			{
				QMessageBox msg(QMessageBox::Question,tr("Question"),m_strInfo,QMessageBox::Yes | QMessageBox::No);
				msg.setMinimumWidth(MINW);
				msg.setMinimumHeight(MINH);

				msg.setButtonText(QMessageBox::Yes,tr("Yes"));
				msg.setButtonText(QMessageBox::No,tr("No"));
				msg.setDefaultButton(QMessageBox::No);
				msg.button(QMessageBox::Yes)->setFixedSize(WIDTH, HEIGHT);
				msg.button(QMessageBox::No)->setFixedSize(WIDTH, HEIGHT);
				msg.setFont(m_tFont);
				msg.exec();
				if (msg.clickedButton() == msg.button(QMessageBox::Yes))
				{
					bRet = true;
				}
				break;
			}
		case E_MSGTYPE_WARN:
			{
				QMessageBox msg(QMessageBox::Warning,tr("Warning"),m_strInfo,QMessageBox::Yes | QMessageBox::No);

				msg.setButtonText(QMessageBox::Yes,tr("Yes"));
				msg.setButtonText(QMessageBox::No,tr("No"));
				msg.setDefaultButton(QMessageBox::No);
				msg.button(QMessageBox::Yes)->setFixedSize(WIDTH, HEIGHT);
				msg.button(QMessageBox::No)->setFixedSize(WIDTH, HEIGHT);
				msg.setFont(m_tFont);
				msg.exec();
				if (msg.clickedButton() == msg.button(QMessageBox::Yes))
				{
					bRet = true;
				}
				break;
			}    
		case E_MSGTYPE_CRIT:
			{
				QMessageBox msg(QMessageBox::Critical,tr("Error"),m_strInfo,QMessageBox::Yes | QMessageBox::No);
				msg.setMinimumWidth(MINW);
				msg.setMinimumHeight(MINH);

				msg.setButtonText(QMessageBox::Yes,tr("Yes"));
				msg.setButtonText(QMessageBox::No,tr("No"));
				msg.setDefaultButton(QMessageBox::No);
				msg.button(QMessageBox::Yes)->setFixedSize(WIDTH, HEIGHT);
				msg.button(QMessageBox::No)->setFixedSize(WIDTH, HEIGHT);
				msg.setFont(m_tFont);
				msg.exec();
				if (msg.clickedButton() == msg.button(QMessageBox::Yes))
				{
					bRet = true;
				}
				break;
			}
		case E_MSGTYPE_TIME:
			{
				m_pMsgBox = new QMessageBox(QMessageBox::Information,tr("Information"),m_strInfo,QMessageBox::Yes);
				if ( m_pMsgBox != NULL)
				{
					m_pMsgBox->setMinimumWidth(MINW);
					m_pMsgBox->setMinimumHeight(MINH);
				    m_pMsgBox->setButtonText(QMessageBox::Yes,tr("OK")+"("+QString::number(m_nWaitTime)+")");
					QTimer cTimer;
					connect(&cTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));  
					cTimer.start(1000);  //1000ms 表示1秒  
					m_pMsgBox->button(QMessageBox::Yes)->setFixedSize(WIDTH + 10, HEIGHT);
					m_pMsgBox->setFont(m_tFont);
					m_pMsgBox->exec();
					cTimer.stop();
					delete m_pMsgBox;
					m_pMsgBox = NULL;
					bRet = true;
					break;
				}			
			}
		default:
				break;
     }
     return bRet;
 }

 void CMessage::slotTimeOut()
 {
	 m_pMsgBox->setButtonText(QMessageBox::Yes,tr("OK")+"("+QString::number(--m_nWaitTime)+")");
	 if (m_nWaitTime == 0)
	 {
		  m_pMsgBox->accept();
	 }
 }
