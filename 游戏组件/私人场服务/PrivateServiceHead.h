#ifndef PRIVATE_SERVICE_HEAD_HEAD_FILE
#define PRIVATE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef PRIVATE_SERVICE_CLASS
	#ifdef  PRIVATE_SERVICE_DLL
		#define PRIVATE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PRIVATE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateService.dll")			//组件名字
#else
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class PriaveteGame;

//////////////////////////////////////////////////////////////////////////////////
//比赛参数
struct tagPrivateManagerParameter
{
	//配置参数
	tagGameServiceOption *			pGameServiceOption;					//服务配置
	tagGameServiceAttrib *			pGameServiceAttrib;					//服务属性

	IGameServiceManager *			m_pIGameServiceManager;				//服务管理

	//内核组件
	ITimerEngine *					pITimerEngine;						//时间引擎
	IDBCorrespondManager *			pICorrespondManager;				//数据引擎
	ITCPNetworkEngineEvent *		pTCPNetworkEngine;					//网络引擎

	//服务组件
	IAndroidUserManager *			pIAndroidUserManager;				//机器管理
	IServerUserManager *			pIServerUserManager;				//用户管理
	IMainServiceFrame *				pIMainServiceFrame;					//服务框架
	IServerUserItemSink *			pIServerUserItemSink;				//用户接口
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPrivateServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IPrivateServiceManager={0xd1234ace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IPrivateServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IPrivateServiceManager={0xd12342ce,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
#endif

//比赛服务器管理接口
interface IPrivateServiceManager : public IUnknownEx
{
	//控制接口
public:
	//停止服务
	virtual bool StopService()=NULL;
	//启动服务
	virtual bool StartService()=NULL;

	//管理接口
public:
	//创建比赛
	virtual bool CreatePrivateMatch()=NULL;
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wChairID)=NULL;
	//初始化接口
	virtual bool InitPrivateInterface(tagPrivateManagerParameter & MatchManagerParameter)=NULL;	
	
	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;

	//网络事件
public:
	//私人房事件
	virtual bool OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	

	//用户接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//进入
	virtual bool OnEventEnterPrivate(DWORD dwSocketID, VOID* pData, DWORD dwUserIP, bool bIsMobile)=NULL;	
	//用户参加
	virtual bool OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)=NULL;
	//用户退出
	virtual bool OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD)=NULL;

	//用户请求
public:
	//
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem)=NULL;

	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink()=NULL;
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGamePrivateItem INTERFACE_VERSION(1,1)
static const GUID IID_IGamePrivateItem={0xd5131234,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IGamePrivateItem INTERFACE_VERSION(1,1)
static const GUID IID_IGamePrivateItem={0x7d381234,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
#endif

//游戏比赛接口
interface IGamePrivateItem : public IUnknownEx
{
	//控制接口
public:
	//启动通知
	virtual void OnStartService()=NULL;

	//管理接口
public:	
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)=NULL;
	//初始化接口
	virtual bool InitPrivateInterface(tagPrivateManagerParameter & MatchManagerParameter)=NULL;	
	
	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;

	//网络事件
public:
	//比赛事件
	virtual bool OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	

	//信息接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//进入事件
	virtual bool OnEventEnterPrivate(DWORD dwSocketID, VOID* pData, DWORD dwUserIP, bool bIsMobile)=NULL;	
	//用户参赛
	virtual bool OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)=NULL;
	//用户退赛
	virtual bool OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD)=NULL;

	//用户请求
public:
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPrivateEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPrivateEventSink={0x9d49ab20,0x472c,0x4b3a,0x00bc,0xb4,0x92,0xfe,0x8c,0x41,0xcd,0xaa};
#else
#define VER_IPrivateEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPrivateEventSink={0x91cf29a0,0x04d3,0x48da,0x0083,0x36,0x64,0xb2,0xda,0x6a,0x21,0xdb};
#endif

//游戏事件
interface IPrivateEventSink :public IUnknownEx
{
	//游戏开始
	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
	//游戏结束
	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//游戏记录
	virtual bool WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount, datastream& kData) = NULL;	
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) = NULL;
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户同意
	virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
	//用户进去游戏
	virtual bool OnEventClientReady(WORD wChairID,IServerUserItem * pIServerUserItem)=NULL;

	//私人场用户事件
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex)=NULL;
};

///////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_ITableFramePrivate INTERFACE_VERSION(1,1)
static const GUID IID_ITableFramePrivate={0xe9f19de8,0xfccb,0x42bd,0x0099,0x85,0xac,0xe9,0x26,0xf3,0xc4,0x2b};
#else
#define VER_ITableFramePrivate INTERFACE_VERSION(1,1)
static const GUID IID_ITableFramePrivate={0x94a90d45,0x570e,0x41d6,0x009f,0x20,0x01,0x8d,0x68,0x16,0xd5,0x0f};
#endif

//桌子钩子接口
interface ITableFramePrivate : public IUnknownEx
{
	//管理接口
public:
	//设置接口
	virtual bool  SetPrivateEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//初始化
	virtual bool  InitTableFramePrivate(IUnknownEx * pIUnknownEx)=NULL;
	//写牌局信息
	virtual bool WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,datastream& kData)=NULL;	
	//用户进去游戏
	virtual bool OnEventClientReady(WORD wChairID,IServerUserItem * pIServerUserItem) = NULL;


	//游戏事件
public:
	//游戏开始
	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
	//游戏结束
	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//用户事件
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_HELPER(PrivateServiceManager,PRIVATE_SERVICE_DLL_NAME,"CreatePrivateServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif