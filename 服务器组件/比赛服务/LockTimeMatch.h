#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

#include "..\游戏服务器\DataBasePacket.h"

///////////////////////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+1)					//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+2)					//结束时钟

//移除理由
#define REMOVE_BY_ENDMATCH			250											//比赛结束

///////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//比赛状态
enum emMatchStatus
{
	MatchStatus_Free=0,
	MatchStatus_Playing,
	MatchStatus_End
};

//////////////////////////////////////////////////////////////////////////////////////////

//定时赛
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,IServerUserItemSink
{
	//状态变量
protected:
	emMatchStatus						m_MatchStatus;					//比赛状态

	//变量定义
protected:
	CDistributeManager					m_DistributeManage;             //分组管理

	//比赛配置
protected:
	tagGameMatchOption *				m_pMatchOption;					//比赛配置	
	tagGameServiceOption *				m_pGameServiceOption;			//服务配置
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//服务属性

	//内核接口
protected:
	ITableFrame	**						m_ppITableFrame;				//框架接口
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
	CLockTimeMatch();
	//析构函数
	virtual ~CLockTimeMatch(void);

	//基础接口
public:
 	//释放对象
 	virtual VOID Release(){ delete this; }
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动通知
	virtual void OnStartService() { return ; }

	//管理接口
public:
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//初始化接口
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//比赛事件
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);

	//事件接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//进入事件
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//用户参赛
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退赛
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //功能函数
public:
	 //游戏开始
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //游戏结束
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	 //用户事件
public:
	 //用户坐下
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户起来
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户同意
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//清除玩家
	void ClearSameTableUser(DWORD dwUserID);
	//插入用户
	void InserSameTableUser(DWORD dwUserID,DWORD dwTableUserID);
	//发送分数
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//移除分组
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//插入用户
	bool InsertDistribute(IServerUserItem * pIServerUserItem);		

	//校验函数
private:
	//参赛校验
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//时间校验
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		
	//发送数据
	bool SendMatchInfo(IServerUserItem * pIServerUserItem);

	//辅助函数
protected:
	//分配用户
	bool PerformDistribute();
	//开始间隔
	DWORD GetMatchStartInterval();	
	//获得名次
	WORD GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame);
	//发送比赛信息
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//写入奖励
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
};

#endif