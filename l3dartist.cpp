#include "l3dartist.h"
#include "previewwindow.h"
#include "dockpropertyeditor.h"
#include "dockscene.h"
#include "dockanimation.h"
#include "scenetree.h"
#include "docktoolbox.h"
#include "scenetreeitem.h"
#include "aboutdlg.h"
#include "ShortcutDescription.h"
#include "toolbutton.h"
#include "l3dartistdefs.h"
#include "L3DTexParamDefs.h"
#include "QTSharedWindow/rolllabel.h"

#include "Logger\Logging.h"
#include "ParamsCfg.h"
#include "changerootdlg.h"
#include "changeftpdlg.h"
#include "BevelSet.h"
#include "QtPainter.h"
#include "ParamsCfg.h"
#include "QTSharedWindow\file_ImEx.h"

#include "textedit.h"

#include <QDockWidget>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QActionGroup>
#include <QSettings>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QToolButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDir>
#include <QPushButton>
#include <QWidgetAction>
#include <QProgressBar>
#include <QStyleFactory>
#include <QMimeData>
#include <QClipboard>
#include <QTranslator>
#include <qcolordialog.h>
#include "gradientdlg.h"
#include <qprocess.h>
#include <QMenuBar>
#include <QStatusBar>
#include <QDebug>

// �ص��ӿ�
#include "nodeoperatecb.h"
#include "notifier.h"
#include "sceneoperatecb.h"

#include "nodelistwidget.h"
#include "nodeitem.h"
#include "QTSharedWindow\QBaseWidget\mediadecoder.h"    //added by xyc 2015.7.8 for ��Ƶ�ļ�����
#include "ScenePack.h"                                  //for C3DProjectDirManager
#include "..\..\Common\L3DEleLock\FickleCheckLic.h"
#include "..\..\Common\L3DEleLock\EleLock.h"
#include "..\..\Common\QTSharedWindow\Login\L3DLoginWidget.h"
#include "..\..\Common\L3DSystemConfig.h"
#include "CenterDBIF.h"
#include <boost\thread.hpp>
#include "CallBackDefine.h"
#include "../Dll/L3DCommUI/l3dcommui.h"
#include "l3dmsgdef.h"
#include "setcardparamdlg.h"
#include <boost/algorithm/string.hpp>
#include "setuploadparam.h"
#include "packdlg.h"
#include "ScopeGuard.h"
#include "MaterialEditor.h"
// #include "customsizebar.h"
using namespace L3DArtistDef;

L3DArtist *L3DArtist::m_pInstance = nullptr;
bool L3DArtist::m_bTranslate = false;

QString DockWidgetFocusStyleSheet = "                       \
									QDockWidget::title {                                    \
									padding-left: 5px;                                      \
									padding-right: 15px;                                    \
									padding-top: 1px;                                       \
									padding-bottom: 1px;                                    \
									text-align: left;                                       \
									border-top: 1px solid rgb(37, 37, 38);                  \
									border-left: 1px solid rgb(37, 37, 38);                 \
									border-right: 1px solid rgb(37, 37, 38);                \
									background-color: rgb(87, 87, 89);                     \
									}\
									";

QString DockWidgetUnfocusStyleSheet = "                     \
									  QDockWidget::title {                                    \
									  padding-left: 5px;                                      \
									  padding-right: 15px;                                    \
									  padding-top: 1px;                                       \
									  padding-bottom: 1px;                                    \
									  text-align: left;                                       \
									  border-top: 1px solid rgb(37, 37, 38);                  \
									  border-left: 1px solid rgb(37, 37, 38);                 \
									  border-right: 1px solid rgb(37, 37, 38);                \
									  }\
									  ";
//> added by xyc ���ڹ�����õ�����ʱ�Թ����������ı�
QString DockToolFocusStyleSheet = "*{border-top: 1px solid rgb(37, 37, 38);		\
								  border-left: 1px solid rgb(37, 37, 38);	\
								  border-right: 1px solid rgb(37, 37, 38);	\
								  background-color:rgb(87, 87, 89);}";

QString DockToolUnfocusStyleSheet = "*{border-top: 1px solid rgb(37, 37, 38);	\
									border-left: 1px solid rgb(37, 37, 38);	\
									border-right: 1px solid rgb(37, 37, 38);	\
									background-color:rgb(45, 45, 48);}";

L3DArtist::L3DArtist(QWidget *pParent, int nCmdParam)
    : QMainWindow(pParent)
    , CheckOpenFile_(false)
    , m_eNodeCtrl(NODECTRL_MOVE)
    , m_nCurTexture(0)
    , m_nCurMaterial(0)
    , m_bIsNewScene(false)
    , m_nCurPos(0)
    , m_closeEvent(false)
    , m_bPreClose(false)
    , m_nCurSceneWidth(1280)//1920
    , m_nCurSceneHeight(720)//1080
    , m_pCentralWidget(nullptr)
    , m_bRMgrInit(true)
    , m_bRMgrUnInit(true)
    , m_bRecording(false)
    , m_isResetProject(false)
    , m_bIsOpenScene(false)
    , m_hasPlayed(false)
    , m_nCmdParam(nCmdParam)
    , m_extractTex(false)
    , secAniSeq_(1)
    , m_vecEdit()
    , m_bNeedUpload(true)
    ,_isMulSel(false)
	//    , _materialTexNum(0)
{
	Q_ASSERT(m_pInstance == nullptr);
	m_pInstance = this;
	ui.setupUi(this);
	setFont(QApplication::font());

	// by ShenX Load Chinese file
	translator_cn = new QTranslator;
	translator_cn->load(":/L3DArtist/l3dartist_zh.qm");
	translator_default = new QTranslator;
	translator_default->load(":/L3DArtist/qt_zh_CN.qm");

	//ע�� �ź� �������ͣ�����ǰ��
	RegisterMetaType();

	setWindowFlags(Qt::FramelessWindowHint);
	//setTabShape(QTabWidget::Triangular);

	CreateCtrlBar();
	CreateMenuBar();
	CreateToolBar();
	CreateStatusBar();

	m_pTBtnUndo->SetButtonDisabled(true);
	m_pTBtnRedo->SetButtonDisabled(true);


	//��¼�ź�
	connect(this, SIGNAL(slgUpateStartDlg(const QStringList&)), L3DLoginWidget::instance(), SLOT(slotUpdateStartCombos(const QStringList&)));
	connect(this, SIGNAL(slgSelectStartDlg(const QString&)), L3DLoginWidget::instance(), SLOT(slotSelectStartCombo(const QString&)));

	m_checkIsModifiedTimer = new QTimer(this);
	connect(m_checkIsModifiedTimer, SIGNAL(timeout()), this, SLOT(SlotCheckSceneIsModified()));

	m_nodeParamUpdTimer = new QTimer(this);
	connect(m_nodeParamUpdTimer, SIGNAL(timeout()), this, SLOT(SlotUpdNodeTimer()));
	m_nodeParamUpdTimer->start(500);

	qRegisterMetaType<E_DB_STATUS>("E_DB_STATUS");
	connect(this, &L3DArtist::sglDBStatusChanged, this, &L3DArtist::slotDBStatusChanged, Qt::QueuedConnection);

	//> added by TaiYue 2019.6.5 for ��ʼ��ʱ���ò������ۺ���������dock�������emit�����±꣨index��
	connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
	//> added by TaiYue 2019.6.5 for ��ʼ��ʱ�����ƶ�״̬�µ��������±���м�¼
	//connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetMoveIndex(int)));
	//> added by TaiYue 2019.6.5 for ���ý���
	connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetFocusXYZ(int)));
	//connect(this, &L3DArtist::SglUpdateBaseProperty, DockPropertyEditor::pDockPropertyEdit, &DockPropertyEditor::SlotUpdateBaseProperty);
}

void L3DArtist::intialize(QDBTOOLS::Logger::LogInitParams* logParams, const QString& projectName)
{
	m_strProjectName = projectName;

	if (projectName.isEmpty())
		ArtistParams::instance()->VideoMode(EVML_1080I_50);
	else
		ArtistParams::instance()->VideoMode(projectName);

	if (!InitDll(logParams))
	{
		LOG(QERROR) << _T("The engine failed to initialize");
		MSGBOX_ERROR_TIP(tr("The engine failed to initialize"), this);
		// ��ʾ���������浯�������������UI��ʾ��ȱ��
		//return;
	}

	//�����źţ��������
	GetDLLManager();
	InitManager();

	CreateDockWidgets(ArtistParams::instance()->isArtistEditMode());
	CreateConntent();

	// ���ý����빤����صĿؼ��Ĺ�������
	setProject(projectName);

	ReadSettings(); //��ȡ�����ļ� ��ԭ�ϴδ򿪵�ͣ������״̬

					// TODO: post exception if load .qm file failed
	TranslateLanguage();

	emit SglActionEnable(false);//added by xyc
	emit SglPropertyEnable(false);

	if (ArtistParams::instance()->isArtistEditMode())
		initEditMode();
}

void L3DArtist::setProject(const QString& proj)
{
	if (proj.isEmpty())
		ArtistParams::instance()->VideoMode(EVML_1080I_50);
	else
		ArtistParams::instance()->VideoMode(proj);

	//��������Ŀ¼���ز�Ŀ¼
	CreateSceneAndMediaDirectory();
	emit SglUpdateProjectPath();

	//CardInitParam param;
	resetPwnResolution(/*param*/);
	m_pDockScene->setEnabled(false);                      //�л�����ʱ,ʹ�����б���治�ܲ���,���������������������� added by wuheng 2017.10.12
	dynamic_cast<DockScene*>(m_pDockScene->widget())->setProject(m_strProjectPath);
	m_pDockScene->setEnabled(true);
	dynamic_cast<DockToolBoxMaterial*>(m_pDockToolBoxMaterial->widget())->setProject(proj);
	dynamic_cast<DockToolBoxProperty*>(m_pDockToolBoxProperty->widget())->setProject(proj);
	dynamic_cast<DockAnimation*>(m_pDockAnimation->widget())->setProject();

	if (m_isResetProject)
		resetDllProject();

	SetCurSceneDirectory();

	m_pDockScene->raise();
}

void L3DArtist::resetDllProject()
{
	QString path = m_strProjectPath.left(m_strProjectPath.length() - m_strProjectName.length() - 1).replace('/', '\\');
	CEngineKerDll::setSceneProject(path.tQtToStdString(), m_strProjectName.tQtToStdString(), ArtistParams::instance()->VideoMode(),
		ERWOT_NOLIMIT, localLangue_.tQtToStdString());
	m_isResetProject = false;
	SlotUpdateCurSceneDir(QStringLiteral("scenes"));
}

void L3DArtist::resetPwnResolution(/*CardInitParam& param*/)
{
	SVideoFMTInfo videlFMInfo;
	E_VIDEO_MODE_L3D videoMode = ArtistParams::instance()->VideoMode();
	CEngineKerDll::getVideoFmtInfo(videoMode, videlFMInfo);

	m_nCurSceneWidth = videlFMInfo.width_;
	m_nCurSceneHeight = videlFMInfo.height_;
	m_FrameRate = videlFMInfo.fps_;
	if (m_pPreviewWnd)
	{
		m_pPreviewWnd->SetWHScale(m_nCurSceneWidth, m_nCurSceneHeight);
		QSize size = m_pPreviewWnd->size();
		size.setWidth(size.width() + 1);
		m_pPreviewWnd->resize(size); // ǿ��qt�Զ�����һ�´�С
	}
}

// ��ʼ����ز�����
void L3DArtist::initial_ol_thread(SDBCPInitParam param, std::wstring user/*,HCLCommClient* HCLClient*/)
{
	try
	{
		// ��ʼ�����ݿ����ӳ�
		bool useSub = !m_ctrlLinkClient.isMainUsed();
		if (!CDBConnectPool::instance()->initialize(param, useSub)) {
			DBGPrintf("initialize:connect db fail!\n");
			return;
		}

		CDBConnectPool::instance()->registCallback(std::bind(&L3DArtist::DBStatusChangedCallback, this, std::placeholders::_1));

		// ��ʼ���������ݿ���ʲ�����Ľӿ�
		if (!CenterDBIF::instance()->Init(CDBConnectPool::instance())) {
			DBGPrintf("initialize:connect CenterToDB fail!\n");
			return;
		}

		ProjectVecPtr vecProj = CenterDBIF::instance()->GetProjectInfo();
		// ��ȡdataServer FTP��Ϣ���ڴ���ϴ�
		dataSerInfoVec = CenterDBIF::instance()->GetDataServerInfo();

		QStringList allowedPrj;
		auto susers = CenterDBIF::instance()->GetUserInfoByName(user);
		if (susers && !susers->empty())
		{
			std::vector<tstring> schannel_IDs;
			boost::split(schannel_IDs, susers->at(0).u_chIDs, boost::is_any_of(_T(";")));
			for (tstring id : schannel_IDs)
			{
				std::vector<SChannelInfo> ch = CenterDBIF::instance()->selectChInfoFromDB(QString::fromStdWString(id).toInt());
				if (!ch.empty())
				{
					allowedPrj.push_back(QString::fromLocal8Bit(ch[0].projectName_.c_str()));
				}
			}
		}
		// ֪ͨ��ʼ�������Ƶ����Ϣ
		QStringList projects;
		std::vector<S3DProject>::iterator item = vecProj->begin();

		for (; item != vecProj->end(); item++)
		{
			QString tmp;
#ifdef UNICODE
			tmp = QString::fromStdWString(item->prj_name_);
#else
			tmp = QString::fromStdString(item->prj_name_);
#endif
			if (CenterDBIF::instance()->IsSuperUser(user) || allowedPrj.contains(tmp))
				projects << tmp;
		}
		emit slgUpateStartDlg(projects);
		emit slgSelectStartDlg(ArtistParams::instance()->getLoginPrj());
		ArtistParams::instance()->setPrjNames_Ltech(projects);
	}
	catch (std::exception&)
	{
		//QMessageBox::information(this,tr("Message"),QString::fromLocal8Bit(e.what()));
	}
}

L3DArtist::~L3DArtist()
{
	m_nodeParamUpdTimer->stop();
	delete translator_cn;
	delete translator_default;
	delete m_checkIsModifiedTimer;
	CAsynOperatingBass::clear(); //��� ��ز���״̬
								 //> ����ʼ����־ϵͳ
	QDBTOOLS::Logger::CLogger::getInstance()->unInitialize();
	CEngineKerDll::fini();
	CenterDBIF::instance()->unInit();
	CDBConnectPool::instance()->unInitialize();
	CenterDBIF::unInstance();
	CDBConnectPool::unInstance();
	qDebug() << _T("L3DArtist::~L3DArtist()ִ����artist����־ϵͳ����� CEngineKerDll::fini()");
}

//added by xyc
void L3DArtist::SlotActionEnable(bool isEnable)
{
	ui.m_pSaveAsScene->setEnabled(isEnable);
	ui.m_pRenameScene->setEnabled(isEnable);
	ui.m_pCloseScene->setEnabled(isEnable);
	ui.m_pOutputVideo->setEnabled(isEnable);
	ui.m_pExportScene->setEnabled(isEnable);
	ui.m_pSaveScene->setEnabled(isEnable);
	ui.m_pCopy->setEnabled(isEnable);
    m_pTBtnVAlign->setEnabled(_isMulSel);
    m_pTBtnHAlign->setEnabled(_isMulSel);
	ui.m_pPaste->setEnabled(isEnable);
	ui.m_pCut->setEnabled(isEnable);
	ui.m_pSelectAll->setEnabled(isEnable);
	ui.m_pDelete->setEnabled(isEnable);
	ui.m_pDeselect->setEnabled(isEnable);
	ui.m_pSceneLayer->setEnabled(isEnable);
	//pTBtnExportScene->setEnabled(isEnable);
	pTBtnSaveScene->setEnabled(isEnable);
	pTBtnCut->setEnabled(isEnable);
	pTBtnCopy->setEnabled(isEnable);
    pTBtnVAlign->setEnabled(_isMulSel);
    pTBtnHAlign->setEnabled(_isMulSel);

	pTBtnPaste->setEnabled(isEnable);
	/******************************************��Ⱦ���ڶ��빦������*********************************
	pTBtnAlignX->setEnabled(isEnable);
	pTBtnAlignY->setEnabled(isEnable);
	pTBtnAlignZ->setEnabled(isEnable);
	/******************************************��Ⱦ���ڶ��빦������*********************************/
	if (isEnable)
		setSomeBtnDisable();
}

bool L3DArtist::PreClose()
{
	return m_bPreClose;
}

//const QString L3DArtist::GetProjectRootPath()const 
//{
//	return ArtistParams::instance()->ProjectRootPath(); 
//}
//
//const QString L3DArtist::GetTempPath()const
//{
//	return ArtistParams::instance()->TempPath(); 
//}
//
//const QString L3DArtist::GetLocalUploadPath()const
//{
//	return ArtistParams::instance()->LocalUploadPath(); 
//}
//
//int L3DArtist::ArtistParams::instance()->GetCurSizeHint() const
//{
//	return ArtistParams::instance()->GetCurSizeHint();
//}
//
//void L3DArtist::SetCurSizeHint(int cur_size)
//{
//	ArtistParams::instance()->SetCurSizeHint(cur_size);
//}

//bool L3DArtist::isLocalUpload() const
//{
//	return ArtistParams::instance()->isLocalUpload();
//}
//
//bool L3DArtist::isStudio() const
//{
//	return ArtistParams::instance()->isStudio();
//}

//��ʼ��DLL
bool L3DArtist::InitDll(QDBTOOLS::Logger::LogInitParams* logParams)
{
	LOG(INFO) << _T("��ʼ��DLL");

	SVideoFMTInfo videlFMInfo;
	E_VIDEO_MODE_L3D videoMode = ArtistParams::instance()->VideoMode();

	// ���Ӷ��Զ���ֱ��ʵĴ���modify by litianming 2016/05/11
	// �Զ������Ⱦ�ֱ���
	if (videoMode == EVML_UserDef)
	{
		int renderWidth = 0, renderHeight = 0, fps = 25;
		APP_MODE appMode = L3DSystemConfig::instance()->getAppMode();
		switch (appMode)
		{
			// Ŀǰֻ���ݲ���ģʽ��֧���Զ���ֱ��ʣ���������Ļ��Ҫ�Զ���ֱ��ʣ����������伴�ɣ�by litianming 2016/05/12
		case APP_MODE_STUDIO:
			L3DSystemConfig::instance()->getEngineUserDef(m_nCmdParam, renderWidth, renderHeight);
			break;
		default:
			break;
		}
		if (renderWidth >1 && renderHeight > 1)
		{
			assert(fps != 0);
			if (fps == 0)
			{
				LOG(QERROR) << _T("L3DArtist::InitDll() is failed,���̶�Ӧ֡��Ϊ0�����鹤�����ò�����");
				return false;
			}
			videlFMInfo.width_ = renderWidth;
			videlFMInfo.height_ = renderHeight;
			videlFMInfo.frame_space_ = 1000 / fps;
			videlFMInfo.fps_ = fps;
			videlFMInfo.field_space_ = videlFMInfo.frame_space_ / 2;
			videlFMInfo.remain_ = 0;

			// �ڳ�ʼ��EngineKernel֮ǰ�����Զ���ķֱ��ʱ��浽ȫ�ֱ�����
			// ��ȻEngineKernel�����ܸ���videoMode��ȡ����Ӧ���Զ���ֱ��ʣ�by litianming 2016/05/11
			CEngineKerDll::setCustomRenderSize(videlFMInfo);
		}
		else
		{
			LOG(QERROR) << _T("L3DArtist::InitDll() is failed,���̷ֱ��ʴ�С�����������鹤�����ò�����");
			return false;
		}
	}
	// for L3DMediaFile.dll init
	MediaDecoder::initialize();
	//��ʼ�� ȫ��DLL

	GlobalCfgParam& EKParam = ArtistParams::instance()->getEKParams();
	EKParam.dll_use_mode = E_EKD_USE_MODE::EEUM_Manufacture;
	EKParam.projectName_ = m_strProjectName.tQtToStdString();
	ui.m_pSafe->setChecked(EKParam.isSafetyArea);
	ui.m_pTitleSafe->setChecked(EKParam.isTitleSafetyArea);
	ui.m_pShowRuler->setChecked(EKParam.isDrawRulers);
	int nCardNum = ArtistParams::instance()->getVideoCardNo();//settings.value("VideoCardNo", 0).toInt();
	int nCardChannNum = ArtistParams::instance()->getVideoCardChannNo();
	EKParam.nAudioChannels = 2;
	EKParam.bEnableSoundCard = true;

	E_VIDEOCARD_TYPE  card_type = ArtistParams::instance()->VideoCardType();


	bool hasVideoCard = (ArtistParams::instance()->VideoCardMode() && card_type > EVT_CGS_VGA);
	EKParam.hasVideoCard = hasVideoCard;

	SCGCardParams outputCardParam;
	if (hasVideoCard)
	{
		// ��ʼ����Ƶ��  		
		outputCardParam.card_no_ = nCardNum;
		outputCardParam.cgc_type_ = card_type;
		outputCardParam.cgc_use_t_ = ECUT_Broadcast;
		outputCardParam.output_ch_count_ = 1;

		// �������ͨ������
		SCGChannelParam cardOutCh;
		cardOutCh.ch_no_ = nCardChannNum;
		cardOutCh.out_type_ = EOT_Card;
		cardOutCh.ch_type_ = SCGChannelParam::ECT_Output;
		cardOutCh.has_audio_ = true;// ��ʼ��ʱ�ײ����Ƶ�����Ƿ�����򲶻���Ƶ���ϲ�ͨ�������������趨
		cardOutCh.video_mode_ = ArtistParams::instance()->VideoMode();
		outputCardParam.out_ch_param_vec_.push_back(cardOutCh);
	}

	//// VGA���ͨ��
	//SCGChannelParam VGAOutCh;			
	//VGAOutCh.out_type_ = EOT_Wnd;
	//VGAOutCh.ch_no_ =  outputCardParam.out_ch_param_vec_.size();	
	//VGAOutCh.sync_key_out_ch_no_ = -1;
	//VGAOutCh.ch_type_ = SCGChannelParam::ECT_Output;
	//VGAOutCh.has_audio_ = true;
	//VGAOutCh.video_mode_ = ArtistParams::instance()->VideoMode();
	//VGAOutCh.hWnd_ = (HWND)m_pPreviewWnd->GetShowWinID();

	//outputCardParam.out_ch_param_vec_.push_back(VGAOutCh);

	outputCardParam.output_ch_count_ = outputCardParam.out_ch_param_vec_.size();
	EKParam.cardParams.push_back(outputCardParam);

	try
	{
		if (CEngineKerDll::init(EKParam, logParams) == EKRSLT_S_OK)
		{
			if (CEngineKerDll::isInited())
			{
				// �����ص��ӿ�ָ��
				INotifierPtr_ = std::make_shared<Notifier>();
				ISceneOperateCBPtr_ = std::make_shared<SceneOperateCB>();
				INodeOperateCBPtr_ = std::make_shared<NodeOperateCB>();
				// ���ûص��ӿ�ָ��
				CEngineKerDll::setNotifier(INotifierPtr_.get());
				CEngineKerDll::setSceneCallback(ISceneOperateCBPtr_.get());
				CEngineKerDll::setNodeCallback(INodeOperateCBPtr_.get());
				return true;
			}
		}
	}
	catch (...)
	{
	}

	LOG(QERROR) << _T("��ʼ��DLLʧ��");
	return false;
}


void L3DArtist::getAllProject(SDBCPInitParam param, HCLCommClient HCLClient, const std::wstring& user)
{
#ifndef FOR_XOR_CO_VER
	m_ctrlLinkClient = HCLClient;
	CLCDllMgr::set_switch_db_noti_cbf(&L3DArtist::on_switch_db);
	boost::thread(boost::bind(&L3DArtist::initial_ol_thread, this, param, user/*,HCLClient*/));
#endif
}

void L3DArtist::getFTP(std::vector<SChannelNoCenter> &servers)
{
	if (!dataSerInfoVec)
		return;

	SChannelNoCenter server;
	for (auto& t : *dataSerInfoVec)
	{
		server.ip_ = t.svr_ip_;
		server.userName_ = t.svr_ftp_name_;
		server.userPwd_ = t.svr_ftp_pwd_;
		QString strPort = tQtFromStdString(t.svr_ftp_port_);
		server.port_ = strPort.toInt();
		server.channelName_ = t.svr_name_;
		servers.push_back(server);
	}
}

//��ȡdllȫ����ع�����
bool L3DArtist::GetDLLManager()
{
	//������ݾ����������ʼ����Ӧ�Ĺ�����
#ifdef Q_OS_WIN //> ����Windowsƽ̨�ľ��ΪHWND
	//��������������������ǰ��HRenderManager��Ҫ�ڴ򿪳������½�����ʱ��������ɾ�����´��룬deleded by wang 2014.3.12
	//RenderManager_ = HRenderManager::create(false, (HWND)m_pPreviewWnd->GetShowWinID());//����������

	/*E_VIDEO_MODE_L3D videoMode = ArtistParams::instance()->VideoMode();
	SVideoFMTInfo fmt;
	CEngineKerDll::getVideoFmtInfo(videoMode,fmt);
	QFrame* prenderWnd = new QFrame();
	prenderWnd->setFixedSize(fmt.width_,fmt.height_);
	prenderWnd->hide();*/

	RenderManager_ = HRenderManager::create(false, (HWND)m_pPreviewWnd->GetShowWinID()/*prenderWnd->winId()*/,
		ArtistParams::instance()->VideoCardMode(), ArtistParams::instance()->VideoCardType());
	//> for test by CaiYL 2014.02.18
	//RenderManager_ = HRenderManager::create(false, (HWND)m_pPreviewWnd->GetShowWinID(), true, EVT_CGS_XORMEDIA);//����������
#endif
	SceneManager_ = HSceneManager::create(); //����������û�в���

	return true;
}

bool L3DArtist::InitManager()
{
	LOG(INFO) << _T("��ʼ��������");
	RECT rc;
	bool bCardMode = ArtistParams::instance()->VideoCardMode();
	HWND hwWin = (HWND)m_pPreviewWnd->GetShowWinID();

	if (RenderManager_.isValid() && (!RenderManager_.isInitialized())) {

		E_VIDEO_MODE_L3D videoMode = ArtistParams::instance()->VideoMode();
		resetPwnResolution();

		localLangue_ = ArtistParams::instance()->LocalLangue();
		if (localLangue_.isEmpty())
			localLangue_ = "chs";

#ifdef NDEBUG // for Release	
		L3DEleLockCG4Artist usbkey;
		if (!usbkey.CheckIsOK(videoMode))
		{
			LOG(QERROR) << _T("���ܹ���֧�ֵ�ǰ���̵���Ƶģʽ!");
			return false;
		}
#endif
		E_VOUT_CTRL_TYPE type;
		if (bCardMode)
			type = EVCT_ALL;
		else
			type = EVCT_VGA;
		try
		{
			RenderManager_.initialize(videoMode, type, ERWOT_NOLIMIT, localLangue_.tQtToStdString());
		}
		catch (...)
		{
			QMessageBox::information(this, "Error", "exception!");
		}

		slotDrawRulers();
		slotSetRulers();

	}

	if (bCardMode)
	{
		::SetWindowPos(hwWin, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		RenderManager_.setIsSyncRenderWnd(true);
		QSize size = m_pPreviewWnd->size();
		size.setWidth(size.width() + 1);
		m_pPreviewWnd->resize(size); // ǿ��qt�Զ�����һ�´�С
	}

	if (CGCardManager_.isValid()/* && (!CGCardManager_.isInitialized())*/) {
	}
	if (AudioManager_.isValid()) {
		AudioManager_.openAudio(); //��ʼ  
	}
	if (SceneManager_.isValid() && (!SceneManager_.isInitialized())) {
		SceneManager_.initialize(E_NODE_OP_TYPE::ENOT_NOLIMIT);
	}

	return true;
}

bool L3DArtist::UninitDll()
{
	LOG(INFO) << _T("ж��DLL");
	if (CEngineKerDll::isInited()) {
		//�ÿճ������½��棬���ٽ��������Ѿ������DLL�ڲ�ֵ
		UpdateCurSene(HScene());
		//����������ͷ�

		if (SceneManager_.isValid()) {
			SceneManager_.unInitialize();
			SceneManager_.reset();
		}
		if (AudioManager_.isValid()) {
			AudioManager_.reset();
		}
		/*if (CGCardManager_.isValid()) {
		CGCardManager_.unInitialize();
		CGCardManager_.reset();
		}*/
		// 		if (RenderManager_.isValid()) {                     //��Ⱦ����������ʼ����ʱ̫����ʱ����Ⱦ�߳�stop����������ʱ�Ƴ� by wuheng 2018.01.30
		// 			RenderManager_.unInitialize();
		// 			RenderManager_.reset();
		// 		}

		// for L3DMediaFile.dll fini
		MediaDecoder::unInitialize();
		close();
		return true;
	}

	/*if (card_output_.isValid()) {
	card_output_.unInitialize();
	card_output_.reset();
	}*/

	LOG(QERROR) << _T("ж��DLLʧ��");
	return false;
};

//> ע���Զ����ź����ͺ���
void L3DArtist::RegisterMetaType()
{
	qRegisterMetaType<SERIALIZE_TEX>("SERIALIZE_TEX");
	qRegisterMetaType<NODE_OPERATE>("NODE_OPERATE");
	qRegisterMetaType<CPOINT_OPERATE>("CPOINT_OPERATE");
	qRegisterMetaType<SCENE_PLAY>("SCENE_PLAY");
	qRegisterMetaType<SCENE_SOUND_OPERATE>("SCENE_SOUND_OPERATE");
	qRegisterMetaType<RENDER_OPERATE>("RENDER_OPERATE");
	qRegisterMetaType<CGCIS_INIT>("CGCIS_INIT");
	qRegisterMetaType<CGC_OPEN_CLOSE>("CGC_OPEN_CLOSE");
	qRegisterMetaType<SCENE_OPERATE>("SCENE_OPERATE");
	qRegisterMetaType<DESTROY_MGR>("DESTROY_MGR");
	qRegisterMetaType<NODE_TRANSFORM>("NODE_TRANSFORM");
	qRegisterMetaType<TEXTRUE_OPERATE>("TEXTRUE_OPERATE");
	qRegisterMetaType<ANIMATOR_OPERATE>("ANIMATOR_OPERATE");
	qRegisterMetaType<KEYFRAME_OPERATE>("KEYFRAME_OPERATE");

	qRegisterMetaType<HNode>("HNode");
	qRegisterMetaType<LEKResult>("LEKResult");
	qRegisterMetaType<ic::vector3df>("ic::vector3df");
	qRegisterMetaType<MeshDeformPtr>("MeshDeformPtr");
	qRegisterMetaType<iv::SMaterial>("iv::SMaterial");
	qRegisterMetaType<TexturePtr>("TexturePtr");
	qRegisterMetaType<AppTexturePtr>("AppTexturePtr");
	qRegisterMetaType<LightPassPtr>("LightPassPtr");
	qRegisterMetaType<AnimatorPtr>("AnimatorPtr");
	qRegisterMetaType<KeyFramePtr>("KeyFramePtr");
	qRegisterMetaType<HRenderManager>("HRenderManager");
	qRegisterMetaType<L3DEngineKernel::IContext>("IContext");
	qRegisterMetaType<HCGCardManager>("HCGCardManager");
	qRegisterMetaType<E_CGC_CHANNEL_TYPE>("E_CGC_CHANNEL_TYPE");
	qRegisterMetaType<HSceneManager>("HSceneManager");
	qRegisterMetaType<HScene>("HScene");
	qRegisterMetaType<SceneSoundPtr>("SceneSoundPtr");
	qRegisterMetaType<CtrlPointPtr>("CtrlPointPtr");
	qRegisterMetaType<E_L3DNODE_TYPE>("E_L3DNODE_TYPE");
	qRegisterMetaType<E_ANIMATOR_TYPE>("E_ANIMATOR_TYPE");
	qRegisterMetaType<FontData>("FontData");
	qRegisterMetaType<TexEffectPtr>("TexEffectPtr");
}

//added by xyc 2014.08.12 ��ӹ���������ͼ��
void L3DArtist::InitToolBarTexture()
{
	pFrameTexture = new QFrame(this);
	pFrameTexture->setProperty("framecolor", true);
	pHLayoutTexture = new QHBoxLayout(pFrameTexture);
	pHLayoutTexture->setContentsMargins(5, 0, 5, 0);
	pHLayoutTexture->setSpacing(5);
	ui.m_pTbarTexture->addSeparator();
	ui.m_pTbarTexture->addWidget(pFrameTexture);
	ui.m_pTbarTexture->setVisible(true);

	pTextureToolBtnGroup = new QButtonGroup(this);
	pTextureToolBtnGroup->setExclusive(true);

	CreateTexureToolButton(&pTBtnTextureAll, "TextureAll_", tr("All"), 0);
	CreateTexureToolButton(&pTBtnTextureFront, "TextureFront_", tr("Front"), 1);
	CreateTexureToolButton(&pTBtnTextureBack, "TextureBack_", tr("Back"), 2);
	CreateTexureToolButton(&pTBtnTextureOutSide, "TextureOutSide_", tr("OutSide"), 3);
	CreateTexureToolButton(&pTBtnTextureSide, "TextureSide_", tr("Side"), 4);
	CreateTexureToolButton(&pTBtnTextureLeft, "TextureLeft_", tr("Left"), 5);
	CreateTexureToolButton(&pTBtnTextureRight, "TextureRight_", tr("Right"), 6);
	CreateTexureToolButton(&pTBtnTextureUp, "TextureUp_", tr("Up"), 7);
	CreateTexureToolButton(&pTBtnTextureDown, "TextureDown_", tr("Down"), 8);
	CreateTexureToolButton(&pTBtnTextureInside, "TextureInside_", tr("Inside"), 9);
	CreateTexureToolButton(&pTBtnTextureFace, "TextureFace_", tr("Face"), 10);
	CreateTexureToolButton(&pTBtnTextureFrontchamfer, "TextureFrontchamfer_", tr("Frontchamfer"), 11);
	CreateTexureToolButton(&pTBtnTextureBackchamfer, "TextureBackchamfer_", tr("Backchamfer"), 12);
	CreateTexureToolButton(&pTBtnTextureShadow, "TextureShadow_", tr("Shadow"), 13);
	CreateTexureToolButton(&pTBtnTextureBaseBoard, "TextureBaseBoard_", tr("BaseBoard"), 14);

	connect(pTextureToolBtnGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed), this, &L3DArtist::SlotTextureToolBtnGroupPressed);
	ToolBarTextureBtnInit();
}

//> ��Ӧ������������,intΪ��ǰ�����ֹ�������idֵ
void L3DArtist::SlotTextureToolBtnGroupPressed(int index)
{
	QString str = strlistTex[index];
	int i = strlistTexDef.indexOf(str);
	if (index != -1) {
		pTextureToolBtnGroup->button(index)->setChecked(true);
	}
	emit SglTextureToolBtnGroupPressed(i);
}

//> ��Ӧ���µ�ǰѡ����,intΪ��ǰ�����ֹ�������idֵ
void L3DArtist::SlotSelectTexurePartToolBar(int index)
{
	int checked_index = pTextureToolBtnGroup->checkedId();
	if (checked_index == index) return;
	if (index != -1) {
		pTextureToolBtnGroup->button(index)->setChecked(true);
	}
}

//> ���������ֹ���ͼ��
void L3DArtist::CreateTexureToolButton(ToolButton** ToolBtn, QString StrPicName, const QString StrObjectName, int GroupId)
{
	QString StrPicPath = ":/L3DArtist/Resources/" + StrPicName + ".png";
	QString StrDisPicPath = ":/L3DArtist/Resources/" + StrPicName + "h.png";
	(*ToolBtn) = new ToolButton(this);
	(*ToolBtn)->setObjectName(StrObjectName);
	(*ToolBtn)->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	(*ToolBtn)->setToolTip(StrObjectName);
	(*ToolBtn)->SetNormalIcon(StrPicPath);
	(*ToolBtn)->SetDisableIcon(StrDisPicPath);
	(*ToolBtn)->setCheckable(true);
	//pTBtnTextureAll->setIcon(QIcon(":/L3DArtist/Resources/TextureAll_.png"));//���Сͼ��
	(*ToolBtn)->SetStyleSheetTextureToolBarNormal();
	pHLayoutTexture->addWidget(*ToolBtn);
	TextureList << (*ToolBtn);
	strlistTex << StrObjectName;
	pTextureToolBtnGroup->addButton(*ToolBtn);
	pTextureToolBtnGroup->setId(*ToolBtn, GroupId);
}

//> �����ֹ���������/����ʾ
void L3DArtist::SlotClearTexurePartToolBar()
{
	ToolBarTextureBtnInit();
}

//> �����ֹ�����ʹ�ܸ���
void L3DArtist::SlotUpdateTexturePartToolBar(const tstring& strNodeId)
{
	HNode node = GetCurScene().getNodeByGUID(strNodeId);
	if (strNodeId.empty() || !node.isValid())
		return;

	strlistTexDef.clear();
	int nTexCount = node.getAppTextureCount();
	QString str;

#ifdef ENGINEKER_ENGLISH
	str = tQtFromStdString(gszMyIsEntire);
	E_L3DNODE_TYPE nodeType = node.getType();
	strlistTexDef << str;
	for (int i = 0; i < nTexCount; ++i)
	{
		str = tQtFromStdString(node.getAppTexPartName(i));

		if (nodeType == E_L3DNODE_TYPE::ELNT_BackGround)
			continue;

		if ((nodeType == E_L3DNODE_TYPE::ELNT_ExM_Sphere || nodeType == E_L3DNODE_TYPE::ELNT_ExM_Torus) && str == QStringLiteral("����"))
			continue;

		if (nodeType == E_L3DNODE_TYPE::ELNT_3DText && str == QStringLiteral("��Ӱ"))
			continue;

		if ((nodeType == E_L3DNODE_TYPE::ELNT_Particle || nodeType == E_L3DNODE_TYPE::ELNT_Plasma) && str == QStringLiteral("��"))
			continue;

		strlistTexDef << str;
	}
#else
	// 	str = tQtFromStdString(gszMyIsEntire);
	// 	strlistTexDef<< (L3DArtist::GetTanslateStatus() ? str : m_hashPropertyDictionary[str]);
	//     for (int i = 0; i < nTexCount; ++i) {
	// 		str = tQtFromStdString(node.getAppTexPartName(i));
	// 		strlistTexDef << (L3DArtist::GetTanslateStatus() ? str : m_hashPropertyDictionary[str]);
	//     }
	str = tQtFromStdString(gszMyIsEntire);
	strlistTexDef << str;
	for (int i = 0; i < nTexCount; ++i) {
		str = tQtFromStdString(node.getAppTexPartName(i));
		strlistTexDef << str;
	}
#endif

	SetToolBarTexturePic(strlistTexDef);
	int nIndex = node.getCuTexIndex();
	str = strlistTexDef[nIndex];
	emit SglSelectTexurePartToolBar(strlistTex.indexOf(str));
}

// ʹ��������ʹ��/��ʾ
void L3DArtist::SetToolBarTexturePic(QStringList strlist)
{
	for (auto TextureTmp : TextureList)
	{
		auto strTmp = TextureTmp->objectName();
		if (strTmp == "Backchamfer")
			strTmp = "Back chamfer";
		else if (strTmp == "Frontchamfer")
			strTmp = "Front chamfer";
		int i = strlist.indexOf(strTmp);

		if (i != -1)
			TextureTmp->setVisible(true);
		else
			TextureTmp->setVisible(false);
	}
}

// ���õ���״̬ʹ��
void L3DArtist::SlotSetToolBarStatusPic(int selsize)
{
	pTBtnExportScene->setEnabled(selsize == 1);
	pTBtnDeleteScene->setEnabled(selsize >= 1);
	// �˵����е�ʹ��  added by wuheng 2015.12.30
	ui.m_pExportScene->setEnabled(selsize == 1);
	ui.m_pDeleteScene->setEnabled(selsize >= 1);
}

//> �������޸Ĳ˵���
void L3DArtist::CreateMenuBar()
{
    m_pTBtnVAlign = new QAction(this);
    m_pTBtnVAlign->setObjectName(QStringLiteral("m_pTBtnVAlign"));
    m_pTBtnVAlign->setCheckable(true);   
    m_pTBtnVAlign->setToolTip(tr("Vertical Align"));
    m_pTBtnHAlign = new QAction(this);
    m_pTBtnHAlign->setObjectName(QStringLiteral("m_pTBtnHAlign"));
    m_pTBtnHAlign->setCheckable(true);  
    m_pTBtnHAlign->setToolTip(tr("Horizonal Align"));
    ui.m_pMenuEdit->addAction(m_pTBtnHAlign);
    ui.m_pMenuEdit->addAction(m_pTBtnVAlign);
	QActionGroup *pActionGroup = new QActionGroup(menuBar());
	pActionGroup->addAction(ui.m_pMove);
	pActionGroup->addAction(ui.m_pRotate);
    pActionGroup->addAction(m_pTBtnVAlign);
    pActionGroup->addAction(m_pTBtnHAlign);
	pActionGroup->addAction(ui.m_pZoom);
	pActionGroup->addAction(ui.m_pSize);
	pActionGroup->setExclusive(true);
	ui.m_pMenuEdit->insertActions(ui.m_pMaterial, pActionGroup->actions());
	ui.m_pMenuEdit->insertSeparator(ui.m_pMaterial);

	ui.m_pMove->setProperty("checked", true);
	ui.m_pRotate->setProperty("checked", true);
    m_pTBtnVAlign->setProperty("checked", true);
    m_pTBtnHAlign->setProperty("checked", true);
	ui.m_pZoom->setProperty("checked", true);
	ui.m_pSize->setProperty("checked", true);
	ui.m_pMove->setEnabled(false);
	ui.m_pRotate->setEnabled(false);
    m_pTBtnVAlign->setEnabled(false);
    m_pTBtnHAlign->setEnabled(false);
	ui.m_pZoom->setEnabled(false);
	ui.m_pSize->setEnabled(false);

	ui.m_pSafe->setCheckable(true);
	ui.m_pTitleSafe->setCheckable(true);
	ui.m_pShowRuler->setCheckable(true);

	ui.m_pImportNode->setVisible(false);
	ui.m_pExportNode->setVisible(false);
	ui.m_pExtractNode->setVisible(false);
	ui.m_pMaterial->setVisible(false);
	ui.m_pTexture->setVisible(false);
	ui.m_pReplicate->setVisible(false);
	ui.m_pShowGrid->setVisible(false);
	ui.m_pGridColor->setVisible(false);
	//	ui.m_pShowRuler->setVisible(false);
	ui.m_pOutput->setVisible(false);
	ui.m_pMultiView->setVisible(false);
	ui.m_pActualSize->setVisible(false);
	ui.m_pHelp->setVisible(false);
	ui.m_pDeleteScene->setDisabled(true);

	QActionGroup *pActionGroup_size = new QActionGroup(menuBar());
	pActionGroup_size->addAction(ui.actionSmall);
	pActionGroup_size->addAction(ui.actionMid);
	pActionGroup_size->addAction(ui.actionLarge);
	pActionGroup_size->setExclusive(true);
	ui.menuChangeSize->addActions(pActionGroup_size->actions());
	connect(pActionGroup_size, &QActionGroup::triggered, this, &L3DArtist::slotChangeSizeMsg);

	ui.actionSmall->setCheckable(true);
	ui.actionSmall->setObjectName("0");
	ui.actionMid->setCheckable(true);
	ui.actionMid->setObjectName("1");
	ui.actionLarge->setCheckable(true);
	ui.actionLarge->setObjectName("2");

	auto act = pActionGroup_size->actions().at(ArtistParams::instance()->GetCurSizeHint());
	act->setChecked(true);

	return;
}

void L3DArtist::slotChangeSizeMsg(QAction* act)
{
	MSGBOX_INFO_TIP(tr("When you start to take effect at next time"), this);
	act->setChecked(true);
	ArtistParams::instance()->SetCurSizeHint(act->objectName().toInt());
}

//> �������޸Ĺ�����
void L3DArtist::CreateToolBar()
{
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, ui.m_pTBarFile->toggleViewAction());
	ui.m_pTBarFile->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	//> added by xyc 2014.08.12 
	ui.m_pTbarTexture->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	ui.m_pMove->setChecked(true);

	SetToolBarIcon();
}

//> ������������ť��������ͼ�꣬����
void L3DArtist::SetToolBarIcon()
{
	//> //////////////////////////////////////////////////////////////////////////////////
	pTBtnNewScene = new ToolButton(this);
	pTBtnNewScene->setObjectName("pTBtnNewScene");
	pTBtnNewScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnNewScene->SetCurAction(ui.m_pNewScene);
	pTBtnNewScene->SetNormalIcon(":/L3DArtist/Resources/NewScene_.png");
	pTBtnNewScene->SetHoverIcon(":/L3DArtist/Resources/NewScene_h.png");
	pTBtnNewScene->SetPressedIcon(":/L3DArtist/Resources/NewScene_p.png");
	pTBtnNewScene->SetStyleSheetExt();

	pTBtnDeleteScene = new ToolButton(this);
	pTBtnDeleteScene->setObjectName("pTBtnDeleteScene");
	pTBtnDeleteScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnDeleteScene->SetCurAction(ui.m_pDeleteScene);
	pTBtnDeleteScene->SetNormalIcon(":/L3DArtist/Resources/Remove_.png");
	pTBtnDeleteScene->SetHoverIcon(":/L3DArtist/Resources/Remove_h.png");
	pTBtnDeleteScene->SetPressedIcon(":/L3DArtist/Resources/Remove_p.png");
	pTBtnDeleteScene->SetStyleSheetExt();
	pTBtnDeleteScene->setDisabled(true);  //added by wuheng 2016.01.05

	pTBtnImportScene = new ToolButton(this);
	pTBtnImportScene->setObjectName("pTBtnImportScene");
	pTBtnImportScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnImportScene->SetCurAction(ui.m_pImportScene);
	pTBtnImportScene->SetNormalIcon(":/L3DArtist/Resources/Import_.png");
	pTBtnImportScene->SetHoverIcon(":/L3DArtist/Resources/Import_h.png");
	pTBtnImportScene->SetPressedIcon(":/L3DArtist/Resources/Import_p.png");
	pTBtnImportScene->SetStyleSheetExt();

	pTBtnExportScene = new ToolButton(this);
	pTBtnExportScene->setObjectName("pTBtnExportScene");
	pTBtnExportScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnExportScene->SetCurAction(ui.m_pExportScene);
	pTBtnExportScene->SetNormalIcon(":/L3DArtist/Resources/Export_.png");
	pTBtnExportScene->SetHoverIcon(":/L3DArtist/Resources/Export_h.png");
	pTBtnExportScene->SetPressedIcon(":/L3DArtist/Resources/Export_p.png");
	pTBtnExportScene->SetStyleSheetExt();
	pTBtnExportScene->setEnabled(false);

	pTBtnSaveScene = new ToolButton(this);
	pTBtnSaveScene->setObjectName("pTBtnSaveScene");
	pTBtnSaveScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnSaveScene->SetCurAction(ui.m_pSaveScene);
	pTBtnSaveScene->SetNormalIcon(":/L3DArtist/Resources/Save_.png");
	pTBtnSaveScene->SetHoverIcon(":/L3DArtist/Resources/Save_h.png");
	pTBtnSaveScene->SetPressedIcon(":/L3DArtist/Resources/Save_p.png");
	pTBtnSaveScene->SetStyleSheetExt();
	pTBtnSaveScene->setDisabled(true);

	pTBtnUploadScene = new ToolButton(this);
	pTBtnUploadScene->setObjectName("pTBtnSaveScene");
	pTBtnUploadScene->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnUploadScene->SetNormalIcon(":/L3DArtist/Resources/Upload.png");
	pTBtnUploadScene->SetHoverIcon(":/L3DArtist/Resources/Upload_disable.png");
	pTBtnUploadScene->SetPressedIcon(":/L3DArtist/Resources/Upload.png");
	pTBtnUploadScene->SetStyleSheetExt();
	pTBtnUploadScene->setEnabled(false);
	pTBtnUploadScene->setToolTip(QStringLiteral("�ϴ�����"));
	if (ArtistParams::instance()->isArtistEditMode())
		pTBtnUploadScene->setVisible(false);

	QFrame *pFrameGroupScene = new QFrame(this);
	pFrameGroupScene->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupScene = new QHBoxLayout(pFrameGroupScene);
	pHLayoutGroupScene->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupScene->setSpacing(5);
	pHLayoutGroupScene->addWidget(pTBtnNewScene);
	pHLayoutGroupScene->addWidget(pTBtnDeleteScene);
	pHLayoutGroupScene->addWidget(pTBtnImportScene);
	pHLayoutGroupScene->addWidget(pTBtnExportScene);
	pHLayoutGroupScene->addWidget(pTBtnSaveScene);
	pHLayoutGroupScene->addWidget(pTBtnUploadScene);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupScene);

	connect(pTBtnUploadScene, &ToolButton::clicked, this, &L3DArtist::SlotSaveAndUpload);

	//> //////////////////////////////////////////////////////////////////////////////////
	m_pTBtnUndo = new ToolButton(this);
	m_pTBtnUndo->setObjectName("pTBtnUndo");
	m_pTBtnUndo->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnUndo->SetCurAction(ui.m_pUndo);
	m_pTBtnUndo->SetNormalIcon(":/L3DArtist/Resources/Undo_.png");
	m_pTBtnUndo->SetHoverIcon(":/L3DArtist/Resources/Undo_h.png");
	m_pTBtnUndo->SetPressedIcon(":/L3DArtist/Resources/Undo_p.png");
	m_pTBtnUndo->SetStyleSheetExt();

	m_pTBtnRedo = new ToolButton(this);
	m_pTBtnRedo->setObjectName("pTBtnRedo");
	m_pTBtnRedo->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnRedo->SetCurAction(ui.m_pRedo);
	m_pTBtnRedo->SetNormalIcon(":/L3DArtist/Resources/Redo_.png");
	m_pTBtnRedo->SetHoverIcon(":/L3DArtist/Resources/Redo_h.png");
	m_pTBtnRedo->SetPressedIcon(":/L3DArtist/Resources/Redo_p.png");
	m_pTBtnRedo->SetStyleSheetExt();

	QFrame *pFrameGroupUndoRedo = new QFrame(this);
	pFrameGroupUndoRedo->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupURdo = new QHBoxLayout(pFrameGroupUndoRedo);
	pHLayoutGroupURdo->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupURdo->setSpacing(5);
	pHLayoutGroupURdo->addWidget(m_pTBtnUndo);
	pHLayoutGroupURdo->addWidget(m_pTBtnRedo);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupUndoRedo);

	//> //////////////////////////////////////////////////////////////////////////////////
	pTBtnCut = new ToolButton(this);
	pTBtnCut->setObjectName("pTBtnCut");
	pTBtnCut->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnCut->SetCurAction(ui.m_pCut);
	pTBtnCut->SetNormalIcon(":/L3DArtist/Resources/Cut_.png");
	pTBtnCut->SetHoverIcon(":/L3DArtist/Resources/Cut_h.png");
	pTBtnCut->SetPressedIcon(":/L3DArtist/Resources/Cut_p.png");
	pTBtnCut->SetStyleSheetExt();

	pTBtnCopy = new ToolButton(this);
	pTBtnCopy->setObjectName("pTBtnCopy");
	pTBtnCopy->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnCopy->SetCurAction(ui.m_pCopy);
	pTBtnCopy->SetNormalIcon(":/L3DArtist/Resources/Copy_.png");
	pTBtnCopy->SetHoverIcon(":/L3DArtist/Resources/Copy_h.png");
	pTBtnCopy->SetPressedIcon(":/L3DArtist/Resources/Copy_p.png");
	pTBtnCopy->SetStyleSheetExt();

	pTBtnPaste = new ToolButton(this);
	pTBtnPaste->setObjectName("pTBtnPaste");
	pTBtnPaste->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnPaste->SetCurAction(ui.m_pPaste);
	pTBtnPaste->SetNormalIcon(":/L3DArtist/Resources/Paste_.png");
	pTBtnPaste->SetHoverIcon(":/L3DArtist/Resources/Paste_h.png");
	pTBtnPaste->SetPressedIcon(":/L3DArtist/Resources/Paste_p.png");
	pTBtnPaste->SetStyleSheetExt();

	QFrame *pFrameGroupCCP = new QFrame(this);
	pFrameGroupCCP->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupCCP = new QHBoxLayout(pFrameGroupCCP);
	pHLayoutGroupCCP->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupCCP->setSpacing(5);
	pHLayoutGroupCCP->addWidget(pTBtnCut);
	pHLayoutGroupCCP->addWidget(pTBtnCopy);
	pHLayoutGroupCCP->addWidget(pTBtnPaste);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupCCP);

    //> //////////////////////////////////////////////////////////////////////////////////
    pTBtnVAlign = new ToolButton(this);
    pTBtnVAlign->setObjectName("pTBtnVAlign");
    pTBtnVAlign->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
    pTBtnVAlign->SetCurAction(m_pTBtnVAlign);
    pTBtnVAlign->SetNormalIcon(":/L3DArtist/Resources/VerticalAlign.png");
    pTBtnVAlign->SetHoverIcon(":/L3DArtist/Resources/VerticalAlign_h.png");
    pTBtnVAlign->SetPressedIcon(":/L3DArtist/Resources/VerticalAlign_p.png");
    pTBtnVAlign->SetStyleSheetExt();

    pTBtnHAlign = new ToolButton(this);
    pTBtnHAlign->setObjectName("pTBtnHAlign");
    pTBtnHAlign->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
    pTBtnHAlign->SetCurAction(m_pTBtnHAlign);
    pTBtnHAlign->SetNormalIcon(":/L3DArtist/Resources/HorizonalAlign.png");
    pTBtnHAlign->SetHoverIcon(":/L3DArtist/Resources/HorizonalAlign_h.png");
    pTBtnHAlign->SetPressedIcon(":/L3DArtist/Resources/HorizonalAlign_p.png");
    pTBtnHAlign->SetStyleSheetExt();

    QFrame *pFrameGroupVH = new QFrame(this);
    pFrameGroupVH->setProperty("framecolor", true);
    QHBoxLayout *pHLayoutGroupVH = new QHBoxLayout(pFrameGroupVH);
    pHLayoutGroupVH->setContentsMargins(5, 0, 5, 0);
    pHLayoutGroupVH->setSpacing(5);
    pHLayoutGroupVH->addWidget(pTBtnVAlign);
    pHLayoutGroupVH->addWidget(pTBtnHAlign);
    ui.m_pTBarFile->addSeparator();
    ui.m_pTBarFile->addWidget(pFrameGroupVH);

	//> //////////////////////////////////////////////////////////////////////////////////
	pTBtnMove = new ToolButton(this);
	pTBtnMove->setObjectName("pTBtnMove");
	pTBtnMove->setShortcut(Qt::Key_F5);
	pTBtnMove->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnMove->SetCurAction(ui.m_pMove);
	pTBtnMove->SetNormalIcon(":/L3DArtist/Resources/Move_.png");
	pTBtnMove->SetHoverIcon(":/L3DArtist/Resources/Move_h.png");
	pTBtnMove->SetPressedIcon(":/L3DArtist/Resources/Move_p.png");
	pTBtnMove->SetStyleSheetExt();
	pTBtnMove->setCheckable(true);
	pTBtnMove->setDisabled(true);   //added by wuheng 2016.03.23
 
    pTBtnRotate = new ToolButton(this);
    pTBtnRotate->setObjectName("pTBtnRotate");
    pTBtnRotate->setShortcut(Qt::Key_F6);
    pTBtnRotate->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
    pTBtnRotate->SetCurAction(ui.m_pRotate);
    pTBtnRotate->SetNormalIcon(":/L3DArtist/Resources/Rotate_.png");
    pTBtnRotate->SetHoverIcon(":/L3DArtist/Resources/Rotate_h.png");
    pTBtnRotate->SetPressedIcon(":/L3DArtist/Resources/Rotate_p.png");
    pTBtnRotate->SetStyleSheetExt();
    pTBtnRotate->setCheckable(true);
    pTBtnRotate->setDisabled(true);

	pTBtnZoom = new ToolButton(this);
	pTBtnZoom->setObjectName("pTBtnZoom");
	pTBtnZoom->setShortcut(Qt::Key_F7);
	pTBtnZoom->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnZoom->SetCurAction(ui.m_pZoom);
	pTBtnZoom->SetNormalIcon(":/L3DArtist/Resources/Scale_.png");
	pTBtnZoom->SetHoverIcon(":/L3DArtist/Resources/Scale_h.png");
	pTBtnZoom->SetPressedIcon(":/L3DArtist/Resources/Scale_p.png");
	pTBtnZoom->SetStyleSheetExt();
	pTBtnZoom->setCheckable(true);
	pTBtnZoom->setDisabled(true);

	pTBtnSize = new ToolButton(this);
	pTBtnSize->setObjectName("pTBtnSize");
	pTBtnSize->setShortcut(Qt::Key_F8);
	pTBtnSize->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnSize->SetCurAction(ui.m_pSize);
	pTBtnSize->SetNormalIcon(":/L3DArtist/Resources/Size_.png");
	pTBtnSize->SetHoverIcon(":/L3DArtist/Resources/Size_1.png");
	pTBtnSize->SetPressedIcon(":/L3DArtist/Resources/Size_1.png");
	pTBtnSize->SetStyleSheetExt();
	pTBtnSize->setCheckable(true);
	pTBtnSize->setDisabled(true);

	connect(m_pPreviewWnd, &PreviewWindow::SglClickedPosition, pTBtnMove, &ToolButton::click);
	connect(m_pPreviewWnd, &PreviewWindow::SglClickedRotate, pTBtnRotate, &ToolButton::click);
	connect(m_pPreviewWnd, &PreviewWindow::SglClickedScale, pTBtnZoom, &ToolButton::click);
	connect(m_pPreviewWnd, &PreviewWindow::SglClickedSize, pTBtnSize, &ToolButton::click);   

	QButtonGroup *pBtnGroup = new QButtonGroup(this);
	pBtnGroup->setExclusive(true);
	pBtnGroup->addButton(pTBtnMove);
	pBtnGroup->addButton(pTBtnRotate);
	pBtnGroup->addButton(pTBtnZoom);
	pBtnGroup->addButton(pTBtnSize);

	QFrame *pFrameGroupOperatate = new QFrame(this);
	pFrameGroupOperatate->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupOp = new QHBoxLayout(pFrameGroupOperatate);
	pHLayoutGroupOp->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupOp->setSpacing(5);
	pHLayoutGroupOp->addWidget(pTBtnMove);
	pHLayoutGroupOp->addWidget(pTBtnRotate);
	pHLayoutGroupOp->addWidget(pTBtnZoom);
	pHLayoutGroupOp->addWidget(pTBtnSize);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupOperatate);

	//> //////////////////////////////////////////////////////////////////////////////////
	pTBtnOutput = new ToolButton(this);
	pTBtnOutput->setObjectName("pTBtnOutput");
	pTBtnOutput->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnOutput->SetCurAction(ui.m_pOutput);
	pTBtnOutput->SetNormalIcon(":/L3DArtist/Resources/Output_.png");
	pTBtnOutput->SetHoverIcon(":/L3DArtist/Resources/Output_h.png");
	pTBtnOutput->SetPressedIcon(":/L3DArtist/Resources/Output_p.png");
	pTBtnOutput->SetStyleSheetExt();
	pTBtnOutput->setVisible(false);

	pTBtnMultiView = new ToolButton(this);
	pTBtnMultiView->setObjectName("pTBtnMultiView");
	pTBtnMultiView->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnMultiView->SetCurAction(ui.m_pMultiView);
	pTBtnMultiView->SetNormalIcon(":/L3DArtist/Resources/Multiview_.png");
	pTBtnMultiView->SetHoverIcon(":/L3DArtist/Resources/Multiview_h.png");
	pTBtnMultiView->SetPressedIcon(":/L3DArtist/Resources/Multiview_p.png");
	pTBtnMultiView->SetStyleSheetExt();
	pTBtnMultiView->setVisible(false);

	QFrame *pFrameGroupVideo = new QFrame(this);
	pFrameGroupVideo->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupVideo = new QHBoxLayout(pFrameGroupVideo);
	pHLayoutGroupVideo->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupVideo->setSpacing(5);
	pHLayoutGroupVideo->addWidget(pTBtnOutput);
	pHLayoutGroupVideo->addWidget(pTBtnMultiView);
	ui.m_pTBarFile->addWidget(pFrameGroupVideo);

	//> //////////////////////////////////////////////////////////////////////////////////
	pTBtnAbout = new ToolButton(this);
	pTBtnAbout->setObjectName("pTBtnAbout");
	pTBtnAbout->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	pTBtnAbout->SetCurAction(ui.m_pAbout);
	pTBtnAbout->SetNormalIcon(":/L3DArtist/Resources/About_.png");
	pTBtnAbout->SetHoverIcon(":/L3DArtist/Resources/About_h.png");
	pTBtnAbout->SetPressedIcon(":/L3DArtist/Resources/About_p.png");
	pTBtnAbout->SetStyleSheetExt();

	QFrame *pFrameGroupAbout = new QFrame(this);
	pFrameGroupAbout->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupAbout = new QHBoxLayout(pFrameGroupAbout);
	pHLayoutGroupAbout->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupAbout->setSpacing(5);
	pHLayoutGroupAbout->addWidget(pTBtnAbout);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupAbout);

	if (ArtistParams::instance()->isArtistEditMode())
	{
		pCheckBoxSync = new QCheckBox(this);
		pCheckBoxSync->setText(QStringLiteral("ͬ��������"));
		pCheckBoxSync->setToolTip(QStringLiteral("��ѡ����,����ʱ��ͬʱ�޸������µĳ���"));
		pCheckBoxSync->setChecked(ArtistParams::instance()->isSyncArtist());
		ui.m_pTBarFile->addWidget(pCheckBoxSync);
		pCheckBoxSync->setEnabled(true);
	}

	ui.m_pTBarFile->addSeparator();
	/******************************************��Ⱦ���ڶ��빦������*********************************
	//��Ⱦ���ڶ��빦��
	//X�᷽��,�����,�Ҷ���,���ж���
	AlignMenuX = new QMenu();
	AlignMenuX_Left = new QAction(AlignMenuX);
	AlignMenuX_Left->setText(tr("XLeft"));
	AlignMenuX_Left->setCheckable(true);
	connect(AlignMenuX_Left, &QAction::triggered, this, &L3DArtist::SlotAlignX);

	AlignMenuX_Right = new QAction(AlignMenuX);
	AlignMenuX_Right->setText(tr("XRight"));
	AlignMenuX_Right->setCheckable(true);
	connect(AlignMenuX_Right, &QAction::triggered, this, &L3DArtist::SlotAlignX);

	AlignMenuX_Center = new QAction(AlignMenuX);
	AlignMenuX_Center->setText(tr("XCenter"));
	AlignMenuX_Center->setCheckable(true);
	connect(AlignMenuX_Center, &QAction::triggered, this, &L3DArtist::SlotAlignX);

	AlignMenuX_Aliquots = new QAction(AlignMenuX);
	AlignMenuX_Aliquots->setText(tr("XAliquots"));
	AlignMenuX_Aliquots->setCheckable(true);
	connect(AlignMenuX_Aliquots, &QAction::triggered, this, &L3DArtist::SlotAlignX);

	AlignMenuX->addAction(AlignMenuX_Left);
	AlignMenuX->addAction(AlignMenuX_Right);
	AlignMenuX->addAction(AlignMenuX_Center);
	AlignMenuX->addAction(AlignMenuX_Aliquots);
	//��ӽ���
	QActionGroup *actionGroupX =new QActionGroup(this);
	actionGroupX->addAction(AlignMenuX_Left);
	actionGroupX->addAction(AlignMenuX_Right);
	actionGroupX->addAction(AlignMenuX_Center);
	actionGroupX->addAction(AlignMenuX_Aliquots);

	pTBtnAlignX = new ToolButton(this);
	pTBtnAlignX->setObjectName("pTBtnAlignX");
	pTBtnAlignX->setFixedSize(34, 24);
	//pTBtnAlignX->SetCurAction(ui.m_pAlignX);
	pTBtnAlignX->setIcon((QIcon)":/L3DArtist/Resources/About_.png");
	pTBtnAlignX->setIconSize(QSize(24,24));
	pTBtnAlignX->setMenu(AlignMenuX);
	pTBtnAlignX->setPopupMode(QToolButton::MenuButtonPopup);

	//Y�᷽��,�����,�Ҷ���,���ж���
	AlignMenuY = new QMenu();
	AlignMenuY_Left = new QAction(AlignMenuY);
	AlignMenuY_Left->setText(tr("YLeft"));
	AlignMenuY_Left->setCheckable(true);
	connect(AlignMenuY_Left, &QAction::triggered, this, &L3DArtist::SlotAlignY);

	AlignMenuY_Right = new QAction(AlignMenuY);
	AlignMenuY_Right->setText(tr("YRight"));
	AlignMenuY_Right->setCheckable(true);
	connect(AlignMenuY_Right, &QAction::triggered, this, &L3DArtist::SlotAlignY);

	AlignMenuY_Center = new QAction(AlignMenuY);
	AlignMenuY_Center->setText(tr("YCenter"));
	AlignMenuY_Center->setCheckable(true);
	connect(AlignMenuY_Center, &QAction::triggered, this, &L3DArtist::SlotAlignY);

	AlignMenuY_Aliquots = new QAction(AlignMenuY);
	AlignMenuY_Aliquots->setText(tr("YAliquots"));
	AlignMenuY_Aliquots->setCheckable(true);
	connect(AlignMenuY_Aliquots, &QAction::triggered, this, &L3DArtist::SlotAlignY);

	AlignMenuY->addAction(AlignMenuY_Left);
	AlignMenuY->addAction(AlignMenuY_Right);
	AlignMenuY->addAction(AlignMenuY_Center);
	AlignMenuY->addAction(AlignMenuY_Aliquots);
	//��ӽ���
	QActionGroup *actionGroupY =new QActionGroup(this);
	actionGroupY->addAction(AlignMenuY_Left);
	actionGroupY->addAction(AlignMenuY_Right);
	actionGroupY->addAction(AlignMenuY_Center);
	actionGroupY->addAction(AlignMenuY_Aliquots);

	pTBtnAlignY = new ToolButton(this);
	pTBtnAlignY->setObjectName("pTBtnAlignY");
	pTBtnAlignY->setFixedSize(34, 24);
	//pTBtnAlignY->SetCurAction(ui.m_pAlignY);
	pTBtnAlignY->setIcon((QIcon)":/L3DArtist/Resources/About_.png");
	pTBtnAlignY->setIconSize(QSize(24,24));
	pTBtnAlignY->setMenu(AlignMenuY);
	pTBtnAlignY->setPopupMode(QToolButton::MenuButtonPopup);

	//Z�᷽��,�����,�Ҷ���,���ж���
	AlignMenuZ = new QMenu();
	AlignMenuZ_Left = new QAction(AlignMenuZ);
	AlignMenuZ_Left->setText(tr("ZLeft"));
	AlignMenuZ_Left->setCheckable(true);
	connect(AlignMenuZ_Left, &QAction::triggered, this, &L3DArtist::SlotAlignZ);

	AlignMenuZ_Right = new QAction(AlignMenuZ);
	AlignMenuZ_Right->setText(tr("ZRight"));
	AlignMenuZ_Right->setCheckable(true);
	connect(AlignMenuZ_Right, &QAction::triggered, this, &L3DArtist::SlotAlignZ);

	AlignMenuZ_Center = new QAction(AlignMenuZ);
	AlignMenuZ_Center->setText(tr("ZCenter"));
	AlignMenuZ_Center->setCheckable(true);
	connect(AlignMenuZ_Center, &QAction::triggered, this, &L3DArtist::SlotAlignZ);

	AlignMenuZ_Aliquots = new QAction(AlignMenuZ);
	AlignMenuZ_Aliquots->setText(tr("ZCenter"));
	AlignMenuZ_Aliquots->setCheckable(true);
	connect(AlignMenuZ_Aliquots, &QAction::triggered, this, &L3DArtist::SlotAlignZ);

	AlignMenuZ->addAction(AlignMenuZ_Left);
	AlignMenuZ->addAction(AlignMenuZ_Right);
	AlignMenuZ->addAction(AlignMenuZ_Center);
	AlignMenuZ->addAction(AlignMenuZ_Aliquots);

	//��ӽ���
	QActionGroup *actionGroupZ =new QActionGroup(this);
	actionGroupZ->addAction(AlignMenuZ_Left);
	actionGroupZ->addAction(AlignMenuZ_Right);
	actionGroupZ->addAction(AlignMenuZ_Center);
	actionGroupZ->addAction(AlignMenuZ_Aliquots);

	pTBtnAlignZ = new ToolButton(this);
	pTBtnAlignZ->setObjectName("pTBtnAlignZ");
	pTBtnAlignZ->setFixedSize(34, 24);
	//pTBtnAlignZ->SetCurAction(ui.m_pAlignZ);
	pTBtnAlignZ->setIcon((QIcon)":/L3DArtist/Resources/About_.png");
	pTBtnAlignZ->setIconSize(QSize(24,24));
	pTBtnAlignZ->setMenu(AlignMenuZ);
	pTBtnAlignZ->setPopupMode(QToolButton::MenuButtonPopup);

	QFrame *pFrameGroupAlign = new QFrame(this);
	pFrameGroupAlign->setProperty("framecolor", true);
	QHBoxLayout *pHLayoutGroupAlign = new QHBoxLayout(pFrameGroupAlign);
	pHLayoutGroupAlign->setContentsMargins(5, 0, 5, 0);
	pHLayoutGroupAlign->setSpacing(5);
	pHLayoutGroupAlign->addWidget(pTBtnAlignX);
	pHLayoutGroupAlign->addWidget(pTBtnAlignY);
	pHLayoutGroupAlign->addWidget(pTBtnAlignZ);
	ui.m_pTBarFile->addSeparator();
	ui.m_pTBarFile->addWidget(pFrameGroupAlign);
	/******************************************��Ⱦ���ڶ��빦������*********************************/
	InitToolBarTexture();

	//> ////////////////////////////////////////////////
	ui.m_pMove->setChecked(true);
	pTBtnMove->setChecked(true);

}

//> �������޸�״̬��
void L3DArtist::CreateStatusBar()
{
	bool bsimple = ArtistParams::instance()->isArtistEditMode();

	statusBar()->setContentsMargins(5, 0, 30, 0);
	m_pLblStatus = new QLabel(this);
	m_pLblStatus->setProperty("statusbk", true);
	m_pLblStatus->hide();

	m_pLblTitle = new QLabel(tr("Artist CG Manufacture"));
	m_pLblTitle->setProperty("statusbk", true);
	QLabel *pLblSpace5 = new QLabel("                                             ");
	pLblSpace5->setProperty("statusbk", true);

	m_pProgress = new QProgressBar(this);
	m_pProgress->setStyle(QStyleFactory::create("windows"));
	m_pProgress->setTextVisible(false);
	m_pProgress->setFixedSize(80, 15);
	m_pProgress->setRange(0, 0);
	m_pProgress->hide();

	QLabel *pLblSpace4 = new QLabel("     ");
	pLblSpace4->setProperty("statusbk", true);

	m_pLblProject = new QLabel(this);
	m_pLblProject->setProperty("statusbk", true);
	m_pLblStatusProject = new RollLabel(this);
	m_pLblStatusProject->SetText(m_strProjectName, 500);
	m_pLblStatusProject->setFixedWidth(95);
	m_pLblStatusProject->setProperty("statusbk", true);
	QLabel *pLblSpace1 = new QLabel("   ");
	pLblSpace1->setProperty("statusbk", true);

	m_pLableScene = new QLabel(this);
	m_pLableScene->setProperty("statusbk", true);
	m_pLblStatusScene = new RollLabel(this);
	m_pLblStatusScene->SetText(tr("None"), 500);
	m_pLblStatusScene->setFixedWidth(85);
	m_pLblStatusScene->setProperty("statusbk", true);
	QLabel *pLblSpace2 = new QLabel("   ");
	pLblSpace2->setProperty("statusbk", true);

	if (!bsimple)
	{
		statusBar()->addWidget(m_pLblStatus);
		statusBar()->addPermanentWidget(m_pLblTitle);
		statusBar()->addPermanentWidget(pLblSpace5);
		statusBar()->addPermanentWidget(m_pProgress);
		statusBar()->addPermanentWidget(pLblSpace4);
		statusBar()->addPermanentWidget(m_pLblProject);
		statusBar()->addPermanentWidget(m_pLblStatusProject);
		statusBar()->addPermanentWidget(pLblSpace1);
		statusBar()->addPermanentWidget(m_pLableScene);
		statusBar()->addPermanentWidget(m_pLblStatusScene);
		statusBar()->addPermanentWidget(pLblSpace2);
	}
	else
	{
		statusBar()->addPermanentWidget(m_pLblTitle);
		m_pLblTitle->setText(tr("Scene Editor"));
		m_pLblProject->setVisible(false);
		m_pLableScene->setVisible(false);
		pLblSpace1->setVisible(false);
		pLblSpace2->setVisible(false);
		pLblSpace4->setVisible(false);
		pLblSpace5->setVisible(false);
		m_pLblStatusScene->setVisible(false);
		m_pLblStatusProject->setVisible(false);
	}

	/*m_pLblStatusUser = new RollLabel(this);
	m_pLblStatusUser->SetText("Admin", 500);
	m_pLblStatusUser->setFixedWidth(85);
	m_pLblStatusUser->setProperty("statusbk", true);
	statusBar()->addPermanentWidget(m_pLblStatusUser);
	QLabel *pLblSpace3 = new QLabel("   ");
	pLblSpace3->setProperty("statusbk", true);
	statusBar()->addPermanentWidget(pLblSpace3);

	m_pLblStatusUser->hide();
	pLblSpace3->hide();

	m_pLblStatusTime = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	m_pLblStatusTime->setProperty("statusbk", true);
	statusBar()->addPermanentWidget(m_pLblStatusTime);*/

	m_pLblSizeGrip = new QLabel(this);
	m_pLblSizeGrip->setProperty("statusbk", true);
	QPixmap pixmap(":/L3DArtist/Resources/sizegrip.png");
	m_pLblSizeGrip->setPixmap(pixmap);
	statusBar()->addPermanentWidget(m_pLblSizeGrip);
	m_pLblSizeGrip->hide();

	statusBar()->setSizeGripEnabled(false);
}

//> ����������
void L3DArtist::CreateCtrlBar()
{
	//> added by TaiYue 2019.6.5 for λ�Ʋ���ѡ��������
	m_PDComboxChoose = new QComboBox(this);
	m_PDComboxChoose->addItem("0.01",Qt::AlignCenter);
	m_PDComboxChoose->addItem("0.1", Qt::AlignCenter);
	m_PDComboxChoose->addItem("1", Qt::AlignCenter);
	m_PDComboxChoose->addItem("10", Qt::AlignCenter);
	m_PDComboxChoose->setCurrentIndex(1);
	m_PDComboxChoose->setToolTip(tr("Step Size"));

	PreNodeType = NODECTRL_MOVE;

	m_nPreMoveStatus = 1;// Ĭ�ϳ�ʼ��ʱ����Ϊ�±�indexΪ1��0.1����
	m_nPreRotateStatus = 1;// Ĭ�ϲ���Ϊ�±�indexΪ1��0.1��ת����
	m_nPreScaleStatus = 0;

	m_nGetMoveFocusXYZ = 0;
	m_nGetRotateFocusXYZ = 0;
	m_nGetScaleFocusXYZ = 0;
	
	m_rotateFirstState = true;
	m_scaleFirstState = true;

	QFont fontTmp = font();
	QLabel *pLblX = new QLabel(" X:", this);
	pLblX->setObjectName("ctrlbk_x");
	//> x���ڿ�
	m_pDSpinBoxX = new SpinBox_ControlBox(this);
	m_pDSpinBoxX->setProperty("ctrlspinbox", true);
	m_pDSpinBoxX->setRange(-1000000000.00, 1000000000.00);
	m_pDSpinBoxX->setSingleStep(0.1);
	QLabel *pLblY = new QLabel(" Y:", this);
	pLblY->setObjectName("ctrlbk_y");
	//> y���ڿ�
	m_pDSpinBoxY = new SpinBox_ControlBox(this);
	m_pDSpinBoxY->setProperty("ctrlspinbox", true);
	m_pDSpinBoxY->setRange(-1000000000.00, 1000000000.00);
	m_pDSpinBoxY->setSingleStep(0.1);
	QLabel *pLblZ = new QLabel(" Z:", this);
	pLblZ->setObjectName("ctrlbk_z");
	//> z���ڿ�
	m_pDSpinBoxZ = new SpinBox_ControlBox(this);
	m_pDSpinBoxZ->setProperty("ctrlspinbox", true);
	m_pDSpinBoxZ->setRange(-1000000000.00, 1000000000.00);
	m_pDSpinBoxZ->setSingleStep(0.1);


	m_PDComboxChoose->setFixedSize(QSize(60, 20));
	pLblX->setFixedHeight(20);
	pLblY->setFixedHeight(20);
	pLblZ->setFixedHeight(20);
	m_pDSpinBoxX->setFont(fontTmp);
	m_pDSpinBoxY->setFont(fontTmp);
	m_pDSpinBoxZ->setFont(fontTmp);
	m_pDSpinBoxX->setFixedWidth(control_box_width[ArtistParams::instance()->GetCurSizeHint()]);
	m_pDSpinBoxY->setFixedWidth(control_box_width[ArtistParams::instance()->GetCurSizeHint()]);
	m_pDSpinBoxZ->setFixedWidth(control_box_width[ArtistParams::instance()->GetCurSizeHint()]);
	m_pDSpinBoxX->setAlignment(Qt::AlignCenter);
	m_pDSpinBoxY->setAlignment(Qt::AlignCenter);
	m_pDSpinBoxZ->setAlignment(Qt::AlignCenter);

	connect(m_pDSpinBoxX, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &L3DArtist::SlotValueChangeX);
	connect(m_pDSpinBoxY, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &L3DArtist::SlotValueChangeY);
	connect(m_pDSpinBoxZ, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &L3DArtist::SlotValueChangeZ);

	//> ��λ��ť
	m_pTBtnReset = new ToolButton(this);
	m_pTBtnReset->setObjectName("m_pTBtnResetPreview");
	m_pTBtnReset->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnReset->SetNormalIcon(":/L3DArtist/Resources/Refresh_.png");
	m_pTBtnReset->SetHoverIcon(":/L3DArtist/Resources/Refresh_h.png");
	m_pTBtnReset->SetPressedIcon(":/L3DArtist/Resources/Refresh_p.png");
	m_pTBtnReset->SetStyleSheetExt();
	connect(m_pTBtnReset, &ToolButton::clicked, this, &L3DArtist::SlotBtnResetOnClick, Qt::QueuedConnection);

	m_pTBtnPre = new ToolButton(this);
	m_pTBtnPre->setObjectName("m_pTBtnPre");
	m_pTBtnPre->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnPre->SetNormalIcon(":/L3DArtist/Resources/TMPre_.png");
	m_pTBtnPre->SetHoverIcon(":/L3DArtist/Resources/TMPre_h.png");
	m_pTBtnPre->SetPressedIcon(":/L3DArtist/Resources/TMPre_p.png");
	m_pTBtnPre->SetStyleSheetExt();
	m_pTBtnPre->setToolTip(tr("Begin"));
	connect(m_pTBtnPre, &ToolButton::clicked, this, &L3DArtist::SlotBtnPreOnCli, Qt::QueuedConnection);
	m_pTBtnBack = new ToolButton(this);
	m_pTBtnBack->setObjectName("m_pTBtnBack");
	m_pTBtnBack->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnBack->SetNormalIcon(":/L3DArtist/Resources/TMBack_.png");
	m_pTBtnBack->SetHoverIcon(":/L3DArtist/Resources/TMBack_h.png");
	m_pTBtnBack->SetPressedIcon(":/L3DArtist/Resources/TMBack_p.png");
	m_pTBtnBack->SetStyleSheetExt();
	m_pTBtnBack->setToolTip(tr("back"));
	connect(m_pTBtnBack, &ToolButton::clicked, this, &L3DArtist::SlotBtnBackOnCli, Qt::QueuedConnection);
	m_pTBtnPlay = new ToolButton(this);
	m_pTBtnPlay->setObjectName("m_pTBtnPlay");
	m_pTBtnPlay->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnPlay->SetNormalIcon(":/L3DArtist/Resources/TMPlay_.png");
	m_pTBtnPlay->SetHoverIcon(":/L3DArtist/Resources/TMPlay_h.png");
	m_pTBtnPlay->SetPressedIcon(":/L3DArtist/Resources/TMPlay_p.png");
	m_pTBtnPlay->SetStyleSheetExt();
	m_pTBtnPlay->setToolTip(tr("Play (Space)"));
	m_pTBtnPlay->setShortcut(QKeySequence(Qt::Key_Space));
	// ��ʼ�� ��ť״̬ 
	connect(m_pTBtnPlay, &ToolButton::clicked, this, &L3DArtist::SlotBtnPlayOnCli, Qt::QueuedConnection);

	m_pTBtnPlay->setEnabled(true);

	m_pTBtnForward = new ToolButton(this);
	m_pTBtnForward->setObjectName("m_pTBtnForward");
	m_pTBtnForward->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnForward->SetNormalIcon(":/L3DArtist/Resources/TMForward_.png");
	m_pTBtnForward->SetHoverIcon(":/L3DArtist/Resources/TMForward_h.png");
	m_pTBtnForward->SetPressedIcon(":/L3DArtist/Resources/TMForward_p.png");
	m_pTBtnForward->SetStyleSheetExt();
	m_pTBtnForward->setToolTip(tr("Forward"));
	connect(m_pTBtnForward, &ToolButton::clicked, this, &L3DArtist::SlotBtnForwardOnCli, Qt::QueuedConnection);
	m_pTBtnNext = new ToolButton(this);
	m_pTBtnNext->setObjectName("m_pTBtnNext");
	m_pTBtnNext->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnNext->SetNormalIcon(":/L3DArtist/Resources/TMNext_.png");
	m_pTBtnNext->SetHoverIcon(":/L3DArtist/Resources/TMNext_h.png");
	m_pTBtnNext->SetPressedIcon(":/L3DArtist/Resources/TMNext_p.png");
	m_pTBtnNext->SetStyleSheetExt();
	m_pTBtnNext->setToolTip(tr("End"));
	connect(m_pTBtnNext, &ToolButton::clicked, this, &L3DArtist::SlotBtnNextOnCli, Qt::QueuedConnection);

	m_pTBtnLoop = new ToolButton(this);
	m_pTBtnLoop->setObjectName("m_pTBtnLoop");
	m_pTBtnLoop->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnLoop->SetNormalIcon(":/L3DArtist/Resources/Circle_Y.png");
	m_pTBtnLoop->SetHoverIcon(":/L3DArtist/Resources/Circle_Y.png");
	m_pTBtnLoop->SetPressedIcon(":/L3DArtist/Resources/Circle_Y.png");
	m_pTBtnLoop->SetStyleSheetExt();
	m_pTBtnLoop->setToolTip(tr("Loop"));
	connect(m_pTBtnLoop, &ToolButton::clicked, this, &L3DArtist::SlotBtnLoopOnCli, Qt::QueuedConnection);

	m_pTBtnFullScreen = new ToolButton(this);
	m_pTBtnFullScreen->setObjectName("m_pTBtnFullScreen");
	m_pTBtnFullScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F11));
	m_pTBtnFullScreen->setFixedSize(cur_toolbtn[ArtistParams::instance()->GetCurSizeHint()]);
	m_pTBtnFullScreen->SetNormalIcon(":/L3DArtist/Resources/FullScreen_.png");
	m_pTBtnFullScreen->SetHoverIcon(":/L3DArtist/Resources/FullScreen_h.png");
	m_pTBtnFullScreen->SetPressedIcon(":/L3DArtist/Resources/FullScreen_p.png");
	m_pTBtnFullScreen->SetStyleSheetExt();

	QFrame *pFrameCtrl = new QFrame(this);
	pFrameCtrl->setObjectName("FrameCtrl");
	pFrameCtrl->setFixedHeight(30);
	QHBoxLayout *pHLayoutCtrl = new QHBoxLayout;
	pHLayoutCtrl->setContentsMargins(10, 0, 10, 0);
	pHLayoutCtrl->addStretch();
	pHLayoutCtrl->addWidget(m_PDComboxChoose);
	pHLayoutCtrl->addWidget(pLblX);
	pHLayoutCtrl->addWidget(m_pDSpinBoxX);
	pHLayoutCtrl->addWidget(pLblY);
	pHLayoutCtrl->addWidget(m_pDSpinBoxY);
	pHLayoutCtrl->addWidget(pLblZ);
	pHLayoutCtrl->addWidget(m_pDSpinBoxZ);
	pHLayoutCtrl->addWidget(m_pTBtnReset);
	pHLayoutCtrl->addSpacing(20);
	pHLayoutCtrl->addWidget(m_pTBtnPre);
	pHLayoutCtrl->addWidget(m_pTBtnBack);
	pHLayoutCtrl->addWidget(m_pTBtnPlay);
	pHLayoutCtrl->addWidget(m_pTBtnForward);
	pHLayoutCtrl->addWidget(m_pTBtnNext);
	pHLayoutCtrl->addWidget(m_pTBtnLoop);
	pHLayoutCtrl->addStretch();
	pHLayoutCtrl->addWidget(m_pTBtnFullScreen);
	pFrameCtrl->setLayout(pHLayoutCtrl);

	GlobalCfgParam& EKParam = ArtistParams::instance()->getEKParams();
	m_pPreviewWnd = new PreviewWindow(EKParam.isDrawRulers, this);
	m_pPreviewWnd->setObjectName("m_pPreviewWnd");

	connect(m_pTBtnFullScreen, &ToolButton::clicked, m_pPreviewWnd, &PreviewWindow::SlotFullScreen);

	QVBoxLayout *pVLayoutCen = new QVBoxLayout;
	pVLayoutCen->setContentsMargins(20, 20, 20, 20);
	if (ArtistParams::instance()->isArtistEditMode())
		pVLayoutCen->setContentsMargins(0, 0, 0, 0);
	pVLayoutCen->addWidget(m_pPreviewWnd);

	QHBoxLayout *pHLayoutSimple = new QHBoxLayout;
	if (ArtistParams::instance()->isArtistEditMode())
	{
		m_pTBtnFullScreen->setVisible(false);
		connect(this, &L3DArtist::SglUploadSimple, this, &L3DArtist::SlotUpload);
	}

	QVBoxLayout *pVLayoutCentral = new QVBoxLayout;
	pVLayoutCentral->setContentsMargins(0, 0, 0, 0);
	pVLayoutCentral->setSpacing(0);
	pVLayoutCentral->addLayout(pVLayoutCen);
	pVLayoutCentral->addWidget(pFrameCtrl);
	if (ArtistParams::instance()->isArtistEditMode())
	{
		//pVLayoutCentral->addStretch();
		pVLayoutCentral->addSpacing(10);
		pVLayoutCentral->addLayout(pHLayoutSimple);
		pVLayoutCentral->addSpacing(10);
	}

	if (nullptr == m_pCentralWidget) 
	{
		m_pCentralWidget = new QWidget(this);
		m_pCentralWidget->setLayout(/*pHLayoutMain*/pVLayoutCentral);
		m_pCentralWidget->setObjectName("centralWidget");
	}

	setCentralWidget(m_pCentralWidget);
}

//> �����ڵ����� ������º���
void L3DArtist::UpdateNodeChange()
{
	//> �����б��������
	emit SglUpdateSceneList();
	// ��� �ڵ��ʱ��Ӧ�ý� �ڵ�����Ҳ���
	SlotSceneTreeItemCk();
}

//> �����ͱ��,�رյ�ǰ�����Ľ�����º���
void L3DArtist::UpdateCurSene(HScene pScene)
{
	CurOpenScene_ = pScene;
	secAniSeq_ = 1;
	//���³����ڵ���
	emit SglRepaintSceneTree();
	// ���¶�����,�ڸ��½ڵ���֮������ʱ����
	//emit SglUpdateAni();

	emit SglUpdateCtrlPoint();
	// ˢ�³����ļ��б� ��for ��ʱ��ʾ����������ͼ��modified by wangww 2014.3.17
	emit SglUpdateSceneList();
	// ��� ������ʱ��Ӧ�ý� �ڵ�����Ҳ���
	SlotSceneTreeItemCk();
	//�����䶯 ���� ��Ԥ������

	// ���������ͱ��֮����Ҫ���л��ļ����ڻص�֮�󱣴��ļ�
	if (CurOpenScene_.isValid() && !CheckOpenFile_) {
		CheckOpenFile_ = false;
		CurOpenScene_.serializeXML();
	}
	else if (!CheckOpenFile_) //����½�����
	{

	}
}

//>  �������л�������泡���ļ�
void L3DArtist::SaveHScene(const QString &SceneName, const QString &xml)
{
	emit SglSceneFileSave(SceneName, xml);
}

//> ����ͣ������
void L3DArtist::CreateDockWidgets(bool bEditMode)
{
	setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);
	//> ������Ա༭��Dock
	m_pDockPropertyEdit = new l3dDockWidget(this);
	DockPropertyEditor *pDockPropertyEdit = new DockPropertyEditor(m_pDockPropertyEdit);
	DockScaleIndex = pDockPropertyEdit;
	pDockPropertyEdit->setProperty("background", true);
	m_pDockPropertyEdit->setWidget(pDockPropertyEdit);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockPropertyEdit->toggleViewAction());
	m_pDockPropertyEdit->setObjectName("m_pDockPropertyEdit");
	m_pDockPropertyEdit->setContentsMargins(0, 0, 0, 0);
	m_pDockPropertyEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    if(!bEditMode)
	   addDockWidget(Qt::RightDockWidgetArea, m_pDockPropertyEdit);
	//> ��ӳ����б�Dock
	m_pDockScene = new l3dDockWidget(this);
	pDockScene = new DockScene(m_pDockScene);
	CreateConnectSceneList(pDockScene);
	pDockScene->setProperty("background", true);
	m_pDockScene->setWidget(pDockScene);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockScene->toggleViewAction());
	m_pDockScene->setObjectName("m_pDockScene");
	m_pDockScene->setContentsMargins(0, 0, 0, 0);
	m_pDockScene->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	//> ��Ӷ�������Dock
	m_pDockAnimation = new l3dDockWidget(this);
	DockAnimation *pDockAnimation = new DockAnimation(m_pDockAnimation);
	pDockAnimation->setProperty("background", true);
	m_pDockAnimation->setWidget(pDockAnimation);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockAnimation->toggleViewAction());
	m_pDockAnimation->setObjectName("m_pDockAnimation");
	m_pDockAnimation->setContentsMargins(0, 0, 0, 0);
	m_pDockAnimation->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	addDockWidget(Qt::BottomDockWidgetArea, m_pDockAnimation);
    pDockAnimation->setMinimumHeight(280);
	//> ��ӽڵ���Dock
	m_pDockSceneTree = new l3dDockWidget(this);
	SceneTree *pDockSceneTree = new SceneTree(m_pDockSceneTree);
	pDockSceneTree->setProperty("background", true);
	m_pDockSceneTree->setWidget(pDockSceneTree);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockSceneTree->toggleViewAction());
	m_pDockSceneTree->setObjectName("m_pDockSceneTree");
	m_pDockSceneTree->setContentsMargins(0, 0, 0, 0);
	m_pDockSceneTree->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	addDockWidget(Qt::LeftDockWidgetArea, m_pDockSceneTree);

    if(bEditMode)
       addDockWidget(Qt::LeftDockWidgetArea, m_pDockPropertyEdit);

    if(!bEditMode)
	    addDockWidget(Qt::LeftDockWidgetArea, m_pDockScene);
	CreateConnectSceneTree(pDockSceneTree);
	CreateConnectDockProperty(pDockPropertyEdit, pDockSceneTree);
	CreateConnectTimeLine(pDockAnimation, pDockSceneTree);
	CreateConnectPreviewWindow(pDockSceneTree);

	m_pDockToolBoxObj = new l3dDockWidget(tr("Object Library"), this);
	DockToolBoxObject* pDockToolBoxObj = new DockToolBoxObject(m_pDockToolBoxObj);
	pDockToolBoxObj->setProperty("background", true);
	m_pDockToolBoxObj->setWidget(pDockToolBoxObj);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockToolBoxObj->toggleViewAction());
	m_pDockToolBoxObj->setObjectName("m_pDockToolBoxObj");
	m_pDockToolBoxObj->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	addDockWidget(Qt::BottomDockWidgetArea, m_pDockToolBoxObj);

	m_pDockToolBoxMaterial = new l3dDockWidget(tr("Media Library"), this);
	DockToolBoxMaterial* pDockToolBoxMaterial = new DockToolBoxMaterial(m_pDockToolBoxMaterial);
	pDockToolBoxMaterial->setProperty("background", true);
	m_pDockToolBoxMaterial->setWidget(pDockToolBoxMaterial);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockToolBoxMaterial->toggleViewAction());
	m_pDockToolBoxMaterial->setObjectName("m_pDockToolBoxMaterial");
	m_pDockToolBoxMaterial->setContentsMargins(0, 0, 0, 0);
	m_pDockToolBoxMaterial->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	addDockWidget(Qt::BottomDockWidgetArea, m_pDockToolBoxMaterial);

	m_pDockToolBoxProperty = new l3dDockWidget(tr("Effect Library"), this);
	DockToolBoxProperty* pDockToolBoxProperty = new DockToolBoxProperty(m_pDockToolBoxProperty);
	pDockToolBoxProperty->setProperty("background", true);
	m_pDockToolBoxProperty->setWidget(pDockToolBoxProperty);
	ui.m_pMenuView->insertAction(ui.m_pBackgroundColor, m_pDockToolBoxProperty->toggleViewAction());
	m_pDockToolBoxProperty->setObjectName("m_pDockToolBoxMaterial");
	m_pDockToolBoxProperty->setContentsMargins(0, 0, 0, 0);
	m_pDockToolBoxProperty->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	addDockWidget(Qt::BottomDockWidgetArea, m_pDockToolBoxProperty);
	ui.m_pMenuView->insertSeparator(ui.m_pBackgroundColor);

   
    tabifyDockWidget(m_pDockAnimation,m_pDockToolBoxObj  );
    tabifyDockWidget(m_pDockToolBoxObj, m_pDockToolBoxMaterial);
    tabifyDockWidget(m_pDockToolBoxMaterial,m_pDockToolBoxProperty);
    if(!bEditMode)
        tabifyDockWidget(m_pDockScene,m_pDockSceneTree);  
    else
        tabifyDockWidget(m_pDockSceneTree,m_pDockPropertyEdit);  
    

	m_lstDockWidget.push_back(m_pDockScene);
	m_lstDockWidget.push_back(m_pDockPropertyEdit);
	m_lstDockWidget.push_back(m_pDockAnimation);
	m_lstDockWidget.push_back(m_pDockSceneTree);
	m_lstDockWidget.push_back(m_pDockToolBoxObj);
	m_lstDockWidget.push_back(m_pDockToolBoxMaterial);
	m_lstDockWidget.push_back(m_pDockToolBoxProperty);
	//added by xyc ���Կ�ʹ������,���ڳ�ʼ����򿪳���ʱ
	connect(this, &L3DArtist::SglPropertyEnable, pDockPropertyEdit, &DockPropertyEditor::SlotPropertyEnable, Qt::QueuedConnection);
	// added by yejia ���Կ�༭��Ƶ���ſ�ʼ��ʱ�������ź�֪ͨʱ���������λ�øı�
	connect(pDockPropertyEdit, &DockPropertyEditor::SglSetVideoTex, DockAnimation::GetInstance(), &DockAnimation::SlotSetVideoTex);
	//> added by TaiYue 2019.6.5 for ��dock������巢��������״̬�µ�indexֵ
	connect(this, SIGNAL(SglSentScaleIndex(int)), DockScaleIndex, SLOT(SlotGetScaleIndex(int)));
    connect(pDockPropertyEdit, &DockPropertyEditor::SglSetTexEffTime, this, 
        &L3DArtist::SlotSetTexEffKeyFrameTime);
    connect(pDockPropertyEdit, &DockPropertyEditor::SglDeleteAnimator, this, &L3DArtist::SlotDeleteTexEffAnimator);
}

//> ���������б�����
void L3DArtist::CreateConnectSceneList(DockScene* pDockScene)
{
	connect(pDockScene, &DockScene::SglNewScene, this, &L3DArtist::SlotNewScene, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglImportScene, this, &L3DArtist::SlotImportScene, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglOpenFile, this, &L3DArtist::SlotOpenSceneFile, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglExportScene, this, &L3DArtist::SlotExportScene, Qt::QueuedConnection);
	connect(ui.m_pDeleteScene, &QAction::triggered, pDockScene, &DockScene::SlotRemoveScene, Qt::QueuedConnection);
	connect(ui.m_pRenameScene, &QAction::triggered, pDockScene, &DockScene::SlotRenameScene, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglCurOpenFile, this, &L3DArtist::SlotUpdateCurOpenFile/*, Qt::QueuedConnection*/);
	connect(this, &L3DArtist::SglSceneFileSave, pDockScene, &DockScene::SlotSaveSceneFile, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglChangeIsNewScene, [&](bool bIsNewScene) { m_bIsNewScene = bIsNewScene; });
	connect(this, &L3DArtist::SglUpdateSceneList, pDockScene, &DockScene::SlotUpdateFilInfo, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglRemoveScene, this, &L3DArtist::SlotRemoveScene, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglCurSceneDirUpdate, this, &L3DArtist::SlotUpdateCurSceneDir, Qt::QueuedConnection);
	connect(this, &L3DArtist::SglUpdateProjectPath, pDockScene, &DockScene::SlotUpdateProjectPath, Qt::QueuedConnection);
	connect(pDockScene, &DockScene::SglOpenFileName, [&](const QString &strFileName) { m_strCurSceneItemName = strFileName; });
	connect(pDockScene, &DockScene::SglToolBarStatusChange, this, &L3DArtist::SlotSetToolBarStatusPic);
}

//> ��������������
void L3DArtist::CreateConnectSceneTree(SceneTree* pDockSceneTree)
{
	/*************************������SceneTree���ź�***********************************************************************/
	connect(ui.m_pCopy, &QAction::triggered, pDockSceneTree, &SceneTree::SglCopy);
	connect(ui.m_pPaste, &QAction::triggered, pDockSceneTree, &SceneTree::SglPaste);
	connect(ui.m_pCut, &QAction::triggered, pDockSceneTree, &SceneTree::SglCut);
	connect(ui.m_pDelete, &QAction::triggered, pDockSceneTree, &SceneTree::SglRemove);
	connect(ui.m_pSelectAll, &QAction::triggered, pDockSceneTree, &SceneTree::SglAllSelect);
	connect(ui.m_pDeselect, &QAction::triggered, pDockSceneTree, &SceneTree::SglClearSelect);
	connect(this, &L3DArtist::SglClearSceneTreeSel, pDockSceneTree, &SceneTree::SglClearSelect);
	connect(this, &L3DArtist::SglNodeClick, pDockSceneTree, &SceneTree::SglNodeClick);
	connect(this, &L3DArtist::SglNodeLose, pDockSceneTree, &SceneTree::SglNodeLose);
	connect(this, &L3DArtist::SglInsertNode, pDockSceneTree, &SceneTree::SglInsertNode);
	connect(this, &L3DArtist::SglTrNodeName, pDockSceneTree, &SceneTree::SglTrNodeName);
	connect(this, &L3DArtist::SglRemoveNode, pDockSceneTree, &SceneTree::SglRemoveNode);
	connect(this, &L3DArtist::SglRepaintSceneTree, pDockSceneTree, &SceneTree::SglRepaintTree);
	//connect(m_pPreviewWnd, &PreviewWindow::SglRemove, pDockSceneTree, &SceneTree::SglRemove);

	connect(this, &L3DArtist::SglUpdateSceneTree, pDockSceneTree, &SceneTree::SlotUpdate);
	/*************************SceneTree�������ź�***********************************************************************/
	connect(pDockSceneTree, &SceneTree::SglAllSelected, this, &L3DArtist::SlotAllSelected);
	connect(pDockSceneTree, &SceneTree::SglClearSelected, this, &L3DArtist::SlotClearSelected);
	connect(pDockSceneTree, &SceneTree::SglNodeClicked, this, &L3DArtist::SlotNodeClicked);
	connect(pDockSceneTree, &SceneTree::SglNodeFocused, this, &L3DArtist::SlotNodeFocused);
	connect(pDockSceneTree, &SceneTree::SglNodeLosed, this, &L3DArtist::SlotNodeLosed);
	connect(pDockSceneTree, &SceneTree::SglInsertNoded, this, &L3DArtist::SlotInsertNoded);
	connect(pDockSceneTree, &SceneTree::SglRemoveNoded, this, &L3DArtist::SlotRemoveNoded);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeNamed, this, &L3DArtist::SlotUpdateNodeNamed);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeEdabled, this, &L3DArtist::SlotUpdateNodeEdabled);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeVisibled, this, &L3DArtist::SlotUpdateNodeVisibled);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeLocked, this, &L3DArtist::SlotUpdateNodeLocked);
	connect(pDockSceneTree, &SceneTree::SglMoveGrouped, this, &L3DArtist::SlotMoveGrouped);
	connect(pDockSceneTree, &SceneTree::SglReleaseGrouped, this, &L3DArtist::SlotReleaseGrouped);
	connect(pDockSceneTree, &SceneTree::SglCuted, this, &L3DArtist::SlotCuted);
	connect(pDockSceneTree, &SceneTree::SglCopied, this, &L3DArtist::SlotCopied);
	connect(pDockSceneTree, &SceneTree::SglPasted, this, &L3DArtist::SlotPasted);
	connect(pDockSceneTree, &SceneTree::SglRemoved, this, &L3DArtist::SlotRemoved);
	connect(pDockSceneTree, &SceneTree::SglRenamed, this, &L3DArtist::SlotRenamed);
	connect(pDockSceneTree, &SceneTree::SglAdded, this, &L3DArtist::SlotAdded, Qt::QueuedConnection);
	connect(pDockSceneTree, &SceneTree::SglMoveUped, this, &L3DArtist::SlotMoveUped);
	connect(pDockSceneTree, &SceneTree::SglMoveDowned, this, &L3DArtist::SlotMoveDowned);
    connect(pDockSceneTree, &SceneTree::SglSetMulSel, this, &L3DArtist::SlotSetAlignBtnEnable);
}

//> ����ʱ��������
void L3DArtist::CreateConnectTimeLine(DockAnimation* pDockAnimation, SceneTree* pDockSceneTree)
{
	//> ���ѡ��
	connect(this, &L3DArtist::SglClearTimeLine, pDockAnimation, &DockAnimation::SlotClear);
	//> ����ѡ����״̬
	connect(this, &L3DArtist::SglTimeLineSelectNode, pDockAnimation, &DockAnimation::SlotSetSelectedNode);
	connect(this, &L3DArtist::SglUpdateAni, pDockAnimation, &DockAnimation::SlotUpdateAni);
	connect(this, &L3DArtist::SglUpdataNodeAni, pDockAnimation, &DockAnimation::SlotUpdateNodeAni, Qt::QueuedConnection);
    connect(this, &L3DArtist::SglCreateTexEffAnimator, pDockAnimation, 
        &DockAnimation::SlotCreateTexEffAnimator);
	//> ˢ��
	connect(pDockSceneTree, &SceneTree::SglRepaintedTree, this, &L3DArtist::SlotUpdateAni);

	//> ɾ���ڵ�
	connect(pDockSceneTree, &SceneTree::SglDeleteItem, pDockAnimation, &DockAnimation::SlotDeleteItem);
	//> �����ڵ�
	connect(pDockSceneTree, &SceneTree::SglAddItem, pDockAnimation, &DockAnimation::SlotAddItem);
	//> ���ýڵ�״̬
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeEdabled, pDockAnimation, &DockAnimation::SlotNodeEditabled);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeVisibled, pDockAnimation, &DockAnimation::SlotNodeVisibled);
	connect(pDockSceneTree, &SceneTree::SglUpdateNodeLocked, pDockAnimation, &DockAnimation::SlotNodeLocked);
	connect(pDockAnimation, &DockAnimation::SglItemEditable, pDockSceneTree, &SceneTree::SglUpdateNodeEdable);
	connect(pDockAnimation, &DockAnimation::SglUpdateEnableAudio, this, &L3DArtist::SlotUpdateEnableAudio);   // added by wuheng 2016.05.20
																											  //> �������ڵ�
	connect(pDockSceneTree, &SceneTree::SglRenamed, pDockAnimation, &DockAnimation::SlotRenamed);
	//> �ڵ�ѡ��
	connect(pDockAnimation, &DockAnimation::SglSingleSelected, pDockSceneTree, &SceneTree::SglSingleClick);

	connect(m_pPreviewWnd, &PreviewWindow::SglSetKeyFrameValue, pDockAnimation, &DockAnimation::SlotSetKeyFrameValue);
	connect(m_pDSpinBoxX, &SpinBox_ControlBox::sglSetKeyFrameValue, pDockAnimation, &DockAnimation::SlotSetKeyFrameValue);
	connect(m_pDSpinBoxY, &SpinBox_ControlBox::sglSetKeyFrameValue, pDockAnimation, &DockAnimation::SlotSetKeyFrameValue);
	connect(m_pDSpinBoxZ, &SpinBox_ControlBox::sglSetKeyFrameValue, pDockAnimation, &DockAnimation::SlotSetKeyFrameValue);

    connect(this, &L3DArtist::SglSetTexEffKeyFrameTime, pDockAnimation, &DockAnimation::SlotSetTexEffTime);
    connect(this, &L3DArtist::SglDeleteTexEffAnimator, pDockAnimation, &DockAnimation::SlotDeleteTexEffAnimator);
}

//> �������Կ�����
void L3DArtist::CreateConnectDockProperty(DockPropertyEditor* pDockPropertyEdit, SceneTree* pDockSceneTree)
{
	//> ���Կ����
	connect(pDockPropertyEdit, &DockPropertyEditor::SglCut, pDockSceneTree, &SceneTree::SglCut);
	connect(pDockPropertyEdit, &DockPropertyEditor::SglCopy, pDockSceneTree, &SceneTree::SglCopy);
	connect(pDockPropertyEdit, &DockPropertyEditor::SglPaste, pDockSceneTree, &SceneTree::SglPaste);
	connect(pDockPropertyEdit, &DockPropertyEditor::SglRemove, pDockSceneTree, &SceneTree::SglRemove);

	//> ���µ�ǰ��ʾҳ������
	connect(this, &L3DArtist::SglUpdateProperty, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateProperty);// ����Ϊ�Ŷӷ�ʽ,����᲻��Ӧ

																											 //> �����»������Լ���չ����
	connect(this, &L3DArtist::SglUpdateBaseProperty, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateBaseProperty, Qt::QueuedConnection);

	//> �����¶�������
	connect(this, &L3DArtist::SglUpdateExtProperty, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateExtProperty, Qt::QueuedConnection);

	//> ������Կ���������,ÿ����¾��Դ����,һ�㲻����
	connect(this, &L3DArtist::SglClearProperty, pDockPropertyEdit, &DockPropertyEditor::SlotClearProperty);

	//> ��������ʹ��,�ݽ���
	connect(this, &L3DArtist::SglUpdateExtProp, pDockPropertyEdit, &DockPropertyEditor::SlotExtPropertyUpdate, Qt::QueuedConnection);

	//> ��Ƶ����ı�,��ʱ��������϶�ʱ����
	connect(this, &L3DArtist::SglVideoTextureChanged, pDockPropertyEdit, &DockPropertyEditor::SlotVideoTexChanged);

	//> ����Nodeλ������
	connect(this, &L3DArtist::SglNodeMove, pDockPropertyEdit, &DockPropertyEditor::SlotNodeMove);

	//> �����������ֵ����,�����Կⷢ���޸��ź�
	connect(pDockPropertyEdit, &DockPropertyEditor::SglSetSpinBoxValue, this, &L3DArtist::SlotUpdateToolBarXYZ);


	//> ����Node��ת����
	connect(this, &L3DArtist::SglNodeRotation, pDockPropertyEdit, &DockPropertyEditor::SlotNodeRotation);

	//> ����Node��������
	connect(this, &L3DArtist::SglNodeScale, pDockPropertyEdit, &DockPropertyEditor::SlotNodeScale);

	//> ��Ⱦ�������ӵĴ�����ɾ��
	connect(m_pPreviewWnd, &PreviewWindow::SglCreateConnect, pDockPropertyEdit, &DockPropertyEditor::SlotCreateConnect, Qt::QueuedConnection);
	connect(m_pPreviewWnd, &PreviewWindow::SglRemoveConnect, pDockPropertyEdit, &DockPropertyEditor::SlotRemoveConnect, Qt::QueuedConnection);

	//> ���¹���������
	connect(this, &L3DArtist::SglUpdateLightPass, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateLightPass, Qt::QueuedConnection);

	//> ���²���������
	connect(this, &L3DArtist::SglUpdateMaterialGroup, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateMaterialGroup, Qt::QueuedConnection);

	//> ����ʱ�Ե�ǰ��ѡ��,�ⲿ�������,intΪ��ǰ����������idֵ
	connect(this, &L3DArtist::SglSelectTexurePartToolBar, this, &L3DArtist::SlotSelectTexurePartToolBar);

	//> �������а�ť������,���ⲿ���и���,intΪ��ǰ�ڵ㵱ǰ������indexֵ
	connect(this, &L3DArtist::SglTextureToolBtnGroupPressed, pDockPropertyEdit, &DockPropertyEditor::SlotUpdateTextureSelected);

	//> ����������Ч�ģ���Ӧ֪ͨ�����Բ�������
	connect(this, &L3DArtist::SglCreateTextureEffect, pDockPropertyEdit,
		&DockPropertyEditor::SlotCreateTextureEffect);

	// ˫�����ı��༭�� added by yejia 2014-10-31
	connect(m_pPreviewWnd, &PreviewWindow::SglOpenTextEdit, pDockPropertyEdit, &DockPropertyEditor::SlotOpenTextEdit, Qt::QueuedConnection);

	//> ���µ�ǰ�ڵ����������
	connect(pDockPropertyEdit, &DockPropertyEditor::SglUpdateCurTexIndex, [&](int nIndex) {	m_nCurTexture = nIndex;	});

	//> ���µ�ǰ�ڵ�Ĳ�������
	connect(pDockPropertyEdit, &DockPropertyEditor::SglUpdateCurMatIndex, [&](int nIndex) {	m_nCurMaterial = nIndex; });

	//> ��Ƶ���Ա��   added by wuheng 2016.05.20
	connect(pDockPropertyEdit, &DockPropertyEditor::SglAudioPropertyChanged, this, &L3DArtist::SlotUpdateAudioToNodeItem);
	connect(pDockPropertyEdit, &DockPropertyEditor::SglInsertAusioFile, this, &L3DArtist::SglInsertNode);
	connect(pDockPropertyEdit, &DockPropertyEditor::SglUpdateTexToolBar, this, &L3DArtist::SglUpdateTexurePartToolBar);

	// ��ɫ��ɫ�༭�Ի���
	connect(m_pPreviewWnd, &PreviewWindow::SglOpenGradientOrSolid, this, &L3DArtist::SlotOpenGradientOrSolid);

	// �������ƶԻ���
	connect(m_pPreviewWnd, &PreviewWindow::SglOpenPolygon, this, &L3DArtist::CreatePolygon);
	//     //> ODBC���Ա��
	//     connect(pDockPropertyEdit, &DockPropertyEditor::SglODBCPropertyChanged, this,&L3DArtist::SlotUpdateODBCToNodeItem);

	//> �ߴ繤�����޸�ֵ
	//     connect(pDockPropertyEdit, &DockPropertyEditor::SglUpdateSize, m_pSizeBar, &CustomSizeBar::slotUpdateItem);
	//     connect(m_pSizeBar, &CustomSizeBar::sglchangeCustom, pDockPropertyEdit, &DockPropertyEditor::slotchangeCustom);

	/***********************************DVE����ʱȡ��ע��***********************************************************
	//DVE���������Ӧ����
	connect(this, &L3DArtist::SglUpdataDVE, pDockPropertyEdit, &DockPropertyEditor::SlotUpdataDVE, Qt::QueuedConnection);
	/**************************************************************************************************************/

	//> added by TaiYue for ���ò�����������λ�ò���ֵ
	connect(this, SIGNAL(SglMgrSingleStep(double dStep , NodeCtrl nodectrl)), pDockPropertyEdit, SLOT(SlotSetMgrSingleStep(double dStep,NodeCtrl nodectrl)));
	
    connect(this, &L3DArtist::SglTexEffTimeChange, pDockPropertyEdit, &DockPropertyEditor::SlotTexEffTimeChange);

    connect(this, &L3DArtist::SglUpdateTexEffTime, pDockPropertyEdit,
        &DockPropertyEditor::SlotUpdteTexEffTime);
    connect(this, &L3DArtist::SglDeleteTexEffect, pDockPropertyEdit, &DockPropertyEditor::SlotDeleteTexEffect);
  
}

//> ������Ⱦ��������
void L3DArtist::CreateConnectPreviewWindow(SceneTree* pDockSceneTree)
{
	connect(m_pPreviewWnd, &PreviewWindow::SglAllSelected, pDockSceneTree, &SceneTree::SglAllSelect);
	connect(m_pPreviewWnd, &PreviewWindow::SglEscSelected, pDockSceneTree, &SceneTree::SglClearSelect);
	connect(m_pPreviewWnd, &PreviewWindow::SglCut, pDockSceneTree, &SceneTree::SglCut);
	connect(m_pPreviewWnd, &PreviewWindow::SglMoveGroup, pDockSceneTree, &SceneTree::SglMoveGroup);
	connect(m_pPreviewWnd, &PreviewWindow::SglReleaseGroup, pDockSceneTree, &SceneTree::SlotReleaseGroup);
	connect(m_pPreviewWnd, &PreviewWindow::SglCopy, pDockSceneTree, &SceneTree::SglCopy);
	connect(m_pPreviewWnd, &PreviewWindow::SglPaste, pDockSceneTree, &SceneTree::SglPaste);
	connect(m_pPreviewWnd, &PreviewWindow::SglRemove, pDockSceneTree, &SceneTree::SglRemove);
	connect(m_pPreviewWnd, &PreviewWindow::SglRename, pDockSceneTree, &SceneTree::SglRename);
	connect(pDockSceneTree, &SceneTree::SglSetMulSel, m_pPreviewWnd, &PreviewWindow::SlotSetMulSel);

	connect(m_pPreviewWnd, &PreviewWindow::SglVerAlign, pDockSceneTree, &SceneTree::SglVerAlign);    //added by wuheng 2016.05.18
	connect(m_pPreviewWnd, &PreviewWindow::SglHorAlign, pDockSceneTree, &SceneTree::SglHorAlign);

	connect(m_pPreviewWnd, &PreviewWindow::SglExtractScene, this, &L3DArtist::SlotExtractScene);
	connect(m_pPreviewWnd, &PreviewWindow::SglCopyMaterial, this, &L3DArtist::SlotCopyMaterial);
	connect(m_pPreviewWnd, &PreviewWindow::SglPasteMaterial, this, &L3DArtist::SlotPasteMaterial);
	connect(m_pPreviewWnd, &PreviewWindow::SglRemoveMaterial, this, &L3DArtist::SlotRemoveMaterial);
	connect(m_pPreviewWnd, &PreviewWindow::SglExtractMaterial, this, &L3DArtist::SlotExtractMaterial);
	connect(m_pPreviewWnd, &PreviewWindow::SglCopyTex, this, &L3DArtist::SlotCopyTex);
	connect(m_pPreviewWnd, &PreviewWindow::SglPasteTex, this, &L3DArtist::SlotPasteTex);
	connect(m_pPreviewWnd, &PreviewWindow::SglRemoveTex, this, &L3DArtist::SlotRemoveTex);
	connect(m_pPreviewWnd, &PreviewWindow::SglExtractTexture, this, &L3DArtist::SlotExtractTexture);
	connect(m_pPreviewWnd, &PreviewWindow::SglArrayCopy, this, &L3DArtist::SlotArrayCopy);
	connect(m_pPreviewWnd, &PreviewWindow::SglConvert3DText, this, &L3DArtist::SlotConvert3DText);
	connect(m_pPreviewWnd, &PreviewWindow::SglConvert2DText, this, &L3DArtist::SlotConvert2DText);
	connect(m_pPreviewWnd, &PreviewWindow::SglExtractAni, this, &L3DArtist::SlotExtractAni);
	connect(m_pPreviewWnd, &PreviewWindow::SglExtractNode, this, &L3DArtist::SlotExtractNode);
	connect(m_pPreviewWnd, &PreviewWindow::SglResizePlayWnd, this, &L3DArtist::SlotResizePlayWnd);
	connect(m_pPreviewWnd, &PreviewWindow::sglSetDrawRulers, this, &L3DArtist::slotSetRulers);
	connect(m_pPreviewWnd, &PreviewWindow::SglAside, this, &L3DArtist::SlotAside);
}

//> д��ini�����ļ�
void L3DArtist::WriteSettings(bool bIsMax)
{
	ArtistParams::instance()->Translated(m_bTranslate);
	ArtistParams::instance()->MainWinState(saveState());
	if (!bIsMax)
	{
		ArtistParams::instance()->MainWinPos2(pos());
		ArtistParams::instance()->MainWinSize2(size());
	}
}

//> ��ȡini�����ļ�
void L3DArtist::ReadSettings()
{
	if (ArtistParams::instance()->Translated())
	{
		m_pOwner->installTranslator(translator_cn);
		m_pOwner->installTranslator(translator_default);

		m_bTranslate = true;
	}
	move(ArtistParams::instance()->MainWinPos2());
	resize(ArtistParams::instance()->MainWinSize2());
	QByteArray array = ArtistParams::instance()->MainWinState();
	bool bTrue = restoreState(array);
	m_pDockScene->raise();
}

//> �ر��¼�
void L3DArtist::closeEvent(QCloseEvent *pEvent)
{
	if (!isMaximized())
		WriteSettings(false);
	else
		WriteSettings(true);

	parentWidget()->close();
	pEvent->accept();
}

//> �������¼�
void L3DArtist::enterEvent(QEvent *pEvent)
{
	setCursor(Qt::ArrowCursor);
	QMainWindow::enterEvent(pEvent);
}

//> �����źŲ�����
void L3DArtist::CreateConntent()
{
#ifdef FOR_XOR_CO_VER
	ui.m_pModifyPwd->setVisible(false);
#else
	ui.m_pModifyPwd->setVisible(L3DSystemConfig::instance()->getAppMode() != APP_MODE_STUDIO);
	connect(ui.m_pModifyPwd, &QAction::triggered, this, &L3DArtist::SlotModifyPwd, Qt::QueuedConnection);
#endif
	connect(ui.m_pNewScene, &QAction::triggered, this, &L3DArtist::SlotNewScene, Qt::QueuedConnection);
	connect(ui.m_pImportNode, &QAction::triggered, this, &L3DArtist::SlotImportNode, Qt::QueuedConnection);
	connect(ui.m_pExportNode, &QAction::triggered, this, &L3DArtist::SlotExtractNode, Qt::QueuedConnection);
	connect(ui.m_pImportScene, &QAction::triggered, this, &L3DArtist::SlotImportScene, Qt::QueuedConnection);
	connect(ui.m_pExportScene, &QAction::triggered, this, &L3DArtist::SlotExportScene, Qt::QueuedConnection);
	connect(ui.m_pCloseScene, &QAction::triggered, this, &L3DArtist::SlotCloseScene, Qt::QueuedConnection);
	connect(ui.m_pSaveScene, &QAction::triggered, this, &L3DArtist::SlotSaveScene, Qt::QueuedConnection);
	connect(ui.m_pSaveAsScene, &QAction::triggered, this, &L3DArtist::SlotSaveAsScene, Qt::QueuedConnection);
	connect(ui.m_pExtractNode, &QAction::triggered, this, &L3DArtist::SlotExtractNode, Qt::QueuedConnection);
	connect(ui.m_pExtractScene, &QAction::triggered, this, &L3DArtist::SlotExportScene, Qt::QueuedConnection);
	connect(ui.m_pExit, &QAction::triggered, this, &L3DArtist::SlotExit, Qt::QueuedConnection);
	connect(ui.m_pAbout, &QAction::triggered, this, &L3DArtist::SlotAbout, Qt::QueuedConnection);
	connect(ui.m_pShortCut, &QAction::triggered, this, &L3DArtist::SlotShortCut);
	connect(ui.m_pSceneLayer, &QAction::triggered, this, &L3DArtist::SlotSceneLayer, Qt::QueuedConnection);
	connect(ui.m_pOutputVideo, &QAction::triggered, this, &L3DArtist::SlotOutputVideo, Qt::QueuedConnection);
	connect(ui.actionProjectSet, &QAction::triggered, this, &L3DArtist::SlotProjectSet, Qt::QueuedConnection);
	connect(ui.actionFtpSetting, &QAction::triggered, this, &L3DArtist::SlotFtpSetting, Qt::QueuedConnection);
	connect(ui.m_pBackgroundColor, &QAction::triggered, this, &L3DArtist::SlotBackgroundColor, Qt::QueuedConnection);
	connect(ui.m_pSafe, &QAction::triggered, this, &L3DArtist::SlotShowSafe, Qt::QueuedConnection);
	connect(ui.m_pTitleSafe, &QAction::triggered, this, &L3DArtist::SlotShowTitleSafe, Qt::QueuedConnection);
	connect(ui.m_pTga_To_Flg, &QAction::triggered, this, &L3DArtist::SlotTga, Qt::QueuedConnection);
	connect(ui.m_pShowRuler, &QAction::triggered, this, &L3DArtist::slotDrawRulers, Qt::QueuedConnection);
	connect(this, &L3DArtist::sglClearAllRulerItem, m_pPreviewWnd, &PreviewWindow::slotClearAllRulerItem);
	connect(this, &L3DArtist::sglShowAllRulerItem, m_pPreviewWnd, &PreviewWindow::slotShowAllRulerItem);
	connect(ui.m_pSetCard, &QAction::triggered, this, &L3DArtist::SlotSetCard, Qt::QueuedConnection);  // added by wuheng 2016.12.12

	connect(ui.m_pUndo, &QAction::triggered, this, &L3DArtist::SlotUndo, Qt::QueuedConnection);
	connect(ui.m_pRedo, &QAction::triggered, this, &L3DArtist::SlotRedo, Qt::QueuedConnection);
    connect(m_pTBtnVAlign, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SglVerAlign, Qt::QueuedConnection); // added by wangjieke 2019.07.05
    connect(m_pTBtnHAlign, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SglHorAlign, Qt::QueuedConnection); // added by wangjieke 2019.07.05

	//���ڵ�����ź����ӵ� ��Ԥ����ʾ������ȥ
	connect(ui.m_pMove, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SlotMoveNode, Qt::QueuedConnection);
	connect(ui.m_pMove, &QAction::triggered, [&]() { SlotSetNodeCtrl(NODECTRL_MOVE); });
	connect(ui.m_pRotate, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SlotRotateNode, Qt::QueuedConnection);
	connect(ui.m_pRotate, &QAction::triggered, [&]() { SlotSetNodeCtrl(NODECTRL_ROTATE); });
	connect(ui.m_pZoom, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SlotZoom, Qt::QueuedConnection);
	connect(ui.m_pZoom, &QAction::triggered, [&]() { SlotSetNodeCtrl(NODECTRL_SCALE); });
	connect(ui.m_pSize, &QAction::triggered, m_pPreviewWnd, &PreviewWindow::SlotSize, Qt::QueuedConnection);
	connect(ui.m_pSize, &QAction::triggered, [&]() { SlotSetNodeCtrl(NODECTRL_SIZE); });
	connect(ui.actionChangeLanguage, &QAction::triggered, this, &L3DArtist::SlotChangeLanguage, Qt::QueuedConnection);//���Է��벻���л� deleted by yejia 2015-05-05

	connect(this, &L3DArtist::sglsetProject, this, &L3DArtist::setProject, Qt::QueuedConnection /*[&](const QString& prjname){ setProject(prjname); }*/);

	/******************************************��Ⱦ���ڶ��빦������*********************************
	//��Ⱦ���ڶ��빦�ܰ�ť��Ӧ
	connect(pTBtnAlignX, &ToolButton::clicked, this, &L3DArtist::SlotAlignX, Qt::QueuedConnection);
	connect(pTBtnAlignY, &ToolButton::clicked, this, &L3DArtist::SlotAlignY, Qt::QueuedConnection);
	connect(pTBtnAlignZ, &ToolButton::clicked, this, &L3DArtist::SlotAlignZ, Qt::QueuedConnection);
	/******************************************��Ⱦ���ڶ��빦������*********************************/

	// �ڵ�ѡ��
	//connect(m_pTBtnTex, &QToolButton::triggered, this, &L3DArtist::SlotTexBtnSetIcon);	

	//**********************************************************************************************/
	//> Notifier�źŲ�����
	//**********************************************************************************************/
	//> ����ʵʱ��Ⱦ���ƻص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnRenderSceneOperate,
		this, &L3DArtist::SlotOnRenderSceneOperate, Qt::QueuedConnection);
	//> ¼�Ƴ�����Ⱦ��Ƶ�����ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnEndSaveVOSFile,
		this, &L3DArtist::SlotOnEndSaveVOSFile, Qt::QueuedConnection);
	//> ¼�Ƴ�����Ⱦ��Ƶ���Ȼص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSaveVOSProcess,
		this, &L3DArtist::SlotOnSaveVOSProcess, Qt::QueuedConnection);
	//> ��Ļ����ʼ������ʼ�������ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnCGCInitOperate,
		this, &L3DArtist::SlotOnCGCInitOperate, Qt::QueuedConnection);
	//> ��Ļ�������رս����ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnCGCChannelOperate,
		this, &L3DArtist::SlotOnCGCChannelOperate, Qt::QueuedConnection);
	//> ��Ļ�����ú�ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnCGCardSetuped,
		this, &L3DArtist::SlotOnCGCardSetuped, Qt::QueuedConnection);
	//> �������󴴽������ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSceneCreated,
		this, &L3DArtist::SlotOnSceneCreated, Qt::QueuedConnection);
	//> �������󱣴涯���ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSceneSaveFile,
		this, &L3DArtist::SlotOnSceneSaveFile, Qt::QueuedConnection);
	//> ������������ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSceneOperate,
		this, &L3DArtist::SlotOnSceneOperate, Qt::QueuedConnection);
	//> �������������󷴳�ʼ���ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSMUnInitialized,
		this, &L3DArtist::SlotOnSMUnInitialized, Qt::QueuedConnection);
	//> ����ģ��
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnSceneTempleteOpened,
		this, &L3DArtist::SlotOnSceneTempleteOpened, Qt::QueuedConnection);
	//> Undo/Redo�ص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnObjectUpdate,
		this, &L3DArtist::SlotOnObjectUpdate, Qt::QueuedConnection);
	//> ��Ⱦ��������Ӧ�ڲ��������ٻص��źŲ�����
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnDestoryMgr,
		this, &L3DArtist::SlotOnDestoryMgr, Qt::QueuedConnection);
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnRMgrInitialized,
		this, &L3DArtist::SlotOnOnRMgrInitialized, Qt::QueuedConnection);
	connect(static_cast<Notifier *>(INotifierPtr_.get()), &Notifier::SglOnRMgrUnInitialized,
		this, &L3DArtist::SlotOnRMgrUnInitialized, Qt::QueuedConnection);
	//**********************************************************************************************/
	//> NodeOperateCB�źŲ�����
	//**********************************************************************************************/
	//> �ڵ����ĸ��ڵ����Ըı��ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnParentUpdate,
		this, &L3DArtist::SlotOnParentUpdate, Qt::QueuedConnection);
	//> �ڵ�����ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeOperate,
		this, &L3DArtist::SlotOnNodeOperate, Qt::QueuedConnection);
	//> �ڵ�������л�������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeSerialized,
		this, &L3DArtist::SlotOnNodeSerialized, Qt::QueuedConnection);
	//> �ڵ�ɼ������Ե��ûص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeVisible,
		this, &L3DArtist::SlotOnNodeVisible, Qt::QueuedConnection);
	//> �ڵ�任�ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeTransform,
		this, &L3DArtist::SlotOnNodeTransform, Qt::QueuedConnection);
	//> �ڵ㱬ը�����ı�ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeExplosion,
		this, &L3DArtist::SlotOnNodeExplosion, Qt::QueuedConnection);
	//> �ڵ�ģ�ͱ���������ݲ����仯�ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeMeshDeform,
		this, &L3DArtist::SlotOnNodeMeshDeform, Qt::QueuedConnection);
	//> �ڵ�ģ�ͱ��β������л�������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeMDSerizlized,
		this, &L3DArtist::SlotOnNodeMDSerizlized, Qt::QueuedConnection);
	//> �ڵ�ģ�ͱ��β��ַ����л�������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnNodeMDDeserialized,
		this, &L3DArtist::SlotOnNodeMDDeserialized, Qt::QueuedConnection);
	//> �ڵ������ز����ı��ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnMaterialUpdate,
		this, &L3DArtist::SlotOnMaterialUpdate, Qt::QueuedConnection);
	//> ���ʶ��󴴽���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnMaterialCreated,
		this, &L3DArtist::SlotOnMaterialCreated, Qt::QueuedConnection);
	//> �ڵ�������л���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglonMaterialSerialized,
		this, &L3DArtist::SlotOnMaterialSerialized, Qt::QueuedConnection);
	//> ����������󴴽���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTextureCreated,
		this, &L3DArtist::SlotOnTextureCreated, Qt::QueuedConnection);
	//> Ӧ��������󴴽���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnAppTextureCreated,
		this, &L3DArtist::SlotOnAppTextureCreated, Qt::QueuedConnection);
	//> ������Ч������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTexEffectCreated,
		this, &L3DArtist::SlotOnTexEffectCreated, Qt::QueuedConnection);
	//> Ӧ������������л���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnAppTextureSerialized,
		this, &L3DArtist::SlotOnAppTextureSerialized, Qt::QueuedConnection);
	//> Ӧ������������л���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnAppTextureDeserialized,
		this, &L3DArtist::SlotOnAppTextureDeserialized, Qt::QueuedConnection);
	//> ��������������л���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTextureSerialized,
		this, &L3DArtist::SlotOnTextureSerialized, Qt::QueuedConnection);
	//> ���ɻ�����������ͼ�ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnThumbnailOperate,
		this, &L3DArtist::SlotOnThumbnail, Qt::QueuedConnection);
	//> ��������������л���ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTextureDeserialized,
		this, &L3DArtist::SlotOnTextureDeserialized, Qt::QueuedConnection);
	//> �����������ûص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTextureSeted,
		this, &L3DArtist::SlotOnTextureSeted, Qt::QueuedConnection);
	//> ������������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnTextureOperate,
		this, &L3DArtist::SlotOnTextureOperate, Qt::QueuedConnection);
	//> �����������øı�ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnLightPassUpdate,
		this, &L3DArtist::SlotOnLightPassUpdate, Qt::QueuedConnection);
	//> ���������ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnAnimatorOperate,
		this, &L3DArtist::SlotOnAnimatorOperate, Qt::QueuedConnection);
	//> ��ȡ�����ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnAnimatorExtract,
		this, &L3DArtist::SlotOnAnimatorExtract, Qt::QueuedConnection);
	//> �ڵ㶯���������йؼ�֡�����ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnKeyFrameOperate,
		this, &L3DArtist::SlotOnKeyFrameOperate, Qt::QueuedConnection);
	//> ��չ���Ը��»ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnExtPropUpdate,
		this, &L3DArtist::SlotOnExtPropUpdate, Qt::QueuedConnection);
	//> ��չ���Ը��»ص��źŲ�����
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnKeyFrameTimeUpdate,
		this, &L3DArtist::SlotOnKeyFrameTimeUpdate, Qt::QueuedConnection);
	//> ���»�������
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnUpdateBasePara,
		this, &L3DArtist::SlotOnUpdateBasePara, Qt::QueuedConnection);
	//> Enginekernel���͹����ľ�����Ϣ
	connect(static_cast<NodeOperateCB *>(INodeOperateCBPtr_.get()), &NodeOperateCB::SglOnWarningInfo,
		this, &L3DArtist::SlotOnWarningInfo, Qt::QueuedConnection);
	//**********************************************************************************************/
	//> SceneOperateCB�źŲ�����
	//**********************************************************************************************/
	//> ���л�or�������л�����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSerializeTex,
		this, &L3DArtist::SlotOnSerializeTex, Qt::QueuedConnection);
	//> �����������л������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSceneDeserialize,
		this, &L3DArtist::SlotOnSceneDeserialize, Qt::QueuedConnection);
	//> �ڵ�����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSceneNodeOperate,
		this, &L3DArtist::SlotOnSceneNodeOperate, Qt::QueuedConnection);
	//> ����ڵ���»ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnFocusOn,
		this, &L3DArtist::SlotOnFocusOn, Qt::QueuedConnection);
	//> ѡ�нڵ������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSelected,
		this, &L3DArtist::SlotOnSelected, Qt::QueuedConnection);
	//> ��ǰ��������������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnCameraNodeSet,
		this, &L3DArtist::SlotOnCameraNodeSet, Qt::QueuedConnection);
	//> ���Ƶ�����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnCPointOperate,
		this, &L3DArtist::SlotOnCPointOperate, Qt::QueuedConnection);
	//> ���Ƶ���������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnCPointSetPos,
		this, &L3DArtist::SlotOnCPointSetPos, Qt::QueuedConnection);
	//> ���Ƶ����л���ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnAllCPSerialize,
		this, &L3DArtist::SlotOnAllCPSerialize, Qt::QueuedConnection);
	//> ���Ƶ㷴���л��ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnAllCPDeserialize,
		this, &L3DArtist::SlotOnAllCPDeserialize, Qt::QueuedConnection);
	//> ���Ƶ������������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnCPointTriggered,
		this, &L3DArtist::SlotOnCPointTriggered, Qt::QueuedConnection);
	//> ���Ƶ�������л������ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnCPointSerialize,
		this, &L3DArtist::SlotOnCPointSerialize, Qt::QueuedConnection);
	//> �������ſ��ƻص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnScenePlayOperate,
		this, &L3DArtist::SlotOnScenePlayOperate, Qt::QueuedConnection);
	//> ��������ʱ����ʱ������ʵʱλ��ͨ���ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSceneCurPosition,
		this, &L3DArtist::SlotOnSceneCurPosition, Qt::QueuedConnection);
	//> �����������ֲ����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSceneSoundOperate,
		this, &L3DArtist::SlotOnSceneSoundOperate, Qt::QueuedConnection);
	//> �����������ֲ��Żص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnSceneSoundPlay,
		this, &L3DArtist::SlotOnSceneSoundPlay, Qt::QueuedConnection);
	//> ����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnAddGroup,
		this, &L3DArtist::SlotOnAddGroup, Qt::QueuedConnection);
	//> ����ص��źŲ�����
	connect(static_cast<SceneOperateCB *>(ISceneOperateCBPtr_.get()), &SceneOperateCB::SglOnReleaseGroup,
		this, &L3DArtist::SlotOnReleaseGroup, Qt::QueuedConnection);
	//���ֹ���ʹ��
	connect(this, &L3DArtist::SglActionEnable, this, &L3DArtist::SlotActionEnable, Qt::QueuedConnection);
	//> �����ֹ������������
	connect(this, &L3DArtist::SglUpdateTexurePartToolBar, this, &L3DArtist::SlotUpdateTexturePartToolBar);
	connect(this, &L3DArtist::SglClearTexurePartToolBar, this, &L3DArtist::SlotClearTexurePartToolBar);
	//> �ߴ粿�ֹ������������
	//     connect(this, &L3DArtist::SglUpdateSizeToolBar, this, &L3DArtist::SlotUpdateSizeToolBar);
	//     connect(this, &L3DArtist::SglClearSizeToolBar, this, &L3DArtist::SlotClearSizeToolBar);
}

//> ��ȡ��ǰ������focus�ڵ�
HNode L3DArtist::GetCurFocusNode()
{
	return CurOpenScene_.getFocusedNode();
}

HNode L3DArtist::GetCurSelectedNode()
{
	std::vector<HNode> nodes;
	CurOpenScene_.getSelectedNodes(nodes);
	if (nodes.empty())
		return HNode();
	return nodes[0];
}

std::vector<HNode> L3DArtist::GetCurSelectedNodes()
{
	std::vector<HNode> nodes;
	CurOpenScene_.getSelectedNodes(nodes);
	return nodes;
}

//> ���õ�ǰ������focus�ڵ�
void L3DArtist::SetCurFocusNode(HNode node)
{
	if (!CurOpenScene_.isValid()) {
		return;
	}
	//CurOpenScene_.setFocusedNode(node); //����  ����ӿڻᵼ�� �ڵ� ��д�� ��ס�� 
	CurOpenScene_.setSelectedNode(node);
}

//> ״̬��ʼ��
void L3DArtist::ToolBarTextureBtnInit()
{
	pTBtnTextureAll->setVisible(false);
	pTBtnTextureFront->setVisible(false);
	pTBtnTextureBack->setVisible(false);
	pTBtnTextureSide->setVisible(false);
	pTBtnTextureLeft->setVisible(false);
	pTBtnTextureRight->setVisible(false);
	pTBtnTextureUp->setVisible(false);
	pTBtnTextureDown->setVisible(false);
	pTBtnTextureInside->setVisible(false);
	pTBtnTextureFace->setVisible(false);
	pTBtnTextureFrontchamfer->setVisible(false);
	pTBtnTextureBackchamfer->setVisible(false);
	pTBtnTextureShadow->setVisible(false);
	pTBtnTextureBaseBoard->setVisible(false);
	pTBtnTextureOutSide->setVisible(false);
}

//> �½�����
void L3DArtist::SlotNewScene()
{

	if (!m_bRMgrInit)
		return;

	pTBtnUploadScene->setEnabled(true);


BEGIN:
	{
		bool bIsTrue;
		QString strSceneName(QInputDialog::getText(this, tr("New Scene"), tr("Please enter the scene name:"), QLineEdit::Normal, QString(), &bIsTrue, Qt::WindowCloseButtonHint));
		if (!bIsTrue)
			return;
		if (!isLegal(strSceneName))
		{
			MSGBOX_WARNING_TIP(tr("File name is invalid."), this);
			return;
		}
		if (bIsTrue && !strSceneName.isEmpty()) {
			if (!SceneManager_.isValid()) {
				LOG(QERROR) << _T("Scene manager is invalid");
				MSGBOX_ERROR_TIP(tr("Scene manager is invalid."), this);
				return;
			}
			//QStringList strlistFiles = GetSameFiles(m_strProjectPath + '/' + m_strSceneListPath, strSceneName + ".l3d");
			QFileInfo filePath = checkFileName(m_strProjectPath + '/' + m_strSceneListPath, strSceneName + ".l3d");
			if (filePath.exists())
			{
				int nBtn(-1);
				MSGBOX_WARNING_OKANDCANCEL(tr("Scene already exists, Are you sure to replace?"), nBtn, this);
				if (nBtn != QMessageBox::Ok) {
					goto BEGIN;
				}
				else   //�滻���г����������������һ�����г����ǵ�ǰ�򿪳�����ɾ����ǰ�����ٴ����µĳ�����һ�����г���û�б�����������
				{
					//1.���г����ǵ�ǰ�򿪳���,�رյ�ǰ����
					if (m_strCurSceneItemName == strSceneName)
					{
						HScene pScene = SceneManager_.getSceneByName(tstring());
						SceneManager_.closeScene(pScene);
					}

					//2.ɾ�������ļ�������ͼ(����ȥ��������ͼ��û��)
					QString thumFile = filePath.absoluteFilePath();
					int len = thumFile.size();
					thumFile.replace(len - QString(SceneFileSuffix).size(), len, SceneThmNlSuffix);
					QFile::remove(filePath.absoluteFilePath());
					QFile::remove(thumFile);

					//3.�����³���
					m_strCurSceneItemName = strSceneName;
					SceneManager_.createScene(strSceneName.tQtToStdString(), m_strProjectName.tQtToStdString(), true, m_nCurSceneWidth, m_nCurSceneHeight);
					m_bIsNewScene = true;
					CheckOpenFile_ = false;
					return;
				}
			}

			LOG(INFO) << _T("Create Scene : ") << filePath.absoluteFilePath().toStdWString();

			HScene pScene = SceneManager_.getSceneByName(tstring());
			if (pScene.isValid()) {
				if (!pScene.isModified())//��ǰ�򿪵ĳ���û�б��޸���ֱ�ӹرգ�modified by wangww 2014.6.13
				{
					SceneManager_.closeScene(pScene);
				}
				else
				{
					QMessageBox msgBox;
					msgBox.setWindowTitle(tr("Warning"));
					msgBox.setText(tr("You have a scene file is opened"));
					msgBox.setInformativeText(tr("Do you need to save?"));
					msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
					msgBox.setDefaultButton(QMessageBox::Save);
					msgBox.setIconPixmap(QPixmap(":/L3DArtist/Resources/Warning_.png"));
					int nBtn = msgBox.exec();
					if (QMessageBox::Cancel == nBtn)
						return;

					if (QMessageBox::Save == nBtn)
					{
						SceneManager_.saveSceneFile(m_strCurScenePath.tQtToStdString(), pScene);
						SceneManager_.closeScene(pScene);
					}
					else
					{
						SceneManager_.closeScene(pScene);
						if (m_bIsNewScene)
							QFile::remove(m_strCurScenePath);
					}
				}
			}
			m_strCurSceneItemName = strSceneName;
			SceneManager_.createScene(strSceneName.tQtToStdString(), m_strProjectName.tQtToStdString(), true, m_nCurSceneWidth, m_nCurSceneHeight);
			m_bIsNewScene = true;
			CheckOpenFile_ = false;
		}
	}
}

//>  �򿪳����ļ�
void L3DArtist::SlotOpenSceneFile(const QString &strFileName)
{
	pTBtnUploadScene->setEnabled(true);

	if (!m_bRMgrInit)
		return;

	bool bSame = false;
	if (!strFileName.isEmpty())
	{
		if (!SceneManager_.isValid())
		{
			LOG(QERROR) << _T("Scene manager is invalid");
			MSGBOX_ERROR_TIP(tr("Invalid Scene Manager"), this);
			return;
		}
		HScene pScene = SceneManager_.getSceneByName(tstring());
		if (pScene.isValid()) {
			if (QFileInfo(strFileName).filePath() == L3DArtist::m_strCurScenePath)
			{
				int nBtnSame(-1);
				MSGBOX_WARNING_OKANDCANCEL(tr("Scene has been opened, Are you sure to reopen it?"), nBtnSame, this);

				if (nBtnSame != QMessageBox::Ok)
					return;
				else
					bSame = true;
			}
			if (!ArtistParams::instance()->isArtistEditMode() && pScene.isModified())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle(tr("Warning"));
				msgBox.setText(tr("Scene file is opening"));
				msgBox.setInformativeText(tr("Do you want to save?"));
				msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
				msgBox.setDefaultButton(QMessageBox::Save);
				msgBox.setIconPixmap(QPixmap(":/L3DArtist/Resources/Warning_.png"));
				int nBtn = msgBox.exec();
				if (QMessageBox::Cancel == nBtn)
					return;
				else if (QMessageBox::Save == nBtn)
					SceneManager_.saveSceneFile(m_strCurScenePath.tQtToStdString(), pScene);
				else
				{
					if (m_bIsNewScene && !bSame)
						QFile::remove(m_strCurScenePath);
				}
			}
			SceneManager_.closeScene(HScene());
			m_bIsOpenScene = true;
		}

		SlotShowProgress(true);
		if (!m_bIsOpenScene)
		{
			if (!ArtistParams::instance()->isArtistEditMode())
				SceneManager_.openSceneFile(strFileName.tQtToStdString(), m_strProjectName.tQtToStdString());
			else
				SceneManager_.openSceneFile(strFileName.tQtToStdString(), m_strProjectName.tQtToStdString(), nullptr, m_vecEdit);
		}
		m_bIsNewScene = false;
		m_strCurScenePath = strFileName;
		CheckOpenFile_ = true;
	}
	m_checkIsModifiedTimer->start(500);
	// 	emit SglActionEnable(true);// Modify by XYC 2014.07.04
	// 	emit SglPropertyEnable(true);
}

//> ���µ�ǰ���ļ���Ϣ
void L3DArtist::SlotUpdateCurOpenFile(const QString &filename)
{
	m_strCurScenePath = filename;
	SetCurSceneDirectory(filename);  // ���ó���Ŀ¼ 2014-11-24 added by yejia 
}

//> ����ڵ�
void L3DArtist::SlotImportNode()
{
	//QString strFilename(QFileDialog::getOpenFileName(this, QObject::tr("����ڵ�"), QString(),
	//    QObject::tr("��ͼ3D��Ļ����(*.l3dn)")));
	//if (!strFilename.isEmpty()) {
	//    SlotOpenSceneFile(strFilename);
	//    CheckOpenFile_ = false; //�����ⲿ�����Ļ�����Ҫ�ڱ��ر����¡�
	//}
}

//> ���볡��
void L3DArtist::SlotImportScene()
{
	// modified by yejia 2015-05-13 ��ģʽ�Ի���
	//FileImEx file_import(this);
	FileImEx *file_import = new FileImEx(this);
	file_import->setAttribute(Qt::WA_DeleteOnClose);
	connect(file_import, &FileImEx::SglImportCompleted, this, &L3DArtist::SglUpdateSceneList, Qt::QueuedConnection);
	QStringList strFilesname(QFileDialog::getOpenFileNames(this, tr("Import Scene"), QString(),
		tr("3D Subtitle Scene(*.l3dp)")));

	QFileInfo scene_file = GetCurProjectPath() + '/' + GetCurSceneListPath();
	QDir dirroot("./");
	file_import->setPath(scene_file.absoluteFilePath(), GetCurProjectPath(), GetCurMediaPath(), dirroot.absolutePath() + "/Temp", ArtistParams::instance()->VideoMode());

	file_import->Copy(strFilesname, scene_file.absoluteFilePath(), CopyMode_Import);
	file_import->show();
}

//> ��������
void L3DArtist::SlotExportScene()
{
	DockScene* pDockScene = static_cast<DockScene*>(m_pDockScene->widget());
	QFileInfoList export_files = pDockScene->GetFileInfo(false);
	if (!(export_files.size() == 1 && export_files[0].isFile()))
	{
		MSGBOX_WARNING_TIP(tr("Please select a single file for export exported!"), this); // ��ѡ�񵥸��ļ����е���!
		return;
	}

	auto export_file = export_files[0];
	// ���浼���ļ�
	QString export_filePath_ = QFileDialog::getSaveFileName(this, tr("Export Scene"), m_pExportScenePath + export_file.completeBaseName(), tr("3D Subtitle Scene(*.l3dp)"));
	if (export_filePath_.isEmpty())
		return;

	int index = export_filePath_.lastIndexOf("/");
	if (index > -1)
		m_pExportScenePath = export_filePath_.left(index + 1); //���浼������Ŀ¼

	if (!SceneManager_.isValid())
	{
		MSGBOX_ERROR_TIP(tr("Invalid Scene Manager"), this);
		return;
	}

	if (!export_file.exists())
	{
		MSGBOX_WARNING_TIP(tr("scene file [%1] not exist!").arg(export_file.fileName()), this);
		return;
	}

	FileImEx *fileImEx = new FileImEx(this);
	fileImEx->setAttribute(Qt::WA_DeleteOnClose);
	if (QFile::exists(export_filePath_))
		QFile::remove(export_filePath_);
	QDir dirroot("./");
	fileImEx->setPath(export_file.absoluteFilePath(), GetCurProjectPath(), GetCurMediaPath(), dirroot.absolutePath() + "/Temp");
	QStringList strfromDirs;
	QFileInfo tofile(export_filePath_);

	strfromDirs << export_filePath_;
	fileImEx->Copy(strfromDirs, tofile.absolutePath(), CopyMode_Export);
	fileImEx->show();
}

//�ж��ļ����Ƿ��зǷ��ַ�
bool L3DArtist::isLegal(QString &strName)
{
	while (strName[0].isSpace())
		strName = strName.right(strName.length() - 1);

	if (strName.isEmpty())
		return false;

	while (strName.right(1) == " ")
		strName = strName.left(strName.length() - 1);

	//������ʽ�ж������ַ�
	QRegExp rxp("['<>*?:\\\\\"/&!|]");
	return !strName.contains(rxp);
}

//> �л�����
void L3DArtist::SlotChangeLanguage()
{
	if (m_bTranslate)
	{
		m_pOwner->removeTranslator(translator_cn);
		m_pOwner->removeTranslator(translator_default);
		m_bTranslate = false;
	}
	else
	{
		m_pOwner->installTranslator(translator_cn);
		m_pOwner->installTranslator(translator_default);
		m_bTranslate = true;
	}
	ArtistParams::instance()->Translated(m_bTranslate);
	TranslateLanguage();
}

//> �رճ���
void L3DArtist::SlotCloseScene()
{
	if (GetIsMaybeSave())
	{
		m_strCurScenePath.clear();
		emit SglUpdateSceneList();
		pTBtnUploadScene->setEnabled(false);
	}
	m_checkIsModifiedTimer->stop();
}

//> ���泡��
void L3DArtist::SlotSaveScene()
{
#ifdef NDEBUG
	// ���ܹ���֤ Added by ZY 2015.1.4
	CFickleCheckLic lic;
	if (!lic.Check(6, 6)) {
		return;
	}
#endif
	if (!SceneManager_.isValid())
	{
		MSGBOX_ERROR_TIP(tr("Invalid Scene Manager"), this);
		return;
	}

	if( CurOpenScene_.isValid()/* && CurOpenScene_.isModified() */)
    {
         SceneManager_.saveSceneFile(m_strCurScenePath.tQtToStdString(), HScene());
    }
}

//> ���Ϊ����
void L3DArtist::SlotSaveAsScene()
{
#ifdef NDEBUG
	// ���ܹ���֤ Added by ZY 2015.1.4
	CFickleCheckLic lic;
	if (!lic.Check(6, 6)) {
		return;
	}
#endif
	if (!GetCurScene().isValid())
	{
		MSGBOX_ERROR_TIP(tr("No scene opened"), this);
		return;
	}

	if (!SceneManager_.isValid())
	{
		MSGBOX_ERROR_TIP(tr("Invalid Scene Manager"), this);
		return;
	}

	bool bIsTrue(false);
	QString strSceneName(tQtFromStdString(GetCurScene().getName()));
	for (;;)
	{
		strSceneName = QInputDialog::getText(this, tr("Save As Scene"), tr("Please Enter a Scene Name:"),
			QLineEdit::Normal, strSceneName, &bIsTrue, Qt::WindowCloseButtonHint);

		if (!bIsTrue) break;

		if (!isLegal(strSceneName))
		{
			MSGBOX_ERROR_TIP(tr("Scene file name is invalid!"), this);//�ļ����Ƿ�
			continue;
		}

		QString confictFileName = checkfileConfict(strSceneName + ".l3d");
		if (!confictFileName.isEmpty())
		{
			MSGBOX_WARNING_TIP(tr("file name has exist, please try again!\nconfict file path:[%1]").arg(confictFileName), this);
			continue;
		}
		SceneManager_.saveSceneFile((GetCurProjectPath() + '/' + m_strSceneListPath + '/' + strSceneName + ".l3d").tQtToStdString(), HScene(), true);
		break;
	}
}

//> ��ȡ�ڵ�
void L3DArtist::SlotExtractNode()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}

	//�첽����������ڵ㼴Ϊ��ýڵ��xml���л����� ���﷢������
	CAsynOperatingBassPtr pdata = CAsynOperatingBass::CreatOperating(CAsynOperatingBass::OperatingEnum_ExtractNode);
	setOperating(pdata);
	node.serializeXML();
}

void L3DArtist::SlotAside()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}
	HNode node = scene.getFocusedNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}
	node.setPosToRulers();
}

//> ��ȡ����
void L3DArtist::SlotExtractScene()
{
	if (!GetCurScene().isValid())
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}

	//�첽����������ڵ㼴Ϊ��ýڵ��xml���л����� ���﷢������
	CAsynOperatingBassPtr pdata = CAsynOperatingBass::CreatOperating(CAsynOperatingBass::OperatingEnum_ExtractScene);
	setOperating(pdata);
	GetCurScene().serializeXML();
}

//> ���Ʋ��ʲۺ���
void L3DArtist::SlotCopyMaterial()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
		return;

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
		return;

	CAsynOperatingBassPtr pdata = CAsynOperatingBass::CreatOperating(CAsynOperatingBass::OperatingEnum_CopyMaterial);
	setOperating(pdata);
	int nIndex = node.getCurMaterialIndex();
	if (nIndex > 0)
		node.serializeMaterial(nIndex - 1);
	else
		node.serializeMaterial(0);
}

//> ճ�����ʲۺ���
void L3DArtist::SlotPasteMaterial()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
		return;

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
		return;

	CAsynOperatingBassPtr pdata = GetOperating();
	if (pdata && pdata->GetType() == CAsynOperatingBass::OperatingEnum_CopyMaterial)
		pdata->OnNextOperating(&node);
}

//> ɾ������
void L3DArtist::SlotRemoveMaterial()
{
	HScene scene = GetCurScene();
	if (!scene.isValid()) 
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}
	
	int btnSel = -1;
	MSGBOX_WARNING_OKANDCANCEL(tr("Are you sure to remove material?"), btnSel, this);
	if (btnSel != QMessageBox::Ok)
		return;

	uint meshIdx = node.getCurMaterialIndex();
	iv::SMaterial	mat;
	if (meshIdx > 0)
		mat = node.getMaterial(meshIdx - 1);
	else // meshIdx == 0,ȫ����
		mat = node.getMaterial(0);

	mat.AmbientColor = iv::SColor(255, 0, 0, 0);				// ������
	mat.DiffuseColor = iv::SColor(255, 255, 255, 255);			// ����� 
	mat.EmissiveColor = iv::SColor(255, 0, 0, 0);				// �Է���
	mat.SpecularColor = iv::SColor(255, 255, 255, 255);			// �����
	mat.Shininess = 100.0f;										// ����߹�
	
	for (int textureIdx = 0; textureIdx < MATERIAL_MAX_TEXTURES; textureIdx++)
	{
		auto appTexture = node.getAppTexture(textureIdx);
		if (!appTexture)
			continue;

		node.deleteAppTexture(textureIdx);
	}

	node.setMaterial(mat, meshIdx - 1, false);

	// ������е���չ��������
	for (int i = 0; i < MaxMatExtSize; i++)
	{
		for (int j = 0; j < MATERIAL_MAX_TEXTURES; j++)
		{
			auto appTexture = node.getAppTextureExt(meshIdx - 1, i, j);
			if (!appTexture)
				continue;

			node.removeAppTextureExt(meshIdx - 1, i, j);
		}
	}

	for (int i = 0; i < MaxMatExtSize; i++)
	{
		node.removeMaterialExt(meshIdx - 1, i);
	}
}

//> ��ȡ����
void L3DArtist::SlotExtractMaterial()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}

	std::vector<SMaterial> vecMat;
	uint meshIdx = node.getCurMaterialIndex();

	iv::SMaterial mat;
	if (meshIdx > 0)
		mat = node.getMaterial(meshIdx - 1);
	else // meshIdx == 0,ȫ����
		mat = node.getMaterial(0);

	// �����ϵķ�ʽ�����л�һ������
	tstring xml;
	_materialXMLPath = QApplication::applicationDirPath().replace('\\', '/') +
		QString("/Model/Property/Materia/") + tQtFromStdString(node.getName()) + ".l3d";

	tostringstream oss;
	ba_xml_toarchive oa(oss, 1);
	oa << bs::make_nvp("Material", mat);

	int extTexNum = 1;
	if (mat.MaterialType == EMT_GLSL_FOURTEXTURE_BLEND)
	{
		for (int idx = 0; idx < MaxMatExtSize; idx++) 
		{
			SMaterial pMat;
			bool rslt = node.getMaterialExt(meshIdx, idx, pMat);
			if (!rslt)
				break;

			vecMat.push_back(pMat);
		}

		// ���л���չ����
		oa << bs::make_nvp("extMaterials", vecMat);
		xml = oss.str();

		for (int mi = 0; mi < MaxMatExtSize; mi++) 
		{
			for (int ti = 0; ti < MATERIAL_MAX_TEXTURES; ti++) 
			{
				AppTexturePtr appTex = node.getAppTextureExt(meshIdx, mi, ti);
				if (!appTex.isNull())
					extTexNum++;
			}
		}

		oa << bs::make_nvp("extAppTextureNum", extTexNum);

		AppTexturePtr defaultAppTex = node.getAppTexture(meshIdx);
		if (defaultAppTex)
			defaultAppTex->serialize();

		for (int mi = 0; mi < MaxMatExtSize; mi++)
		{
			for (int ti = 0; ti < MATERIAL_MAX_TEXTURES; ti++) 
			{
				AppTexturePtr appTex = node.getAppTextureExt(meshIdx, mi, ti);
				if (!appTex.isNull())
					appTex->serialize();
			}
		}
	}
	else 
	{
		_materialXML = oss.str();
		saveMaterialToXML();
	}
}

void L3DArtist::saveMaterialToXML()
{
	if (!_materialXMLPath.isEmpty())
	{
		QFile file(_materialXMLPath);
		if (file.open(QFile::ReadWrite | QFile::Text))
		{
			QTextStream out(&file);
			out << tQtFromStdString(_materialXML);
			file.close();

			QMessageBox::information(nullptr, QObject::tr("Infomation"), QObject::tr("Extract material successful!"), QMessageBox::Ok);
		}
	}
}

//> ��������
void L3DArtist::SlotCopyTex()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
		return;

	HNode node = scene.getFocusedNode();
	if (node.isValid())
	{
		int nIndex = node.getCuTexIndex();
		if (nIndex > 0)
			node.serializeBaseTexture(nIndex - 1);
		else
			node.serializeBaseTexture(0);
	}
}

//> ճ������
void L3DArtist::SlotPasteTex()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
		return;

	// ���ԶԶ���ڵ����ճ������
	//HNode node = scene.getFocusedNode();
	std::vector<HNode> nodeVec;
	scene.getSelectedNodes(nodeVec);
	auto iter = nodeVec.begin();
	for (; iter != nodeVec.end(); iter++)
	{
		if ((*iter).isValid())
		{
			QClipboard *pBoard = QApplication::clipboard();
			const QMimeData *pMimeData = pBoard->mimeData();
			QByteArray byte = pMimeData->data("texture");
			if (byte.isEmpty())
				return;

			(*iter).createTexture(QString(byte).tQtToStdString());
		}
	}
}

//> ɾ������
void L3DArtist::SlotRemoveTex()
{
	HScene scene = GetCurScene();
	if (!scene.isValid()) {
		return;
	}

	HNode node = scene.getFocusedNode();
	if (node.isValid())
	{
		int i = node.getCuTexIndex();
		node.deleteAppTexture(i - 1);
		UpdateNodeAnimator(node);
	}
}

void L3DArtist::SlotExtractTexture()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
		return;

	HNode node = scene.getFocusedNode();
	if (node.isValid()) {
		int i = node.getCuTexIndex();
		auto apt = node.getAppTexture(i - 1);
		if (!apt)
			return;
		auto bt = apt->getBaseTexture();
		if (!bt)
			return;
		QDir dir = qApp->applicationDirPath();
		dir.cd("Model");
		dir.cd("Material");
		dir.cd("Texture");
		if (bt->getImageType() == EBTIT_GRADIENT)
			dir.cd(QStringLiteral("��ɫ"));
		else
			dir.cd(QStringLiteral("��ɫ"));

		QString name = "untitled.l3d";
		if (!dir.exists(name))
			m_extractPath = dir.absolutePath() + "/" + name;
		else {
			int i = 1;
			while (true) {
				QString tmp = "untitled_" + QString::number(i) + ".l3d";
				if (!dir.exists(tmp)) {
					m_extractPath = dir.absolutePath() + "/" + tmp;
					break;
				}
				i++;
			}
		}

		if (m_extractPath.isEmpty())
			return;

		if (m_extractPath.right(4) != ".l3d")
			m_extractPath += ".l3d";

		m_extractTex = true;
		node.serializeBaseTexture(i - 1);
		QMessageBox::information(this, tr("extract"), tr("extract texture success"));
	}
}

//> ������
void L3DArtist::SlotArrayCopy()
{

}

//> ת��Ϊ3D���ֲۺ���
void L3DArtist::SlotConvert3DText()
{
	HScene scene = GetCurScene();
	if (!scene.isValid()) {
		return;
	}

	HNode node = scene.getFocusedNode();
	if (node.isValid()) {
		if (node.getType() == E_L3DNODE_TYPE::ELNT_2DText) {
			//> TODO
		}
	}
}

//> ת��Ϊ2D���ֲۺ���
void L3DArtist::SlotConvert2DText()
{
	HScene scene = GetCurScene();
	if (!scene.isValid()) {
		return;
	}

	HNode node = scene.getFocusedNode();
	if (node.isValid()) {
		if (node.getType() == E_L3DNODE_TYPE::ELNT_3DText) {
			//> TODO
		}
	}
}

//> ��ȡ�����ۺ���
void L3DArtist::SlotExtractAni()
{
	HScene scene = GetCurScene();
	if (!scene.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Current scene is invalid!"), this);
		return;
	}

	HNode node = scene.getFocusedNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}

	std::vector<AnimatorPtr> vecAni;
	node.getAllAnimator(vecAni);
	if (vecAni.size() == 0)
	{
		MSGBOX_ERROR_TIP(tr("This object have no animation!"), this);
		return;
	}
	//�첽������������ǰ�������Ķ������ڻص������������򱣴��
	CAsynOperatingBassPtr pdata = CAsynOperatingBass::CreatOperating(CAsynOperatingBass::OperatingEnum_ExtractAnim);
	setOperating(pdata);
	node.extractAnimator();
}

//> �˳�
void L3DArtist::SlotExit()
{
	SlotPreClose();
}

/******************************************��Ⱦ���ڶ��빦������*********************************
void L3DArtist::SlotAlignX()
{
QMessageBox::information(this, "Clicked", "AlignX is clicked!");
}

void L3DArtist::SlotAlignY()
{
QMessageBox::information(this, "Clicked", "AlignY is clicked!");
}

void L3DArtist::SlotAlignZ()
{
QMessageBox::information(this, "Clicked", "AlignZ is clicked!");
}
/******************************************��Ⱦ���ڶ��빦������*********************************/

//> ����
void L3DArtist::SlotAbout()
{
#ifdef FOR_XOR_CO_VER
	QString copyright = ArtistParams::instance()->Copyright();
	QString copyrightTime = ArtistParams::instance()->CopyrightTime();
	QString hotline = ArtistParams::instance()->SeviceHotline();
	QString support = ArtistParams::instance()->Support();
	DoAboutBox(MODULE_Artist, "", NULL, false);
#else
	DoAboutBox(MODULE_Artist, "", NULL);
#endif
}

void L3DArtist::SlotShortCut()
{
	ShortcutDescription shortcutDes(this);
	shortcutDes.exec();
}

//added by yj 2014-6-23 ���ñ�����ɫ
void L3DArtist::SlotBackgroundColor()
{
	bool isbackcolor(false), ispicground(false);
	QColor color;
	QString picpath;
	ArtistParams::instance()->getBackGround(isbackcolor, color);
	ArtistParams::instance()->getPicBackGround(ispicground, picpath);

	iv::SColor backClr = CEngineKerDll::getActionSafeColor();
	BackGroundColor	newColorWindow(isbackcolor, ispicground, color, picpath, this);
	int width(0), height(0);
	ArtistParams::instance()->getCustomRenderSize(width, height);
	newColorWindow.setLabelSize(width, height);

	if (newColorWindow.exec() != QDialog::Accepted)
		return;

	newColorWindow.getInfo(isbackcolor, ispicground, color, picpath);

	ArtistParams::instance()->setBackGround(isbackcolor, color);
	ArtistParams::instance()->setPicBackGround(ispicground, picpath);
	CEngineKerDll::setIsDrawBack(isbackcolor);

	if (isbackcolor && !ispicground)
	{
		backClr.setRed(color.red());
		backClr.setGreen(color.green());
		backClr.setBlue(color.blue());
	}
	CEngineKerDll::setBackColor(backClr);

}

void L3DArtist::SlotShowSafe()
{
	if (!ui.m_pSafe->isChecked())
	{
		CEngineKerDll::setbSafetyArea(false);
		ui.m_pSafe->setChecked(false);
	}
	else
	{
		CEngineKerDll::setbSafetyArea(true);
		ui.m_pSafe->setChecked(true);
	}
	ArtistParams::instance()->ShowSafe(CEngineKerDll::getbSafetyArea());//RenderManager_.getbSafetyArea()
}

void L3DArtist::SlotShowTitleSafe()
{
	if (!ui.m_pTitleSafe->isChecked())
	{
		CEngineKerDll::setbTitleSafetyArea(false);
		ui.m_pTitleSafe->setChecked(false);
	}
	else
	{
		CEngineKerDll::setbTitleSafetyArea(true);
		ui.m_pTitleSafe->setChecked(true);
	}
	ArtistParams::instance()->ShowTitle(CEngineKerDll::getbTitleSafetyArea());//RenderManager_.getbTitleSafetyArea()
}

void L3DArtist::slotDrawRulers()
{
	bool draw = ui.m_pShowRuler->isChecked();
	CEngineKerDll::setIsDrawRulers(draw);
	ArtistParams::instance()->isDrawRulers(draw);
	if (!draw)
	{
		//CEngineKerDll::clearRulerParams();
		//ArtistParams::instance()->clearRulerParams();
		emit sglClearAllRulerItem();
	}
	else
		emit sglShowAllRulerItem();
}

// ���������õ�Artist�������У��ٵ��ô˽ӿ�
void L3DArtist::slotSetRulers()
{
	std::set<int> h, v;
	ArtistParams::instance()->getDrawRulers(h, v);
	CEngineKerDll::setDrawRulers(h, v);
}

void L3DArtist::SlotTga()
{
	//> ��Ӧ�ó���ķ���״̬д��L3DTga2Flg.exe�������ļ��У��Ա�֤����һ���� added by yejia 2014-12-17
	QSettings settings("settings_tgatoflg.ini", QSettings::IniFormat);
	settings.beginGroup("tgatoflg");
	if (L3DArtist::m_bTranslate == true)
		settings.setValue("translated", true);
	else
		settings.setValue("translated", false);
	settings.endGroup();
	QProcess::startDetached("L3DTga2Flz.exe");
}

void L3DArtist::SlotSceneLayer()
{
	HScene scene = GetCurScene();
	if (scene.isValid())
	{
		int i = scene.getShowLayer();
		bool bIsTrue = false;
		int index = QInputDialog::getInt(this, tr("Scene Layer"), tr("Please enter scene layer:"),
			i, 0, 100, 1, &bIsTrue, Qt::WindowCloseButtonHint);
		if (bIsTrue)
			scene.setShowLayer(index);
	}
}

//> Undo�ۺ���
void L3DArtist::SlotUndo()
{
	if (SceneManager_.isValid())
		SceneManager_.undo();
}

//> Redo�ۺ���
void L3DArtist::SlotRedo()
{
	if (SceneManager_.isValid())
		SceneManager_.redo();
}

//> �����ڵ�ۺ���
void L3DArtist::SlotCreatNode(E_L3DNODE_TYPE type, void *param, HNode parent)
{
	if (CurOpenScene_.isValid()) {
		CurOpenScene_.createNode(type, param, parent);
	}
}

//> ����Udo  -redo ϵͳ 
void L3DArtist::SlotUpdataUdRdSystem(const QStringList &UndoList, const QStringList &RedoList)
{
	/// to do

}

//> LEKResultֵ�ж�
void L3DArtist::LEKResultMessage(LEKResult rslt)
{
	QString strErrorMsg("");
	switch (rslt) {
	case EKRSLT_S_OK:
		return;
	case EKRSLT_S_CMD_PENDING:
		strErrorMsg = tr("Function call will be executed asynchronously");//�������ý��첽ִ��
		break;
	case EKRSLT_S_FUNC_CALLED:
		strErrorMsg = tr("Function has been called or the functionality is implemented,so don't need to call again");//�����ú����Ѿ����ù��ˡ���ú���Ҫ��ɵĹ����Ѿ�ʵ���ˣ������ٴε���
		break;
	case EKRSLT_E_GENERAL:
		strErrorMsg = tr("Unclassified Error");//δ�������
		break;
	case EKRSLT_E_FUN_PARAMS:
		strErrorMsg = tr("Parameter failed the examination");//�������ò���δͨ�����
		break;
	case EKRSLT_E_THREAD_NR:
		strErrorMsg = tr("Error of Inner thread or task scheduling,such as abnormal exit");//DLL�ڲ��̡߳�������ȵȳ�����δ�������쳣�˳���
		break;
	case EKRSLT_E_EXECPT:
		strErrorMsg = tr("Internal exception of Dll");//DLL�ڲ��쳣
		break;
	case EKRSLT_E_UN_INIT:
		strErrorMsg = tr("Error corresponding to an uninitialized success or to the internal object in a state of invalid");//DLL�ڲ���Ӧ����δ��ʼ���ɹ�����Ҫʹ�õ��ڲ���������Ч״̬
		break;
	case EKRSLT_E_FALSE_USE:
		strErrorMsg = tr("Using functionality provided by the DLL in the wrong way");//�Դ���ķ�ʽʹ��DLL�ṩ�Ĺ���
		break;
	case EKRSLT_E_SCENE_RE:
		strErrorMsg = tr("The same scene has been in existence");//ͬ�������Ѵ���
		break;
	case EKRSLT_E_SCENE_NO:
		strErrorMsg = tr("The scene doesn't exist or scene file doesn't exist or damaged");//Ҫ�����ĳ��������ڡ��򳡾��ļ�(XML�ַ���)�����ڻ���
		break;
	case EKRSLT_E_NOT_PROCESSED:
		strErrorMsg = tr("Internal scheduling (command) or a function cannot execute");//�ڲ�������Ϣ(����)�������޷�ִ��
		break;
	case EKRSLT_E_INVALID_HANDLE:
		strErrorMsg = tr("Internal(handle)object is invalid or failed in creation");//�ڲ�(���)������Ч�򴴽�ʧ��
		break;
	case EKRSLT_E_NO_IMPL_FUN:
		strErrorMsg = tr("Internal functions isn't complemented");//���õ�DLL�ڲ�������δʵ��
		break;
	case EKRSLT_E_INNER_OBJ_ERR:
		strErrorMsg = tr("Invalid state of Inernal object or unable to support this feature");//�������ù�����,DLL�ڲ�ʹ�õ�ĳ���ڲ�����״̬��������������Ч�����޷�֧�ִ˹���
		break;
	case EKRSLT_E_NODE_RE_E:
		strErrorMsg = tr("The same node or other object has been in existence, or  repetitive operation");//��ͬ�Ľڵ�(����������)�����Ѵ��ڡ����ظ�����
		break;
	case EKRSLT_E_OBJECT_NO_E:
		strErrorMsg = tr("The object to obtain doesn't exist or is invalid");//Ҫ��ȡ�Ķ��󲻴��ڻ���Ч
		break;
	case EKRSLT_E_NOT_MEM:
		strErrorMsg = tr("Memory allocation failure");//�ڴ����ʧ��
		break;
	case EKRSLT_E_NO_RENDERING:
		strErrorMsg = tr("Rendering logic doesn't start or fail to start");//��Ⱦ�߼�δ������������ʧ��
		break;
	case EKRSLT_E_AniVideoInit_ERROR:
		strErrorMsg = tr("failure to initiate the generated animated video tools");//���ɶ�����Ƶ���߳�ʼ��ʧ��
		break;
	case EKRSLT_E_OverLoad_Scene_ERROR:
		strErrorMsg = tr("Scene is OverLoaded");
		break;
	case EKRSLT_E_Text_OFFLIMIT_ERROR:
		strErrorMsg = tr("Text is off limit");
		break;
	case EKRSLT_E_OPEN_SCENEFILE_ERROR:
		strErrorMsg = tr("Please save scene file later");
		break;
	default:
		strErrorMsg = tr("Unknown error");//��������δ֪
		break;
	}
	LOG(QERROR) << strErrorMsg.tQtToStdString();

	MSGBOX_ERROR_TIP(strErrorMsg, this);
}

//> ��������item���������node�ۺ���
void L3DArtist::SlotSceneTreeItemCk()
{
	//emit SglUpdateProperty();
}

//> �ı���Ⱦ���ڴ�С����
void L3DArtist::SlotResizePlayWnd(int x, int y, int width, int height, bool repaint)
{
	if (RenderManager_.isValid())
		RenderManager_.moveRenderWindow(x, y, width, height, repaint);
}

//> ���¹�����xyz����ؼ�
void L3DArtist::SlotUpdateToolBarXYZ(double dX, double dY, double dZ, NodeCtrl nodectrl)
{
	//> ֻ��ʾ��ǰ�ڵ�Ŀ���ģʽ�е�XYZ��ֵ
	if (m_eNodeCtrl == nodectrl || nodectrl == NODECTRL_INIT)
	{
		m_pDSpinBoxX->blockSignals(true);
		m_pDSpinBoxY->blockSignals(true);
		m_pDSpinBoxZ->blockSignals(true);
		if (nodectrl == NODECTRL_INIT)
		{
			m_pDSpinBoxX->clearFlag();
			m_pDSpinBoxY->clearFlag();
			m_pDSpinBoxZ->clearFlag();
		}
		m_pDSpinBoxX->setValue(dX);
		m_pDSpinBoxY->setValue(dY);
		m_pDSpinBoxZ->setValue(dZ);
		m_pDSpinBoxX->blockSignals(false);
		m_pDSpinBoxY->blockSignals(false);
		m_pDSpinBoxZ->blockSignals(false);
	}

	emit SglUpdateKeyFrameSetXYZ(dX, dY, dZ, nodectrl);//added by xyc 2014.09.18
}

//> ���xyz��Ϊ0
void L3DArtist::SlotClearSpinBox()
{
	SlotUpdateToolBarXYZ(0, 0, 0, NODECTRL_INIT);
}

void L3DArtist::SlotSetNodeCtrl(NodeCtrl nodectrl)
{
	m_eNodeCtrl = nodectrl;
	if (GetCurFocusNode().isValid())
	{
		ic::vector3df vec3df;
		switch (nodectrl)
		{
		case NODECTRL_MOVE: {
			setComboxMoveType();//>�������ƶ�����µ���������Ϊ����ֵ
			comboBoxMoveSetting(nodectrl);//> added by TaiYue 2019.6.5 
			vec3df = GetCurFocusNode().getPosition();
			setMoveFocusXYZ();
			break;
		}
		case NODECTRL_ROTATE: {
			setRotateType();//> added by TaiYue 2019.6.5 
			m_PDComboxChoose->setVisible(true);//> ��ComboBox�������������
			comboBoxRotateSetting(nodectrl);
			vec3df = GetCurFocusNode().getRotation();
			setRotateFocusXYZ();
			break;
		}
		case NODECTRL_SCALE: {
			setComboxScaleType();//> added by TaiYue 2019.6.5 for ��������������µ������б�ΪXYZ��XY,XZ,None
			comboBoxScaleSetting(nodectrl);//> added by TaiYue 2019.6.5 for ѡ������״̬ʱ�ĳ�ʼ���벿�ֹ���
			vec3df = GetCurFocusNode().getScale();
			setScaleFocusXYZ();
			break;
		}
		case NODECTRL_SIZE: {
			m_PDComboxChoose->setVisible(false);//> added by TaiYue 2019.6.5 
			SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);
			
		}
							return;
		default:
			break;
		}
		SlotUpdateToolBarXYZ(vec3df.X, vec3df.Y, vec3df.Z, nodectrl);
	}
	else
	{
		SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);
	}
}

//> ������Ű�ť
void L3DArtist::SlotBtnPlayOnCli()
{
	if (!CurOpenScene_.isValid())
		return;

	LOG(INFO) << _T("��ʼ���ų���");
	int secAniCount = CurOpenScene_.sectionAniCount();

	if (CurOpenScene_.isPlaying())
	{
		if (secAniCount != 0 /*&& secAniSeq_ <= secAniCount*/)
		{
			//if (secAniSeq_ > secAniCount)
			//{
			//	secAniSeq_ = 1;
			//	//CurOpenScene_.seekScene(0);
			//	RulerMoved(0);
			//	if ( CurOpenScene_.pauseScene())
			//		changePlayState(false);
			//	return;
			//}

			if (!CurOpenScene_.playSceneAni(/*++secAniSeq_*/0))
			{
				RulerMoved(0);
				if (CurOpenScene_.pauseScene())
					changePlayState(false);
				return;
			}
		}
		else
		{
			if (CurOpenScene_.pauseScene())
				changePlayState(false);
		}
	}
	else
	{
		if (m_hasPlayed)
		{
			CurOpenScene_.continueScene();
			/*if (secAniCount != 0)
			secAniSeq_++;*/
		}
		else
		{
			CurOpenScene_.seekScene(0);
			if (CurOpenScene_.playScene(false, -1))
			{
				m_hasPlayed = true;
				secAniSeq_ = 1;
			}

#if defined(FOR_XOR_CO_VER)   // for��xor bug 22185 ,added by wuheng 2015.12.29
			else
			{
				HScene scene = GetCurScene();
				if (scene.isValid())
				{
					scene.seekScene(0);
					RulerMoved(0);
					CurOpenScene_.playScene(false, -1);
				}
			}
#endif
		}
	}
}

//> ���õ�ǰ�ڵ����״̬
void L3DArtist::SlotSetCurNodeCtrl()
{
	//m_eNodeCtrl = eNc;
	switch (m_eNodeCtrl)
	{
	case NODECTRL_MOVE:
		if (GetCurFocusNode().isValid())
		{
			ic::vector3df vec3df = GetCurFocusNode().getPosition();
			SlotUpdateToolBarXYZ(vec3df.X, vec3df.Y, vec3df.Z, m_eNodeCtrl);//modify by xyc 2014.09.18Ϊ�˸��ؼ�֡���ô����ṩ�ж�����
		}
		else {
			SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);//modify by xyc 2014.09.18
		}
		break;
	case NODECTRL_ROTATE:
		if (GetCurFocusNode().isValid()) {
			ic::vector3df vec3df = GetCurFocusNode().getRotation();
			SlotUpdateToolBarXYZ(vec3df.X, vec3df.Y, vec3df.Z, m_eNodeCtrl);//modify by xyc 2014.09.18
		}
		else {
			SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);//modify by xyc 2014.09.18
		}
		break;
	case NODECTRL_SCALE:
		if (GetCurFocusNode().isValid()) {
			ic::vector3df vec3df = GetCurFocusNode().getScale();
			SlotUpdateToolBarXYZ(vec3df.X, vec3df.Y, vec3df.Z, m_eNodeCtrl);//modify by xyc 2014.09.18
		}
		else {
			SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);//modify by xyc 2014.09.18
		}
		break;
	case NODECTRL_SIZE:
		SlotUpdateToolBarXYZ(0.0, 0.0, 0.0, NODECTRL_INIT);
		break;
	default:
		Q_ASSERT(false);
	}
}

//> ѯ���Ƿ񱣴�
bool L3DArtist::GetIsMaybeSave()
{
	if (CurOpenScene_.isValid())
	{
		if (CurOpenScene_.isModified() && !ArtistParams::instance()->isArtistEditMode())
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Warning"));
			msgBox.setText(tr("You have a scene file unsaved"));
			msgBox.setInformativeText(tr("Do you want to save?"));
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Save);
			msgBox.setIconPixmap(QPixmap(":/L3DArtist/Resources/Warning_.png"));
			int nBtn = msgBox.exec();
			if (QMessageBox::Save == nBtn)
			{
				SceneManager_.saveSceneFile(m_strCurScenePath.tQtToStdString(), CurOpenScene_);
				SceneManager_.closeScene(CurOpenScene_);
				return true;
			}
			else if (QMessageBox::Cancel == nBtn)
			{
				m_bPreClose = false;
				m_closeEvent = false;//ȡ��������ȡ�������˳����̣�added by wangww 2014.6.12
				return false;
			}
			else {
				SceneManager_.closeScene(CurOpenScene_);
				if (m_bIsNewScene)
					QFile::remove(m_strCurScenePath);

				return true;
			}
		}
		else  //����û�б��޸�Ҳ�رչرյ�ǰ����
			SceneManager_.closeScene(CurOpenScene_);
	}
	else
	{
		if (m_closeEvent)
		{
			UninitDll();
			m_bPreClose = false;
			m_closeEvent = false;//added by wagnww 2014.1.9
		}
		//         else if (m_isResetProject)
		//         {
		//             UpdateCurSene(HScene());
		//             emit SglClearCtrlPoint();
		//             resetDllProject();
		//         }
	}
	return true;
}

CAsynOperatingBassPtr L3DArtist::GetOperating()
{
	return AsynOperating_;
}

void L3DArtist::setOperating(CAsynOperatingBassPtr pdata)
{
	AsynOperating_ = pdata;
}

//�������ñ�������Ϣ, added by xyc 2014.07.10 
void L3DArtist::setToolBoxTitleBar(l3dDockWidget::ToolBoxType type, const QString &strDirPath)
{
	l3dDockWidget *pdata;
	QString TitleBar;
	switch (type)
	{
	case l3dDockWidget::null:
		break;
	case l3dDockWidget::ToolBoxObj:
		pdata = m_pDockToolBoxObj;
		TitleBar = tr("Object Library");
		break;
	case l3dDockWidget::ToolBoxMaterial:
		pdata = m_pDockToolBoxMaterial;
		TitleBar = tr("Material Library");
		break;
	case l3dDockWidget::ToolBoxProperty:
		pdata = m_pDockToolBoxProperty;
		TitleBar = tr("Property Library");
		break;
	}
	pdata->setTitleBar(TitleBar + strDirPath);

}

//> �¼�������
bool L3DArtist::eventFilter(QObject *pObject, QEvent *pEvent)
{
	if (pObject == ui.m_pMenuFile) {
		if (pEvent->type() == QEvent::Close) {
			ui.m_pMenuFile->setWindowFlags(Qt::Popup);
			m_pActionWidgetFile->setVisible(true);
			m_pActionWidgetFile->defaultWidget()->show();
			return true;
		}
		else {
			return false;
		}
	}
	else if (pObject == ui.m_pMenuEdit) {
		if (pEvent->type() == QEvent::Close) {
			ui.m_pMenuEdit->setWindowFlags(Qt::Popup);
			m_pActionWidgetEdit->setVisible(true);
			m_pActionWidgetEdit->defaultWidget()->show();
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return QMainWindow::eventFilter(pObject, pEvent);
	}
}

//> �����¼�
void L3DArtist::keyPressEvent(QKeyEvent *pEvent)
{
	HScene scene = GetCurScene();
	if (scene.isValid())
	{
		if (pEvent->key() == Qt::Key_Delete)
		{
			emit m_pPreviewWnd->SglRemove();
			return;
		}

		if ((pEvent->modifiers() & Qt::ControlModifier) && (pEvent->modifiers() & Qt::ShiftModifier))         //added by wuheng 2018.02.03
			m_eNodeCtrl = NodeCtrl::NODECTRL_INIT;

		if ((pEvent->modifiers() & Qt::ControlModifier) && (pEvent->key() == Qt::Key_S))
			SlotSaveScene();
		if (scene.isPlaying() || scene.isPause())
		{
			CtrlPointPtr ctPtrPre = scene.getCPointByPos(m_nCurPos, ECPSD_Before);
			CtrlPointPtr ctPtrNext = scene.getCPointByPos(m_nCurPos, ECPSD_After);
			CtrlPointPtr ctPtrCur = scene.getCPointByPos(m_nCurPos, ECPSD_CurPosition);
			TriggerShortCutCP(ctPtrPre, ctPtrNext, ctPtrCur, pEvent->key());
		}
		else if ((pEvent->modifiers() & Qt::AltModifier) && (pEvent->key() == Qt::Key_A))
			((DockAnimation*)m_pDockAnimation->widget())->setFrameValue(scene.getFocusedNode(), static_cast<int>(m_eNodeCtrl));
		else if ((pEvent->modifiers() & Qt::ControlModifier) && (pEvent->key() == Qt::Key_B))
			((DockAnimation*)m_pDockAnimation->widget())->RetreatOneFrame();
		else if ((pEvent->modifiers() & Qt::ControlModifier) && (pEvent->key() == Qt::Key_F))
			((DockAnimation*)m_pDockAnimation->widget())->AdvanceOneFrame();
		else if ((pEvent->modifiers() & Qt::ControlModifier) && (pEvent->key() == Qt::Key_N))
			emit((DockScene*)m_pDockScene->widget())->SglNewScene();
	}

	QMainWindow::keyPressEvent(pEvent);
}

//> �������Ƶ��ݼ�����
void L3DArtist::TriggerShortCutCP(CtrlPointPtr prePtr, CtrlPointPtr nextPtr, CtrlPointPtr curPtr, int key)
{
	E_ACP_TRIG_CMD eTrig(EACPTC_Unknown);
	switch (key) {
	case Qt::Key_F1:
		eTrig = EACPTC_F1;
		break;
	case Qt::Key_F2:
		eTrig = EACPTC_F2;
		break;
	case Qt::Key_F3:
		eTrig = EACPTC_F3;
		break;
	case Qt::Key_F4:
		eTrig = EACPTC_F4;
		break;
	case Qt::Key_F5:
		eTrig = EACPTC_F5;
		break;
	case Qt::Key_F6:
		eTrig = EACPTC_F6;
		break;
	case Qt::Key_F7:
		eTrig = EACPTC_F7;
		break;
	case Qt::Key_F8:
		eTrig = EACPTC_F8;
		break;
	case Qt::Key_F9:
		eTrig = EACPTC_F9;
		break;
	case Qt::Key_F10:
		eTrig = EACPTC_F10;
		break;
	case Qt::Key_F11:
		eTrig = EACPTC_F11;
		break;
	case Qt::Key_F12:
		eTrig = EACPTC_F12;
		break;
	default:
		break;
	}

	HScene scene = GetCurScene();
	if (!prePtr.isNull()) {
		prePtr->triggerCP(eTrig);
	}
	if (!nextPtr.isNull()) {
		scene.setCPRealTimeCmd(eTrig);
	}
	if (!curPtr.isNull()) {
		curPtr->triggerCP(eTrig);
	}
}

//> ��ʾ���ؽ�����
void L3DArtist::SlotShowProgress(bool bIsShow)
{
	if (bIsShow) {
		m_pProgress->show();
	}
	else {
		m_pProgress->hide();
	}
}

//>���Խ����ƶ�ģʽ�µ�X���ֵ����ʱ���е�һϵ�в���Ŀǰ�����ڻ���ϴν��㵥λ
void L3DArtist::setMoveFunction_X(vector3df v3df, HNode node, double dValue)
{
	bHasFocusX = /*FocusXYZ_X*/true;
	bHasFocusY = false;
	bHasFocusZ = false;
	m_nGetMoveFocusXYZ = FocusXYZ_X;
}

void L3DArtist::setMoveFunction_Y(vector3df v3df, HNode node, double dValue)
{
	bHasFocusY = /*FocusXYZ_Y*/true;
	bHasFocusX = false;
	bHasFocusZ = false;
	m_nGetMoveFocusXYZ = FocusXYZ_Y;
}

void L3DArtist::setMoveFunction_Z(vector3df v3df, HNode node, double dValue)
{
	bHasFocusZ = /*FocusXYZ_Z*/true;
	bHasFocusX = false;
	bHasFocusY = false;
	m_nGetMoveFocusXYZ = FocusXYZ_Z;
}

void L3DArtist::setRotateFunction_X(vector3df v3df, HNode node, double dValue)
{
	bHasFocusX = /*FocusXYZ_X*/true;
	bHasFocusY = false;
	bHasFocusZ = false;
	m_nGetRotateFocusXYZ = FocusXYZ_X;
}

void L3DArtist::setRotateFunction_Y(vector3df v3df, HNode node, double dValue)
{
	bHasFocusY = /*FocusXYZ_Y*/true;
	bHasFocusX = false;
	bHasFocusZ = false;
	m_nGetRotateFocusXYZ = FocusXYZ_Y;
}

void L3DArtist::setRotateFunction_Z(vector3df v3df, HNode node, double dValue)
{
	bHasFocusZ = /*FocusXYZ_Z*/true;
	bHasFocusX = false;
	bHasFocusY = false;
	m_nGetRotateFocusXYZ = FocusXYZ_Z;
}

//> ����������ģʽģʽ�µȱ����Ź���
void L3DArtist::setScaleFunction_X(vector3df v3df, HNode node, double dValue)
{
	/*������ģʽ�£���¼�ڵ��comboBox֮ǰ�������ĸ������ϣ������ڼ�¼X*/
	bHasFocusX = /*FocusXYZ_X*/true;
	bHasFocusY = false;
	bHasFocusZ = false;
	m_nGetScaleFocusXYZ = FocusXYZ_X;
	v3df = node.getScale();
	switch (m_PDComboxChoose->currentIndex())
	{
	case 0: //�ȱ���������ΪXYZ��
		node.setScale(ic::vector3df(dValue, dValue, dValue), true, true);
		break;
	case 1: //�ȱ���������ΪXY��
		node.setScale(ic::vector3df(dValue, dValue, v3df.Z), true, true);
		break;
	case 2: //�ȱ���������ΪXZ��
		node.setScale(ic::vector3df(dValue, v3df.Y, dValue), true, true);
		break;
	default:	//ȱʡʱ��Ĭ�ϲ����еȱ������ż�Ϊѡ��4��None
		node.setScale(ic::vector3df(dValue, v3df.Y, v3df.Z), true, true);
		break;
	}
}


void L3DArtist::setScaleFunction_Y(vector3df v3df, HNode node, double dValue)
{
	/*������ģʽ�£���¼�ڵ��comboBox֮ǰ�������ĸ������ϣ������ڼ�¼Y*/
	bHasFocusY = /*FocusXYZ_Y*/true;
	bHasFocusX = false;
	bHasFocusZ = false;
	m_nGetScaleFocusXYZ = FocusXYZ_Y;
	v3df = node.getScale();
	switch (m_PDComboxChoose->currentIndex())
	{
	case 0: //�ȱ���������ΪXYZ��
		node.setScale(ic::vector3df(dValue, dValue, dValue), true, true);
		break;
	case 1: //�ȱ���������ΪXY��
		node.setScale(ic::vector3df(dValue, dValue, v3df.Z), true, true);
		break;
	case 2: //�ȱ���������ΪXZ��
		node.setScale(ic::vector3df(v3df.X, dValue, v3df.Z), true, true);
		break;
	default://ȱʡʱ��Ĭ�ϲ����еȱ������ż�Ϊѡ��4��None
		node.setScale(ic::vector3df(v3df.X, dValue, v3df.Z), true, true);
		break;
	}
}


void L3DArtist::setScaleFunction_Z(vector3df v3df, HNode node, double dValue)
{
	/*������ģʽ�£���¼�ڵ��comboBox֮ǰ�������ĸ������ϣ������ڼ�¼Z*/
	bHasFocusZ = /*FocusXYZ_Z*/true;
	bHasFocusX = false;
	bHasFocusY = false;
	m_nGetScaleFocusXYZ = FocusXYZ_Z;
	v3df = node.getScale();
	switch (m_PDComboxChoose->currentIndex())
	{
	case 0: //�ȱ���������ΪXYZ��
		node.setScale(ic::vector3df(dValue, dValue, dValue), true, true);
		break;
	case 1: //�ȱ���������ΪXY��
		node.setScale(ic::vector3df(v3df.X, v3df.Y, dValue), true, true);
		break;
	case 2: //�ȱ���������ΪXZ��
		node.setScale(ic::vector3df(dValue, v3df.Y, dValue), true, true);
		break;
	default://ȱʡʱ��Ĭ�ϲ����еȱ������ż�Ϊѡ��4��None
		node.setScale(ic::vector3df(v3df.X, v3df.Y, dValue), true, true);
		break;
	}
}

//> x����
void L3DArtist::SlotValueChangeX(double dValue)    //modified by wuheng 2016.05.19 
{
	if (m_eNodeCtrl == NODECTRL_INIT)
	{
		if (ui.m_pMove->isChecked())
			m_eNodeCtrl = NODECTRL_MOVE;
		else if (ui.m_pRotate->isChecked())
			m_eNodeCtrl = NODECTRL_ROTATE;
		else if (ui.m_pZoom->isChecked())
			m_eNodeCtrl = NODECTRL_SCALE;
		else if (ui.m_pSize->isChecked())
			m_eNodeCtrl = NODECTRL_SIZE;
	}

	HScene scene = GetCurScene();
	std::vector<HNode> vecNode;
	scene.getSelectedNodes(vecNode);
	double offset = 0.0f;

	HNode node = scene.getFocusedNode();
	if (!node.isValid() || IsPlaying())
		return;
	ic::vector3df v3df;
	switch (m_eNodeCtrl) {
	case NODECTRL_MOVE:
		v3df = node.getPosition();
		node.setPosition(ic::vector3df(dValue, v3df.Y, v3df.Z), true, true);
		//> added by TaiYue 2019.6.5 for ���Խ����ƶ�ģʽ�µ�X���ֵ����ʱ���е�һϵ�в������а�װ��Ŀǰ�����ڻ���ϴν��㵥λ
		setMoveFunction_X(v3df, node, dValue);
		offset = v3df.X - dValue;
		for (auto nodeOther : vecNode)
		{
			if (node != nodeOther)
			{
				ic::vector3df v3dfOther = nodeOther.getPosition();
				nodeOther.setPosition(ic::vector3df(v3dfOther.X - offset, v3dfOther.Y, v3dfOther.Z), false, true);
			}
		}
		break;
	case NODECTRL_ROTATE:
		v3df = node.getRotation();
		node.setRotation(ic::vector3df(dValue, v3df.Y, v3df.Z), true, true);
		setRotateFunction_X(v3df, node, dValue);
		break;
	case NODECTRL_SCALE:
		v3df = node.getScale();
		//> added by TaiYue 2019.6.5
		setScaleFunction_X(v3df, node, dValue);
		//node.setScale(ic::vector3df(dValue, v3df.Y, v3df.Z), true, true);
		break;
	case NODECTRL_SIZE:
		break;
	default:
		Q_ASSERT(false);
	}
}

//> y����
void L3DArtist::SlotValueChangeY(double dValue)
{

	if (m_eNodeCtrl == NODECTRL_INIT)
	{
		if (ui.m_pMove->isChecked())
			m_eNodeCtrl = NODECTRL_MOVE;
		else if (ui.m_pRotate->isChecked())
			m_eNodeCtrl = NODECTRL_ROTATE;
		else if (ui.m_pZoom->isChecked())
			m_eNodeCtrl = NODECTRL_SCALE;
		else if (ui.m_pSize->isChecked())
			m_eNodeCtrl = NODECTRL_SIZE;
	}

	HScene scene = GetCurScene();
	std::vector<HNode> vecNode;
	scene.getSelectedNodes(vecNode);
	double offset = 0.0f;

	HNode node = scene.getFocusedNode();
	if (!node.isValid() || IsPlaying())
		return;
	ic::vector3df v3df;
	switch (m_eNodeCtrl) {
	case NODECTRL_MOVE:
		v3df = node.getPosition();
		node.setPosition(ic::vector3df(v3df.X, dValue, v3df.Z), true, true);
		//> added by TaiYue 2019.6.5
		setMoveFunction_Y(v3df, node, dValue);
		offset = v3df.Y - dValue;
		for (auto nodeOther : vecNode)
		{
			if (node != nodeOther)
			{
				ic::vector3df v3dfOther = nodeOther.getPosition();
				nodeOther.setPosition(ic::vector3df(v3dfOther.X, v3dfOther.Y - offset, v3dfOther.Z), false, true);
			}
		}
		break;
	case NODECTRL_ROTATE:
		v3df = node.getRotation();
		node.setRotation(ic::vector3df(v3df.X, dValue, v3df.Z), true, true);
		setRotateFunction_Y(v3df, node, dValue);
		break;
	case NODECTRL_SCALE:
		v3df = node.getScale();
		//> added by TaiYue 2019.6.5
		setScaleFunction_Y(v3df, node, dValue);
		//node.setScale(ic::vector3df(v3df.X, dValue, v3df.Z), true, true);
		break;
	case NODECTRL_SIZE:
		break;
	default:
		Q_ASSERT(false);
	}
}

//> z����
void L3DArtist::SlotValueChangeZ(double dValue)
{
	if (m_eNodeCtrl == NODECTRL_INIT)
	{
		if (ui.m_pMove->isChecked())
			m_eNodeCtrl = NODECTRL_MOVE;
		else if (ui.m_pRotate->isChecked())
			m_eNodeCtrl = NODECTRL_ROTATE;
		else if (ui.m_pZoom->isChecked())
			m_eNodeCtrl = NODECTRL_SCALE;
		else if (ui.m_pSize->isChecked())
			m_eNodeCtrl = NODECTRL_SIZE;
	}

	HScene scene = GetCurScene();
	std::vector<HNode> vecNode;
	scene.getSelectedNodes(vecNode);
	double offset = 0.0f;

	HNode node = scene.getFocusedNode();
	if (!node.isValid() || IsPlaying())
		return;
	ic::vector3df v3df;
	switch (m_eNodeCtrl) {
	case NODECTRL_MOVE:
		v3df = node.getPosition();
		//> added by TaiYue 2019.6.5
		node.setPosition(ic::vector3df(v3df.X, v3df.Y, dValue), true, true);
		setMoveFunction_Z(v3df, node, dValue);
		offset = v3df.Z - dValue;
		for (auto nodeOther : vecNode)
		{
			if (node != nodeOther)
			{
				ic::vector3df v3dfOther = nodeOther.getPosition();
				nodeOther.setPosition(ic::vector3df(v3dfOther.X, v3dfOther.Y, v3dfOther.Z - offset), false, true);
			}
		}
		break;
	case NODECTRL_ROTATE:
		v3df = node.getRotation();
		node.setRotation(ic::vector3df(v3df.X, v3df.Y, dValue), true, true);
		setRotateFunction_Z(v3df, node, dValue);
		break;
	case NODECTRL_SCALE:
		v3df = node.getScale();
		//> added by TaiYue 2019.6.5
		setScaleFunction_Z(v3df, node, dValue);
		//node.setScale(ic::vector3df(v3df.X, v3df.Y, dValue), true, true);
		break;
	case NODECTRL_SIZE:
		break;
	default:
		Q_ASSERT(false);
	}
}

//> �ر�֮ǰ
void L3DArtist::SlotPreClose()
{
	if (ArtistParams::instance()->isArtistEditMode())
	{
		SlotCloseSceneSimple();
		return;
	}

	int nBtn(-1);
	MSGBOX_WARNING_OKANDCANCEL(QStringLiteral("�Ƿ�ȷ���˳�"), nBtn, this);
	if (nBtn != QMessageBox::Ok)
		return;

	m_bPreClose = true;
	m_closeEvent = true;
	GetIsMaybeSave();
}

//> for xor video card test. added by CaiYL 2014.02.18
//void L3DArtist::SlotVideoSet(bool bIsCheck)
//{
//    RenderManager_.startVideoOutput(EVCT_ALL, bIsCheck);
//}

void L3DArtist::SlotUpdateCurSceneDir(const QString &strFileName)
{
	m_strSceneListPath = strFileName;
}

//> ����
void L3DArtist::TranslateLanguage()
{
	setWindowTitle(tr("AirCG.3D Branding Production"));
	ui.retranslateUi(this);
	// toolbar
	ui.m_pTBarFile->setWindowTitle(tr("Basic ToolBar"));
	ui.m_pTbarTexture->setWindowTitle(tr("Texture ToolBar"));
	//     m_pSizeBar->setWindowTitle(tr("Size ToolBar"));
	pTBtnNewScene->UpdateTooltip(ui.m_pNewScene);
	pTBtnDeleteScene->UpdateTooltip(ui.m_pDeleteScene);
	pTBtnImportScene->UpdateTooltip(ui.m_pImportScene);
	pTBtnExportScene->UpdateTooltip(ui.m_pExportScene);
	pTBtnSaveScene->UpdateTooltip(ui.m_pSaveScene);
	m_pTBtnUndo->UpdateTooltip(ui.m_pUndo);
	m_pTBtnRedo->UpdateTooltip(ui.m_pRedo);
	pTBtnCut->UpdateTooltip(ui.m_pCut);
	pTBtnCopy->UpdateTooltip(ui.m_pCopy);
	pTBtnPaste->UpdateTooltip(ui.m_pPaste);
	pTBtnMove->UpdateTooltip(ui.m_pMove);
	pTBtnRotate->UpdateTooltip(ui.m_pRotate);
    pTBtnVAlign->UpdateTooltip(m_pTBtnVAlign);
    pTBtnHAlign->UpdateTooltip(m_pTBtnHAlign);
	pTBtnZoom->UpdateTooltip(ui.m_pZoom);
	pTBtnSize->UpdateTooltip(ui.m_pSize);
	pTBtnOutput->UpdateTooltip(ui.m_pOutput);
	pTBtnMultiView->UpdateTooltip(ui.m_pMultiView);
	pTBtnAbout->UpdateTooltip(ui.m_pAbout);

	m_pTBtnPre->setToolTip(tr("Begin"));
	m_pTBtnBack->setToolTip(tr("back"));
	m_pTBtnPlay->setToolTip(tr("Play"));
	m_pTBtnForward->setToolTip(tr("Forward"));
	m_pTBtnNext->setToolTip(tr("End"));
	m_pTBtnLoop->setToolTip(tr("Loop"));

	m_pLblStatus->setText(tr("Reday"));
	m_pLblProject->setText(tr("Scene Directory:"));
	m_pLableScene->setText(tr("Scene Name:"));
	if (!GetCurScene().isValid())
		ResetCurSceneDirectory();
	m_pTBtnReset->setToolTip(tr("Reset the value of the object to the default values."));
	m_pTBtnFullScreen->setToolTip(tr("Full Screen(Ctrl+F11)"));

	m_pDockPropertyEdit->setWindowTitle(tr("Parameter Manager"));
	((DockPropertyEditor*)m_pDockPropertyEdit->widget())->TranslateLanguage();
	m_pDockScene->setWindowTitle(tr("Scene List"));
	((DockScene*)m_pDockScene->widget())->TranslateLanguage();
	m_pDockAnimation->setWindowTitle(tr("Timeline"));
	((DockAnimation*)m_pDockAnimation->widget())->TranslateLanguage();
	m_pDockSceneTree->setWindowTitle(tr("Node Tree"));
	((SceneTree*)m_pDockSceneTree->widget())->TranslateLanguage();
	m_pDockToolBoxObj->setWindowTitle(tr("Object Library"));
	((DockToolBoxObject*)m_pDockToolBoxObj->widget())->TranslateLanguage();
	m_pDockToolBoxMaterial->setWindowTitle(tr("Media Library"));
	((DockToolBoxMaterial*)m_pDockToolBoxMaterial->widget())->TranslateLanguage();
	m_pDockToolBoxProperty->setWindowTitle(tr("Property Library"));
	((DockToolBoxProperty*)m_pDockToolBoxProperty->widget())->TranslateLanguage();
	m_pPreviewWnd->TranslateLanguage();

#ifndef FOR_XOR_CO_VER
	ui.actionFtpSetting->setText(tr("Upload Params"));
#endif
}

void L3DArtist::setOwner(QtSingleApplication* pOwner)
{
	m_pOwner = pOwner;
}

bool L3DArtist::checkConfict(QMap<QString, QStringList>& files, QString suff, QString dir)
{
	if (dir.isEmpty())
		dir = GetCurProjectPath() + "/scenes";

	QDir curDir(dir);
	QFileInfoList allFiles = curDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

	bool hasConfictFile(false);
	QString proPath = GetCurProjectPath();
	for (auto file : allFiles)
	{
		if (file.isFile() && file.suffix().toLower() == suff.toLower())
		{
			if (!files.contains(file.fileName()))
			{
				QString tmpPath = file.absoluteFilePath();
				files.insert(file.fileName(), QStringList() << tmpPath.right(tmpPath.length() - proPath.length()));
			}
			else
			{
				QStringList filePathList = files[file.fileName()];
				QString tmpPath = file.absoluteFilePath();
				filePathList << tmpPath.right(tmpPath.length() - proPath.length());
				files[file.fileName()] = filePathList;
				hasConfictFile = true;
			}
		}
		else
		{
			if (file.fileName() == "." || file.fileName() == "..")
				continue;

			if (checkConfict(files, suff, file.absoluteFilePath()))
				hasConfictFile = true;
		}
	}

	return hasConfictFile;
}

QString L3DArtist::checkfileConfict(const QString& initFileName, QString dir)
{
	if (dir.isEmpty())
		dir = GetCurProjectPath() + "/scenes";

	QDir curDir(dir);
	QFileInfoList allFiles = curDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

	for (auto file : allFiles)
	{
		if (file.isFile() && file.fileName() == initFileName)
		{
			return file.filePath();
		}
		else
		{
			if (file.fileName() == "." || file.fileName() == "..")
				continue;

			QString confictFile = checkfileConfict(initFileName, file.absoluteFilePath());
			if (!confictFile.isEmpty())
				return confictFile;
		}
	}

	return QString();
}

bool L3DArtist::GetTanslateStatus()
{
	return m_bTranslate;
}

void L3DArtist::SlotRemoveScene(QString strFileName)
{
	//QDir dir("./");
	//QString tempstr = strFileName;
	//tempstr.replace(0, dir.absolutePath().size(), ".");
	//ScenePathMrg_.deleteScene(tempstr.s());
	//tstring errormsg;
	//ScenePathMrg_.deleteInfo(tempstr.tQtToStdString(),errormsg);
}

void L3DArtist::VideoTextureChange()
{
	emit SglVideoTextureChanged();
}

void L3DArtist::SlotOutputVideo()
{
	if (!GetCurScene().isValid())
	{
		MSGBOX_ERROR_TIP(tr("No open scene"), this);
		return;
	}
	
	long length = GetCurScene().getSceneTotalLength();
	OutputVideo* video = new OutputVideo(m_pPreviewWnd,this);
	video->Init(length);
	connect(video, &OutputVideo::signalStart, this, &L3DArtist::SlotStartVOS);
	connect(video, &OutputVideo::signalStop, this, &L3DArtist::SlotStopVOS);
	connect(this, &L3DArtist::SglVOS_Start, video, &OutputVideo::SlotVOS_Start);
	connect(this, &L3DArtist::SglVOS_End, video, &OutputVideo::SlotVOS_End);
	connect(this, &L3DArtist::SglVOS_Process, video, &OutputVideo::SlotVOS_Process);
	video->show();
}

void L3DArtist::SlotStartVOS(int type, QString path, bool field, long length)
{
	if (m_bRecording)
		return;

	if (path.isEmpty())
	{
		MSGBOX_ERROR_TIP(tr("invalid file path"), this);
		return;
	}
	// 	QDir dir(path);
	// 	if (type == 0 && dir.exists())
	// 	{
	// 		int nBtn(-1);
	// 		MSGBOX_WARNING_OKANDCANCEL( tr("File has already exists, Are you sure to replace it"), nBtn, this );
	// 		if (nBtn != QMessageBox::Ok)
	// 			return;
	// 	}
	// 	else if(type == 1 && !dir.exists())
	// 	{
	// 		int nBtn(-1);
	// 		MSGBOX_WARNING_OKANDCANCEL( tr("File has not exists, Are you sure to create it"), nBtn, this );
	// 		if (nBtn != QMessageBox::Ok)
	// 			return;
	// 		else
	// 			if(!dir.mkdir(path))
	// 				return;
	// 	}
	// 	E_VO_SAVE_FILE_TYPE saveType = EVOSFT_AVI;
	// 	if(type == 0)
	E_VO_SAVE_FILE_TYPE saveType = EVOSFT_TGA;

	RenderManager_.startSaveTGAToFile(saveType, path.tQtToStdString(), length, field);
}

void L3DArtist::SlotStopVOS()
{
	if (!m_bRecording)
		return;

	RenderManager_.endtSaveTGAToFile();
}

void L3DArtist::CreateSolidColor()
{
	HNode hN = GetCurFocusNode();
	if (!hN.isValid())
		return;
	int nIndex = hN.getCuTexIndex();
	AppTexturePtr atp;
	if (nIndex > 0)
		atp = hN.getAppTexture(nIndex - 1);
	else
		atp = hN.getAppTexture(0);

	if (atp)
	{
		TexturePtr baseTex = atp->getBaseTexture();
		if (baseTex)
		{
			SBaseTexIDPMono* pMono = new SBaseTexIDPMono();
			BaseTexIDPPtr idp = BaseTexIDPPtr(pMono);
			baseTex->getImageDataParams(idp);
			QColor initial;
			initial.setRgb(pMono->m_Color.GetR(), pMono->m_Color.GetG(), pMono->m_Color.GetB(), pMono->m_Color.GetAlpha());
			QColorDialog* cd = new QColorDialog(initial, this);
			cd->setOption(QColorDialog::ShowAlphaChannel);
			if (cd->exec() == QDialog::Accepted)
			{
				const QColor newColor = cd->selectedColor();
				pMono->m_Color.SetValue(Gdiplus::Color::MakeARGB(newColor.alpha(), newColor.red(), newColor.green(), newColor.blue()));
				hN.createTexture(E_BTex_Image_Type::EBTIT_SOLID, idp);
			}
			delete cd;
		}
	}
}

void L3DArtist::CreateExtMaterial()
{
	HNode node = GetCurFocusNode();
	if (!node.isValid())
	{
		MSGBOX_WARNING_TIP(tr("Focuse node is invalid!"), this);
		return;
	}

    std::vector<SMaterial> vecMat;
    std::vector<AppTexturePtr> vecTex;
    std::map<AppTexturePtr, int> mapTexToMat;
    std::map<AppTexturePtr, int> mapTexToTex;
    uint curMeshIndex = node.getCurMaterialIndex();
	int count = node.getMaterialCount();
	// ����Ϊ0-ȫ����1-ǰ�棬2-����
	bool bAllMesh = curMeshIndex == 0 ? true : false;

    if (curMeshIndex > 0)
        curMeshIndex = curMeshIndex - 1; // MeshIdx��Ϊ0����
    else
        curMeshIndex = 0;

	SMaterial mat = node.getMaterial(curMeshIndex);
    vecMat.push_back(mat);

	// �������֧��4�㣨1��Ĭ�ϲ��ʣ�3����չ���ʣ���0��ΪĬ�ϲ��ʣ���ÿ��������֧��4������
    if (mat.MaterialType == EMT_GLSL_FOURTEXTURE_BLEND) 
	{
        for (int idx = 1; idx < MaxMatExtSize; idx++)
		{
            SMaterial pMat;
            bool rslt = node.getMaterialExt(curMeshIndex, idx, pMat);
            if (rslt) 
			{
                vecMat.push_back(pMat);
            }
        }

       /* for (int mi = 0; mi <= MaxMatExtSize; mi++) 
		{
            for (int ti = 0; ti < MATERIAL_MAX_TEXTURES; ti++)
			{
                AppTexturePtr appTex;
                appTex = node.getAppTextureExt(curMeshIndex, mi, ti);
                if (!appTex.isNull()) 
				{
                    vecTex.push_back(appTex);
                    mapTexToMat[appTex] = mi;
                    mapTexToTex[appTex] = ti;
                }
            }
        }*/
    }

	std::vector<Layer> layers;
    for (int matIdx = 0; matIdx < vecMat.size(); matIdx++)
	{
		Layer tmpL;
        tmpL.alpha = vecMat[matIdx].Alpha;
        tmpL.ambient = QColor(vecMat[matIdx].AmbientColor.getRed(), vecMat[matIdx].AmbientColor.getGreen(), vecMat[matIdx].AmbientColor.getBlue());
        tmpL.diffuse = QColor(vecMat[matIdx].DiffuseColor.getRed(), vecMat[matIdx].DiffuseColor.getGreen(), vecMat[matIdx].DiffuseColor.getBlue());
        tmpL.emission = QColor(vecMat[matIdx].EmissiveColor.getRed(), vecMat[matIdx].EmissiveColor.getGreen(), vecMat[matIdx].EmissiveColor.getBlue());
        tmpL.enable = true;
        tmpL.specular = QColor(vecMat[matIdx].SpecularColor.getRed(), vecMat[matIdx].SpecularColor.getGreen(), vecMat[matIdx].SpecularColor.getBlue());
        tmpL.enable = true;
        

		for (int texIdx = 0; texIdx < 4; texIdx++)
		{
			SMaterialLayer texLayer = vecMat[matIdx].TextureLayer[texIdx];
			if (!texLayer.Texture)
				continue;

			Texture tmpT;
			tmpT.mode_alpha = texLayer.AlphaMixedMode;
			tmpT.mixValue = texLayer.MixValue;
			tmpT.mode = texLayer.MixedMode;
			tmpT.type = texLayer.AppType;
			tmpT.mixValue_alpha = texLayer.MixValueAlpha;
			tmpT.tex = QString::fromStdWString(texLayer.TexturePath);
			tmpT.enable = true;
			tmpL.textures.push_back(tmpT);
		}

		layers.push_back(tmpL);
    }

	MaterialEditor edit;
	edit.init(layers);
	edit.setStyleSheet("QListWidget { border: 1px solid rgb(53, 53, 54); }\
                        QListWidget::item:selected { background: rgb(0, 111, 185); }");

	if (edit.exec() != QDialog::Accepted)
		return;

    // ������е���չ��������
    for (int i = 0; i < MaxMatExtSize; i++)
	{
		for (int j = 0; j < MATERIAL_MAX_TEXTURES; j++)
		{
            auto ptr = node.getAppTextureExt(curMeshIndex, i, j);
			if (!ptr)
				continue;

			node.removeAppTextureExt(curMeshIndex, i, j);
		}
    }

    for (int i = 0; i < MaxMatExtSize; i++)
	{
        node.removeMaterialExt(curMeshIndex, i);
    }

    std::vector<Layer> newLayers = edit.getLayers();

	//for (int meshIdx = curMeshIndex; meshIdx < (bAllMesh ? count : curMeshIndex); meshIdx++)
	{
		for (int i = 0; i < newLayers.size(); i++)
		{
			SMaterial newMat;
			newMat.Alpha = newLayers[i].alpha;
			newMat.AmbientColor = SColor(255, newLayers[i].ambient.red(), newLayers[i].ambient.green(), newLayers[i].ambient.blue());
			newMat.DiffuseColor = SColor(newMat.Alpha, newLayers[i].diffuse.red(), newLayers[i].diffuse.green(), newLayers[i].diffuse.blue());
			newMat.EmissiveColor = SColor(255, newLayers[i].emission.red(), newLayers[i].emission.green(), newLayers[i].emission.blue());
			newMat.SpecularColor = SColor(255, newLayers[i].specular.red(), newLayers[i].specular.green(), newLayers[i].specular.blue());

			auto textures = newLayers[i].textures;

			for (int j = 0; j < textures.size(); j++)
			{
				auto tex = textures[j];
				newMat.TextureLayer[j].AppType = (E_TEX_APP_TYPE)tex.type;
				newMat.TextureLayer[j].AlphaMixedMode = (E_TEX_BLEND_MODE)tex.mode_alpha;
				newMat.TextureLayer[j].MixedMode = (E_TEX_BLEND_MODE)tex.mode;
				newMat.TextureLayer[j].MixValue = tex.mixValue;
				newMat.TextureLayer[j].MixValueAlpha = tex.mixValue_alpha;
				newMat.TextureLayer[j].TexturePath = tex.tex.toStdWString();
				node.setAppTextureExt(tex.tex.toStdWString(), bAllMesh ? -1 : curMeshIndex, i, j);
			}

			node.setMaterialExt(newMat, bAllMesh ? -1 : curMeshIndex, i);
		}
	}
    
}

void L3DArtist::TexEffectTimeChange(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength)
{
    if (!hNode.isValid())
        return;
    if (animatorType != EAT_TexEffInput && animatorType != EAT_TexEffOutput)
        return;
    if (nTimeLength < 0)
        nTimeLength = 0;

    emit SglTexEffTimeChange(hNode, animatorType, nTimeLength);
}
void L3DArtist::UpdateTexEffectTime(HNode hNode, E_ANIMATOR_TYPE animatorType)
{
    if (!hNode.isValid())
        return;
    if (animatorType != EAT_TexEffInput && animatorType != EAT_TexEffOutput)
        return;

    emit SglUpdateTexEffTime(hNode, animatorType);
}
void L3DArtist::DeleteTexEffect(HNode hNode, E_ANIMATOR_TYPE animatorType)
{
    if (animatorType != EAT_TexEffInput &&
        animatorType != EAT_TexEffOutput)
        return;

    emit SglDeleteTexEffect(hNode, animatorType);
}

void L3DArtist::CreateGradient()
{
	HNode hN = GetCurFocusNode();
	if (!hN.isValid())
		return;
	int nIndex = hN.getCuTexIndex();
	AppTexturePtr atp;
	if (nIndex > 0)
		atp = hN.getAppTexture(nIndex - 1);
	else
		atp = hN.getAppTexture(0);

	if (atp)
	{
		TexturePtr baseTex(atp->getBaseTexture()->clone());
		if (baseTex)
		{
			GradientDlg dlg(baseTex, this);
			if (QDialog::Accepted != dlg.exec())
				return;

			SBaseTexIDPGradient* pMono = dlg.getGradientParam();
			BaseTexIDPPtr idp = BaseTexIDPPtr(pMono);
			hN.createTexture(E_BTex_Image_Type::EBTIT_GRADIENT, idp);
		}
	}
}

void L3DArtist::CreatelLightPass()
{
	HNode hN = GetCurFocusNode();
	if (!hN.isValid())
		return;

	LightPassPtr lightPass = hN.getLightPass();
	if (!lightPass)
	{
		QMessageBox::information(this, tr("propmt"), tr("please enable light pass function first."));
		return;
	}
	AppTexturePtr tex = lightPass->getTexture();
	if (!tex)
	{
		QMessageBox::information(this, tr("propmt"), tr("light pass texture is null."));
		return;
	}
	TexturePtr baseTex = tex->getBaseTexture();
	if (!baseTex)
	{
		QMessageBox::information(this, tr("propmt"), tr("light pass base texture is null."));
		return;
	}

	if (baseTex->getImageType() != EBTIT_GRADIENT)
	{
		QMessageBox::information(this, tr("propmt"), tr("light pass base texture must be gradient(defalut is gradient)."));
		return;
	}

	GradientDlg dlg(baseTex, this);
	if (QDialog::Accepted != dlg.exec())
		return;

	SBaseTexIDPGradient* pMono = dlg.getGradientParam();
	BaseTexIDPPtr idp = BaseTexIDPPtr(pMono);
	lightPass->setTexture(idp);
	//hN.createTexture(E_BTex_Image_Type::EBTIT_GRADIENT, idp);
}

QString L3DArtist::CreateBevel(QString selectedType)
{
	HNode hN = GetCurFocusNode();
	if (!hN.isValid())
		return "";

	QString type;
	vector<SPolygonPoint> pathPoints;
	hN.getBevelPathPointParam(pathPoints);

	BevelSet set(this);
	set.setSelectType(selectedType); // ����ѡ������
	set.setPathPoints(pathPoints); // ����·����
	set.exec();

	set.getSelectType(type);
	set.getPathPoints(pathPoints);
	if (!type.isEmpty())
	{
		hN.setBevelParam(set.getProfiles(), type.toStdWString());
		hN.setBevelPathPointParam(pathPoints);
	}
	set.deleteLater();
	return type;
}

void L3DArtist::CreatePolygon()
{
	HNode hN = GetCurFocusNode();
	if (!hN.isValid())
		return;

	ShapePoints polygons;
	hN.getNodeEPInterface()->GetExdProterty(gszBsmCategroyNames[7], gszPolygonCustomPropNames[1], polygons);
	for (auto& t : polygons)
	{
		t.m_Pos.X /= 144;
		t.m_Pos.Y /= 80;
		t.m_LeftCtl.X /= 144;
		t.m_LeftCtl.Y /= 80;
		t.m_RightCtl.X /= 144;
		t.m_RightCtl.Y /= 80;
	}

	QtPainter paint(this);
	paint.setPloygonsPoints(polygons);
	int result = paint.exec();
	if (result == QDialog::Accepted)
	{
		polygons = paint.getPloygonsPoints();
		for (auto& t : polygons)
		{
			t.m_Pos.X *= 144;
			t.m_Pos.Y *= 80;
			t.m_LeftCtl.X *= 144;
			t.m_LeftCtl.Y *= 80;
			t.m_RightCtl.X *= 144;
			t.m_RightCtl.Y *= 80;
		}
		hN.getNodeEPInterface()->SetExdProterty(gszBsmCategroyNames[7], gszPolygonCustomPropNames[1], polygons);
	}
}

void L3DArtist::SlotProjectSet()
{
	m_isResetProject = false;

	QProjectSetDlg dlg(ArtistParams::instance()->ProjectRootPath(), m_strProjectName, this);
	//dlg.exec();
	if (dlg.exec() == QDialog::Accepted)
	{
		if (m_strProjectName == dlg.project() && ArtistParams::instance()->ProjectRootPath() == dlg.root().replace('/', '\\'))
			return;

		// ����ǰ�г�����,����Ҫ�رյ�ǰ����
		if (CurOpenScene_.isValid())
		{
			int btn;
			MSGBOX_WARNING_OKANDCANCEL(tr("Scene file is open, do you want to close scene?"), btn, this);
			if (btn == QMessageBox::Ok)
			{
				m_isResetProject = true;
				SlotCloseScene();
			}
			else
				return;
		}
		else
		{
			m_isResetProject = true;
			emit sglsetProject(dlg.project());
		}

		m_strProjectName = dlg.project();
		m_strProjectPath = dlg.root() + "/" + dlg.project();

		ArtistParams::instance()->ProjectRootPath(dlg.root());
		ArtistParams::instance()->LocalProjectID(dlg.projectId());
	}
}

// modify by xyc 2015.3.24 FTP������Ϊ����Ƶ����Ϣ
void L3DArtist::SlotFtpSetting()
{
	QString ip;
	int port = 21;
	QString usr;
	QString pwd;
	QString channName;
	QString tempProname;
	SChannelNoCenter channel;
	int id; //Ƶ��ID��
	S3DProject project;
#ifdef FOR_XOR_CO_VER
	ArtistParams::instance()->getCurFtp_Xor(channel);
	ip = tQtFromStdString(channel.ip_);
	port = channel.port_;
	usr = tQtFromStdString(channel.userName_);
	pwd = tQtFromStdString(channel.userPwd_);
	channName = tQtFromStdString(channel.channelName_);
	id = channel.channelID_;
	project = channel.project_;
	ChangeFtpDlg dlg(id, ip, port, usr, pwd, channName, project, this);
	dlg.exec();
#else
	if (ArtistParams::instance()->isLocalUpload()/* || ArtistParams::instance()->isStudio() */)
	{
		ChangeFtpDlg dlg(this, tQtFromStdString(L3DSystemConfig::instance()->getFtpPath()));
		dlg.setWindowTitle(QStringLiteral("�����ϴ�·��"));
		dlg.exec();
	}
	else if (ArtistParams::instance()->isStudio())
	{
		setstudioplay ssd(this, tQtFromStdString(L3DSystemConfig::instance()->getFtpPath()));
		ssd.exec();
	}
	else if (dataSerInfoVec && !dataSerInfoVec->empty())
	{
		ip = tQtFromStdString(dataSerInfoVec->begin()->svr_ip_);
		usr = tQtFromStdString(dataSerInfoVec->begin()->svr_ftp_name_);
		pwd = tQtFromStdString(dataSerInfoVec->begin()->svr_ftp_pwd_);
		QString strPort = tQtFromStdString(dataSerInfoVec->begin()->svr_ftp_port_);
		port = strPort.toInt();
		channName = tQtFromStdString(dataSerInfoVec->begin()->svr_name_);
		id = dataSerInfoVec->begin()->svr_id_;
		ChangeFtpDlg dlg(id, ip, port, usr, pwd, channName, this);
		dlg.exec();
	}
#endif
}

void L3DArtist::SetFocusDockWidget(l3dDockWidget* dw)
{
	for (auto ptr = m_lstDockWidget.begin(); ptr != m_lstDockWidget.end(); ++ptr)
	{
		if ((*ptr) == dw)
		{
			if (!dw->isSelected())
			{
				//added by xyc 2014.07.16 �õ�������ʧȥ����ʱ�ı������
				if ((*ptr)->_titleWidget != nullptr && (*ptr)->titleBarWidget() != NULL)
				{
					(*ptr)->_titleWidget->setStyleSheet(DockToolFocusStyleSheet);
				}
				else
				{
					(*ptr)->setStyleSheet(DockWidgetFocusStyleSheet);
				}
				dw->Select(true);
			}
		}
		else
		{
			if ((*ptr)->isSelected())
			{
				if ((*ptr)->_titleWidget != nullptr && (*ptr)->titleBarWidget() != NULL) {
					(*ptr)->_titleWidget->setStyleSheet(DockToolUnfocusStyleSheet);
				}
				else
					(*ptr)->setStyleSheet(DockWidgetUnfocusStyleSheet);

				(*ptr)->Select(false);
			}
		}
	}
}

void L3DArtist::SlotBtnResetOnClick()
{
	//HNode hn = GetCurFocusNode();
	std::vector<HNode> nodeVec;
	GetCurScene().getSelectedNodes(nodeVec);
	auto iter = nodeVec.begin();
	bool breset = false;
	for (; iter != nodeVec.end(); iter++)
	{
		if ((*iter).isValid())
		{
			switch (m_eNodeCtrl)
			{
			case L3DArtist::NODECTRL_INIT:
				(*iter).setPosition(irr::core::vector3df(0, 0, 0), true, true);
				(*iter).setRotation(irr::core::vector3df(0, 0, 0), true, true);
				(*iter).setScale(irr::core::vector3df(1, 1, 1), true, true);
				breset = true;
				break;
			case L3DArtist::NODECTRL_MOVE:
				(*iter).setPosition(irr::core::vector3df(0, 0, 0), true, true);
				break;
			case L3DArtist::NODECTRL_ROTATE:
				(*iter).setRotation(irr::core::vector3df(0, 0, 0), true, true);
				break;
			case L3DArtist::NODECTRL_SCALE:
				(*iter).setScale(irr::core::vector3df(1, 1, 1), true, true);
				break;
			case L3DArtist::NODECTRL_SIZE:
			default:
				break;
			}
		}
	}

	if (breset)
	{
		if (ui.m_pMove->isChecked())
			m_eNodeCtrl = NODECTRL_MOVE;
		else if (ui.m_pRotate->isChecked())
			m_eNodeCtrl = NODECTRL_ROTATE;
		else if (ui.m_pZoom->isChecked())
			m_eNodeCtrl = NODECTRL_SCALE;
		else if (ui.m_pSize->isChecked())
			m_eNodeCtrl = NODECTRL_SIZE;
	}
}

void L3DArtist::SlotBtnLoopOnCli()
{

}

void L3DArtist::SlotBtnPreOnCli()
{
	if (CurOpenScene_.isValid())
	{
		CurOpenScene_.stopScene(false);
		CurOpenScene_.seekScene(0);
		m_hasPlayed = false;
		emit SglUpdateCursorPos(0);
		m_nCurPos = 0;
	}
}

void L3DArtist::SlotBtnBackOnCli()
{
	if (CurOpenScene_.isValid())
	{
		CurOpenScene_.seekScene(m_nCurPos - 1);
		emit SglUpdateCursorPos(m_nCurPos - 1);
		m_nCurPos--;
	}
}

void L3DArtist::SlotBtnForwardOnCli()
{
	if (CurOpenScene_.isValid())
	{
		CurOpenScene_.seekScene(m_nCurPos + 1);
		emit SglUpdateCursorPos(m_nCurPos + 1);
		m_nCurPos++;
	}
}

void L3DArtist::SlotBtnNextOnCli()
{
	if (CurOpenScene_.isValid()) {
		int length = CurOpenScene_.getSceneTotalLength();
		CurOpenScene_.seekScene(length);
		emit SglUpdateCursorPos(length);
		m_nCurPos = length;
	}
}

void L3DArtist::RulerMoved(long pos)
{
	m_nCurPos = pos;
}

void L3DArtist::RenameSceneInEdit(QString qstr)
{
	m_pLblStatusScene->SetText(qstr, 500);
}

bool L3DArtist::IsPlaying()
{
	return CurOpenScene_.isPlaying();
}

void L3DArtist::OpenSceneTemplete(QString qstr)
{
	if (SceneManager_.openSceneTemplate(qstr.tQtToStdString()) < EKRSLT_S_OK)
	{
		MSGBOX_ERROR_TIP(tr("Import scene templete failed"), this);
		return;
	}
}

void L3DArtist::UpdateNodeAnimator(HNode node)
{
	emit SglUpdataNodeAni(node);
}

QString L3DArtist::getLocalLangue()
{
	if (localLangue_.isEmpty())
		return "chs";
	else
		return localLangue_;
}

void L3DArtist::ResetCurSceneDirectory()
{
	m_pLblStatusProject->SetText(m_strProjectName, 500);
	m_pLblStatusScene->SetText(tr("None"), 500);
}

void L3DArtist::SetCurSceneDirectory(const QString& scenePath)
{
	m_pLblStatusProject->SetText(m_strProjectName + '/' + m_strSceneListPath, 500);
	if (scenePath.isEmpty())
		m_pLblStatusScene->SetText(tQtFromStdString(GetCurScene().getName()), 500);
	else
	{
		QFileInfo scenePathInfo(scenePath);
		m_pLblStatusScene->SetText(scenePathInfo.completeBaseName());
	}
}

//> ȫѡ
void L3DArtist::SlotAllSelected()
{

}

//> ���ѡ��
void L3DArtist::SlotClearSelected()
{
	vector<HNode> vecNode;
	GetCurScene().getSelectedNodes(vecNode);
	for (auto node : vecNode)
	{
		GetCurScene().setSelectedNode(node, true);//��������ص�
	}
	SlotClearSpinBox();
	emit SglClearTimeLine();
	emit SglClearTexurePartToolBar();
	//     emit SglClearSizeToolBar();
	emit SglClearProperty();
	emit SglPropertyEnable(false);  // ʹͣ�������ж�����������չ�����е����Դ��ڲ��ɱ༭״̬ added by yejia 2014-12-09
	setSomeBtnDisable();
}

//> ����Ⱦ�����нڵ㱻���
void L3DArtist::SlotNodeClicked(const tstring& strNodeId, const ENUM_ITEM_TYPE& itemType, const int& itemIndex)
{
	if (!strNodeId.empty())
	{
		HNode node = GetCurScene().getNodeByGUID(strNodeId);
		if (!node.isValid()) return;
		GetCurScene().setSelectedNode(node, false);//��������ص�,����Ϊ����
		emit SglUpdateTexurePartToolBar(strNodeId);
		//         emit SglUpdateSizeToolBar(strNodeId);
		emit SglTimeLineSelectNode(strNodeId, true);
		emit SglUpdateProperty(strNodeId, itemType, itemIndex);
		emit SglPropertyEnable(true);  // ʹͣ�������ж�����������չ�����е����Դ��ڿɱ༭״̬ added by yejia 2014-12-09

									   //ѡ�нڵ�ʱ,Ӧ���յ�ǰ����ģʽ����Ⱦ�����µĲ���������ʾ��ȷ�Ĳ���  modified by wuheng 2017.12.13
		ic::vector3df point(0.0f, 0.0f, 0.0f);
		switch (m_eNodeCtrl)
		{
		case NODECTRL_MOVE:
			point = node.getPosition();
			break;
		case NODECTRL_ROTATE:
			point = node.getRotation();
			break;
		case NODECTRL_SCALE:
			point = node.getScale();
			break;
		default:
			break;
		}

		SlotUpdateToolBarXYZ(point.X, point.Y, point.Z, m_eNodeCtrl);
		setSomeBtnEntable();

		HNode parentNode = node.getParent();
		if (parentNode.isValid() && parentNode.getType() == ELNT_FmtScrlScrn && node.getName() == gszFmtScrlChildNodeNames[3])
			SlotBtnNextOnCli();
	}

}

// ��ʱˢ�½ڵ��λ�á����š���ת��x,y,z�Ȳ���
void L3DArtist::SlotUpdNodeTimer()
{
	if (!CurOpenScene_.isValid())
		return;

	HNode node = CurOpenScene_.getFocusedNode();
	if (!node.isValid() || node.getType() != ELNT_Camera)
		return;

	// ѡ�нڵ�ʱ,Ӧ���յ�ǰ����ģʽ����Ⱦ�����µĲ���������ʾ��ȷ�Ĳ���  modified by wuheng 2017.12.13
	ic::vector3df point(0.0f, 0.0f, 0.0f);
	switch (m_eNodeCtrl)
	{
	case NODECTRL_MOVE:
		point = node.getPosition();
		break;
	case NODECTRL_ROTATE:
		point = node.getRotation();
		break;
	case NODECTRL_SCALE:
		point = node.getScale();
		break;
	default:
		break;
	}

	SlotUpdateToolBarXYZ(point.X, point.Y, point.Z, m_eNodeCtrl);
}

//> ����ѡ��ʱ�ڵ��ȡ����
void L3DArtist::SlotNodeFocused(const tstring& strNodeId)
{
	if (!strNodeId.empty())
	{
		HNode node = GetCurScene().getNodeByGUID(strNodeId);
		if (!node.isValid()) return;
		GetCurScene().setSelectedNode(node, false);//��������ص�,����Ϊ����
	}
}

//> ����Ⱦ�����нڵ㱻��ѡ
void L3DArtist::SlotNodeLosed(const tstring& strNodeId)
{
	if (!strNodeId.empty())
	{
		HNode node = GetCurScene().getNodeByGUID(strNodeId);
		GetCurScene().setSelectedNode(node, true);//��������ص�
		emit SglTimeLineSelectNode(strNodeId, false);
	}
}

//>
void L3DArtist::SlotInsertNoded(const tstring& strParentNodeId, const tstring& strCurNodeId)
{}

//>
void L3DArtist::SlotRemoveNoded(const tstring& strNodeId, const tstring& strNodeName)
{}

//>
void L3DArtist::SlotUpdateNodeNamed(const tstring& strNodeId, const tstring& strNodeName)
{}

//> �ɱ༭״̬�ı�
void L3DArtist::SlotUpdateNodeEdabled(const tstring& strNodeId, bool bEditable)
{
	//����ʱ����ͳһ�޸�
	//HNode node = GetCurScene().getNodeByGUID( strNodeId );
	//if ( !node.isValid() ) 
	//	return;
	//node.setCanEdit(bEditable);
}

//> ����״̬�ı�
void L3DArtist::SlotUpdateNodeVisibled(const tstring& strNodeId, bool bVisible)
{
	//����ʱ����ͳһ�޸�
	//HNode node = GetCurScene().getNodeByGUID( strNodeId );
	//if ( !node.isValid() ) 
	//	return;
	//node.setVisible(bVisible);
}

//> ����״̬�ı�
void L3DArtist::SlotUpdateNodeLocked(const tstring& strNodeId, bool bLock)
{
	//����ʱ����ͳһ�޸�
	//HNode node = GetCurScene().getNodeByGUID( strNodeId );
	//if ( !node.isValid() ) 
	//	return;
	//node.setLock(bLock);
}

//> �ڵ����
void L3DArtist::SlotMoveGrouped()
{
	GetCurScene().addGroup();
}

//> �ڵ����
void L3DArtist::SlotReleaseGrouped(HNode node)
{
	if (node.isValid())
		GetCurScene().releaseGroup(node);
}

//> �ڵ����
void L3DArtist::SlotCuted(HNode node)
{
	CAsynOperatingBassPtr pdata = CAsynOperatingBass::CreatOperating(CAsynOperatingBass::OperatingEnum_CutNode);
	setOperating(pdata);
	node.serializeXML();
	GetCurScene().deleteNode(node);
}

//> �ڵ㸴��
void L3DArtist::SlotCopied(HNode node)
{
	if (node.isValid())
		node.serializeXML();
}

//> �ڵ�ճ��
void L3DArtist::SlotPasted(const tstring nodeXML, HNode node)
{
	if (!nodeXML.empty())
		GetCurScene().createNode(nodeXML, node, true);
}

//> �ڵ�ɾ��
void L3DArtist::SlotRemoved(HNode node)
{
	if (node.isValid())
		GetCurScene().deleteNode(node);
}

//> �ڵ�������
void L3DArtist::SlotRenamed(QString strName, HNode node)
{
	if (node.isValid() && !strName.isEmpty())
		node.setName(strName.tQtToStdString());
}

//> �ڵ����
void L3DArtist::SlotAdded(E_L3DNODE_TYPE type, void* pdata, HNode node)
{
	if (type == E_L3DNODE_TYPE::ELNT_ExternModel_Group)           //������ⲿģ��
	{
		QString strFileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), ("3DModel files (*.3DS;*.obj;*.x)"));
		if (!strFileName.isEmpty())
			strFileName.replace("/", "\\");
		tstring filename = strFileName.toStdWString();
		if (filename.empty())
			return;
		GetCurScene().createNode(type, filename, node);
		return;
	}

	if (type != E_L3DNODE_TYPE::ELNT_Unknown)
		GetCurScene().createNode(type, pdata, node);
}

//> �ڵ�����
void L3DArtist::SlotMoveUped(HNode node)
{
	if (node.isValid())
		GetCurScene().sortNode(node, true);
	emit SglUpdateAni();
}

//> �ڵ�����
void L3DArtist::SlotMoveDowned(HNode node)
{
	if (node.isValid())
		GetCurScene().sortNode(node, false);
	emit SglUpdateAni();
}

//> ����GUID��ȡ���ڵ�
HNode L3DArtist::GetParentNodeByGUID(const tstring strNodeId)
{
	HNode rootnode = GetCurScene().getSceneRootNode();
	vector<HNode> vecNode;
	rootnode.getChildren(vecNode);
	for (auto node : vecNode)
	{
		HNode ParentNode = ParentNodeByGUID(node.getGUID(), strNodeId);
		if (ParentNode.isValid())
			return ParentNode;
	}
	return HNode();
}

//> ����GUID��ȡ���ڵ�,�ݹ��ѯ,���ҽڵ�ID�������ڵ�ID��ͬʱ����Ϊ��
HNode L3DArtist::ParentNodeByGUID(const tstring strParentId, const tstring strNodeId)
{
	HNode ParentNode = GetCurScene().getNodeByGUID(strParentId);
	if (strParentId == strNodeId)
		return HNode();
	vector<HNode> vecNode;
	ParentNode.getChildren(vecNode);
	for (auto node : vecNode)
	{
		//�����ӽڵ�Ϊ���ҽڵ�,���ظ��ڵ�
		if (strNodeId == node.getGUID())
			return ParentNode;
		//��������������ݹ��ѯ
		else
		{
			if (node.isHasChileren())
			{
				HNode pParentNode = ParentNodeByGUID(node.getGUID(), strNodeId);
				if (pParentNode.isValid())
					return pParentNode;
			}
		}
	}
	return HNode();
}


void L3DArtist::CreateSceneAndMediaDirectory()
{
	QString root = ArtistParams::instance()->ProjectRootPath();
	root.replace(QString("/"), QString("\\"));
	tstring rootPath = root.tQtToStdString();
	I3DProjectDirManager* dirManage = I3DProjectDirManager::instance();
	if (dirManage->init(rootPath, false) != I3DProjectDirManager::IPDM_RSLT_SUCCESS)
	{
		LOG(QERROR) << _T("create root path unsuccessfully");
		MSGBOX_ERROR_TIP(tr("create root path unsuccessfully"), this);
		return;
	}

	// ��ȡ����Ŀ¼
	QString root_Path = tQtFromStdString(dirManage->getRootPath());
	if (!root_Path.isEmpty())
	{
		root_Path.replace(QString("\\"), QString("/"));
	}
	m_strProjectPath = root_Path + "/" + m_strProjectName;

	// ��ȡ����Ŀ¼
	tstring curScenePath;

	if (dirManage->getScenesDir(m_strProjectName.tQtToStdString(), curScenePath, true) != I3DProjectDirManager::IPDM_RSLT_SUCCESS)
	{
		LOG(QERROR) << _T("create scene dir unsuccessfully");
		MSGBOX_ERROR_TIP(tr("create scene dir unsuccessfully"), this);
		return;
	}
	QString curSceneDirPath = tQtFromStdString(curScenePath).replace(QString("\\"), QString("/")); //������Ŀ¼��ַ��ʽ\\����/  

	if (curSceneDirPath.indexOf(m_strProjectPath) >= 0)
	{
		int scenePathLen = curSceneDirPath.length();
		int proPathLen = m_strProjectPath.length();
		SlotUpdateCurSceneDir(curSceneDirPath.right(scenePathLen - proPathLen - 1));
	}
	else
	{
		LOG(QERROR) << _T("scene dir error");
		return;
	}


	// �����ز�·��
	tstring curMateriaDir;
	if (dirManage->getMaterialDir(m_strProjectName.tQtToStdString(), I3DProjectDirManager::IPDM_MT_Image, curMateriaDir) != I3DProjectDirManager::IPDM_RSLT_SUCCESS ||
		dirManage->getMaterialDir(m_strProjectName.tQtToStdString(), I3DProjectDirManager::IPDM_MT_Video, curMateriaDir) != I3DProjectDirManager::IPDM_RSLT_SUCCESS ||
		dirManage->getMaterialDir(m_strProjectName.tQtToStdString(), I3DProjectDirManager::IPDM_MT_Audio, curMateriaDir) != I3DProjectDirManager::IPDM_RSLT_SUCCESS)
	{
		LOG(QERROR) << _T("create meterial dir unsuccessfully");
		MSGBOX_ERROR_TIP(tr("create meterial dir unsuccessfully"), this);
		return;
	}

#ifdef FOR_XOR_CO_VER    //added by wuheng 2016.08.12  �ڹ���Ŀ¼�´��� text Ŀ¼
	tstring curTextDir;
	if (dirManage->getTextDir(m_strProjectName.tQtToStdString(), curScenePath, true) != I3DProjectDirManager::IPDM_RSLT_SUCCESS)
	{
		LOG(QERROR) << _T("create text dir unsuccessfully");
		MSGBOX_ERROR_TIP(tr("create text dir unsuccessfully"), this);
		return;
	}
#endif

	curMateriaDir = dirManage->getMaterialRootDir(m_strProjectName.tQtToStdString());
	m_strMediaPath = tQtFromStdString(curMateriaDir);
}

void L3DArtist::SlotUpdateAni()
{
	// ���ִ��ڴ�С����
	QSize cueAniSize = m_pDockAnimation->size();
	emit SglUpdateAni();
	m_pDockAnimation->resize(cueAniSize);
}

void L3DArtist::SlotSetAlignBtnEnable(bool isMulSel)
{
    _isMulSel = isMulSel;
    pTBtnVAlign->setEnabled(_isMulSel);
    m_pTBtnVAlign->setEnabled(_isMulSel);
    pTBtnHAlign->setEnabled(_isMulSel);
    m_pTBtnHAlign->setEnabled(_isMulSel);
}

void L3DArtist::setSomeBtnEntable()
{
	pTBtnMove->setEnabled(true);
	ui.m_pMove->setEnabled(true);

	pTBtnRotate->setEnabled(true);
	ui.m_pRotate->setEnabled(true);

	pTBtnZoom->setEnabled(true);
	ui.m_pZoom->setEnabled(true);

	pTBtnSize->setEnabled(true);
	ui.m_pSize->setEnabled(true);

	pTBtnCopy->setEnabled(true);
	ui.m_pCopy->setEnabled(true);

	pTBtnCut->setEnabled(true);
	ui.m_pCut->setEnabled(true);

	pTBtnPaste->setEnabled(true);
	ui.m_pPaste->setEnabled(true);

	ui.m_pDelete->setEnabled(true);
}

void L3DArtist::setSomeBtnDisable()
{
	pTBtnMove->setEnabled(false);
	ui.m_pMove->setEnabled(false);

	pTBtnRotate->setEnabled(false);
	ui.m_pRotate->setEnabled(false);

	pTBtnZoom->setEnabled(false);
	ui.m_pZoom->setEnabled(false);

	pTBtnSize->setEnabled(false);
	ui.m_pSize->setEnabled(false);

	pTBtnCopy->setEnabled(false);
	ui.m_pCopy->setEnabled(false);

	pTBtnCut->setEnabled(false);
	ui.m_pCut->setEnabled(false);

	pTBtnPaste->setEnabled(false);
	ui.m_pPaste->setEnabled(false);

	ui.m_pDelete->setEnabled(false);
}

void L3DArtist::SlotCheckSceneIsModified()
{
	if (GetCurScene().isValid())
	{
		if (GetCurScene().isModified())
		{
			pTBtnSaveScene->setEnabled(true);
		}
		else
		{
			pTBtnSaveScene->setEnabled(false);
		}
	}
}

void L3DArtist::SlotUpdateEnableAudio(const tstring& strNodeId, bool bValue)
{
	emit SglUpdateProperty(strNodeId, ENUM_ITEM_TYPE::EItem_Type_Node, 0);
}

void L3DArtist::SlotUpdateAudioToNodeItem(const tstring& strNodeId, bool bValue)
{
	emit SglUpdateAutio(strNodeId, bValue);
}

void L3DArtist::SlotOpenGradientOrSolid()
{
	HNode node = GetCurFocusNode();
	if (!node.isValid())
		return;

	int nIndex = node.getCuTexIndex();
	if (nIndex > 0)
	{
		TexturePtr pTex = node.getAppTexture(nIndex - 1)->getBaseTexture();
		E_BTex_Image_Type texType = pTex->getImageType();
		switch (texType)
		{
		case EBTIT_SOLID:
			CreateSolidColor();
			break;
		case EBTIT_GRADIENT:
			CreateGradient();
			break;
		default:
			break;
		}
	}
}

void L3DArtist::SlotSetCard()
{
	SetCardParamDlg dlg(this);

	if (dlg.exec() == QDialog::Accepted)
	{
		ArtistParams::instance()->VideoCardMode(dlg.isCardModel());
		ArtistParams::instance()->VideoCardType(dlg.getVideoCardType());
		ArtistParams::instance()->save();
		QMessageBox::information(this, tr("Tip"), tr("Reboot to make the setting effective"));
	}
}

#ifndef FOR_XOR_CO_VER
void L3DArtist::SlotModifyPwd()
{
	ModifyPassDlg* dlg = new ModifyPassDlg(this);
	dlg->setParam(L3DLoginWidget::instance()->getUserName());
	dlg->exec();
	dlg->deleteLater();
}
#endif

void L3DArtist::SlotUpdateCtrlNode(const tstring& nodeid, E_ANIMATOR_TYPE animatorType)
{
	HNode node = GetCurScene().getNodeByGUID(nodeid);
	if (!node.isValid())
		return;

	if (animatorType == E_ANIMATOR_TYPE::EAT_Position)
		node.setCtrlNodeHide(false);
	else
		node.setCtrlNodeHide(true);
}

void L3DArtist::DBStatusChangedCallback(E_DB_STATUS status)
{
	emit sglDBStatusChanged(status);
}

void L3DArtist::slotDBStatusChanged(E_DB_STATUS status)
{
	if (status == DB_MAIN_DOWN)
	{
		LOG(QERROR) << _T("�����ݿ����ӷ����жϻ�ʧ�ܣ�");
		if (L3DSystemConfig::instance()->isEnableBackupSvr() && m_pInstance->m_ctrlLinkClient.isMainUsed())
		{
			LOG(QERROR) << _T("��ʼ�л��������ݿ⣡");
			if (!CDBConnectPool::instance()->turnSubDBConnect()) // �������첽�ģ����ﲻ���ɹ����ж�
			{
				LOG(QERROR) << _T("�л��������ݿ�ʧ�ܣ�");
				return;
			}

			LOG(QERROR) << _T("��ʼ�л��������ӣ�");
			if (!m_pInstance->m_ctrlLinkClient.turnToBackupCon()) // �������첽�ģ����ﲻ���ɹ����ж�
			{
				LOG(QERROR) << _T("�л���������ʧ�ܣ�");
				return;
			}
		}
	}

	if (status == DB_SUB_DOWN)
	{
		LOG(QERROR) << _T("�����ݿ����ӷ����жϻ�ʧ�ܣ�");
	}

	if (L3DSystemConfig::instance()->isEnableBackupSvr())
	{
		if (status == DB_CONN_TO_MAIN || status == DB_CONN_TO_SUB)
		{
			bool DBMainUsed = CDBConnectPool::instance()->isMainDBUsed();
			if (DBMainUsed != m_pInstance->m_ctrlLinkClient.isMainUsed())
			{
				bool ret = false;
				if (DBMainUsed)
					ret = m_pInstance->m_ctrlLinkClient.turnToMainCon();
				else
					ret = m_pInstance->m_ctrlLinkClient.turnToBackupCon();

				if (!ret)
				{
					LOG(QERROR) << _T("�л�ʧ�ܣ����鱻�л������ķ����������ݿ��Ƿ���á�");
					return;
				}
			}
		}
	}
}

CLMResult L3DArtist::on_switch_db()
{
	LOG(INFO) << _T("��ʼ�л��������ݿ⣡");
	if (!CDBConnectPool::instance()->turnMainDBConnect())
	{
		LOG(QERROR) << _T("�л��������ݿ�ʧ�ܣ�");
		return CLM_RSLT_E_NORMAL;
	}

	LOG(INFO) << _T("��ʼ�л������ӣ�");
	if (!m_pInstance->m_ctrlLinkClient.turnToMainCon())
	{
		LOG(QERROR) << _T("�л���������ʧ�ܣ�");
		return CLM_RSLT_E_NORMAL;
	}

	return CLM_RSLT_S_OK;
}

void L3DArtist::initEditMode()
{
	m_pDockScene->setVisible(false);
	//     m_pDockAnimation->setVisible(false);
	//     m_pDockToolBoxObj->setVisible(false);
	//     m_pDockToolBoxMaterial->setVisible(false);
	//     m_pDockToolBoxProperty->setVisible(false);

	//ui.m_pTBarFile->setVisible(false);
	ui.menuBar->setVisible(false);
	pTBtnNewScene->setVisible(false);
	pTBtnDeleteScene->setVisible(false);
	pTBtnImportScene->setVisible(false);
	pTBtnAbout->setVisible(false);
}
//> added by TaiYue 2019.6.5 for �������ƶ�����µ���������Ϊ����ֵ
void L3DArtist::setComboxMoveType()
{
	m_PDComboxChoose->setToolTip(tr("Step Size"));
	m_PDComboxChoose->setItemText(0, "0.01");
	m_PDComboxChoose->setItemText(1, "0.1");
	m_PDComboxChoose->setItemText(2, "1");
	m_PDComboxChoose->setItemText(3, "10");

	setPreMoveComboxIndex();
}

void L3DArtist::setRotateType()
{
	m_PDComboxChoose->setToolTip(tr("Step Size"));
	m_PDComboxChoose->setItemText(0, "0.01");
	m_PDComboxChoose->setItemText(1, "0.1");
	m_PDComboxChoose->setItemText(2, "1");
	m_PDComboxChoose->setItemText(3, "10");

	setPreRotateComboxIndex();
}

// ��������������µ���������ΪXYZ��ȫ�ȱ�������ȫ�ȱ�
void L3DArtist::setComboxScaleType()
{
	//> ͬ������������
	m_PDComboxChoose->setToolTip(tr("Synchronous Zoom Coordinate Axis"));
	m_PDComboxChoose->setItemText(0, "XYZ");
	m_PDComboxChoose->setItemText(1, "XY");
	m_PDComboxChoose->setItemText(2, "XZ");
	m_PDComboxChoose->setItemText(3, "None");

	setPreScaleComboxIndex();
}


void L3DArtist::setPreMoveComboxIndex()
{
	switch (PreNodeType)
	{
	case L3DArtist::NODECTRL_INIT:
		break;
	case L3DArtist::NODECTRL_MOVE:
		break;
	case L3DArtist::NODECTRL_ROTATE:
		m_nPreRotateStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_SCALE:
		m_nPreScaleStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_SIZE:
		break;
	default:
		break;
	}
	PreNodeType = NODECTRL_MOVE;
}

void L3DArtist::setPreRotateComboxIndex()
{
	switch (PreNodeType)
	{
	case L3DArtist::NODECTRL_INIT:
		break;
	case L3DArtist::NODECTRL_MOVE:
		m_nPreMoveStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_ROTATE:
		break;
	case L3DArtist::NODECTRL_SCALE:
		m_nPreScaleStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_SIZE:
		break;
	default:
		break;
	}
	PreNodeType = NODECTRL_ROTATE;
}

void L3DArtist::setPreScaleComboxIndex()
{
	switch (PreNodeType)
	{
	case L3DArtist::NODECTRL_INIT:
		break;
	case L3DArtist::NODECTRL_MOVE:
		m_nPreMoveStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_ROTATE:
		m_nPreRotateStatus = m_PDComboxChoose->currentIndex();
		break;
	case L3DArtist::NODECTRL_SCALE:
		break;
	case L3DArtist::NODECTRL_SIZE:
		break;
	default:
		break;
	}
	PreNodeType = NODECTRL_SCALE;
}



void L3DArtist::SlotOpenSceneSimple()
{
	m_vecEdit.clear();
	tstring sceneInfoPath = CFileSys::GetAppExePath(false) + _T("\\SceneEdit.xml");
	if (!boost::filesystem::exists(sceneInfoPath))
	{
		LOG(QERROR) << _T("û�г�����Ϣ�ļ�!");
		emit SglHideArtistEdit();
		return;
	}

	pugi::xml_document xml_file;
	if (!xml_file.load_file(sceneInfoPath.c_str(), pugi::parse_default))
	{
		LOG(QERROR) << _T("���س�����Ϣ�ļ�ʧ��!");
		emit SglHideArtistEdit();
		return;
	}

	tstring path = _T("");
	pugi::xml_node  scene_edit_node = xml_file.child(_T("SceneEdit"));
	if (scene_edit_node)
		path = scene_edit_node.attribute(_T("path")).as_string();

	auto edit_node_range = scene_edit_node.children(_T("EditItem"));
	if (edit_node_range.begin() == edit_node_range.end())
		LOG(INFO) << _T("�����������ɱ༭����Ϣ!");

	int i = 0;
	auto item_iter = edit_node_range.begin();
	for (; item_iter != edit_node_range.end(); item_iter++)
	{
		EditableItemInfo edit;
		edit.nodeGuid_ = item_iter->attribute(_T("nodeID")).as_string();
		edit.editableItemID_ = i;
		edit.editableItemType_ = (E_S_EDITABLE_TYPE)item_iter->attribute(_T("editType")).as_int();
		edit.szContents_ = item_iter->attribute(_T("content")).as_string();
		if (edit.editableItemType_ == E_S_EDITABLE_TYPE::ESEIT_NodeMedia)
		{
			tstring tmp = edit.szContents_;
			tstring tmp_front = _T("");
			tstring tmp_back = _T("");

			size_t pos = tmp.find(_T("scenepkges"));
			if (pos != tstring::npos)
			{
				tmp_front = tmp.substr(0, pos);
				tmp_back = tmp.substr(pos, -1);

				size_t pos2 = tmp_back.find(_T("media"));
				if (pos2 != tstring::npos)
					tmp_back = tmp_back.substr(pos2, -1);
			}

			tstring projectName = GetCurProjectName().toStdWString();
			edit.szContents_ = tmp_front + _T("\\") + projectName + _T("\\") + tmp_back;
		}

		i++;
		m_vecEdit.push_back(edit);
	}

	QString project_path = ArtistParams::instance()->ProjectRootPath();
	QString scene_path = QString::fromStdWString(path.c_str());
	if (scene_path.isEmpty())
	{
		LOG(QERROR) << _T("���������༭�������ļ�·��Ϊ�գ�");
		QMessageBox::warning(this, tr("Warning"), tr("Can't find the scene"));
		emit SglHideArtistEdit();
		return;
	}

	QString file = project_path;
	if (scene_path[0] != '\\')
		file += "\\";
	file += scene_path;

	if (!QFile::exists(file))
	{
		LOG(QERROR) << _T("���������༭���Ҳ�����Ӧ�ĳ����ļ���") << file.toStdWString();
		QMessageBox::warning(this, tr("Warning"), tr("Can't find the scene: ") + file);
		emit SglHideArtistEdit();
		return;
	}

	if (!pCheckBoxSync->isChecked())
	{
		tstring scenePath = file.tQtToStdString();
		//.bak ���ڻָ�����Ŀ¼���ļ�
		tstring scenePath_bak = scenePath + _T(".bak");
		bf::copy_file(scenePath, scenePath_bak, bf::copy_option::overwrite_if_exists);

		//����ͼ
		tstring thumbnailPath = scenePath;
		size_t pos = thumbnailPath.find_last_of(_T("."));
		if (pos != tstring::npos)
		{
			thumbnailPath = thumbnailPath.substr(0, pos + 1);
			thumbnailPath += _T("jpg");
		}

		if (bf::exists(thumbnailPath))
		{
			tstring thumbnailPath_bak = thumbnailPath + _T(".bak");
			bf::copy_file(thumbnailPath, thumbnailPath_bak, bf::copy_option::overwrite_if_exists);
		}

		//.sbak ���ڻָ��ϴ��޸ĵ��ļ�
		tstring scenePath_sbak = scenePath + _T(".sbak");
		if (bf::exists(scenePath_sbak))
		{
			boost::system::error_code err;
			bf::remove(scenePath);
			bf::rename(scenePath_sbak, bf::path(scenePath), err);
		}
	}

	SlotOpenSceneFile(file);
}

void L3DArtist::SlotUpload()
{
	QString pack_path = ArtistParams::instance()->ProjectRootPath();
	int npos = pack_path.lastIndexOf("\\Project");
	pack_path = pack_path.left(npos) + "\\Temp";

	QString local_upload_root = ArtistParams::instance()->LocalUploadPath();
	if (local_upload_root.isEmpty())
	{
		MSGBOX_WARNING_TIP(tr("The upload path is empty! Please through the [CenterServer] option under the [Settings] menu set the path!"), this);
		return;
	}

	QFileInfoList filelist;
	if (ArtistParams::instance()->isArtistEditMode())
		filelist = pDockScene->GetFileInfo();
	else
		filelist.push_back(m_strCurScenePath);
	if (filelist.empty())
	{
		MSGBOX_WARNING_TIP(tr("Upload files list is empty!"), this);
		return;
	}
	else
	{
		//SlotCloseScene();
		QString cur_scenepath = GetCurProjectPath() + "/" + GetCurSceneListPath();

		if (ArtistParams::instance()->isArtistEditMode())
			QApplication::setQuitOnLastWindowClosed(false);
		else if (CurOpenScene_.isValid() && CurOpenScene_.isModified())
		{
			SceneManager_.saveSceneFile(m_strCurScenePath.tQtToStdString(), CurOpenScene_);
			Sleep(500);
		}	

		PackDlg *dlg = new PackDlg(cur_scenepath, this);

		//connect(dlg, &QDialog::finished, this, &SceneList::finishUpload);
		IUpload::UploadType type = static_cast<IUpload::UploadType>(L3DSystemConfig::instance()->getAppMode());
		if (dlg->Init(filelist, pack_path, type))
		{
			dlg->show();
			// ������ݲ���ģʽ�����Զ��ϴ����ɣ����ٲ������裬modified by litianming 2019/07/24
			//if (ArtistParams::instance()->isArtistEditMode())
			if (L3DSystemConfig::instance()->getAppMode() == APP_MODE_STUDIO)
				dlg->start();
		}
	}
}

void L3DArtist::SlotCloseSceneSimple()
{
	restoreSceneFile();
	SlotCloseScene();
	emit SglHideArtistEdit();
}

void L3DArtist::SlotSaveAndUpload()
{
	if (ArtistParams::instance()->isArtistEditMode())
		return;

	if (m_strCurScenePath.isEmpty())
		return;

	SlotSaveScene();
	SlotUpload();
}

void L3DArtist::SlotHideEdit()
{
	restoreSceneFile();
	emit SglArtistEditNotice();
	emit SglHideArtistEdit();
}

//> added by TaiYue 2019.6.5 for ��õ�ǰ�����������index
double L3DArtist::GetStepValue(int index)
{
	double value = 0.1;
	switch (index)
	{
	case  0:value = 0.01; break;
	case  1:value = 0.1; break;
	case  2:value = 1; break;
	case  3:value = 10; break;
	default:
		break;
	}
	return value;
}


//> ѡ��λ�ƵĲ�����С
void L3DArtist::SlotSetSpinSingleStep(int index) 
{
	double dStep = GetStepValue(index);
	m_pDSpinBoxX->setSingleStep(dStep);
	m_pDSpinBoxY->setSingleStep(dStep);
	m_pDSpinBoxZ->setSingleStep(dStep);
	m_pDSpinBoxX->setAttribute(Qt::WA_ShowWithoutActivating, true);
	NodeCtrl nodectrl = NODECTRL_MOVE;
	//>��������Dock���Ͳ��������ź�
	emit SglMgrSingleStep(dStep , nodectrl);
}

//> ��ת״̬�����ò�����С
void L3DArtist::SlotSetRotateSingleStep(int index)
{
	double dStep = GetStepValue(index);
	m_pDSpinBoxX->setSingleStep(dStep);
	m_pDSpinBoxY->setSingleStep(dStep);
	m_pDSpinBoxZ->setSingleStep(dStep);
	m_pDSpinBoxX->setAttribute(Qt::WA_ShowWithoutActivating, true);
	NodeCtrl nodectrl = NODECTRL_ROTATE;
	//>��������Dock���Ͳ��������ź�
	emit SglMgrSingleStep(dStep,nodectrl);
}

//> ѡ���ƶ�״̬ʱ�Ĳ��ֹ������ʼ��						
void L3DArtist::comboBoxMoveSetting(NodeCtrl nodectrl)
{
		SlotSetSpinSingleStep(m_nPreMoveStatus);
		m_PDComboxChoose->blockSignals(true);
		m_PDComboxChoose->setCurrentIndex(m_nPreMoveStatus);
		m_PDComboxChoose->blockSignals(false);

		// ���������������ʾ
		m_PDComboxChoose->setVisible(true);
		//> �Ͽ�����ת������״̬�¼�¼��һ��comboBox��indexֵ
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetScaleIndex(int)));
		//disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateIndex(int)));
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateSingleStep(int)));
		//> �����ƶ�ģʽ�µĲ���
		connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
		//> ��move״̬�¼�¼��һ��comboBox��indexֵ
		//connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetMoveIndex(int)));
}

void L3DArtist::comboBoxRotateSetting(NodeCtrl nodectrl)
{
	if (m_rotateFirstState == true)
	{
		SlotSetSpinSingleStep(1);
		m_rotateFirstState = false;
		m_PDComboxChoose->blockSignals(true);
		m_PDComboxChoose->setCurrentIndex(1);
		m_PDComboxChoose->blockSignals(false);
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
		connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateSingleStep(int)));
	}
	else if (m_rotateFirstState == false)
	{
		SlotSetSpinSingleStep(m_nPreRotateStatus);
		m_PDComboxChoose->blockSignals(true);
		m_PDComboxChoose->setCurrentIndex(m_nPreRotateStatus);
		m_PDComboxChoose->blockSignals(false);

		// ���������������ʾ
		m_PDComboxChoose->setVisible(true);
		//> �Ͽ���Move,scale״̬�¼�¼��һ��comboBox��indexֵ
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
		//disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetScaleIndex(int)));
		//disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetMoveIndex(int)));
		//> �����ƶ�ģʽ�µĲ���
		connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateSingleStep(int)));
		//> ��move״̬�¼�¼��һ��comboBox��indexֵ
		//connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateIndex(int)));
	}
}

//> ѡ������״̬ʱ�ĳ�ʼ���벿�ֹ���
void L3DArtist::comboBoxScaleSetting(NodeCtrl nodectrl)
{
	if (m_scaleFirstState == true)
	{
		m_PDComboxChoose->setCurrentIndex(0);
		m_scaleFirstState = false;

		m_PDComboxChoose->blockSignals(true);
		m_PDComboxChoose->setCurrentIndex(0);
		m_PDComboxChoose->blockSignals(false);
		m_PDComboxChoose->setVisible(true);

		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateSingleStep(int)));

		connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetScaleIndex(int)));
	}
	else if (m_scaleFirstState == false)
	{
		m_PDComboxChoose->blockSignals(true);
		m_PDComboxChoose->setCurrentIndex(m_nPreScaleStatus);
		m_PDComboxChoose->blockSignals(false);
		m_PDComboxChoose->setVisible(true);

		m_pDSpinBoxX->setSingleStep(0.1);
		m_pDSpinBoxY->setSingleStep(0.1);
		m_pDSpinBoxZ->setSingleStep(0.1);
		//> �Ͽ����ƶ�,��ת״̬�����ò���
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetSpinSingleStep(int)));
		disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateSingleStep(int)));
		//> �Ͽ���move��rotate״̬�¼�¼��һ��comboBox��indexֵ
		//disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetMoveIndex(int)));
		//disconnect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetRotateIndex(int)));
		//> ��scale״̬�¼�¼��һ��comboBox��indexֵ
		connect(m_PDComboxChoose, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotSetScaleIndex(int)));
	}
}

//> ������λ���ƶ�����µ���һ�β�����index��ֵ
void L3DArtist::SlotSetMoveIndex(int index)
{
	m_nPreMoveStatus = m_PDComboxChoose->currentIndex();//��¼��λ���ƶ�����µ���һ�ε�index��ֵ
}

void L3DArtist::SlotSetRotateIndex(int index)
{
	m_nPreRotateStatus = m_PDComboxChoose->currentIndex();
}
//> ������������������´����һ��indexֵ
void L3DArtist::SlotSetScaleIndex(int index)
{
	//m_nPreScaleStatus = m_PDComboxChoose->currentIndex();	//��¼����������µ���һ�ε�index��ֵ
	emit SglSentScaleIndex(m_PDComboxChoose->currentIndex());//��dock��巢��������״̬�µ�indexֵ
}

void L3DArtist::setMoveFocusXYZ()
{
	switch (m_nGetMoveFocusXYZ)
	{
	case L3DArtist::FocusXYZ_X:
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Y:
		m_pDSpinBoxY->setFocus();
		m_pDSpinBoxY->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Z:
		m_pDSpinBoxZ->setFocus();
		m_pDSpinBoxZ->setFocusPolicy(Qt::ClickFocus);
		break;
	default://> û���κ�ֵ�����ı䣬Ĭ��ΪX�����������Ϊ����
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	}
}

void L3DArtist::setRotateFocusXYZ()
{
	switch (m_nGetRotateFocusXYZ)
	{
	case L3DArtist::FocusXYZ_X:
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Y:
		m_pDSpinBoxY->setFocus();
		m_pDSpinBoxY->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Z:
		m_pDSpinBoxZ->setFocus();
		m_pDSpinBoxZ->setFocusPolicy(Qt::ClickFocus);
		break;
	default://> û���κ�ֵ�����ı䣬Ĭ��ΪX�����������Ϊ����
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	}
}

void L3DArtist::setScaleFocusXYZ()
{
	switch (m_nGetScaleFocusXYZ)
	{
	case L3DArtist::FocusXYZ_X:
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Y:
		m_pDSpinBoxY->setFocus();
		m_pDSpinBoxY->setFocusPolicy(Qt::ClickFocus);
		break;
	case L3DArtist::FocusXYZ_Z:
		m_pDSpinBoxZ->setFocus();
		m_pDSpinBoxZ->setFocusPolicy(Qt::ClickFocus);
		break;
	default://> û���κ�ֵ�����ı䣬Ĭ��ΪX�����������Ϊ����
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
		break;
	}
}


// added by TaiYue 2019.6.5 for ѡ�в�ͬ�����򣨲���ֵ���ȱ����ţ���Ľ���ع�
void L3DArtist::SlotSetFocusXYZ(int index)
{
	//>���������X��m_nFocusXYZ=FocusXYZ_X��ͬ���ϴν�����Y��FocusXYZ_Y
	if (bHasFocusX)
		m_nFocusXYZ = FocusXYZ_X;
	if (bHasFocusY)
		m_nFocusXYZ = FocusXYZ_Y;
	if (bHasFocusZ)
		m_nFocusXYZ = FocusXYZ_Z;

	//> ����ϴν����¼ΪFocusXYZ_X����X�ᷢ����ֵ�䣬��X�������ý���
	if (m_nFocusXYZ == FocusXYZ_X)
	{
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
	}
	else if (m_nFocusXYZ == FocusXYZ_Y)
	{
		m_pDSpinBoxY->setFocus();
		m_pDSpinBoxY->setFocusPolicy(Qt::ClickFocus);
	}
	else if (m_nFocusXYZ == FocusXYZ_Z)
	{
		m_pDSpinBoxZ->setFocus();
		m_pDSpinBoxZ->setFocusPolicy(Qt::ClickFocus);
	}
	else //> û���κ�ֵ�����ı䣬Ĭ��ΪX�����������Ϊ����
	{			
		m_pDSpinBoxX->setFocus();
		m_pDSpinBoxX->setFocusPolicy(Qt::ClickFocus);
	}
}

void L3DArtist::SlotSetTexEffKeyFrameTime(E_ANIMATOR_TYPE eAnimatorType, long iTimeLength)
{
    HNode hNode = GetCurFocusNode();
    if (!hNode.isValid())
        return;

    if (eAnimatorType != EAT_TexEffInput && eAnimatorType != EAT_TexEffOutput)
        return;

    emit SglSetTexEffKeyFrameTime(hNode, eAnimatorType, iTimeLength);
}

void L3DArtist::SlotDeleteTexEffAnimator(HNode hNode, E_ANIMATOR_TYPE eAnimatorType)
{
    emit SglDeleteTexEffAnimator(hNode, eAnimatorType);
}

void L3DArtist::restoreSceneFile()
{
	if (!ArtistParams::instance()->isArtistEditMode())
		return;

	ArtistParams::instance()->setSyncArtist(pCheckBoxSync->isChecked());

	if (!pCheckBoxSync->isChecked())
	{
		boost::system::error_code err;
		tstring scenePath = m_strCurScenePath.tQtToStdString();
		tstring scenePath_bak = scenePath + _T(".bak");

		tstring scenePath_sbak = scenePath + _T(".sbak");
		bf::copy_file(scenePath, scenePath_sbak, bf::copy_option::overwrite_if_exists);

		if (bf::exists(scenePath_bak))
		{
			bf::remove(scenePath);
			bf::rename(scenePath_bak, bf::path(scenePath), err);
		}

		tstring thumbnailPath = scenePath;
		size_t pos = thumbnailPath.find_last_of(_T("."));
		if (pos != tstring::npos)
		{
			thumbnailPath = thumbnailPath.substr(0, pos + 1);
			thumbnailPath += _T("jpg");
		}

		tstring thumbnailPath_bak = thumbnailPath + _T(".bak");
		if (bf::exists(thumbnailPath_bak))
		{
			bf::remove(thumbnailPath);
			bf::rename(thumbnailPath_bak, bf::path(thumbnailPath), err);
		}
	}
}

QWidget* L3DArtist::GetPropertyEditorWidget()
{
	if (m_pDockPropertyEdit == nullptr)
		return nullptr;
	return m_pDockPropertyEdit->widget();
}
QWidget* L3DArtist::GetDockToolBoxProperty()
{
    if (m_pDockToolBoxProperty == nullptr)
        return nullptr;
    return m_pDockToolBoxProperty->widget();
}

void L3DArtist::AddTexEffAnimator(HNode hNode, E_TexEff_App_Type texEffAppType)
{

    if (texEffAppType != ETEAT_Input && texEffAppType != ETEAT_Output)
        return;

    emit SglCreateTexEffAnimator(hNode, texEffAppType);
}