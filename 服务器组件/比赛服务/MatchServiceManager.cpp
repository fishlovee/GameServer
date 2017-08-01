#include "StdAfx.h"
#include "AfxTempl.h"
#include "LockTimeMatch.h"
#include "ImmediateMatch.h"
#include "MatchServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CMatchServiceManager::CMatchServiceManager(void)
{
	//状态变量
	m_bIsService=false;

	//设置变量
	m_pIGameMatchItem=NULL;

	return;
}

//析构函数
CMatchServiceManager::~CMatchServiceManager(void)
{	
	//释放指针
	if(m_pIGameMatchItem!=NULL) SafeDelete(m_pIGameMatchItem);
}

//停止服务
bool CMatchServiceManager::StopService()
{
	//状态判断
	ASSERT(m_bIsService==true);

	//设置状态
	m_bIsService=false;

	//释放指针
	if(m_pIGameMatchItem!=NULL) SafeRelease(m_pIGameMatchItem);

	return true;
}

//启动服务
bool CMatchServiceManager::StartService()
{
	//状态判断
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//设置状态
	m_bIsService=true;

	//启动通知
	if(m_pIGameMatchItem!=NULL) m_pIGameMatchItem->OnStartService();

	return true;
}
//接口查询
void *  CMatchServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IMatchServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建比赛
bool CMatchServiceManager::CreateGameMatch(BYTE cbMatchType)
{
	//接口判断
	ASSERT(m_pIGameMatchItem==NULL);
	if(m_pIGameMatchItem!=NULL) return false;

	try
	{
		//定时赛
		if(cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			m_pIGameMatchItem = new CLockTimeMatch();
			if(m_pIGameMatchItem==NULL) throw TEXT("定时赛创建失败！");
		}

		//即时赛
		if(cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			m_pIGameMatchItem = new CImmediateMatch();
			if(m_pIGameMatchItem==NULL) throw TEXT("即时赛创建失败！");
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		return false;
	}

	return m_pIGameMatchItem!=NULL;
}

//初始化桌子框架
bool CMatchServiceManager::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->BindTableFrame(pTableFrame,wTableID);	
	}

	return true;
}


//初始化接口
bool CMatchServiceManager::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->InitMatchInterface(MatchManagerParameter);
	}

	return true;
}

//时间事件
bool CMatchServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventTimer(dwTimerID,dwBindParameter);	
	}

	return true;
}

//数据库事件
bool CMatchServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);	
	}

	return true;
}


//命令消息
bool CMatchServiceManager::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);	
	}

	return true;
}

//用户登录
bool CMatchServiceManager::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserLogon(pIServerUserItem);	
	}

	return true;
}

//用户登出
bool CMatchServiceManager::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserLogout(pIServerUserItem);	
	}

	return true;
}

//进入比赛
bool CMatchServiceManager::OnEventEnterMatch(DWORD dwSocketID,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventEnterMatch(dwSocketID, pData, dwUserIP, bIsMobile);	
	}

	return true;
	
}

//用户参赛
bool CMatchServiceManager::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserJoinMatch(pIServerUserItem,cbReason,dwSocketID);	
	}

	return true;
}

//用户退赛
bool  CMatchServiceManager::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserQuitMatch(pIServerUserItem,cbReason,pBestRank,dwContextID);	
	}

	return true;
}

//用户接口
IUnknownEx * CMatchServiceManager::GetServerUserItemSink()
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchItem,IServerUserItemSink);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(MatchServiceManager);

//////////////////////////////////////////////////////////////////////////
