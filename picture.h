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
    // 主菜单
    QMenu*       m_pFileMenu;
    QMenu*       m_pEditMenu;
    QMenu*       m_pViewMenu;
    QMenu*       m_pInsertMenu;
    QMenu*       m_pModeMenu;
    QMenu*       m_pToolMenu;
    QMenu*       m_pHelpMenu;

    // 文件菜单的下拉菜单
    QAction*     m_pNewFile;
    QAction*     m_pOpenFile;
    QAction*     m_pSaveFile;
    QAction*     m_pSaveOther; // 另存为


    QAction*     m_pCreateCircle;
    QAction*     m_pCreateRectangle;
    QAction*     m_pCreateOval;
    QAction*     m_pCreateTriangle;

    // 工具栏相关
    QToolBar*    m_pFileToolBar;

    QToolBar*    m_twoDToolBar;
    // 创建对象窗口
    QDockWidget*  m_objTree;// 对象树停靠窗口

    // 显示窗口
    QDockWidget*     m_previewWindow;

	//circleNode*      
};
