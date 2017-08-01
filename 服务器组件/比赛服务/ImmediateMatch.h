#ifndef IMMEDIATE_MATCH_HEAD_FILE
#define IMMEDIATE_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "MatchServiceManager.h"
#include "ImmediateGroup.h"

#include "..\游戏服务器\DataBasePacket.h"

///////////////////////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+1)					//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+2)					//结束时钟

//移除理由
#define REMOVE_BY_ENDMATCH			250											//比赛结束

///////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//定时器子项
struct tagMatchTimerItem
{
	//DWORD								dwTimerID;						//定时器 ID
	DWORD								dwRealID;						//实际ID
	DWORD								dwRepeatTimes;					//重复次数
	WPARAM								wBindParam;						//绑定参数
};

//比赛奖励
struct tagMatchReward
{
	DWORD								dwGold;							//金币奖励
	DWORD								dwMedal;						//奖牌奖励
	DWORD								dwExperience;					//经验奖励
};

///////////////////////////////////////////////////////////////////////////////////////////
//类型定义

//数组定义
typedef CWHArray<BYTE> CMatchLoopTimerArray;
typedef CWHArray<CImmediateGroup *> CMatchGroupArray;
typedef CWHArray<tagMatchReward*> CMatchRewardArray;
typedef CWHArray<tagMatchTimerItem *> CTimerItemPtr;
typedef CWHArray<CTableFrameHook *> CMatchTableArray;
typedef CWHArray<IServerUserItem *> CMatchUserItemArray;

///////////////////////////////////////////////////////////////////////////////////////////

//即时赛
class CImmediateMatch : public IGameMatchItem,public IImmediateGroupSink,IServerUserItemSink
{
	//变量定义
protected:
	CMatchTableArray					m_MatchTableArray;				//比赛桌子

	//存储变量
protected:
	CMatchUserItemArray					m_OnMatchUserItem;				//参赛用户
	CMatchLoopTimerArray				m_LoopTimer;					//循环定时器

	//比赛分组
protected:	
	CMatchGroupArray					m_MatchGroup;					//进行的比赛组
	CMatchGroupArray					m_OverMatchGroup;				//结束的比赛组
	CImmediateGroup *					m_pCurMatchGroup;				//当前比赛组


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
	CImmediateMatch();
	//析构函数
	virtual ~CImmediateMatch(void);

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

	//用户事件
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

	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);
	//比赛事件
public:
	//开始比赛
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch);
	//比赛结束
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch);
		 
	//定时器接口
public:
	//删除定时器
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch);
	//设置定时器
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch);

	//发送函数
public:
	//发送数据
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem);
	//发送信息
	virtual bool SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch);
	//发送分数
	virtual bool SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch);
	//发送状态
	virtual bool SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch);	
	//发送消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送数据
	virtual bool SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch);
	//发送数据
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//功能函数
public:
	//插入空的桌子
	virtual void InsertNullTable();
	//获取机器人
	virtual IAndroidUserItem * GetFreeAndroidUserItem();	
	//用户淘汰
	virtual bool OnUserOut(IServerUserItem *pUserItem,LPCTSTR szReason,WORD wLen,WORD wRank,CImmediateGroup *pMatch);	
	
	//内部功能
private:
	//获取空的桌子
	WORD GetNullTable();	
	//删除用户
	bool DeleteUserItem(DWORD dwUserIndex);
	//移除参赛用户
	bool RemoveMatchUserItem(IServerUserItem *pIServerUserItem);
	//写入奖励
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
};

#endif