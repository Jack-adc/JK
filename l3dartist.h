/*
* Copyright (c) LTECH Corp. 2012-2020. All Rights Reserved.
* 
* Authors: CaiYangLe, 2013.7
* Version: 1.0.0.0
*
* Purpose: Artist�����ڣ�ʹ�õ��ĵ�
* Note:    
* Note2:   
*/

#ifndef L3DARTIST_H
#define L3DARTIST_H

#if defined(WIN32_LEAN_AND_MEAN)
# undef WIN32_LEAN_AND_MEAN
#endif

#include "ui_l3dartist.h"

#include "notifier.h"
#include "L3DEKInterface.h"
#include "L3DEKInterface2.h"
//#include "L3DSnLibMgrInterface.h"	//added by wangww 2014.3.17
#include "CallBackDefine.h"

#include "L3DCPInterface.h" //CtrlPointPtr
#include "MeshDeformIF.h"// MeshDeformPtr
#include "TexInterface.h"// TexturePtr AppTexturePtr
#include "LightPassIF.h" //LightPassPtr
#include "AniInterface.h" //AnimatorPtr KeyFramePtr

#include <QMainWindow>
#include <vector>
#include "AsynOperating.h"
#include "QTSharedWindow\QtSingleApplication\qtsingleapplication.h"
#include "timeline\spinbox.h"
#include "l3dartistdefs.h"
#include "l3ddockwidget.h"
#include "outputvideo.h"
#include "backgroundcolor.h"
#include "ScenePack.h"//added by xyc Ϊ����ӵ��������еĲ����ú����е�eScenePackStatus
#include "DBConnect.h"
#include "../../inc/CLCInterface.h"
#include "DBAssistDef.h"
#include "scenetreeitem.h"
#include <QCheckBox>
#include <QComboBox>
using namespace L3DEngineKernel;
using namespace CallBackEnum;
using namespace std;

class QDockWidget;
class QLabel;
class PreviewWindow;
class SceneTree;
class DockAnimation;
class SceneTreeItem;
class DockPropertyEditor;
class AboutDlg;
class BaseWidget;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class ToolButton;
class QWidgetAction;
class QProgressBar;
class QMimeData;
class QPushButton;
class RollLabel;
class DockScene;

class L3DArtist : public QMainWindow
{
	Q_OBJECT
    Q_DISABLE_COPY(L3DArtist)

public:
    L3DArtist( QWidget *pParent = nullptr,int nCmdParam = -1);
	virtual ~L3DArtist();

	//> �ڵ����ģʽ
	enum NodeCtrl
	{
        //> ��ʼ/���
        NODECTRL_INIT,
		//> �ƶ�
		NODECTRL_MOVE,
		//> ��ת
		NODECTRL_ROTATE,
		//> ����
		NODECTRL_SCALE,
        //> �޸ĳߴ�
        NODECTRL_SIZE
	};

	//���岻ͬ�ؼ��Ľ���
	enum FocusXYZ
	{
		//> �������X�������
		FocusXYZ_X=1,
		//> �������Y�������
		FocusXYZ_Y=2,
		//> �������Z�������
		FocusXYZ_Z=3
	};

public:
	static L3DArtist *GetInstance() { return m_pInstance; }

    void intialize(QDBTOOLS::Logger::LogInitParams* logParams, const QString& projectName);
    void setProject(const QString& proj);

	void GetCurSceneSize(int& width, int& height) const { width = m_nCurSceneWidth; height = m_nCurSceneHeight; }
	inline int GetCurProFrameRate() { return m_FrameRate; }//added by wangww 2014.6.25
	//added by wangww 2014.4.10
	QString GetCurScenePath()const { return m_strCurScenePath; }
	//> ��ȡ��ǰ����
	HScene GetCurScene() const { return CurOpenScene_; }
	//> ��ȡRenderManager
	HRenderManager GetRenderManager() {return RenderManager_;};

	//> ��ȡ��ǰ������focus�ڵ�
	HNode GetCurFocusNode();
    //> ��ȡ��ǰ����ѡ�еĽڵ�
    HNode GetCurSelectedNode();
	std::vector<HNode> GetCurSelectedNodes();
	//> ���õ�ǰ������focus�ڵ�
	void SetCurFocusNode(HNode node);
	//> ��ȡ��ǰ������ʽ
	NodeCtrl GetCurNodeCtrl() const { return m_eNodeCtrl; }
	//> ��ȡ��ǰ�ڵ��������
	int GetCurMateIndex() const { return m_nCurMaterial; }
	//> ��ȡ��ǰ�ڵ���������
	int GetCurTexIndex() const { return m_nCurTexture; }
	//> ��ȡ������
	const QString &GetCurProjectName() const { return m_strProjectName; }
	const QString& GetCurProjectPath() const { return m_strProjectPath; }
	/*const QString GetProjectRootPath()const;
	const QString GetTempPath()const;
	const QString GetLocalUploadPath()const;*/
    // added by wuheng 2016.03.11
    QList<QString>& GetTempNodeStrList()  { return m_tempNodeStrList;} 
    void            ClearTempNodeStrList(){ m_tempNodeStrList.clear();}

	/*  int GetCurSizeHint() const;
	void SetCurSizeHint(int cur_size);*/
    // �Ƿ�Ϊ����ģʽ
    //bool isLocalUpload() const;
    // �Ƿ�Ϊ�ݲ���ģʽ
   // bool isStudio() const;
	const QString& GetCurSceneListPath() const { return m_strSceneListPath; }
	//> ��ȡ�ز�·�� added by yejia 2014-12-23
	const QString& GetCurMediaPath() const{ return m_strMediaPath;} 
	//> ��ȡ�˵���ָ��
	QMenuBar *GetMenuBar() { return menuBar(); }
	//> ��ȡ״̬��ָ��
	QStatusBar *GetStatusBar() { return statusBar(); }

	QString getLocalLangue();
    //> ����		by ShenX
	void VideoTextureChange();
    void TranslateLanguage();
	void setOwner(QtSingleApplication* pOwner);
    //> ���ҹ���Ŀ¼���ļ��Ƿ��г�ͻ,�г�ͻ�򷵻س�ͻ�ļ�ȫ·��;���򷵻�falseΪ��
    //  files : QMap<�ļ���,�ļ�·��> ; suff : ����ͬ���ļ���׺; dir : ����·��,Ĭ��Ϊ��,��ʾ��ǰ����·����; 
    bool checkConfict(QMap<QString,QStringList>& files = QMap<QString,QStringList>(), QString suff = "l3d", QString dir = QString());
    //> ���ҹ���Ŀ¼����û�ж�Ӧ�ļ��ĳ�ͻ,initFile:������ļ�ȫ·��
    QString checkfileConfict(const QString& initFileName, QString dir = QString());

	static bool GetTanslateStatus();
    //> ��������    by ShenX
    void CreateSolidColor();
    void CreateGradient();
	void CreatelLightPass();
	// ��������
	QString CreateBevel(QString selectedType = "");
	void CreatePolygon();
    bool PreClose();
    void SetFocusDockWidget(l3dDockWidget* dw);
    void RenameSceneInEdit(QString qstr);
    bool IsPlaying();
    void OpenSceneTemplete(QString qstr);
    //> ���¶�����
    void UpdateNodeAnimator(HNode node);
    void RulerMoved(long pos);
    // �༭��չ����
    void CreateExtMaterial();
    // ��Ч�������ؼ�֡ʱ���ı�
    void TexEffectTimeChange(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    void UpdateTexEffectTime(HNode hNode, E_ANIMATOR_TYPE animatorType);
    // ��Ч������ɾ��ʱ��ɾ����Ӧ����Ч����
    void DeleteTexEffect(HNode hNode, E_ANIMATOR_TYPE animatorType);
private:
	QtSingleApplication* m_pOwner;
	QTranslator* translator_cn;
	QTranslator* translator_default;
	static bool m_bTranslate;

private:
	// ������ͼ�꣬�޸�Ϊ��Ա����
	ToolButton *pTBtnNewScene;
	ToolButton *pTBtnDeleteScene;
	ToolButton *pTBtnImportScene;
	ToolButton *pTBtnExportScene;
	ToolButton *pTBtnSaveScene;
    ToolButton *pTBtnUploadScene;                   //�����ϴ���ť
	ToolButton *pTBtnCut;
	ToolButton *pTBtnCopy;
	ToolButton *pTBtnPaste;
	ToolButton *pTBtnMove;
	ToolButton *pTBtnRotate;
    ToolButton *pTBtnVAlign;
    ToolButton *pTBtnHAlign;
	ToolButton *pTBtnZoom;
    ToolButton *pTBtnSize;
	ToolButton *pTBtnOutput;
	ToolButton *pTBtnMultiView;
	ToolButton *pTBtnAbout;
    QAction	   *m_pTBtnVAlign;
    QAction    *m_pTBtnHAlign;
    QCheckBox* pCheckBoxSync;                 //����ģʽ���Ƿ�ͬ���޸�����ģʽ�µĳ���

	// ����������ͼ��
	ToolButton *pTBtnTextureAll;
	ToolButton *pTBtnTextureFront;
	ToolButton *pTBtnTextureBack;
	ToolButton *pTBtnTextureSide;
	ToolButton *pTBtnTextureLeft;
	ToolButton *pTBtnTextureRight;
	ToolButton *pTBtnTextureUp;
	ToolButton *pTBtnTextureDown;
	ToolButton *pTBtnTextureInside;
    ToolButton *pTBtnTextureOutSide;
	ToolButton *pTBtnTextureFace;
	ToolButton *pTBtnTextureFrontchamfer;
	ToolButton *pTBtnTextureBackchamfer;
	ToolButton *pTBtnTextureShadow;
	ToolButton *pTBtnTextureBaseBoard;
	//���ڴ洢����������
	QList<ToolButton*>	TextureList;
	//> ���ȫ�������ֹ���������
	QStringList			strlistTex;
	//> ��ŵ�ǰ�����ֹ���������,��ʵʱ����
	QStringList			strlistTexDef;
	QButtonGroup		*pTextureToolBtnGroup;
	QHBoxLayout			*pHLayoutTexture;
	QFrame				*pFrameTexture;

	/******************************************��Ⱦ���ڶ��빦������*********************************
	//��Ⱦ���ڶ��빦��
	QMenu		*AlignMenuX;
	QAction		*AlignMenuX_Left;
	QAction		*AlignMenuX_Right;
	QAction		*AlignMenuX_Center;
	QAction		*AlignMenuX_Aliquots;
	QMenu		*AlignMenuY;
	QAction		*AlignMenuY_Left;
	QAction		*AlignMenuY_Right;
	QAction		*AlignMenuY_Center;
	QAction		*AlignMenuY_Aliquots;
	QMenu		*AlignMenuZ;
	QAction		*AlignMenuZ_Left;
	QAction		*AlignMenuZ_Right;
	QAction		*AlignMenuZ_Center;
	QAction		*AlignMenuZ_Aliquots;
	ToolButton	*pTBtnAlignX;
	ToolButton	*pTBtnAlignY;
	ToolButton	*pTBtnAlignZ;
	/******************************************��Ⱦ���ڶ��빦������*********************************/

public: // AsynOperating ��ؽӿ�
	CAsynOperatingBassPtr	GetOperating();
	void setOperating(CAsynOperatingBassPtr pdata);
	//added by xyc 2014.07.10�������ñ�������Ϣ
	void setToolBoxTitleBar(l3dDockWidget::ToolBoxType type, const QString &strDirPath);
	//�ж������ַ��Ƿ�Ϸ� add by xyc
	bool isLegal(QString &strName);
	// added by xyc 2014.08.12 ���ù���������ͼ�����ʾ
	void SetToolBarTexturePic(QStringList strlist);
	void ToolBarTextureBtnInit();
	void InitToolBarTexture();
	double GetStepValue(int);
	//> �������ƶ�״̬�µ�X�ᷢ���仯ʱ�����ϴν����¼���������ܣ������䣩
	void setMoveFunction_X(vector3df, HNode, double);
	//> �������ƶ�״̬�µ�X�ᷢ���仯ʱ�����ϴν����¼����������
	void setMoveFunction_Y(vector3df, HNode, double);
	//> �������ƶ�״̬�µ�X�ᷢ���仯ʱ�����ϴν����¼����������
	void setMoveFunction_Z(vector3df, HNode, double);
	void setRotateFunction_X(vector3df, HNode, double);
	void setRotateFunction_Y(vector3df, HNode, double);
	void setRotateFunction_Z(vector3df, HNode, double);
	//> ����������״̬�µ�X�ᷢ���仯ʱ�����е�һЩ���ã����������ϴν����¼��ֵ�ĸ�ֵ
	void setScaleFunction_X(vector3df,HNode,double);
	//> ����������״̬�µ�Y�ᷢ���仯ʱ�����е�һЩ���ã����������ϴν����¼��ֵ�ĸ�ֵ
	void setScaleFunction_Y(vector3df, HNode, double);
	//> ����������״̬�µ�Z�ᷢ���仯ʱ�����е�һЩ���ã����������ϴν����¼��ֵ�ĸ�ֵ
	void setScaleFunction_Z(vector3df, HNode, double);
	void comboBoxMoveSetting(NodeCtrl nodectrl);
	void comboBoxRotateSetting(NodeCtrl nodectrl);
	void comboBoxScaleSetting(NodeCtrl nodectrl);

	

signals:
	void SglClearTexurePartToolBar();
	void SglUpdateTexurePartToolBar(const tstring &strNodeId);
	//> ����ʱ�Ե�ǰ��ѡ��,�ⲿ�������,intΪ��ǰ����������idֵ
	void SglSelectTexurePartToolBar(int);
	//> �������а�ť������,���ⲿ���и���,intΪ��ǰ�ڵ㵱ǰ������indexֵ
	void SglTextureToolBtnGroupPressed(int);
    //> ����༭ģʽ��,���������ź�
    void SglHideArtistEdit();
	//> ����༭ģʽ��,���ͳ����������ϴ����֪ͨ�ź�
	void SglArtistEditNotice();
	//> ���ò���ֵ���������λ�ò��������ź�
	void SglMgrSingleStep(double dStep, NodeCtrl nodectrl);
	//> ��������ֵ�����DOCK��������XYZ���������ź�
	//void SglMgrScaleValue(double X, double Y, double Z);


private slots:
	//> �����ֹ���������/����ʾ
	void SlotClearTexurePartToolBar();
	//> �����ֹ�����ʹ�ܸ���
	void SlotUpdateTexturePartToolBar(const tstring& strNodeId);
	//> ��Ӧ���µ�ǰѡ����,intΪ��ǰ�����ֹ�������idֵ
	void SlotSelectTexurePartToolBar(int);
	//> ��Ӧ������������,intΪ��ǰ�����ֹ�������idֵ
	void SlotTextureToolBtnGroupPressed(int);
    //> ��Ӧ����״̬�ı�
    void SlotSetToolBarStatusPic(int selsize);

private:
	//> ���������ֹ���ͼ��
	void CreateTexureToolButton(ToolButton** ToolBtn, QString StrObjectName, const QString StrTipName, int GroupId);
    //> ����/ֹͣ״̬
    void changePlayState(bool isPlay);
        
public slots:
	//> ���õ�ǰ�ڵ����״̬
	void SlotSetCurNodeCtrl();
	//> ��ʾ���ؽ�����
	void SlotShowProgress(bool bIsShow);
    //> �ر�֮ǰ
    void SlotPreClose();
	void SlotStartVOS(int type, QString path, bool field, long length);
	void SlotStopVOS();
    //> ���¿��Ƶ�
    void SlotUpdateCtrlNode(const tstring& nodeid,E_ANIMATOR_TYPE animatorType);
    //> ����༭ģʽ�´򿪳���,���ⲿ�źŴ���
    void SlotOpenSceneSimple();
    //> ����༭ģʽ���ϴ�����
    void SlotUpload();
    //> ����༭ģʽ�¹رճ�������������ʾ
    void SlotCloseSceneSimple();
    //> ���沢�ϴ��ۺ���
    void SlotSaveAndUpload();
    //>  ����༭ģʽ������������
    void SlotHideEdit();
	//> ѡ��λ�ƵĲ�����С
	void SlotSetSpinSingleStep(int index);
	//> ѡ������ת�Ĳ�����С
	void SlotSetRotateSingleStep(int index);
	//> ������λ���ƶ�����µ���һ�β�����index��ֵ
	void SlotSetMoveIndex(int index);
	//> ��������ת����µ���һ�β�����Index��ֵ
	void SlotSetRotateIndex(int index);
	//> ������������������´����һ��indexֵ
	void SlotSetScaleIndex(int index);
	//> ��������ѡ��ComboBox��Ľ�������
	void SlotSetFocusXYZ(int);
	
    //> ������Ч�������ؼ�֡λ��
    void SlotSetTexEffKeyFrameTime(E_ANIMATOR_TYPE eAnimatorType, long iTimeLength);
    //> ͨ��ɾ�����Բ�����Чɾ����Ч������
    void SlotDeleteTexEffAnimator(HNode hNode, E_ANIMATOR_TYPE eAnimatorType);

private: //���� �����浱ǰ����״̬
	CAsynOperatingBassPtr		AsynOperating_;

protected:
	//> �ر��¼�
	virtual void closeEvent(QCloseEvent *pEvent) override;
	//> �������¼�
	virtual void enterEvent(QEvent *pEvent) override;
	//> �¼�������
	virtual bool eventFilter(QObject *pObject, QEvent *pEvent) override;
    //> �����¼�
    virtual void keyPressEvent(QKeyEvent *pEvent) override;

private: //�����棬��� ����DLL ������ú���
	 //��ʼ��DLL
	bool InitDll(QDBTOOLS::Logger::LogInitParams* logParams);
	bool UninitDll();
	//��ȡdllȫ����ع�����
	bool GetDLLManager();
	//��ʼ����صĹ�����.
	bool InitManager();
	// ע��QT �ź����ͺ���
	void RegisterMetaType();
    //> ����dll�Ĺ�������
    void resetDllProject();
    //> ������Ⱦ���ڷֱ���
    void resetPwnResolution(/*CardInitParam& param*/);
	// �ӱ����ļ��лָ������ļ���ArtistEdit����Ч
	void restoreSceneFile();

private:
	//> �����ͱ����ǰ�����Ľ�����º���
	void UpdateCurSene(HScene pScene);
	//> �����ڵ����� ������º���
	void UpdateNodeChange();
	//> �������л�������泡���ļ�
	void SaveHScene(const QString &SceneName, const QString &xml);
	// addded by taiyue
	void setMoveFocusXYZ();
	void setRotateFocusXYZ();
	void setScaleFocusXYZ();

private: //dll�Ļص��ӿ�
	std::shared_ptr<INotifier>				INotifierPtr_;					// ȫ�ֲ����ص��ӿ�
	std::shared_ptr<ISceneOperateCB>		ISceneOperateCBPtr_;			// ���������ص��ӿ�
	std::shared_ptr<INodeOperateCB>			INodeOperateCBPtr_;				// �ڵ�����ص��ӿ�

private: //���������
	HSceneManager							SceneManager_;					//����������
	HAudioManager							AudioManager_;					//��Ƶ���롢���������
	HCGCardManager							CGCardManager_;					//��Ļ��������
	HRenderManager							RenderManager_;					//��Ⱦ������
	//HSceneLibMgr							ScenePathMrg_;					//����·��������
	HCGCardManager							card_output_;

//������ص����ݷ��� ����
	HScene									CurOpenScene_;					//��ǰ�򿪵ĳ���
	int										secAniSeq_;						//��ǰ���ŵķֶζ������
	//QString									sceneXML_;						//�������л�������ַ���
	QString									TextureXML_;					//�������л�������ַ���
	QString									CtrlSerialize_;					//���Ƶ� ���л��Ľ�� ����
	volatile bool							CheckOpenFile_;					//���ڱ�ʶ ��ǰ�Ƿ��Ǵ��ļ���������Ǵ��ļ�������Ҫ�ڱ��棩
	bool                                    m_isResetProject;               //���ڱ�ʶ �Ƿ�Ϊ���ù��̶��رյ�ǰ����
signals:
	//> �����»������Լ���չ����
	void SglUpdateBaseProperty();
	//> �����¶�������
	void SglUpdateExtProperty();
	//> ��������ʹ��,�ݽ��� added by yejia 2014-10-16 
	void SglUpdateExtProp(QString gropName,QString propName);
	//> ////////////////////////////////////////////////////////////////////////////////////////
	//> �ص����������źţ�֪ͨ��Ҫ����״̬���Ӵ��ڸ���״̬
	//> ////////////////////////////////////////////////////////////////////////////////////////
	//> ��ն�����,ʱ����ѡ����,������Կ�
	void SglClearSceneTreeSel();
	void SglClearTimeLine();
	//> ������Կ���������,ÿ����¾��Դ����,һ�㲻����
	void SglClearProperty();
	//> ���¶�����,ʱ����,�������Կ�,������Ϊ��ǰ������
	void SglUpdateSceneTree();
	void SglUpdateTimeLine();
	//> ���µ�ǰ��ʾҳ������
	void SglUpdateProperty(const tstring &strNodeId,const ENUM_ITEM_TYPE& itemType,const int& itemIndex);
	//> ������,ʱ�������ѡ����,tureΪѡ��,falseΪ��ѡ
	void SglSceneTreeSelectNode(const tstring &strNodeId, bool bIsSel);
	void SglTimeLineSelectNode(const tstring &strNodeId, bool bIsSel);

	//> �����ļ�����
	void SglSceneFileSave(const QString &filename, const QString &xmlStr);
	//> ��Ƶ����ı�,��ʱ��������϶�ʱ����
	void SglVideoTextureChanged();
	//> ����Nodeλ������
	void SglNodeMove();
	//> ����Node��ת����
	void SglNodeRotation();
	//> ����Node��������
	void SglNodeScale();
	//> ���½ڵ���ƴ���
	void SglUpdateAni();
	//> ���µ����ڵ㶯�������
	void SglUpdataNodeAni(HNode node);
	//> ���³����б�
	void SglUpdateSceneList();
	//> �������Ƶ㴴���ɹ�����¿��Ƶ���ʾ
	void SglUpdateCtrlPoint();
	//> �������Ÿ���ʱ����ָ��λ��
	void SglUpdateCursorPos(long lPosition);
	//> ɾ������������
	void SglRemoveAni(HNode node, E_ANIMATOR_TYPE eAniType = EAT_UNKNOWN);

	//> ���¹���������
	void SglUpdateLightPass(LightPassPtr pLP, bool isEnable);
    //> �ڵ�������
    void SglUpdateNodeName(const tstring &strNodeId, const QString &strNodeName);
    //> ɾ���ؼ�֡
    void SglRemoveKeyFrame(KeyFramePtr pKF);
    //> ɾ�����Ƶ�
    void SglRemoveCtrlPoint(CtrlPointPtr pCP);
	//> �رճ���������Ƶ�
	void SglClearCtrlPoint();
    //> ���²���������
    void SglUpdateMaterialGroup(int nCur);
	//> ���ڵ�Ĺؼ�֡λ�øı���Ҫ���´��,added by wangww 2014.3.3
	void SglReKeyFram(HNode node);
	//> �����Ƶ���
	void SglVOS_Start();
	void SglVOS_End();
	void SglVOS_Process(long frame);
    // ���Ĺ���·��
    void SglUpdateProjectPath();
	//> ����ѡ����ü�ʹ��,���ڳ�ʼ�����ر�ʱ//added by xyc
	void SglActionEnable(bool isEnable);
	//> ���Կ���ü�ʹ��,���ڳ�ʼ�����ر�ʱ
	void SglPropertyEnable(bool isEnable);
	//> ��ǰ�����ֹ����������ı��źŸı����Կ�����������ֵ
	void SglSelectedToolBarTexture(int Index);
	//added by xyc 2014.09.18
	void SglUpdateKeyFrameSetXYZ(double dX, double dY, double dZ, NodeCtrl nodectr);
    //> �ı䵱ǰ����
    void sglsetProject(const QString& proj);
    //> ����ҳ��Ƶ���Ա��֪ͨ NodeItem ͼ������Ӧ�仯
    void SglUpdateAutio(const tstring &strNodeId, bool bValue);
    //>  ����Ԥ�����ڱ����
    void sglClearAllRulerItem();
    //>  ��ʾԤ�����ڱ����
    void sglShowAllRulerItem();

	void sglDBStatusChanged(E_DB_STATUS status);
    //> ����༭ģʽ�³�������ɹ���,�ϴ��ź�
    void SglUploadSimple();

	//> ����������Ч
	void SglCreateTextureEffect(HNode node, AppTexturePtr pATex, TexEffectPtr pTexEff, LEKResult rslt);
    //> ������Ч������
    void SglCreateTexEffAnimator(HNode hNode, E_TexEff_App_Type texEffAppType);
    //> ��Ч�������ؼ�֡ʱ���ı�
    void SglTexEffTimeChange(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    //> ��Ч�������ؼ�֡ʱ���ı�ĸ���
    void SglUpdateTexEffTime(HNode hNode, E_ANIMATOR_TYPE animatorType);
    void SglSetTexEffKeyFrameTime(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    // ɾ����Чʱɾ��������
    void SglDeleteTexEffAnimator(HNode hNode, E_ANIMATOR_TYPE eAnimatorType);
    // ɾ����Ч������ʱɾ����Ч
    void SglDeleteTexEffect(HNode hNode, E_ANIMATOR_TYPE animatorType);

private slots:
#ifndef FOR_XOR_CO_VER
	// �޸����룬������Ļ��ʹ��
	void SlotModifyPwd();
#endif
	//> ���½���ɾ�������ȵ�ʵ�ʲ������������������࣬��Ϊ�˷�����Ϣ�۷���
	//> �½�����
	void SlotNewScene();
    //> ����ڵ�
    void SlotImportNode();
	//> ���볡��
	void SlotImportScene();
	//> �򿪳����ļ�
    void SlotOpenSceneFile(const QString &strFileName);
	//> ���xyz��Ϊ0
	void SlotClearSpinBox();
	//> ɾ������
	void SlotRemoveScene(QString strFileName);
	//> �����Ƶ
	void SlotOutputVideo();

	//> �л�����
	void SlotChangeLanguage();
	//> ��������
	void SlotExportScene();
	//> �رճ���
	void SlotCloseScene();
	//> ���泡��
	void SlotSaveScene();
	//> ���Ϊ����
	void SlotSaveAsScene();
	//> ��ȡ�ڵ�
	void SlotExtractNode();
	//> ��ȡ����
	void SlotExtractScene();
	//> ���Ʋ���
	void SlotCopyMaterial();
	//> ճ������
	void SlotPasteMaterial();
	//> ɾ������
	void SlotRemoveMaterial();
	//> ��ȡ����
	void SlotExtractMaterial();
	//> ��������
	void SlotCopyTex();
	//> ճ������
	void SlotPasteTex();
	//> ɾ������
	void SlotRemoveTex();
    //> ��ȡ����
    void SlotExtractTexture();
	//> ������
	void SlotArrayCopy();
	//> ת��Ϊ3D���ֲۺ���
	void SlotConvert3DText();
	//> ת��Ϊ2D���ֲۺ���
	void SlotConvert2DText();
	//> ��ȡ�����ۺ���
	void SlotExtractAni();
	//> �˳�
	void SlotExit();
	//> ����
	void SlotAbout();
    // ��ݼ�
    void SlotShortCut();
	// �����㼶
	void SlotSceneLayer();
    // ������ɫ
    void SlotBackgroundColor();
    // ��ʾ��ȫ����
    void SlotShowSafe();
    // ��ʾ���ⰲȫ����
    void SlotShowTitleSafe();
    // ��ʾ���
    void slotDrawRulers();
    // �޸ı��
    void slotSetRulers();
    // Tga
    void SlotTga();
	//> Undo�ۺ���
	void SlotUndo();
	//> Redo�ۺ���
	void SlotRedo();
	//> �����ڵ�ۺ���
	void SlotCreatNode(E_L3DNODE_TYPE type, void *param, HNode parent);
	//> ��������item���������node�ۺ���
	void SlotSceneTreeItemCk();
	//> ����Undo  -redo ϵͳ 
	void SlotUpdataUdRdSystem(const QStringList &UndoList, const QStringList &RedoList);
	//> �ı���Ⱦ���ڴ�С����
	void SlotResizePlayWnd(int x, int y, int width, int height, bool repaint);
	//> ���µ�ǰ���ļ���Ϣ
    void SlotUpdateCurOpenFile(const QString &strFileName);
	//> �����������ֵ����,��Ӧ�����Կⷢ���޸��ź�
	void SlotUpdateToolBarXYZ(double dX, double dY, double dZ, NodeCtrl nodectrl);//modify by xyc 2014.09.18Ϊ�˸��ؼ�֡���ô����ṩ�ж�����
	//> �ı䵱ǰ�ڵ����ģʽ
	void SlotSetNodeCtrl(NodeCtrl nodectrl);
    //> ��λ
    void SlotBtnResetOnClick();
	//> ������Ű�ť
	void SlotBtnPlayOnCli();
    //> ���ѭ����ť
    void SlotBtnLoopOnCli();
    void SlotBtnPreOnCli();
    void SlotBtnBackOnCli();
    void SlotBtnForwardOnCli();
    void SlotBtnNextOnCli();
	//> x����
	void SlotValueChangeX(double dValue);
	//> y����
	void SlotValueChangeY(double dValue);
	//> z����
	void SlotValueChangeZ(double dValue);
    //> for xor video card test. added by CaiYL 2014.02.18
//     void SlotVideoSet(bool bIsCheck);
	//>���µ�ǰ����Ŀ¼��added by wangww 2014.4.10
	void SlotUpdateCurSceneDir(const QString &strFileName);
    // ���Ĺ����������
    void SlotProjectSet();
    // ����FTP����
    void SlotFtpSetting();
	//> ����ѡ����ü�ʹ��,���ڳ�ʼ�����ر�ʱadded by xyc
	void SlotActionEnable(bool isEnable);
    //> �޸�����
    void slotChangeSizeMsg(QAction* act);
    //> ����ʱ����
    void SlotUpdateAni();
    //> ���¿�����Ƶ��Ϣ    added by wuheng 2016.05.20
    void SlotUpdateEnableAudio(const tstring& strNodeId, bool bValue);
    void SlotUpdateAudioToNodeItem(const tstring& strNodeId, bool bValue);
    //> ����ɫ��ɫ���öԻ��� added by wuheng 2016.10.08
    void SlotOpenGradientOrSolid();
    //> ������Ļ��������� added by wuheng 2016.12.12
    void SlotSetCard();
	/******************************************��Ⱦ���ڶ��빦������*********************************
	//��Ⱦ���ڶ��빦�ܰ�ť��Ӧ����
	void SlotAlignX();
	void SlotAlignY();
	void SlotAlignZ();
	/******************************************��Ⱦ���ڶ��빦������*********************************/
    void SlotSetAlignBtnEnable(bool isMulSel);
	void SlotAside();
private slots:
	/********************************************************************************/
	//> Notifier�ص��ӿ��źŽ��ղ�
	/********************************************************************************/
	//> ����ʵʱ��Ⱦ���ƻص��źŽ��ղ�
	void SlotOnRenderSceneOperate(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic, RENDER_OPERATE ro);
	//> ¼�Ƴ�����Ⱦ��Ƶ�����ص��źŽ��ղ�
	void SlotOnEndSaveVOSFile(HRenderManager hRM, LEKResult rslt, const QString &fileNamePath, L3DEngineKernel::IContext *pic);
	//> ¼�Ƴ�����Ⱦ��Ƶ���Ȼص��źŽ��ղ�
	void SlotOnSaveVOSProcess(HRenderManager hRM, long frame);
	//> ��Ļ����ʼ������ʼ�������ص��źŽ��ղ�
	void SlotOnCGCInitOperate(HCGCardManager hCGCM, LEKResult rslt, L3DEngineKernel::IContext *pic, CGCIS_INIT ci);
	//> ��Ļ�������رս����ص��źŽ��ղ�
	void SlotOnCGCChannelOperate(HCGCardManager hCGCM, E_CGC_CHANNEL_TYPE chType, unsigned int chIdx, L3DEngineKernel::IContext *pic, CGC_OPEN_CLOSE coc);
	//> ��Ļ�����ú�ص��źŽ��ղ�
	void SlotOnCGCardSetuped(HCGCardManager hCGCM, const QString &paramsXML, L3DEngineKernel::IContext *pic);
	//> �������󴴽������ص��źŽ��ղ�
	void SlotOnSceneCreated(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic);
	//>��������򿪶����ص��źŽ��ղ�
	void SlotOnSceneOpened(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic);
	//> �������󱣴涯���ص��źŽ��ղ�
	void SlotOnSceneSaveFile(HSceneManager hSM, LEKResult rslt, HScene scene, const QString &filename, L3DEngineKernel::IContext *pic);
	//> ������������ص��źŽ��ղ�
	void SlotOnSceneOperate(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic, SCENE_OPERATE so);
	//> �������������󷴳�ʼ���ص��źŽ��ղ�
	void SlotOnSMUnInitialized(HSceneManager hSM, LEKResult rslt, L3DEngineKernel::IContext *pic);
    //> ����ģ��
    void SlotOnSceneTempleteOpened(HSceneManager hSM, LEKResult rslt,  const QString& filename);
	//> Undo/Redo�ص��źŽ��ղ�
	void SlotOnObjectUpdate(unsigned int canUndoNum, unsigned int canRedoNum, const QString &ucDescribe, const QString &rcDescribe);
	//> ��Ⱦ��������Ӧ�ڲ��������ٻص��źŽ��ղ�
	void SlotOnDestoryMgr(L3DEngineKernel::IContext *pic, DESTROY_MGR dm);
	//> renderManager ��ʼ���ص��źŽ��ղ�
	void SlotOnOnRMgrInitialized(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic);
	//> enderManager����ʼ���ص��źŽ��ղ�
	void SlotOnRMgrUnInitialized(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic);

	/********************************************************************************/
	//> NodeOperateCB�ص��ӿ��źŽ��ղ�
	/********************************************************************************/
	//> �ڵ����ĸ��ڵ����Ըı��ص��źŽ��ղ�
	void SlotOnParentUpdate(HNode node, HNode new_p, HNode old_p, LEKResult rslt);
	//> �ڵ�����ص��źŽ��ղ�
	void SlotOnNodeOperate(HNode node, HNode clone, LEKResult rslt, NODE_OPERATE no);
	//> �ڵ�������л�������ص��źŽ��ղ�
	void SlotOnNodeSerialized(HNode node, const QString &nodeXML, LEKResult rslt);
	//> �ڵ�ɼ������Ե��ûص��źŽ��ղ�
	void SlotOnNodeVisible(HNode node, bool isVisible, LEKResult rslt);
	//> �ڵ�任�ص��źŽ��ղ�
	void SlotOnNodeTransform(HNode node, ic::vector3df pos, LEKResult rslt, NODE_TRANSFORM nt);
	//> �ڵ㱬ը�����ı�ص��źŽ��ղ�
	void SlotOnNodeExplosion(HNode node, int rate, float range);
	//> �ڵ�ģ�ͱ���������ݲ����仯�ص��źŽ��ղ�
	void SlotOnNodeMeshDeform(HNode node, MeshDeformPtr pMD, bool isTypeModi);
	//> �ڵ�ģ�ͱ��β������л�������ص��źŽ��ղ�
	void SlotOnNodeMDSerizlized(HNode node, const QString &MDXML, LEKResult rslt);
	//> �ڵ�ģ�ͱ��β��ַ����л�������ص��źŽ��ղ�
	void SlotOnNodeMDDeserialized(HNode node, MeshDeformPtr pMD, LEKResult rslt);
	//> �ڵ������ز����ı��ص��źŽ��ղ�
	void SlotOnMaterialUpdate(HNode node, iv::SMaterial mat, unsigned int num);
	//> ���ʶ��󴴽���ص��źŽ��ղ�
	void SlotOnMaterialCreated(HNode node, iv::SMaterial mat, LEKResult rslt);
	//> �ڵ�������л���ص��źŽ��ղ�
	void SlotOnMaterialSerialized(HNode node, const QString& xmlstr, LEKResult rslt);
	//> ����������󴴽���ص��źŽ��ղ�
	void SlotOnTextureCreated(HNode node, TexturePtr pTex, LEKResult rslt,int texIndex);
	//> Ӧ��������󴴽���ص��źŽ��ղ�
	void SlotOnAppTextureCreated(HNode node, AppTexturePtr pATex, LEKResult rslt);
	// >������Ч������ص��źŽ��ղ�
	void SlotOnTexEffectCreated(HNode node, AppTexturePtr pATex, TexEffectPtr pTexEff, LEKResult rslt);
	//> Ӧ������������л���ص��źŽ��ղ�
	void SlotOnAppTextureSerialized(HNode node, AppTexturePtr pTex, const QString &TexXML, LEKResult rslt);
	//> Ӧ������������л���ص��źŽ��ղ�
	void SlotOnAppTextureDeserialized(HNode node, AppTexturePtr pTex, LEKResult rslt);
	//> ��������������л���ص��źŽ��ղ�
	void SlotOnTextureSerialized(HNode node, TexturePtr pTex, const QString &TexXML, LEKResult rslt);
    //> ���ɻ�����������ͼ�ص��źŽ��ղ�
    void SlotOnThumbnail(IImage* image);
	//> ��������������л���ص��źŽ��ղ�
	void SlotOnTextureDeserialized(HNode node, TexturePtr pTex, LEKResult rslt);
	//> �����������ûص��źŽ��ղ�
	void SlotOnTextureSeted(HNode pNode, TexturePtr pTex, LEKResult rslt);
	//> ������������ص��źŽ��ղ�
	void SlotOnTextureOperate(HNode pNode, TexturePtr pTex, TEXTRUE_OPERATE to);
	//> �����������øı�ص��źŽ��ղ�
	void SlotOnLightPassUpdate(HNode pNode, LightPassPtr pLP, bool isEnable);
	//> ���������ص��źŽ��ղ�
	void SlotOnAnimatorOperate(HNode pNode, AnimatorPtr pAni, LEKResult rslt, ANIMATOR_OPERATE ao);
	//> ��ȡ�����ص��źŽ��ղ�
	void SlotOnAnimatorExtract(HNode node, const QString &anisXML, LEKResult rslt);
	//> �ڵ㶯���������йؼ�֡�����ص��źŽ��ղ�
	void SlotOnKeyFrameOperate(HNode node, AnimatorPtr pAni, KeyFramePtr pKF, LEKResult rslt, KEYFRAME_OPERATE ko);
	//> ��չ���Ը��»ص��źŽ��ղ�
	void SlotOnExtPropUpdate(HNode pNode, const QString &extCatName, const QString &extProName,bool isReCreate);
	//> ��չ���Ը��»ص��źŽ��ղ�
	void SlotOnKeyFrameTimeUpdate(HNode pNode);
    void SlotOnUpdateBasePara();
    void SlotOnWarningInfo(HNode pNode, LEKResult rslt);
	/********************************************************************************/
	//> SceneOperateCB�ص��ӿ��źŽ��ղ�
	/********************************************************************************/
	//> ���л�or�������л�����ص��źŽ��ղ�
	void SlotOnSerializeTex(HScene pScene, const QString &sceneXML, LEKResult rslt, SERIALIZE_TEX st);
	//> �����������л������ص��źŽ��ղ�
	void SlotOnSceneDeserialize(HScene pScene, LEKResult rslt);
	//> �ڵ�����ص��źŽ��ղ�
	void SlotOnSceneNodeOperate(HScene pScene, HNode pNode, LEKResult rslt, NODE_OPERATE no);
	//> ����ڵ���»ص��źŽ��ղ�
	void SlotOnFocusOn(HScene pScene, HNode pNode);
	//> ѡ�нڵ������ص��źŽ��ղ�
	void SlotOnSelected(HScene pScene, HNode pNode, bool bIsSel);
	//> ��ǰ��������������ص��źŽ��ղ�
	void SlotOnCameraNodeSet(HScene pScene, HNode pCam, bool bIsLeft);
	//> ���Ƶ�����ص��źŽ��ղ�
	void SlotOnCPointOperate(HScene pScene, CtrlPointPtr pCP, LEKResult rslt, CPOINT_OPERATE co);
	//> ���Ƶ���������ص��źŽ��ղ�
	void SlotOnCPointSetPos(HScene pScene, CtrlPointPtr pCP, LEKResult rslt);
	//> ���Ƶ����л���ص��źŽ��ղ�
	void SlotOnAllCPSerialize(HScene pScene, const QString &cpsXML, LEKResult rslt);
	//> ���Ƶ㷴���л��ص��źŽ��ղ�
	void SlotOnAllCPDeserialize(HScene pScene, LEKResult rslt);
	//> ���Ƶ������������ص��źŽ��ղ�
	void SlotOnCPointTriggered(HScene pScene, CtrlPointPtr pCP);
	//> ���Ƶ�������л������ص��źŽ��ղ�
	void SlotOnCPointSerialize(HScene pScene, CtrlPointPtr pCP, const QString &cpXML, LEKResult rslt);
	//> �������ſ��ƻص��źŽ��ղ�
	void SlotOnScenePlayOperate(HScene pScene, long lPosition, LEKResult rslt, SCENE_PLAY sp);
	//> ��������ʱ����ʱ������ʵʱλ��ͨ���ص��źŽ��ղ�
	void SlotOnSceneCurPosition(HScene pScene, long lPosition);
	//> �����������ֲ����ص��źŽ��ղ�
	void SlotOnSceneSoundOperate(HScene pScene, SceneSoundPtr pSS, LEKResult rslt, SCENE_SOUND_OPERATE sso);
	//> �����������ֲ��Żص��źŽ��ղ�
	void SlotOnSceneSoundPlay(HScene pScene, SceneSoundPtr pSS, int nPayState, long lCurPos);
    //> ����ص��ź�
    void SlotOnAddGroup(HScene pScene, HNode pNode, LEKResult rslt);
    //> ����ص��ź�
    void SlotOnReleaseGroup(HScene pScene, HNode pNode, LEKResult rslt);
	//> ��Ӧ�������������������ʾ
	//void SlotUpdateTexturePart(QString TextureName);

private:
	//> ����ͣ������
	void CreateDockWidgets(bool bEditMode);
	//> �������޸Ĺ�����
	void CreateToolBar();
	//> �������޸�״̬��
	void CreateStatusBar();
	//> �������޸Ĳ˵���
	void CreateMenuBar();
	//> ������������ť��������ͼ�꣬����
	void SetToolBarIcon();
	//> ����������
	void CreateCtrlBar();
	//> д��ini�����ļ�
	void WriteSettings(bool bIsMax);
	//> ��ȡini�����ļ�
	void ReadSettings();
	//> �����źŲ�����
	void CreateConntent();
	//> LEKResultֵ�ж�
	void LEKResultMessage(LEKResult rslt);
	//> ѯ���Ƿ񱣴�
	bool GetIsMaybeSave();
    //> �������Ƶ��ݼ�����
    void TriggerShortCutCP(CtrlPointPtr prePtr, CtrlPointPtr nextPtr, CtrlPointPtr curPtr, int key);
	//> ���õ�ǰ�������ڹ����µ�Ŀ¼   // added by yejia 2014-11-05
	void SetCurSceneDirectory(const QString& scenePath = "");
	//> ��λ��ǰ����Ŀ¼
	void ResetCurSceneDirectory();
	//> ���ݸ�Ŀ¼�빤���� �����������ز�Ŀ¼ added by yejia 2014-12-23
	void CreateSceneAndMediaDirectory();

	void DBStatusChangedCallback(E_DB_STATUS status);
	void slotDBStatusChanged(E_DB_STATUS status);
	static CLMResult on_switch_db();

    // ��ȡ���ʹ���ʹ��
    void saveMaterialToXML();
    tstring _materialXML;
    QString _materialXMLPath;
    //int _materialTexNum;
    //std::vector<std::pair<int, int>> _createTextureIndexes;

private:
	Ui::L3DArtistClass ui;

	//> ʹ�õĳ���
	enum ARTIST_CONST
	{
		//> 
		FILETOOLBARICON_COUNT = 20,
		ACTION_ICON_HEIGHT = 24,
		ACTION_ICON_WIDTH = 24,
	};

	//> ��Ԥ����ʾ����
	PreviewWindow		        *m_pPreviewWnd;

	//> ״̬��ʾlabel
	QLabel				        *m_pLblStatus;
	//> ״̬��ʱ����ʾlabel
	QLabel				        *m_pLblStatusTime;
	//> ״̬���û���ʾlabel
    RollLabel				    *m_pLblStatusUser;
	//> ״̬������������ʾlabel
    RollLabel				    *m_pLblStatusProject;
	//> ״̬������������ʾlabel
    RollLabel				    *m_pLblStatusScene;
	//> ״̬�����Ƴߴ�label
	QLabel				        *m_pLblSizeGrip;
	//> ���Ա༭ͣ������
	l3dDockWidget			    *m_pDockPropertyEdit;
	//> �����б�ͣ������
	l3dDockWidget			    *m_pDockScene;
    DockScene                   *pDockScene;
	//> �����༭ͣ������
	l3dDockWidget			    *m_pDockAnimation;
	//> ������ͣ������
	l3dDockWidget			    *m_pDockSceneTree;
	//> ������ͣ������
    l3dDockWidget               *m_pDockToolBoxObj;
    l3dDockWidget               *m_pDockToolBoxMaterial;
    l3dDockWidget               *m_pDockToolBoxProperty;

    QList<l3dDockWidget*>       m_lstDockWidget;
	//> ��ǰ�����Ĵ洢·��
	QString				        m_strCurScenePath;
	//> λ�Ʋ���ѡ��������
	QComboBox*                  m_PDComboxChoose;
	//> x���ڿ�
	SpinBox_ControlBox*			m_pDSpinBoxX;
	//> y���ڿ�
	SpinBox_ControlBox*			m_pDSpinBoxY;
	//> z���ڿ�
	SpinBox_ControlBox*			m_pDSpinBoxZ;
	//> ��λ��ť
	ToolButton*					m_pTBtnReset;
	//> ���浱ǰ����ģʽ
	NodeCtrl			        m_eNodeCtrl;
	//> undo��ť
	ToolButton*					m_pTBtnUndo;
	//> redo��ť
	ToolButton*					m_pTBtnRedo;
	//> 
	static L3DArtist	        *m_pInstance;
	//> ��ǰ�ڵ���������
	int					        m_nCurTexture;
	//> ��ǰ�ڵ��������
	int					        m_nCurMaterial;
	//> ��ǰ�����Ƿ�Ϊ�½�����
	bool				        m_bIsNewScene;
	//> ��ǰ�����Ƿ���Ҫ��
	bool				        m_bIsOpenScene;
	//> ��ǰ������
	QString				        m_strProjectName;
	//> ��ǰ����·��,��������
    QString                     m_strProjectPath;
	//> ��ǰ�ز�·�� added by yejia 2014-12-23
	QString                     m_strMediaPath;
	//> �ļ��˵�����action
	QWidgetAction		        *m_pActionWidgetFile;
	//> �༭�˵�����action
	QWidgetAction		        *m_pActionWidgetEdit;

	ToolButton			        *m_pTBtnPre;
	ToolButton			        *m_pTBtnBack;
	//> ���Ű�ť
	ToolButton			        *m_pTBtnPlay;
	ToolButton			        *m_pTBtnForward;
	ToolButton			        *m_pTBtnNext;
    ToolButton*                 m_pTBtnLoop;
	//> ȫ����ť
	ToolButton			        *m_pTBtnFullScreen;
	//> ״̬����ʾ������
	QProgressBar		        *m_pProgress;
    //> �����б�ǰ·��,����������·��,EG:scenes
    QString                     m_strSceneListPath;
    //> ��ǰ�����б�򿪳�����
    QString                     m_strCurSceneItemName;
    //> ��ǰ����֡��
    qint64                      m_nCurPos;
    //> �رձ�־
    bool                        m_closeEvent;
    //> Ԥ�رձ�ʶ
    bool                        m_bPreClose;
    //> ��ǰ����
    QLabel                      *m_pLblProject;
    //> ����
    QLabel                      *m_pLblTitle;
    //> ��ǰ����
    QLabel                      *m_pLableScene;
	QWidget						*m_pCentralWidget;

	NodeCtrl					PreNodeType;
	int							m_nFocusXYZ;//���ڼ�¼SpinBox��XYZ��һ�������˱仯��1ΪX,2ΪY��3ΪZ��
	int							m_nGetMoveFocusXYZ;
	int							m_nGetRotateFocusXYZ;
	int							m_nGetScaleFocusXYZ;
	int                         m_nPreMoveStatus;//���ڼ�¼���ƶ�ģʽ����һ�ε�ѡ���index
	int							m_nPreRotateStatus;// ���ڼ�¼����תģʽ����һ�ε�ѡ���index
	int                         m_nPreScaleStatus;// ���ڼ�¼������ģʽ����һ�ε�ѡ���index
	int							m_nCurSceneWidth;
	int							m_nCurSceneHeight;
	int							m_FrameRate;		//֡�ʣ��Ա�artist��Ҫ�˲������ڽ����֡������ʾ�ƣ��ڳ�ʼ��ʱ��ֵ��added by wangww 2014.6.25

	bool						bHasFocusX;
	bool						bHasFocusY;
	bool						bHasFocusZ;
	bool						bMoveFocusX;		//�����жϵ�ǰ�����Ƿ����ƶ�ģʽ�µ�X�ᣬ����Ϊtrue
	bool						bMoveFocusY;		//�����жϵ�ǰ�����Ƿ����ƶ�ģʽ�µ�Y��
	bool						bMoveFocusZ;		//�����жϵ�ǰ�����Ƿ����ƶ�ģʽ�µ�Z��
	bool						bRotateFocusX;		//�����жϵ�ǰ�����Ƿ�����תģʽ�µ�X�ᣬ����Ϊtrue
	bool						bRotateFocusY;		//�����жϵ�ǰ�����Ƿ�����תģʽ�µ�Y��
	bool						bRotateFocusZ;		//�����жϵ�ǰ�����Ƿ�����תģʽ�µ�Z��
	bool						bScaleFocusX;		//�����жϵ�ǰ�����Ƿ�������ģʽ�µ�X�ᣬ����Ϊtrue
	bool						bScaleFocusY;	    //�����жϵ�ǰ�����Ƿ�������ģʽ�µ�Y��
	bool						bScaleFocusZ;		//�����жϵ�ǰ�����Ƿ�������ģʽ�µ�Z��
    bool                        _isMulSel;
	bool						m_bRMgrInit;		//RenderManager ��ʼ����ɱ�־
	bool						m_bRMgrUnInit;		//RenderManager ����ʼ����ɱ�־
	bool						m_bRecording;
	QString					    localLangue_;		//for ��������֧�ֺ��ģ�����Ҫ��������������ʾ���к������壬added by wangww 2014.7.19
    HCLCommClient               m_ctrlLinkClient;	// Ӧ��ģ��ͨѸ���
    ServerInfoVecPtr            dataSerInfoVec;		// ltech ģʽ�£����dataServer ftp��Ϣ�����������һ��ϵͳֻ����һ���������ķ�����������Ϊ�ձ�ʾ���������ķ�����

    QString                     m_pExportScenePath; //��������·�� ���ڼ�¼�ϴδ򿪵�·�� added by yejia 2015-02-04
    QList<QString>              m_tempNodeStrList;  // added by wuheng 2016.03.11

/*****************************************���͸���������Ӧ�ź�**********************/
signals:
    //> ���´����Ľڵ���з���,����Ϊ��ʱ�Գ�ʼ�ڵ���з���,���ڴ򿪳���ʱʹ��
    void SglTrNodeName(const tstring& strNodeId = _T(""));
	//> �����ص��з���,�ڴ����Ѵ���
	void SglInsertNode(const tstring& strParentNodeId, const tstring& strCurNodeId);
	//> ɾ���ص��з���,�ڴ�����ɾ��
	void SglRemoveNode(const tstring& strNodeId);
	//> ������ʾ������
	void SglRepaintSceneTree();
	//> ѡ�нڵ�,�ڴ���δѡ��
	void SglNodeClick(const tstring& strNodeId);
	//> ��ѡ�ڵ�,�ڴ���δѡ��
	void SglNodeLose(const tstring& strNodeId);
    //��¼��أ�added by wangww 2015.1.28
    void slgUpateStartDlg(const QStringList& Project);
    void slgSelectStartDlg(const QString&);
	//>�������巢�͵�ǰToolBar������ģʽ�µ�ComboBox��Indexֵ
	void SglSentScaleIndex(int index);
/*****************************************��������Ӧ�ۺ���**********************/
private slots:
	//> ��ӦSceneTree����ӽڵ�
	void SlotAllSelected();
	void SlotClearSelected();
	void SlotNodeClicked(const tstring& strNodeId,const ENUM_ITEM_TYPE& itemType,const int& itemIndex);
	void SlotNodeFocused(const tstring& strNodeId);//����ȡ����
	void SlotNodeLosed(const tstring& strNodeId);
	void SlotInsertNoded(const tstring& strParentNodeId, const tstring& strCurNodeId);
	void SlotRemoveNoded(const tstring& strNodeId, const tstring& strNodeName);
	void SlotUpdateNodeNamed(const tstring& strNodeId, const tstring& strNodeName);
	void SlotUpdateNodeEdabled(const tstring& strNodeId, bool bEditable);
	void SlotUpdateNodeVisibled(const tstring& strNodeId, bool bVisible);
	void SlotUpdateNodeLocked(const tstring& strNodeId, bool bLock);
	void SlotMoveGrouped();
	void SlotReleaseGrouped(HNode);
	void SlotCuted(HNode);
	void SlotCopied(HNode);
	void SlotPasted(const tstring, HNode);
	void SlotRemoved(HNode);
	void SlotRenamed(QString, HNode);
	void SlotAdded(E_L3DNODE_TYPE, void*, HNode);
	void SlotMoveUped(HNode);
	void SlotMoveDowned(HNode);
    void SlotCheckSceneIsModified();
	// ��ʱˢ�½ڵ��λ�á����š���ת��x,y,z�Ȳ���
	void SlotUpdNodeTimer();

/*****************************************�����Ժ���**********************/
public:
	HNode GetParentNodeByGUID(const tstring strNodeId);

private:
	//> ͨ��GUID��ȡ��ǰ�ڵ�ĸ��ڵ�
	HNode ParentNodeByGUID(const tstring strParentId, const tstring strNodeId);
private:
    void CreateConnectSceneList(DockScene*);
	//> ��Ⱦ���ڻص�Ӱ�����Կ�,������,ʱ����;
	//> ����������Ӱ��ʱ����,ʱ���ᷴ��Ӱ�������;
	//> ��������������Ӧ������Ӱ�����Կ�
	void CreateConnectSceneTree(SceneTree*);
	void CreateConnectTimeLine(DockAnimation*, SceneTree*);
	void CreateConnectDockProperty(DockPropertyEditor*, SceneTree*);
	void CreateConnectPreviewWindow(SceneTree*);

    void initial_ol_thread(SDBCPInitParam param, std::wstring user/*,HCLCommClient HCLClient*/);

    // �����ƶ�����ת�����Ű�ť��ʹ��
    void setSomeBtnEntable();
    void setSomeBtnDisable();

    // �༭ģʽ�£��Խ���UI��һЩ����
    void initEditMode();

	// added by taiyue
	//������λ���ƶ�����£�comboxΪ�������������������Ϊ�ȱ���ǵȱ�����
	void setComboxMoveType();
	//>��������������µ������б�ΪXYZ��XY,XZ,None
	//> ��������ת״̬�µĲ��ֹ������ʼ��
	void setRotateType();
	void setComboxScaleType();
	//> ���Move״̬ʱ��������һ��ģʽ�µ�combox���±�ֵ
	void setPreMoveComboxIndex();
	void setPreRotateComboxIndex();
	void setPreScaleComboxIndex();


public:
    //ltech��ʽ����ʱ����ȡƵ����Ϣ�̣߳�ͨ���źŷ��ͻ�ȡ����Ϣ
    void getAllProject(SDBCPInitParam param, HCLCommClient HCLClient, const std::wstring& user = _T(""));
    //ltechģʽ�»�ȡ���ķ�����FTp
    void getFTP(std::vector<SChannelNoCenter> &serverNames);
 
private:
    bool                             m_hasPlayed;
	// ��¼�Ƿ��һ�δ��ƶ�����ת������
	bool							 m_rotateFirstState;
	bool							 m_scaleFirstState;
    QTimer*                          m_checkIsModifiedTimer;
	QTimer*                          m_nodeParamUpdTimer;
	// ����������������m_nCmdParam(0,1,2,3,4...)����ʹ���ĸ�panel���Զ���ֱ�������
	// �������ݲ���ģʽ�µĶ����Զ���ֱ��ʵĴ���,added by litianming 2016/05/12
	int								 m_nCmdParam;

    // �ɱ༭��Ϣ���ݲ��Ҹ��ĳ���ʱʹ��
    std::vector<EditableItemInfo>    m_vecEdit;

    //>����ģʽ���ڱ�������ص����Ƿ���Ҫ�ϴ�
    std::atomic<bool>                m_bNeedUpload;

private:
    bool							m_extractTex;
    QString							m_extractPath;
	DockPropertyEditor				*DockScaleIndex;

public:
	QWidget* GetPropertyEditorWidget();
    QWidget* GetDockToolBoxProperty();
    // ������Ч�������ӿ�
    void AddTexEffAnimator(HNode hNode, E_TexEff_App_Type texEffAppType);
};

#endif // L3DARTIST_H