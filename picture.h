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
    void createMenu();                                  // �����˵���
    void createToolBar();                               // ����������
    void createStatusBars();							// ����״̬��;
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

    // ��ͼ�˵�����
    QAction*     m_pRetView;


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
    QTabWidget*     m_previewWindow;
    QDockWidget*   m_pRemain1DockWidget;
    QDockWidget*   m_pRemain2DockWidget;
    QTabWidget*    m_pRemain1TabWidget;
    QTabWidget*    m_pRemain2TabWidget;

};
