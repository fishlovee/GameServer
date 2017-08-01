#include "StdAfx.h"
#include "TableFrameHook.h"


//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameHook::m_wPlayerCount=MAX_CHAIR;			//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameHook::CTableFrameHook()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pMatchEventSink=NULL;

	return;
}

//析构函数
CTableFrameHook::~CTableFrameHook(void)
{
}

//接口查询
void *  CTableFrameHook::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameHook,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameHook,Guid,dwQueryVer);
	return NULL;
}

//
bool CTableFrameHook::SetMatchEventSink(IUnknownEx * pIUnknownEx)
{
	m_pMatchEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchEventSink);

	return true;
}

//初始化
bool  CTableFrameHook::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//游戏开始
bool CTableFrameHook::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameStart(pITableFrame, wChairCount);
	}

	return false;
}

//游戏结束
bool  CTableFrameHook::OnEventGameEnd(ITableFrame *pITableFrame, WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameEnd(pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}

//用户坐下
bool CTableFrameHook::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL)  
	{
		return m_pMatchEventSink->OnActionUserSitDown(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户起来
bool CTableFrameHook::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL) 
	{
		return m_pMatchEventSink->OnActionUserStandUp(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户同意
bool CTableFrameHook::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnActionUserOnReady(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
