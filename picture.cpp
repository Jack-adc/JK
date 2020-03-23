#include "picture.h"
#include <QMenuBar>  
#include <QTreeWidget>
#include <QOpenGLWidget>
#include "circlenode.h"

picture::picture(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("Imaginary"));
    this->createAction();
    this->createMenu();
    this->createToolBar();
    this->createDockWidgets();
    this->showMaximized(); // 打开时全屏
	this->createConnectToolBar();
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
}

void picture::createDockWidgets()
{
    m_objTree = new QDockWidget(tr("Node tree"),this);
    m_objTree->setContentsMargins(0, 0, 0, 0);
    m_objTree->setMaximumWidth(300);
    m_objTree->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_objTree->setAllowedAreas(Qt::LeftDockWidgetArea);
    QTreeWidget* pTree = new QTreeWidget(m_objTree);
    m_objTree->setWidget(pTree);
    m_objTree->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    addDockWidget(Qt::LeftDockWidgetArea, m_objTree);
    m_previewWindow = new QDockWidget(tr("Preview Window"),this);
    QOpenGLWidget* GL = new QOpenGLWidget(m_previewWindow);
    m_previewWindow->setWidget(GL);
    m_previewWindow->setContentsMargins(0, 0, 0, 0);
    m_previewWindow->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_previewWindow);
}


void picture::createConnectToolBar()
{
    connect(m_pCreateCircle, SIGNAL(triggered()), this, SLOT(slotCreateCircle()));
}


void picture::slotCreateCircle()
{
	circleNode* node = new circleNode(m_previewWindow);
	m_previewWindow->setWidget(node);
	node->showMaximized();
}