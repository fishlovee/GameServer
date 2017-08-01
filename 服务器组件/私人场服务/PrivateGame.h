#ifndef PRIVATE_HEAD_FILE
#define PRIVATE_HEAD_FILE

#pragma once

//引入文件
#include "CTableFramePrivate.h"
#include "PrivateServiceHead.h"
#include "PrivateTableInfo.h"

///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_DISMISS_WAITE_END		(IDI_PRIVATE_MODULE_START+1)					//请求解散结束

#define DISMISS_WAITE_TIME		30					//请求解散时间

#define AGAIN_WAITE_TIME		30					//重新开始等待时间

struct DBR_GR_Create_Private;

//定时赛
class PriaveteGame 
	:public IGamePrivateItem
	,public IPrivateEventSink
	,public IServerUserItemSink
{
	//比赛配置
protected:
	tagGameServiceOption *				m_pGameServiceOption;			//服务配置
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//服务属性

	CMD_GR_Private_Info					m_kPrivateInfo;
	//内核接口
protected:
	PrivateTableInfo*					m_pTableInfo;				//
	ITimerEngine *						m_pITimerEngine;				//时间引擎
	IDBCorrespondManager *				m_pIDataBaseEngine;				//数据引擎	
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//网络引擎

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口
	IServerUserManager *				m_pIServerUserManager;			//用户管理
	IAndroidUserManager	*				m_pAndroidUserManager;			//机器管理
	IServerUserItemSink *				m_pIServerUserItemSink;			//用户回调

	//函数定义
public:
	//构造函数
	PriaveteGame();
	//析构函数
	virtual ~PriaveteGame(void);

	bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	bool SendTableData(ITableFrame*	pITableFrame, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	void CreatePrivateCost(PrivateTableInfo* pTableInfo);

	bool joinPrivateRoom(IServerUserItem * pIServerUserItem,ITableFrame * pICurrTableFrame);

	bool OnEventCreatePrivate(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,std::string kChannel);

	void sendPrivateRoomInfo(IServerUserItem * pIServerUserItem,PrivateTableInfo* pTableInfo);

	PrivateTableInfo* getTableInfoByRoomID(DWORD dwRoomID);

	PrivateTableInfo* getTableInfoByCreaterID(DWORD dwUserID);

	PrivateTableInfo* getTableInfoByTableID(DWORD dwRoomID);

	PrivateTableInfo* getTableInfoByTableFrame(ITableFrame* pTableFrame);

	void DismissRoom(PrivateTableInfo* pTableInfo);

	void ClearRoom(PrivateTableInfo* pTableInfo);

	void DBR_CreatePrivate(DBR_GR_Create_Private* kInfo,DWORD dwSocketID,IServerUserItem* pIServerUserItem,std::string kHttpChannel);
	//基础接口
public:
 	//释放对象
 	virtual VOID Release(){ delete this; }
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动通知
	virtual void OnStartService();

	//管理接口
public:
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//初始化接口
	virtual bool InitPrivateInterface(tagPrivateManagerParameter & MatchManagerParameter);	

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//私人场消息
	virtual bool OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	
	//创建私人场
	bool OnTCPNetworkSubCreatePrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//重新加入私人场
	bool OnTCPNetworkSubAgainEnter(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//加入私人场
	bool OnTCPNetworkSubJoinPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//解散
	bool OnTCPNetworkSubDismissPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	
	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);	

	//私人场用户事件
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex);

	//事件接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//进入事件
	virtual bool OnEventEnterPrivate(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//用户参赛
	virtual bool OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退赛
	virtual bool OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //功能函数
public:
	 //游戏开始
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //游戏结束
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	 //用户进去游戏
	 virtual bool OnEventClientReady(WORD wChairID,IServerUserItem * pIServerUserItem);

	 //用户事件
public:
	 //用户断线
	 virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	 //用户坐下
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户起来
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户同意
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	
public:
	 //用户起来
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem);

	virtual bool WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,datastream& kData);	
	//辅助函数
protected:
};

#endif