#include "StdAfx.h"
#include "AfxTempl.h"
#include "PrivateGame.h"
#include "PrivateServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CPrivateServiceManager::CPrivateServiceManager(void)
{
	//状态变量
	m_bIsService=false;

	//设置变量
	m_pIGamePrivatetem=NULL;

	return;
}

//析构函数
CPrivateServiceManager::~CPrivateServiceManager(void)
{	
	//释放指针
	if(m_pIGamePrivatetem!=NULL) SafeDelete(m_pIGamePrivatetem);
}

//停止服务
bool CPrivateServiceManager::StopService()
{
	//状态判断
	ASSERT(m_bIsService==true);

	//设置状态
	m_bIsService=false;

	//释放指针
	if(m_pIGamePrivatetem!=NULL) SafeRelease(m_pIGamePrivatetem);

	return true;
}

//启动服务
bool CPrivateServiceManager::StartService()
{
	//状态判断
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//设置状态
	m_bIsService=true;

	//启动通知
	if(m_pIGamePrivatetem!=NULL) m_pIGamePrivatetem->OnStartService();

	return true;
}
//接口查询
void *  CPrivateServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPrivateServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IPrivateServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建比赛
bool CPrivateServiceManager::CreatePrivateMatch()
{
	//接口判断
	ASSERT(m_pIGamePrivatetem==NULL);
	if(m_pIGamePrivatetem!=NULL) return false;

	try
	{
		m_pIGamePrivatetem = new PriaveteGame();
		if(m_pIGamePrivatetem==NULL) throw TEXT("私人房创建失败！");
	}
	catch(...)
	{
		ASSERT(FALSE);
		return false;
	}

	return m_pIGamePrivatetem!=NULL;
}

//初始化桌子框架
bool CPrivateServiceManager::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->BindTableFrame(pTableFrame,wTableID);	
	}

	return true;
}


//初始化接口
bool CPrivateServiceManager::InitPrivateInterface(tagPrivateManagerParameter & MatchManagerParameter)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->InitPrivateInterface(MatchManagerParameter);
	}

	return true;
}

//时间事件
bool CPrivateServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventTimer(dwTimerID,dwBindParameter);	
	}

	return true;
}

//数据库事件
bool CPrivateServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);	
	}

	return true;
}


//命令消息
bool CPrivateServiceManager::OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventSocketPrivate(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);	
	}

	return true;
}

//用户登录
bool CPrivateServiceManager::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserLogon(pIServerUserItem);	
	}

	return true;
}

//用户登出
bool CPrivateServiceManager::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserLogout(pIServerUserItem);	
	}

	return true;
}

//进入比赛
bool CPrivateServiceManager::OnEventEnterPrivate(DWORD dwSocketID,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventEnterPrivate(dwSocketID, pData, dwUserIP, bIsMobile);	
	}

	return true;
	
}

//用户参赛
bool CPrivateServiceManager::OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserJoinPrivate(pIServerUserItem,cbReason,dwSocketID);		
	}

	return true;
}

//用户退赛
bool  CPrivateServiceManager::OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserQuitPrivate(pIServerUserItem,cbReason,pBestRank,dwContextID);	
	}

	return true;
}
bool CPrivateServiceManager::OnEventReqStandUP(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventReqStandUP(pIServerUserItem);	
	}
	return true;
}

//用户接口
IUnknownEx * CPrivateServiceManager::GetServerUserItemSink()
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIGamePrivatetem,IServerUserItemSink);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(PrivateServiceManager);

//////////////////////////////////////////////////////////////////////////
