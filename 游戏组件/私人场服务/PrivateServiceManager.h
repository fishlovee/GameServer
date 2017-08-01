#ifndef GAME_PRIVATE_SERVICE_MANAGER_HEAD_FILE
#define GAME_PRIVATE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "PrivateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//比赛服务管理类
class PRIVATE_SERVICE_CLASS CPrivateServiceManager : public IPrivateServiceManager
{
	//状态变量
protected:
	bool								m_bIsService;					//服务标识	
	
	//接口变量
protected:	
	IGamePrivateItem *					m_pIGamePrivatetem;				//比赛子项

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口

	//函数定义
public:
	//构造函数
	CPrivateServiceManager(void);
	//析构函数
	virtual ~CPrivateServiceManager(void);

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
	virtual bool CreatePrivateMatch();
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
	//私人房事件
	virtual bool OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//用户事件
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//进入
	virtual bool OnEventEnterPrivate(DWORD dwSocketID,VOID* pData,DWORD dwUserIP, bool bIsMobile);
	//用户参加
	virtual bool OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退出
	virtual bool OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);		

	//用户请求
public:
	//用户离开
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem);
	
	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink();
};

//////////////////////////////////////////////////////////////////////////
#endif