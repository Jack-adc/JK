#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenuBar>  
#include <QToolBar>  
#include <QDockWidget>  
#include "circlenode.h"

class picture : public QMainWindow
{
    Q_OBJECT

public:
    picture(QWidget *parent = Q_NULLPTR);

private:
    void createMenu();
    void createToolBar();
    void createAction();
    void createDockWidgets();
    void createConnectToolBar();

private slots:
    void slotCreateCircle();
private:
    // ���˵�
    QMenu*       m_pFileMenu;
    QMenu*       m_pEditMenu;
    QMenu*       m_pViewMenu;
    QMenu*       m_pInsertMenu;
    QMenu*       m_pModeMenu;
    QMenu*       m_pToolMenu;
    QMenu*       m_pHelpMenu;

    // �ļ��˵��������˵�
    QAction*     m_pNewFile;
    QAction*     m_pOpenFile;
    QAction*     m_pSaveFile;
    QAction*     m_pSaveOther; // ���Ϊ


    QAction*     m_pCreateCircle;
    QAction*     m_pCreateRectangle;
    QAction*     m_pCreateOval;
    QAction*     m_pCreateTriangle;

    // ���������
    QToolBar*    m_pFileToolBar;

    QToolBar*    m_twoDToolBar;
    // �������󴰿�
    QDockWidget*  m_objTree;// ������ͣ������

    // ��ʾ����
    QDockWidget*     m_previewWindow;

	//circleNode*      
};
