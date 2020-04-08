#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenuBar>  
#include <QToolBar>  
#include <QDockWidget>  
#include <QStatusBar>
#include <QLabel>
#include <QTabWidget>
//#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include "CCreateFileDlg.h"
class picture : public QMainWindow
{
    Q_OBJECT

public:
    picture(QWidget *parent = Q_NULLPTR);

public slots:
    void slotCloseCheackSystem();
private:
    void createMenu();                                  // 创建菜单栏
    void createToolBar();                               // 创建工具栏
    void createStatusBars();							// 创建状态栏;
    void createAction();
    void createDockWidgets();
    void createConnectToolBar();

private slots:
    void onNewFile();
    void onOpenFile();
    void onDeleteFile();
    void onSaveFile();
    void onSaveOther();

    void onReSetView();
private slots:
    void slotCreateCircle();
    void slotCreateOval();
    void slotCreateRect();
    void slotCreateTri();


private:
    void saveDockFrame();
    void readDockFrame(bool& bExistObjTree, bool& bExistRemain1Dock,bool& bExistRemain2Dock);

    void OnCreateFile(TNFileParam& tParam);
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

    // 视图菜单下拉
    QAction*     m_pRetView;


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
    QTabWidget*     m_previewWindow;
    QDockWidget*   m_pRemain1DockWidget;
    QDockWidget*   m_pRemain2DockWidget;
    QTabWidget*    m_pRemain1TabWidget;
    QTabWidget*    m_pRemain2TabWidget;

};
