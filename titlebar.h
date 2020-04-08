/*
* Copy Right (c) LTECH Corp. 2012-2020. All m_rights Reserved.
* 
* Authors: CaiYangLe, 2013.1
* Version: 1.0.0.0
*
* Purpose: �Զ����������������
* Note:  �޸�ϸ����ʾ modify by litianming   2014��11��3��10:32:56
* Note2:   
*/


#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QProgressBar>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    TitleBar(const QString &strTitle, QMainWindow *pParent = nullptr);
	virtual ~TitleBar();

	// ���ñ���ӿ�
    void SetTitle(const QString &strTitle);
	// ������󻯰�ť�Ƿ�
	void SetMaxBtnEnabled(bool bMaxBtnEnabled);
    // ����
    void TranslateLanguage();   
	void ShowMaxBtn(bool b = true);
signals:
    // �˳�
    void slgExit();
	void slgMax();
	void slgRestore();
public slots:
    // ���ù�����
    void slotSetProjectName(const QString& strProjName);
    // ���ò���������
    void slotSetPlName(const QString& strPLName);
	void slotShowMin();
	void showMax();
	void showRestore();
	void slotClose();
protected:
	// ��굥���¼�
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	// ����ƶ��¼�
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	// ����ͷ��¼�
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
	// ���˫���¼�
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent) override;
private:
	// ��ʼ��
    void Init(const QString &strTitle);
    QString getWeek(QString dataTime);
private:
	QMainWindow*	m_mainWindow;
	// �����
    QPoint m_ptPress;
	// �Ƿ��ƶ�
    bool m_bIsMove;
	// ������С������ڱ���������
	float m_fScale;

	// ��С����ť
    QPushButton* m_pBtnMin;
	// ���/��ԭ��ť
    QPushButton* m_pBtnMax;
	QPushButton* m_pBtnRevert;
    QPushButton* m_pBtnClose;
	// ������ʾ
    QLabel* m_pLblTitle;
    // ������
    QLabel* m_pProjname;
    // ��Ŀ������
    QLabel* m_pPLname;
    // ��������
    QLabel* m_pDate;            
	QPoint m_oldPos;
	QPoint m_move_pos;
	bool   m_mouseDown;
	bool   m_left,m_right,m_bottom;
};

#endif // TITLEBAR_H
