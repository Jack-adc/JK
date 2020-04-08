#ifndef _QMESSAGBOX_DLG_H_
#define _QMESSAGBOX_DLG_H_

#include <QWidget>
#include <QFont>
#include <QMessageBox>
typedef enum
{
	E_MSGTYPE_INFO = 1,						// 提示;
	E_MSGTYPE_QUES = 2,						// 问题;
	E_MSGTYPE_WARN = 3,						// 警告;
	E_MSGTYPE_CRIT = 4,						// 错误;
	E_MSGTYPE_TIME = 5,						// 定时;
}E_MSGTYPE;

class CMessage : public QWidget  
{
    Q_OBJECT
private slots:
	void slotTimeOut();
public:
    CMessage(E_MSGTYPE eType, QString &strInfo,QWidget *parent = 0);
    ~CMessage();
    int exec();

private:
    E_MSGTYPE m_eType;
    QString m_strInfo;
	QFont m_tFont;
	QMessageBox *m_pMsgBox;
	int m_nWaitTime;//倒计时等待时间，单位秒;  
};

#endif // QMESSAGEDLG_H
