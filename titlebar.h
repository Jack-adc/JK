/*
* Copy Right (c) LTECH Corp. 2012-2020. All m_rights Reserved.
* 
* Authors: CaiYangLe, 2013.1
* Version: 1.0.0.0
*
* Purpose: 自定义标题栏，标题栏
* Note:  修改细节显示 modify by litianming   2014年11月3日10:32:56
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

	// 设置标题接口
    void SetTitle(const QString &strTitle);
	// 设置最大化按钮是否
	void SetMaxBtnEnabled(bool bMaxBtnEnabled);
    // 翻译
    void TranslateLanguage();   
	void ShowMaxBtn(bool b = true);
signals:
    // 退出
    void slgExit();
	void slgMax();
	void slgRestore();
public slots:
    // 设置工程名
    void slotSetProjectName(const QString& strProjName);
    // 设置播出表名称
    void slotSetPlName(const QString& strPLName);
	void slotShowMin();
	void showMax();
	void showRestore();
	void slotClose();
protected:
	// 鼠标单击事件
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	// 鼠标移动事件
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	// 鼠标释放事件
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
	// 鼠标双击事件
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent) override;
private:
	// 初始化
    void Init(const QString &strTitle);
    QString getWeek(QString dataTime);
private:
	QMainWindow*	m_mainWindow;
	// 点击点
    QPoint m_ptPress;
	// 是否移动
    bool m_bIsMove;
	// 窗口缩小鼠标所在标题栏比例
	float m_fScale;

	// 最小化按钮
    QPushButton* m_pBtnMin;
	// 最大化/还原按钮
    QPushButton* m_pBtnMax;
	QPushButton* m_pBtnRevert;
    QPushButton* m_pBtnClose;
	// 标题显示
    QLabel* m_pLblTitle;
    // 工程名
    QLabel* m_pProjname;
    // 节目单名称
    QLabel* m_pPLname;
    // 当天日期
    QLabel* m_pDate;            
	QPoint m_oldPos;
	QPoint m_move_pos;
	bool   m_mouseDown;
	bool   m_left,m_right,m_bottom;
};

#endif // TITLEBAR_H
