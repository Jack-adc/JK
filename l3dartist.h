/*
* Copyright (c) LTECH Corp. 2012-2020. All Rights Reserved.
* 
* Authors: CaiYangLe, 2013.7
* Version: 1.0.0.0
*
* Purpose: Artist主窗口，使用单文档
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
#include "ScenePack.h"//added by xyc 为了添加导出场景中的测试用函数中的eScenePackStatus
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

	//> 节点控制模式
	enum NodeCtrl
	{
        //> 初始/清空
        NODECTRL_INIT,
		//> 移动
		NODECTRL_MOVE,
		//> 旋转
		NODECTRL_ROTATE,
		//> 缩放
		NODECTRL_SCALE,
        //> 修改尺寸
        NODECTRL_SIZE
	};

	//定义不同控件的焦点
	enum FocusXYZ
	{
		//> 主面板中X轴输入框
		FocusXYZ_X=1,
		//> 主面板中Y轴输入框
		FocusXYZ_Y=2,
		//> 主面板中Z轴输入框
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
	//> 获取当前场景
	HScene GetCurScene() const { return CurOpenScene_; }
	//> 获取RenderManager
	HRenderManager GetRenderManager() {return RenderManager_;};

	//> 获取当前场景的focus节点
	HNode GetCurFocusNode();
    //> 获取当前场景选中的节点
    HNode GetCurSelectedNode();
	std::vector<HNode> GetCurSelectedNodes();
	//> 设置当前场景的focus节点
	void SetCurFocusNode(HNode node);
	//> 获取当前操作方式
	NodeCtrl GetCurNodeCtrl() const { return m_eNodeCtrl; }
	//> 获取当前节点材质索引
	int GetCurMateIndex() const { return m_nCurMaterial; }
	//> 获取当前节点纹理索引
	int GetCurTexIndex() const { return m_nCurTexture; }
	//> 获取工程名
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
    // 是否为单机模式
    //bool isLocalUpload() const;
    // 是否为演播室模式
   // bool isStudio() const;
	const QString& GetCurSceneListPath() const { return m_strSceneListPath; }
	//> 获取素材路径 added by yejia 2014-12-23
	const QString& GetCurMediaPath() const{ return m_strMediaPath;} 
	//> 获取菜单栏指针
	QMenuBar *GetMenuBar() { return menuBar(); }
	//> 获取状态栏指针
	QStatusBar *GetStatusBar() { return statusBar(); }

	QString getLocalLangue();
    //> 翻译		by ShenX
	void VideoTextureChange();
    void TranslateLanguage();
	void setOwner(QtSingleApplication* pOwner);
    //> 查找工程目录下文件是否有冲突,有冲突则返回冲突文件全路径;否则返回false为空
    //  files : QMap<文件名,文件路径> ; suff : 查找同名文件后缀; dir : 查找路径,默认为空,表示当前工程路径下; 
    bool checkConfict(QMap<QString,QStringList>& files = QMap<QString,QStringList>(), QString suff = "l3d", QString dir = QString());
    //> 查找工程目录下有没有对应文件的冲突,initFile:待检测文件全路径
    QString checkfileConfict(const QString& initFileName, QString dir = QString());

	static bool GetTanslateStatus();
    //> 创建纹理    by ShenX
    void CreateSolidColor();
    void CreateGradient();
	void CreatelLightPass();
	// 创建倒角
	QString CreateBevel(QString selectedType = "");
	void CreatePolygon();
    bool PreClose();
    void SetFocusDockWidget(l3dDockWidget* dw);
    void RenameSceneInEdit(QString qstr);
    bool IsPlaying();
    void OpenSceneTemplete(QString qstr);
    //> 更新动画器
    void UpdateNodeAnimator(HNode node);
    void RulerMoved(long pos);
    // 编辑扩展材质
    void CreateExtMaterial();
    // 特效动画器关键帧时长改变
    void TexEffectTimeChange(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    void UpdateTexEffectTime(HNode hNode, E_ANIMATOR_TYPE animatorType);
    // 特效动画器删除时，删除对应的特效类型
    void DeleteTexEffect(HNode hNode, E_ANIMATOR_TYPE animatorType);
private:
	QtSingleApplication* m_pOwner;
	QTranslator* translator_cn;
	QTranslator* translator_default;
	static bool m_bTranslate;

private:
	// 工具栏图标，修改为成员变量
	ToolButton *pTBtnNewScene;
	ToolButton *pTBtnDeleteScene;
	ToolButton *pTBtnImportScene;
	ToolButton *pTBtnExportScene;
	ToolButton *pTBtnSaveScene;
    ToolButton *pTBtnUploadScene;                   //场景上传按钮
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
    QCheckBox* pCheckBoxSync;                 //精简模式下是否同步修改制作模式下的场景

	// 工具栏纹理图标
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
	//用于存储工具栏纹理
	QList<ToolButton*>	TextureList;
	//> 存放全部纹理部分工具栏名称
	QStringList			strlistTex;
	//> 存放当前纹理部分工具栏名称,需实时更新
	QStringList			strlistTexDef;
	QButtonGroup		*pTextureToolBtnGroup;
	QHBoxLayout			*pHLayoutTexture;
	QFrame				*pFrameTexture;

	/******************************************渲染窗口对齐功能启用*********************************
	//渲染窗口对齐功能
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
	/******************************************渲染窗口对齐功能启用*********************************/

public: // AsynOperating 相关接口
	CAsynOperatingBassPtr	GetOperating();
	void setOperating(CAsynOperatingBassPtr pdata);
	//added by xyc 2014.07.10更新设置标题栏信息
	void setToolBoxTitleBar(l3dDockWidget::ToolBoxType type, const QString &strDirPath);
	//判断输入字符是否合法 add by xyc
	bool isLegal(QString &strName);
	// added by xyc 2014.08.12 设置工具栏纹理图标的显示
	void SetToolBarTexturePic(QStringList strlist);
	void ToolBarTextureBtnInit();
	void InitToolBarTexture();
	double GetStepValue(int);
	//> 设置在移动状态下的X轴发生变化时设置上次焦点记录及其他功能（可扩充）
	void setMoveFunction_X(vector3df, HNode, double);
	//> 设置在移动状态下的X轴发生变化时设置上次焦点记录及其他功能
	void setMoveFunction_Y(vector3df, HNode, double);
	//> 设置在移动状态下的X轴发生变化时设置上次焦点记录及其他功能
	void setMoveFunction_Z(vector3df, HNode, double);
	void setRotateFunction_X(vector3df, HNode, double);
	void setRotateFunction_Y(vector3df, HNode, double);
	void setRotateFunction_Z(vector3df, HNode, double);
	//> 设置在缩放状态下的X轴发生变化时，进行的一些设置，包括设置上次焦点记录，值的赋值
	void setScaleFunction_X(vector3df,HNode,double);
	//> 设置在缩放状态下的Y轴发生变化时，进行的一些设置，包括设置上次焦点记录，值的赋值
	void setScaleFunction_Y(vector3df, HNode, double);
	//> 设置在缩放状态下的Z轴发生变化时，进行的一些设置，包括设置上次焦点记录，值的赋值
	void setScaleFunction_Z(vector3df, HNode, double);
	void comboBoxMoveSetting(NodeCtrl nodectrl);
	void comboBoxRotateSetting(NodeCtrl nodectrl);
	void comboBoxScaleSetting(NodeCtrl nodectrl);

	

signals:
	void SglClearTexurePartToolBar();
	void SglUpdateTexurePartToolBar(const tstring &strNodeId);
	//> 更新时对当前项选中,外部对其更新,int为当前纹理工具栏的id值
	void SglSelectTexurePartToolBar(int);
	//> 工具栏中按钮被按下,对外部进行更新,int为当前节点当前纹理部分index值
	void SglTextureToolBtnGroupPressed(int);
    //> 精简编辑模式下,发送隐藏信号
    void SglHideArtistEdit();
	//> 精简编辑模式下,发送场景保存与上传完成通知信号
	void SglArtistEditNotice();
	//> 设置步进值向参数面板的位置参数发送信号
	void SglMgrSingleStep(double dStep, NodeCtrl nodectrl);
	//> 设置缩放值向参数DOCK面板的缩放XYZ参数发送信号
	//void SglMgrScaleValue(double X, double Y, double Z);


private slots:
	//> 纹理部分工具栏禁用/不显示
	void SlotClearTexurePartToolBar();
	//> 纹理部分工具栏使能更新
	void SlotUpdateTexturePartToolBar(const tstring& strNodeId);
	//> 响应更新当前选中项,int为当前纹理部分工具栏的id值
	void SlotSelectTexurePartToolBar(int);
	//> 响应纹理工具栏按下,int为当前纹理部分工具栏的id值
	void SlotTextureToolBtnGroupPressed(int);
    //> 响应导出状态改变
    void SlotSetToolBarStatusPic(int selsize);

private:
	//> 创建纹理部分工具图标
	void CreateTexureToolButton(ToolButton** ToolBtn, QString StrObjectName, const QString StrTipName, int GroupId);
    //> 播放/停止状态
    void changePlayState(bool isPlay);
        
public slots:
	//> 设置当前节点操作状态
	void SlotSetCurNodeCtrl();
	//> 显示隐藏进度条
	void SlotShowProgress(bool bIsShow);
    //> 关闭之前
    void SlotPreClose();
	void SlotStartVOS(int type, QString path, bool field, long length);
	void SlotStopVOS();
    //> 更新控制点
    void SlotUpdateCtrlNode(const tstring& nodeid,E_ANIMATOR_TYPE animatorType);
    //> 精简编辑模式下打开场景,由外部信号触发
    void SlotOpenSceneSimple();
    //> 精简编辑模式下上传场景
    void SlotUpload();
    //> 精简编辑模式下关闭场景，不给出提示
    void SlotCloseSceneSimple();
    //> 保存并上传槽函数
    void SlotSaveAndUpload();
    //>  精简编辑模式下隐藏主窗口
    void SlotHideEdit();
	//> 选择位移的步进大小
	void SlotSetSpinSingleStep(int index);
	//> 选择在旋转的步进大小
	void SlotSetRotateSingleStep(int index);
	//> 设置在位置移动情况下的上一次步进的index的值
	void SlotSetMoveIndex(int index);
	//> 设置在旋转情况下的上一次步进的Index的值
	void SlotSetRotateIndex(int index);
	//> 用于设置在缩放情况下存放上一次index值
	void SlotSetScaleIndex(int index);
	//> 用于设置选择ComboBox后的焦点在哪
	void SlotSetFocusXYZ(int);
	
    //> 设置特效动画器关键帧位置
    void SlotSetTexEffKeyFrameTime(E_ANIMATOR_TYPE eAnimatorType, long iTimeLength);
    //> 通过删除属性参数特效删除特效动画器
    void SlotDeleteTexEffAnimator(HNode hNode, E_ANIMATOR_TYPE eAnimatorType);

private: //定义 主界面当前操作状态
	CAsynOperatingBassPtr		AsynOperating_;

protected:
	//> 关闭事件
	virtual void closeEvent(QCloseEvent *pEvent) override;
	//> 鼠标进入事件
	virtual void enterEvent(QEvent *pEvent) override;
	//> 事件过滤器
	virtual bool eventFilter(QObject *pObject, QEvent *pEvent) override;
    //> 按键事件
    virtual void keyPressEvent(QKeyEvent *pEvent) override;

private: //主界面，添加 功能DLL 相关设置函数
	 //初始化DLL
	bool InitDll(QDBTOOLS::Logger::LogInitParams* logParams);
	bool UninitDll();
	//获取dll全局相关管理器
	bool GetDLLManager();
	//初始化相关的管理器.
	bool InitManager();
	// 注册QT 信号类型函数
	void RegisterMetaType();
    //> 重置dll的工程设置
    void resetDllProject();
    //> 重置渲染窗口分辨率
    void resetPwnResolution(/*CardInitParam& param*/);
	// 从备份文件中恢复场景文件，ArtistEdit是有效
	void restoreSceneFile();

private:
	//> 创建和变更当前场景的界面更新函数
	void UpdateCurSene(HScene pScene);
	//> 场景节点变更的 界面更新函数
	void UpdateNodeChange();
	//> 根据序列化结果保存场景文件
	void SaveHScene(const QString &SceneName, const QString &xml);
	// addded by taiyue
	void setMoveFocusXYZ();
	void setRotateFocusXYZ();
	void setScaleFocusXYZ();

private: //dll的回调接口
	std::shared_ptr<INotifier>				INotifierPtr_;					// 全局操作回调接口
	std::shared_ptr<ISceneOperateCB>		ISceneOperateCBPtr_;			// 场景操作回调接口
	std::shared_ptr<INodeOperateCB>			INodeOperateCBPtr_;				// 节点操作回调接口

private: //保存管理句柄
	HSceneManager							SceneManager_;					//场景管理句柄
	HAudioManager							AudioManager_;					//音频输入、输出管理句柄
	HCGCardManager							CGCardManager_;					//字幕卡管理句柄
	HRenderManager							RenderManager_;					//渲染管理句柄
	//HSceneLibMgr							ScenePathMrg_;					//场景路径管理句柄
	HCGCardManager							card_output_;

//操作相关的数据返回 保存
	HScene									CurOpenScene_;					//当前打开的场景
	int										secAniSeq_;						//当前播放的分段动画序号
	//QString									sceneXML_;						//场景序列化，结果字符串
	QString									TextureXML_;					//纹理序列化，结果字符串
	QString									CtrlSerialize_;					//控制点 序列化的结果 保存
	volatile bool							CheckOpenFile_;					//用于标识 当前是否是打开文件，（如果是打开文件，不需要在保存）
	bool                                    m_isResetProject;               //用于标识 是否为重置工程而关闭当前场景
signals:
	//> 仅更新基本属性及扩展属性
	void SglUpdateBaseProperty();
	//> 仅更新对象属性
	void SglUpdateExtProperty();
	//> 对象属性使能,暂禁用 added by yejia 2014-10-16 
	void SglUpdateExtProp(QString gropName,QString propName);
	//> ////////////////////////////////////////////////////////////////////////////////////////
	//> 回调函数发射信号，通知需要更新状态的子窗口更新状态
	//> ////////////////////////////////////////////////////////////////////////////////////////
	//> 清空对象树,时间轴选中项,清空属性库
	void SglClearSceneTreeSel();
	void SglClearTimeLine();
	//> 清空属性库所有属性,每项更新均自带清空,一般不调用
	void SglClearProperty();
	//> 更新对象树,时间轴,更新属性库,焦点设为当前焦点项
	void SglUpdateSceneTree();
	void SglUpdateTimeLine();
	//> 更新当前显示页面属性
	void SglUpdateProperty(const tstring &strNodeId,const ENUM_ITEM_TYPE& itemType,const int& itemIndex);
	//> 对象树,时间轴添加选中项,ture为选中,false为落选
	void SglSceneTreeSelectNode(const tstring &strNodeId, bool bIsSel);
	void SglTimeLineSelectNode(const tstring &strNodeId, bool bIsSel);

	//> 场景文件保存
	void SglSceneFileSave(const QString &filename, const QString &xmlStr);
	//> 视频纹理改变,仅时间轴鼠标拖动时发出
	void SglVideoTextureChanged();
	//> 更新Node位置属性
	void SglNodeMove();
	//> 更新Node旋转属性
	void SglNodeRotation();
	//> 更新Node缩放属性
	void SglNodeScale();
	//> 更新节点控制窗口
	void SglUpdateAni();
	//> 更新单个节点动画器相关
	void SglUpdataNodeAni(HNode node);
	//> 更新场景列表
	void SglUpdateSceneList();
	//> 场景控制点创建成功后更新控制点显示
	void SglUpdateCtrlPoint();
	//> 动画播放更新时间轴指针位置
	void SglUpdateCursorPos(long lPosition);
	//> 删除动画器更新
	void SglRemoveAni(HNode node, E_ANIMATOR_TYPE eAniType = EAT_UNKNOWN);

	//> 更新过光组属性
	void SglUpdateLightPass(LightPassPtr pLP, bool isEnable);
    //> 节点重命名
    void SglUpdateNodeName(const tstring &strNodeId, const QString &strNodeName);
    //> 删除关键帧
    void SglRemoveKeyFrame(KeyFramePtr pKF);
    //> 删除控制点
    void SglRemoveCtrlPoint(CtrlPointPtr pCP);
	//> 关闭场景清除控制点
	void SglClearCtrlPoint();
    //> 更新材质组属性
    void SglUpdateMaterialGroup(int nCur);
	//> 当节点的关键帧位置改变需要重新打点,added by wangww 2014.3.3
	void SglReKeyFram(HNode node);
	//> 输出视频相关
	void SglVOS_Start();
	void SglVOS_End();
	void SglVOS_Process(long frame);
    // 更改工程路径
    void SglUpdateProjectPath();
	//> 部分选项禁用及使能,用于初始化及关闭时//added by xyc
	void SglActionEnable(bool isEnable);
	//> 属性框禁用及使能,用于初始化及关闭时
	void SglPropertyEnable(bool isEnable);
	//> 当前纹理部分工具栏发出改变信号改变属性库中纹理部分设值
	void SglSelectedToolBarTexture(int Index);
	//added by xyc 2014.09.18
	void SglUpdateKeyFrameSetXYZ(double dX, double dY, double dZ, NodeCtrl nodectr);
    //> 改变当前工程
    void sglsetProject(const QString& proj);
    //> 属性页音频属性变更通知 NodeItem 图标做相应变化
    void SglUpdateAutio(const tstring &strNodeId, bool bValue);
    //>  隐藏预览窗口标尺项
    void sglClearAllRulerItem();
    //>  显示预览窗口标尺项
    void sglShowAllRulerItem();

	void sglDBStatusChanged(E_DB_STATUS status);
    //> 精简编辑模式下场景保存成功后,上传信号
    void SglUploadSimple();

	//> 创建纹理特效
	void SglCreateTextureEffect(HNode node, AppTexturePtr pATex, TexEffectPtr pTexEff, LEKResult rslt);
    //> 创建特效动画器
    void SglCreateTexEffAnimator(HNode hNode, E_TexEff_App_Type texEffAppType);
    //> 特效动画器关键帧时长改变
    void SglTexEffTimeChange(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    //> 特效动画器关键帧时长改变的更新
    void SglUpdateTexEffTime(HNode hNode, E_ANIMATOR_TYPE animatorType);
    void SglSetTexEffKeyFrameTime(HNode hNode, E_ANIMATOR_TYPE animatorType, long nTimeLength);
    // 删除特效时删除动画器
    void SglDeleteTexEffAnimator(HNode hNode, E_ANIMATOR_TYPE eAnimatorType);
    // 删除特效动画器时删除特效
    void SglDeleteTexEffect(HNode hNode, E_ANIMATOR_TYPE animatorType);

private slots:
#ifndef FOR_XOR_CO_VER
	// 修改密码，网络字幕下使用
	void SlotModifyPwd();
#endif
	//> 将新建，删除场景等的实际操作函数放在主界面类，是为了方便信息槽发送
	//> 新建场景
	void SlotNewScene();
    //> 导入节点
    void SlotImportNode();
	//> 导入场景
	void SlotImportScene();
	//> 打开场景文件
    void SlotOpenSceneFile(const QString &strFileName);
	//> 清除xyz框为0
	void SlotClearSpinBox();
	//> 删除场景
	void SlotRemoveScene(QString strFileName);
	//> 输出视频
	void SlotOutputVideo();

	//> 切换语言
	void SlotChangeLanguage();
	//> 导出场景
	void SlotExportScene();
	//> 关闭场景
	void SlotCloseScene();
	//> 保存场景
	void SlotSaveScene();
	//> 另存为场景
	void SlotSaveAsScene();
	//> 提取节点
	void SlotExtractNode();
	//> 提取场景
	void SlotExtractScene();
	//> 复制材质
	void SlotCopyMaterial();
	//> 粘贴材质
	void SlotPasteMaterial();
	//> 删除材质
	void SlotRemoveMaterial();
	//> 提取材质
	void SlotExtractMaterial();
	//> 复制纹理
	void SlotCopyTex();
	//> 粘贴纹理
	void SlotPasteTex();
	//> 删除纹理
	void SlotRemoveTex();
    //> 提取纹理
    void SlotExtractTexture();
	//> 列阵复制
	void SlotArrayCopy();
	//> 转化为3D文字槽函数
	void SlotConvert3DText();
	//> 转化为2D文字槽函数
	void SlotConvert2DText();
	//> 提取动画槽函数
	void SlotExtractAni();
	//> 退出
	void SlotExit();
	//> 关于
	void SlotAbout();
    // 快捷键
    void SlotShortCut();
	// 场景层级
	void SlotSceneLayer();
    // 背景颜色
    void SlotBackgroundColor();
    // 显示安全区域
    void SlotShowSafe();
    // 显示标题安全区域
    void SlotShowTitleSafe();
    // 显示标尺
    void slotDrawRulers();
    // 修改标尺
    void slotSetRulers();
    // Tga
    void SlotTga();
	//> Undo槽函数
	void SlotUndo();
	//> Redo槽函数
	void SlotRedo();
	//> 创建节点槽函数
	void SlotCreatNode(E_L3DNODE_TYPE type, void *param, HNode parent);
	//> 场景树中item被点击接收node槽函数
	void SlotSceneTreeItemCk();
	//> 更新Undo  -redo 系统 
	void SlotUpdataUdRdSystem(const QStringList &UndoList, const QStringList &RedoList);
	//> 改变渲染窗口大小调用
	void SlotResizePlayWnd(int x, int y, int width, int height, bool repaint);
	//> 更新当前打开文件信息
    void SlotUpdateCurOpenFile(const QString &strFileName);
	//> 控制输入框数值更新,响应由属性库发出修改信号
	void SlotUpdateToolBarXYZ(double dX, double dY, double dZ, NodeCtrl nodectrl);//modify by xyc 2014.09.18为了给关键帧设置窗口提供判断依据
	//> 改变当前节点操作模式
	void SlotSetNodeCtrl(NodeCtrl nodectrl);
    //> 复位
    void SlotBtnResetOnClick();
	//> 点击播放按钮
	void SlotBtnPlayOnCli();
    //> 点击循环按钮
    void SlotBtnLoopOnCli();
    void SlotBtnPreOnCli();
    void SlotBtnBackOnCli();
    void SlotBtnForwardOnCli();
    void SlotBtnNextOnCli();
	//> x更改
	void SlotValueChangeX(double dValue);
	//> y更改
	void SlotValueChangeY(double dValue);
	//> z更改
	void SlotValueChangeZ(double dValue);
    //> for xor video card test. added by CaiYL 2014.02.18
//     void SlotVideoSet(bool bIsCheck);
	//>更新当前场景目录，added by wangww 2014.4.10
	void SlotUpdateCurSceneDir(const QString &strFileName);
    // 更改工程相关设置
    void SlotProjectSet();
    // 更改FTP设置
    void SlotFtpSetting();
	//> 部分选项禁用及使能,用于初始化及关闭时added by xyc
	void SlotActionEnable(bool isEnable);
    //> 修改字体
    void slotChangeSizeMsg(QAction* act);
    //> 更新时间轴
    void SlotUpdateAni();
    //> 更新开启音频信息    added by wuheng 2016.05.20
    void SlotUpdateEnableAudio(const tstring& strNodeId, bool bValue);
    void SlotUpdateAudioToNodeItem(const tstring& strNodeId, bool bValue);
    //> 打开漫色或单色设置对话框 added by wuheng 2016.10.08
    void SlotOpenGradientOrSolid();
    //> 设置字幕卡参数相关 added by wuheng 2016.12.12
    void SlotSetCard();
	/******************************************渲染窗口对齐功能启用*********************************
	//渲染窗口对齐功能按钮响应函数
	void SlotAlignX();
	void SlotAlignY();
	void SlotAlignZ();
	/******************************************渲染窗口对齐功能启用*********************************/
    void SlotSetAlignBtnEnable(bool isMulSel);
	void SlotAside();
private slots:
	/********************************************************************************/
	//> Notifier回调接口信号接收槽
	/********************************************************************************/
	//> 场景实时渲染控制回调信号接收槽
	void SlotOnRenderSceneOperate(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic, RENDER_OPERATE ro);
	//> 录制场景渲染视频结束回调信号接收槽
	void SlotOnEndSaveVOSFile(HRenderManager hRM, LEKResult rslt, const QString &fileNamePath, L3DEngineKernel::IContext *pic);
	//> 录制场景渲染视频进度回调信号接收槽
	void SlotOnSaveVOSProcess(HRenderManager hRM, long frame);
	//> 字幕卡初始化反初始化结束回调信号接收槽
	void SlotOnCGCInitOperate(HCGCardManager hCGCM, LEKResult rslt, L3DEngineKernel::IContext *pic, CGCIS_INIT ci);
	//> 字幕卡开启关闭结束回调信号接收槽
	void SlotOnCGCChannelOperate(HCGCardManager hCGCM, E_CGC_CHANNEL_TYPE chType, unsigned int chIdx, L3DEngineKernel::IContext *pic, CGC_OPEN_CLOSE coc);
	//> 字幕卡设置后回调信号接收槽
	void SlotOnCGCardSetuped(HCGCardManager hCGCM, const QString &paramsXML, L3DEngineKernel::IContext *pic);
	//> 场景对象创建动作回调信号接收槽
	void SlotOnSceneCreated(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic);
	//>场景对象打开动作回调信号接收槽
	void SlotOnSceneOpened(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic);
	//> 场景对象保存动作回调信号接收槽
	void SlotOnSceneSaveFile(HSceneManager hSM, LEKResult rslt, HScene scene, const QString &filename, L3DEngineKernel::IContext *pic);
	//> 场景对象操作回调信号接收槽
	void SlotOnSceneOperate(HSceneManager hSM, LEKResult rslt, HScene scene, L3DEngineKernel::IContext *pic, SCENE_OPERATE so);
	//> 场景管理器对象反初始化回调信号接收槽
	void SlotOnSMUnInitialized(HSceneManager hSM, LEKResult rslt, L3DEngineKernel::IContext *pic);
    //> 场景模板
    void SlotOnSceneTempleteOpened(HSceneManager hSM, LEKResult rslt,  const QString& filename);
	//> Undo/Redo回调信号接收槽
	void SlotOnObjectUpdate(unsigned int canUndoNum, unsigned int canRedoNum, const QString &ucDescribe, const QString &rcDescribe);
	//> 渲染管理句柄对应内部对象销毁回调信号接收槽
	void SlotOnDestoryMgr(L3DEngineKernel::IContext *pic, DESTROY_MGR dm);
	//> renderManager 初始化回调信号接收槽
	void SlotOnOnRMgrInitialized(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic);
	//> enderManager反初始化回调信号接收槽
	void SlotOnRMgrUnInitialized(HRenderManager hRM, LEKResult rslt, L3DEngineKernel::IContext *pic);

	/********************************************************************************/
	//> NodeOperateCB回调接口信号接收槽
	/********************************************************************************/
	//> 节点对象的父节点属性改变后回调信号接收槽
	void SlotOnParentUpdate(HNode node, HNode new_p, HNode old_p, LEKResult rslt);
	//> 节点操作回调信号接收槽
	void SlotOnNodeOperate(HNode node, HNode clone, LEKResult rslt, NODE_OPERATE no);
	//> 节点对象序列化结束后回调信号接收槽
	void SlotOnNodeSerialized(HNode node, const QString &nodeXML, LEKResult rslt);
	//> 节点可见性属性调用回调信号接收槽
	void SlotOnNodeVisible(HNode node, bool isVisible, LEKResult rslt);
	//> 节点变换回调信号接收槽
	void SlotOnNodeTransform(HNode node, ic::vector3df pos, LEKResult rslt, NODE_TRANSFORM nt);
	//> 节点爆炸参数改变回调信号接收槽
	void SlotOnNodeExplosion(HNode node, int rate, float range);
	//> 节点模型变形相关数据产生变化回调信号接收槽
	void SlotOnNodeMeshDeform(HNode node, MeshDeformPtr pMD, bool isTypeModi);
	//> 节点模型变形部分序列化结束后回调信号接收槽
	void SlotOnNodeMDSerizlized(HNode node, const QString &MDXML, LEKResult rslt);
	//> 节点模型变形部分反序列化结束后回调信号接收槽
	void SlotOnNodeMDDeserialized(HNode node, MeshDeformPtr pMD, LEKResult rslt);
	//> 节点材质相关参数改变后回调信号接收槽
	void SlotOnMaterialUpdate(HNode node, iv::SMaterial mat, unsigned int num);
	//> 材质对象创建后回调信号接收槽
	void SlotOnMaterialCreated(HNode node, iv::SMaterial mat, LEKResult rslt);
	//> 节点材质序列化后回调信号接收槽
	void SlotOnMaterialSerialized(HNode node, const QString& xmlstr, LEKResult rslt);
	//> 基本纹理对象创建后回调信号接收槽
	void SlotOnTextureCreated(HNode node, TexturePtr pTex, LEKResult rslt,int texIndex);
	//> 应用纹理对象创建后回调信号接收槽
	void SlotOnAppTextureCreated(HNode node, AppTexturePtr pATex, LEKResult rslt);
	// >纹理特效创建后回调信号接收槽
	void SlotOnTexEffectCreated(HNode node, AppTexturePtr pATex, TexEffectPtr pTexEff, LEKResult rslt);
	//> 应用纹理对象序列化后回调信号接收槽
	void SlotOnAppTextureSerialized(HNode node, AppTexturePtr pTex, const QString &TexXML, LEKResult rslt);
	//> 应用纹理对象反序列化后回调信号接收槽
	void SlotOnAppTextureDeserialized(HNode node, AppTexturePtr pTex, LEKResult rslt);
	//> 基本纹理对象序列化后回调信号接收槽
	void SlotOnTextureSerialized(HNode node, TexturePtr pTex, const QString &TexXML, LEKResult rslt);
    //> 生成基本纹理缩略图回调信号接收槽
    void SlotOnThumbnail(IImage* image);
	//> 基本纹理对象反序列化后回调信号接收槽
	void SlotOnTextureDeserialized(HNode node, TexturePtr pTex, LEKResult rslt);
	//> 基本纹理设置回调信号接收槽
	void SlotOnTextureSeted(HNode pNode, TexturePtr pTex, LEKResult rslt);
	//> 基本纹理操作回调信号接收槽
	void SlotOnTextureOperate(HNode pNode, TexturePtr pTex, TEXTRUE_OPERATE to);
	//> 过光纹理启用改变回调信号接收槽
	void SlotOnLightPassUpdate(HNode pNode, LightPassPtr pLP, bool isEnable);
	//> 创建动画回调信号接收槽
	void SlotOnAnimatorOperate(HNode pNode, AnimatorPtr pAni, LEKResult rslt, ANIMATOR_OPERATE ao);
	//> 提取动画回调信号接收槽
	void SlotOnAnimatorExtract(HNode node, const QString &anisXML, LEKResult rslt);
	//> 节点动画器对象中关键帧操作回调信号接收槽
	void SlotOnKeyFrameOperate(HNode node, AnimatorPtr pAni, KeyFramePtr pKF, LEKResult rslt, KEYFRAME_OPERATE ko);
	//> 扩展属性更新回调信号接收槽
	void SlotOnExtPropUpdate(HNode pNode, const QString &extCatName, const QString &extProName,bool isReCreate);
	//> 扩展属性更新回调信号接收槽
	void SlotOnKeyFrameTimeUpdate(HNode pNode);
    void SlotOnUpdateBasePara();
    void SlotOnWarningInfo(HNode pNode, LEKResult rslt);
	/********************************************************************************/
	//> SceneOperateCB回调接口信号接收槽
	/********************************************************************************/
	//> 序列化or纹理序列化保存回调信号接收槽
	void SlotOnSerializeTex(HScene pScene, const QString &sceneXML, LEKResult rslt, SERIALIZE_TEX st);
	//> 场景对象反序列化结束回调信号接收槽
	void SlotOnSceneDeserialize(HScene pScene, LEKResult rslt);
	//> 节点操作回调信号接收槽
	void SlotOnSceneNodeOperate(HScene pScene, HNode pNode, LEKResult rslt, NODE_OPERATE no);
	//> 焦点节点更新回调信号接收槽
	void SlotOnFocusOn(HScene pScene, HNode pNode);
	//> 选中节点更换后回调信号接收槽
	void SlotOnSelected(HScene pScene, HNode pNode, bool bIsSel);
	//> 当前活动摄像机被更换后回调信号接收槽
	void SlotOnCameraNodeSet(HScene pScene, HNode pCam, bool bIsLeft);
	//> 控制点操作回调信号接收槽
	void SlotOnCPointOperate(HScene pScene, CtrlPointPtr pCP, LEKResult rslt, CPOINT_OPERATE co);
	//> 控制点设置坐标回调信号接收槽
	void SlotOnCPointSetPos(HScene pScene, CtrlPointPtr pCP, LEKResult rslt);
	//> 控制点序列化后回调信号接收槽
	void SlotOnAllCPSerialize(HScene pScene, const QString &cpsXML, LEKResult rslt);
	//> 控制点反序列化回调信号接收槽
	void SlotOnAllCPDeserialize(HScene pScene, LEKResult rslt);
	//> 控制点对象动作触发后回调信号接收槽
	void SlotOnCPointTriggered(HScene pScene, CtrlPointPtr pCP);
	//> 控制点对象序列化结束回调信号接收槽
	void SlotOnCPointSerialize(HScene pScene, CtrlPointPtr pCP, const QString &cpXML, LEKResult rslt);
	//> 场景播放控制回调信号接收槽
	void SlotOnScenePlayOperate(HScene pScene, long lPosition, LEKResult rslt, SCENE_PLAY sp);
	//> 场景播放时，在时间轴上实时位置通报回调信号接收槽
	void SlotOnSceneCurPosition(HScene pScene, long lPosition);
	//> 场景背景音乐操作回调信号接收槽
	void SlotOnSceneSoundOperate(HScene pScene, SceneSoundPtr pSS, LEKResult rslt, SCENE_SOUND_OPERATE sso);
	//> 场景背景音乐播放回调信号接收槽
	void SlotOnSceneSoundPlay(HScene pScene, SceneSoundPtr pSS, int nPayState, long lCurPos);
    //> 编组回调信号
    void SlotOnAddGroup(HScene pScene, HNode pNode, LEKResult rslt);
    //> 解组回调信号
    void SlotOnReleaseGroup(HScene pScene, HNode pNode, LEKResult rslt);
	//> 响应纹理标题栏单击更新显示
	//void SlotUpdateTexturePart(QString TextureName);

private:
	//> 创建停靠窗口
	void CreateDockWidgets(bool bEditMode);
	//> 创建、修改工具栏
	void CreateToolBar();
	//> 创建、修改状态栏
	void CreateStatusBar();
	//> 创建、修改菜单栏
	void CreateMenuBar();
	//> 建立工具栏按钮，并设置图标，分组
	void SetToolBarIcon();
	//> 建立控制条
	void CreateCtrlBar();
	//> 写入ini配置文件
	void WriteSettings(bool bIsMax);
	//> 读取ini配置文件
	void ReadSettings();
	//> 建立信号槽连接
	void CreateConntent();
	//> LEKResult值判断
	void LEKResultMessage(LEKResult rslt);
	//> 询问是否保存
	bool GetIsMaybeSave();
    //> 条件控制点快捷键触发
    void TriggerShortCutCP(CtrlPointPtr prePtr, CtrlPointPtr nextPtr, CtrlPointPtr curPtr, int key);
	//> 设置当前场景所在工程下的目录   // added by yejia 2014-11-05
	void SetCurSceneDirectory(const QString& scenePath = "");
	//> 复位当前场景目录
	void ResetCurSceneDirectory();
	//> 根据根目录与工程名 创建场景与素材目录 added by yejia 2014-12-23
	void CreateSceneAndMediaDirectory();

	void DBStatusChangedCallback(E_DB_STATUS status);
	void slotDBStatusChanged(E_DB_STATUS status);
	static CLMResult on_switch_db();

    // 提取材质功能使用
    void saveMaterialToXML();
    tstring _materialXML;
    QString _materialXMLPath;
    //int _materialTexNum;
    //std::vector<std::pair<int, int>> _createTextureIndexes;

private:
	Ui::L3DArtistClass ui;

	//> 使用的常数
	enum ARTIST_CONST
	{
		//> 
		FILETOOLBARICON_COUNT = 20,
		ACTION_ICON_HEIGHT = 24,
		ACTION_ICON_WIDTH = 24,
	};

	//> 主预览显示窗口
	PreviewWindow		        *m_pPreviewWnd;

	//> 状态显示label
	QLabel				        *m_pLblStatus;
	//> 状态栏时间显示label
	QLabel				        *m_pLblStatusTime;
	//> 状态栏用户显示label
    RollLabel				    *m_pLblStatusUser;
	//> 状态栏工程名称显示label
    RollLabel				    *m_pLblStatusProject;
	//> 状态栏场景名称显示label
    RollLabel				    *m_pLblStatusScene;
	//> 状态栏控制尺寸label
	QLabel				        *m_pLblSizeGrip;
	//> 属性编辑停靠窗口
	l3dDockWidget			    *m_pDockPropertyEdit;
	//> 场景列表停靠窗口
	l3dDockWidget			    *m_pDockScene;
    DockScene                   *pDockScene;
	//> 动画编辑停靠窗口
	l3dDockWidget			    *m_pDockAnimation;
	//> 场景树停靠窗口
	l3dDockWidget			    *m_pDockSceneTree;
	//> 工具箱停靠窗口
    l3dDockWidget               *m_pDockToolBoxObj;
    l3dDockWidget               *m_pDockToolBoxMaterial;
    l3dDockWidget               *m_pDockToolBoxProperty;

    QList<l3dDockWidget*>       m_lstDockWidget;
	//> 当前场景的存储路径
	QString				        m_strCurScenePath;
	//> 位移步进选择下拉框
	QComboBox*                  m_PDComboxChoose;
	//> x调节框
	SpinBox_ControlBox*			m_pDSpinBoxX;
	//> y调节框
	SpinBox_ControlBox*			m_pDSpinBoxY;
	//> z调节框
	SpinBox_ControlBox*			m_pDSpinBoxZ;
	//> 复位按钮
	ToolButton*					m_pTBtnReset;
	//> 保存当前控制模式
	NodeCtrl			        m_eNodeCtrl;
	//> undo按钮
	ToolButton*					m_pTBtnUndo;
	//> redo按钮
	ToolButton*					m_pTBtnRedo;
	//> 
	static L3DArtist	        *m_pInstance;
	//> 当前节点纹理索引
	int					        m_nCurTexture;
	//> 当前节点材质索引
	int					        m_nCurMaterial;
	//> 当前场景是否为新建场景
	bool				        m_bIsNewScene;
	//> 当前场景是否需要打开
	bool				        m_bIsOpenScene;
	//> 当前工程名
	QString				        m_strProjectName;
	//> 当前工程路径,含工程名
    QString                     m_strProjectPath;
	//> 当前素材路径 added by yejia 2014-12-23
	QString                     m_strMediaPath;
	//> 文件菜单弹出action
	QWidgetAction		        *m_pActionWidgetFile;
	//> 编辑菜单弹出action
	QWidgetAction		        *m_pActionWidgetEdit;

	ToolButton			        *m_pTBtnPre;
	ToolButton			        *m_pTBtnBack;
	//> 播放按钮
	ToolButton			        *m_pTBtnPlay;
	ToolButton			        *m_pTBtnForward;
	ToolButton			        *m_pTBtnNext;
    ToolButton*                 m_pTBtnLoop;
	//> 全屏按钮
	ToolButton			        *m_pTBtnFullScreen;
	//> 状态栏显示进度条
	QProgressBar		        *m_pProgress;
    //> 场景列表当前路径,不包含工程路径,EG:scenes
    QString                     m_strSceneListPath;
    //> 当前场景列表打开场景名
    QString                     m_strCurSceneItemName;
    //> 当前播放帧数
    qint64                      m_nCurPos;
    //> 关闭标志
    bool                        m_closeEvent;
    //> 预关闭标识
    bool                        m_bPreClose;
    //> 当前工程
    QLabel                      *m_pLblProject;
    //> 标题
    QLabel                      *m_pLblTitle;
    //> 当前场景
    QLabel                      *m_pLableScene;
	QWidget						*m_pCentralWidget;

	NodeCtrl					PreNodeType;
	int							m_nFocusXYZ;//用于记录SpinBox中XYZ哪一个发生了变化，1为X,2为Y，3为Z；
	int							m_nGetMoveFocusXYZ;
	int							m_nGetRotateFocusXYZ;
	int							m_nGetScaleFocusXYZ;
	int                         m_nPreMoveStatus;//用于记录在移动模式下上一次的选择的index
	int							m_nPreRotateStatus;// 用于记录在旋转模式下上一次的选择的index
	int                         m_nPreScaleStatus;// 用于记录在缩放模式下上一次的选择的index
	int							m_nCurSceneWidth;
	int							m_nCurSceneHeight;
	int							m_FrameRate;		//帧率，以便artist需要此参数用于界面层帧数的显示灯，在初始化时赋值，added by wangww 2014.6.25

	bool						bHasFocusX;
	bool						bHasFocusY;
	bool						bHasFocusZ;
	bool						bMoveFocusX;		//用于判断当前焦点是否在移动模式下的X轴，若在为true
	bool						bMoveFocusY;		//用于判断当前焦点是否在移动模式下的Y轴
	bool						bMoveFocusZ;		//用于判断当前焦点是否在移动模式下的Z轴
	bool						bRotateFocusX;		//用于判断当前焦点是否在旋转模式下的X轴，若在为true
	bool						bRotateFocusY;		//用于判断当前焦点是否在旋转模式下的Y轴
	bool						bRotateFocusZ;		//用于判断当前焦点是否在旋转模式下的Z轴
	bool						bScaleFocusX;		//用于判断当前焦点是否在缩放模式下的X轴，若在为true
	bool						bScaleFocusY;	    //用于判断当前焦点是否在缩放模式下的Y轴
	bool						bScaleFocusZ;		//用于判断当前焦点是否在缩放模式下的Z轴
    bool                        _isMulSel;
	bool						m_bRMgrInit;		//RenderManager 初始化完成标志
	bool						m_bRMgrUnInit;		//RenderManager 反初始化完成标志
	bool						m_bRecording;
	QString					    localLangue_;		//for 本地语言支持韩文，则需要在中文字体里显示所有韩文字体，added by wangww 2014.7.19
    HCLCommClient               m_ctrlLinkClient;	// 应用模块通迅句柄
    ServerInfoVecPtr            dataSerInfoVec;		// ltech 模式下，存放dataServer ftp信息，正常情况下一个系统只会有一个数据中心服务器，可能为空表示无数据中心服务器

    QString                     m_pExportScenePath; //导出场景路径 便于记录上次打开的路径 added by yejia 2015-02-04
    QList<QString>              m_tempNodeStrList;  // added by wuheng 2016.03.11

/*****************************************发送给对象树响应信号**********************/
signals:
    //> 对新创建的节点进行翻译,参数为空时对初始节点进行翻译,仅在打开场景时使用
    void SglTrNodeName(const tstring& strNodeId = _T(""));
	//> 创建回调中发出,内存中已创建
	void SglInsertNode(const tstring& strParentNodeId, const tstring& strCurNodeId);
	//> 删除回调中发出,内存中已删除
	void SglRemoveNode(const tstring& strNodeId);
	//> 重新显示对象树
	void SglRepaintSceneTree();
	//> 选中节点,内存中未选中
	void SglNodeClick(const tstring& strNodeId);
	//> 落选节点,内存中未选中
	void SglNodeLose(const tstring& strNodeId);
    //登录相关，added by wangww 2015.1.28
    void slgUpateStartDlg(const QStringList& Project);
    void slgSelectStartDlg(const QString&);
	//>向参数面板发送当前ToolBar在缩放模式下的ComboBox的Index值
	void SglSentScaleIndex(int index);
/*****************************************对象树响应槽函数**********************/
private slots:
	//> 响应SceneTree中添加节点
	void SlotAllSelected();
	void SlotClearSelected();
	void SlotNodeClicked(const tstring& strNodeId,const ENUM_ITEM_TYPE& itemType,const int& itemIndex);
	void SlotNodeFocused(const tstring& strNodeId);//仅获取焦点
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
	// 定时刷新节点的位置、缩放、旋转的x,y,z等参数
	void SlotUpdNodeTimer();

/*****************************************功能性函数**********************/
public:
	HNode GetParentNodeByGUID(const tstring strNodeId);

private:
	//> 通过GUID获取当前节点的父节点
	HNode ParentNodeByGUID(const tstring strParentId, const tstring strNodeId);
private:
    void CreateConnectSceneList(DockScene*);
	//> 渲染窗口回调影响属性库,对象树,时间轴;
	//> 对象树操作影响时间轴,时间轴反馈影响对象树;
	//> 对象树操作在响应函数中影响属性库
	void CreateConnectSceneTree(SceneTree*);
	void CreateConnectTimeLine(DockAnimation*, SceneTree*);
	void CreateConnectDockProperty(DockPropertyEditor*, SceneTree*);
	void CreateConnectPreviewWindow(SceneTree*);

    void initial_ol_thread(SDBCPInitParam param, std::wstring user/*,HCLCommClient HCLClient*/);

    // 设置移动、旋转、缩放按钮的使能
    void setSomeBtnEntable();
    void setSomeBtnDisable();

    // 编辑模式下，对界面UI的一些设置
    void initEditMode();

	// added by taiyue
	//设置在位置移动情况下，combox为步进下拉框，缩放情况下为等倍与非等倍缩放
	void setComboxMoveType();
	//>设置在缩放情况下的下拉列表为XYZ，XY,XZ,None
	//> 设置在旋转状态下的部分功能与初始化
	void setRotateType();
	void setComboxScaleType();
	//> 点击Move状态时，设置上一个模式下的combox的下标值
	void setPreMoveComboxIndex();
	void setPreRotateComboxIndex();
	void setPreScaleComboxIndex();


public:
    //ltech方式启动时，获取频道信息线程，通过信号发送获取的信息
    void getAllProject(SDBCPInitParam param, HCLCommClient HCLClient, const std::wstring& user = _T(""));
    //ltech模式下获取中心服务器FTp
    void getFTP(std::vector<SChannelNoCenter> &serverNames);
 
private:
    bool                             m_hasPlayed;
	// 记录是否第一次打开移动，旋转，缩放
	bool							 m_rotateFirstState;
	bool							 m_scaleFirstState;
    QTimer*                          m_checkIsModifiedTimer;
	QTimer*                          m_nodeParamUpdTimer;
	// 命令行启动参数，m_nCmdParam(0,1,2,3,4...)代表使用哪个panel的自定义分辨率设置
	// 用于在演播室模式下的多种自定义分辨率的处理,added by litianming 2016/05/12
	int								 m_nCmdParam;

    // 可编辑信息，演播室更改场景时使用
    std::vector<EditableItemInfo>    m_vecEdit;

    //>精简模式下在保存结束回调中是否需要上传
    std::atomic<bool>                m_bNeedUpload;

private:
    bool							m_extractTex;
    QString							m_extractPath;
	DockPropertyEditor				*DockScaleIndex;

public:
	QWidget* GetPropertyEditorWidget();
    QWidget* GetDockToolBoxProperty();
    // 增加特效动画器接口
    void AddTexEffAnimator(HNode hNode, E_TexEff_App_Type texEffAppType);
};

#endif // L3DARTIST_H