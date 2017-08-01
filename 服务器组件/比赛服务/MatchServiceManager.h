#ifndef GAME_MATCH_SERVICE_MANAGER_HEAD_FILE
#define GAME_MATCH_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "MatchServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//比赛服务管理类
class MATCH_SERVICE_CLASS CMatchServiceManager : public IMatchServiceManager
{
	//状态变量
protected:
	bool								m_bIsService;					//服务标识	
	
	//接口变量
protected:	
	IGameMatchItem *					m_pIGameMatchItem;				//比赛子项

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口

	//函数定义
public:
	//构造函数
	CMatchServiceManager(void);
	//析构函数
	virtual ~CMatchServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { delete this; }
	//接口查询
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//控制接口
public:
	//停止服务
	virtual bool StopService();
	//启动服务
	virtual bool StartService();
	
	//管理接口
public:
	//创建比赛
	virtual bool CreateGameMatch(BYTE cbMatchType);
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
	//进入比赛
	virtual bool OnEventEnterMatch(DWORD dwSocketID,VOID* pData,DWORD dwUserIP, bool bIsMobile);
	//用户参赛
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退赛
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);		

	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink();
};

//////////////////////////////////////////////////////////////////////////
#endif