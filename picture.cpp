#include "picture.h"
#include <QMenuBar>  
#include <QTreeWidget>
#include <QOpenGLWidget>
#include <QDesktopServices>
#include <QApplication>
#include <QSettings>
#include <QFile>
#include "CCreateFileDlg.h"
#include "CShowWidget.h"
#define MINWIDTH 1200
#define MINHIGHT  500
#define LOGPATH "./log"

picture::picture(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("Imaginary"));
    this->createAction();
    this->createMenu();
    this->createToolBar();
    this->createDockWidgets();
    createStatusBars();
    this->showMaximized(); // 打开时全屏
    QRect ScreencRect = QApplication::desktop()->availableGeometry();
    this->setGeometry(0, 0, ScreencRect.width(), ScreencRect.height());
    this->setMinimumSize(QSize(1000, 600));
 }

void picture::createMenu()
{
    m_pFileMenu = menuBar()->addMenu(tr("File"));
    m_pEditMenu = menuBar()->addMenu(tr("Edit"));
    m_pViewMenu = menuBar()->addMenu(tr("ViewMode"));
    m_pInsertMenu = menuBar()->addMenu(tr("Insert"));
    m_pModeMenu = menuBar()->addMenu(tr("Mode"));
    m_pToolMenu = menuBar()->addMenu(tr("Tool"));
    m_pHelpMenu = menuBar()->addMenu(tr("Help"));

    m_pFileMenu->addAction(m_pNewFile);
    m_pFileMenu->addAction(m_pOpenFile);
    m_pFileMenu->addAction(m_pSaveFile);
    m_pFileMenu->addAction(m_pSaveOther);

    m_pViewMenu->addAction(m_pRetView);

}

void picture::createToolBar()
{
    m_pFileToolBar = addToolBar(tr("File"));
    m_pFileToolBar->setContextMenuPolicy(Qt::PreventContextMenu);		// 设置QToolBar不响应右键;
    m_pFileToolBar->addAction(m_pNewFile);
    m_pFileToolBar->addAction(m_pOpenFile);
    m_pFileToolBar->addAction(m_pSaveFile);
    m_pFileToolBar->addAction(m_pSaveOther);
    m_pFileToolBar->addSeparator();
    m_twoDToolBar = addToolBar(tr("2D"));
    m_twoDToolBar->setContextMenuPolicy(Qt::PreventContextMenu);		// 设置QToolBar不响应右键;
    m_twoDToolBar->addAction(m_pCreateCircle);
    m_twoDToolBar->addAction(m_pCreateOval);
    m_twoDToolBar->addAction(m_pCreateRectangle);
    m_twoDToolBar->addAction(m_pCreateTriangle);
}

void picture::createAction()
{
    m_pNewFile = new QAction(tr("New File"), this);
    m_pNewFile->setIcon(QIcon(":/picture/Resources/NewFile.png"));
    m_pOpenFile = new QAction(tr("Open File"), this);
    m_pOpenFile->setIcon(QIcon(":/picture/Resources/OpenFile.png"));
    m_pSaveFile = new QAction(tr("Save File"), this);   
    m_pSaveFile->setIcon(QIcon(":/picture/Resources/SaveFile.png"));
    m_pSaveOther = new QAction(tr("Save in other path"), this);
    m_pSaveOther->setIcon(QIcon(":/picture/Resources/SaveAs.png"));

    m_pCreateCircle = new QAction(tr("Create circle"), this);
    m_pCreateCircle->setIcon(QIcon(":/picture/Resources/circle.png"));
    m_pCreateOval = new QAction(tr("Create Oval"), this);
    m_pCreateOval->setIcon(QIcon(":/picture/Resources/oval.png"));
    m_pCreateRectangle = new QAction(tr("Create Rectangle"), this);
    m_pCreateRectangle->setIcon(QIcon(":/picture/Resources/Rectangle.png"));
    m_pCreateTriangle = new QAction(tr("Create Triangle"), this);
    m_pCreateTriangle->setIcon(QIcon(":/picture/Resources/triangle.png"));

    m_pRetView = new QAction(tr("ReSetView"), this);
    connect(m_pNewFile, SIGNAL(triggered()), this, SLOT(onNewFile()));
    connect(m_pOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(m_pSaveFile, SIGNAL(triggered()), this, SLOT(onSaveFile()));
    connect(m_pSaveOther, SIGNAL(triggered()), this, SLOT(onSaveOther()));

    connect(m_pRetView, SIGNAL(triggered()), this, SLOT(onReSetView()));
    connect(m_pCreateCircle, SIGNAL(triggered()), this, SLOT(slotCreateCircle()));
    connect(m_pCreateOval, SIGNAL(triggered()), this, SLOT(slotCreateCircle()));
    connect(m_pCreateRectangle, SIGNAL(triggered()), this, SLOT(slotCreateRect()));
    connect(m_pCreateTriangle, SIGNAL(triggered()), this, SLOT(slotCreateTri()));


}

void picture::createDockWidgets()
{
    m_objTree = new QDockWidget(tr("Node tree"),this);
    m_objTree->setContentsMargins(0, 0, 0, 0);
    m_objTree->setMaximumWidth(300);
    m_objTree->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_objTree->setAllowedAreas(Qt::AllDockWidgetAreas);
    QTreeWidget* pTree = new QTreeWidget(m_objTree);
    m_objTree->setWidget(pTree);
    m_objTree->setFloating(false);
   // m_objTree->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    addDockWidget(Qt::LeftDockWidgetArea, m_objTree);
    m_objTree->setObjectName("Object Tree");
    m_previewWindow = new QTabWidget(this);
   // QOpenGLWidget* GL = new QOpenGLWidget(m_previewWindow);
   // m_previewWindow->setWidget(GL);
  //  m_previewWindow->setContentsMargins(0, 0, 0, 0);
  //  m_previewWindow->setAllowedAreas(Qt::RightDockWidgetArea);
    m_previewWindow->setObjectName("Preview Window");
  //  addDockWidget(Qt::RightDockWidgetArea, m_previewWindow);
    setCentralWidget(m_previewWindow);
    m_pViewMenu->addAction(m_objTree->toggleViewAction());          //设置停靠窗口是否显示
    m_pRemain1DockWidget = new QDockWidget(tr("Remain1"),this);
    m_pRemain1DockWidget->setFocusPolicy(Qt::FocusPolicy::TabFocus); // 接受Tab键焦点
    m_pRemain2DockWidget = new QDockWidget(tr("Remain2"), this);
    m_pRemain1DockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pRemain2DockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pRemain1DockWidget->setObjectName("Remain1");
    m_pRemain2DockWidget->setObjectName("Remain2");
    
    addDockWidget(Qt::BottomDockWidgetArea, m_pRemain1DockWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_pRemain2DockWidget);
    tabifyDockWidget(m_pRemain1DockWidget, m_pRemain2DockWidget);
    m_pRemain1DockWidget->raise();
    m_pViewMenu->addAction(m_objTree->toggleViewAction());          //设置停靠窗口是否显示
    m_pViewMenu->addAction(m_pRemain1DockWidget->toggleViewAction());
    m_pViewMenu->addAction(m_pRemain2DockWidget->toggleViewAction());
//     bool bExistObjTree, bExistRemain1Dock, bExistRemain2Dock;
//     readDockFrame(bExistObjTree, bExistRemain1Dock, bExistRemain2Dock);
//     m_objTree->setHidden(bExistObjTree);
//     m_pRemain1DockWidget->setHidden(bExistRemain1Dock);
//     m_pRemain2DockWidget->setHidden(bExistRemain2Dock);
    QString strPath = "LastLayout.ini";
    QFile file(strPath);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray ba;
        QDataStream in(&file);
        in >> ba;
        file.close();
        this->restoreState(ba);
    }
}

void picture::createStatusBars()
{
    statusBar()->setMaximumHeight(20);
    const QString strBgStyleGreen = "QLabel{ background-color: transparent; color:#409354; }";
    QLabel *pWecLable = new QLabel(tr("welcome you") + "  " + "Jack", this);
    QLabel *pPerLable = new QLabel(tr("Produced by Jack"), this);
    pWecLable->setStyleSheet(strBgStyleGreen);
    statusBar()->addWidget(pWecLable);
    statusBar()->setSizeGripEnabled(true);                  //右下角显示调整大小的小三角
    statusBar()->addPermanentWidget(pPerLable);             //右下角添加永久标签
}

void picture::saveDockFrame()
{
    QString strDock;
    strDock += QString::number(m_objTree->isHidden());
    strDock += ";";
    strDock += QString::number(m_pRemain1DockWidget->isHidden());
    strDock += ";";
    strDock += QString::number(m_pRemain2DockWidget->isHidden());
    strDock += ";";
    QSettings cSettings("Picture", "Dock");
    cSettings.setValue("DockList", strDock);
}

void picture::readDockFrame(bool& bExistObjTree, bool& bExistRemain1Dock, bool& bExistRemain2Dock)
{
    bool bRet[3] = { 1 };
    QSettings cSettings("Picture", "Dock");
    QString strFrame = cSettings.value("DockList").toString();
    std::string strTem = strFrame.toStdString();
    int nIndex = strTem.find(';');
    int nPos = 0;
    while (nIndex != std::string::npos)
    {
        std::string strData = strTem.substr(0, nIndex);
        bRet[nPos++] = atoi(strData.c_str());
        strTem = strTem.substr(nIndex + 1, strTem.length());
        nIndex = strTem.find(';');
    }
    bRet[nPos++] = atoi(strTem.c_str());
    bExistObjTree = bRet[0];
    bExistRemain1Dock = bRet[1];
    bExistRemain2Dock = bRet[2];
}

void picture::createConnectToolBar()
{

}

void picture::slotCreateCircle()
{

}

void picture::slotCreateOval()
{

}

void picture::slotCreateRect()
{

}

void picture::slotCreateTri()
{

}

void picture::onNewFile()
{
    CCreateFileDlg* dlg = new CCreateFileDlg(this);
    if (dlg->exec() == QDialog::Accepted)
        OnCreateFile(dlg->getParam());
    dlg->deleteLater();

}
void picture::OnCreateFile(TNFileParam& tParam)
{
    CShowWidget* showWidget = new CShowWidget(tParam.strFileName, this);
    m_previewWindow->addTab(showWidget, QString::fromStdString(tParam.strFileName));
    m_previewWindow->setCurrentWidget(showWidget);
}
void picture::onDeleteFile()
{

}

void picture::onSaveFile()
{

}

void picture::onSaveOther()
{

}

void picture::onOpenFile()
{

}

void picture::slotCloseCheackSystem()
{
    QFile file("LastLayout.ini");
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out << this->saveState();
        file.close();
    }
  //  saveDockFrame();
    exit(-1);
}

void picture::onReSetView()
{
//     QFile file("MainLayout.ini");
//     if (file.open(QIODevice::WriteOnly))
//     {
//         QDataStream out(&file);
//         out << this->saveState();
//         file.close();
//     }  
    QString strPath = "MainLayout.ini";
    QFile file(strPath);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray ba;
        QDataStream in(&file);
        in >> ba;
        file.close();
        this->restoreState(ba);
    }
}
