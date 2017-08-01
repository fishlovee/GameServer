#include "StdAfx.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////////////

//内存管理
tagDistributeNode * CDistributeNodePool::m_pHeadOfFreeList=NULL;
const int CDistributeNodePool::BLOCK_SIZE = 20;

//常量定义
#define DISTRIBUTE_WAIT_TIMESTAMP			10							//等待时间戳

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDistributeNodePool::CDistributeNodePool()
{
}

//析构函数
CDistributeNodePool::~CDistributeNodePool()
{
	if(m_pHeadOfFreeList!=NULL)
	{
		tagDistributeNode * pDistributeNode=m_pHeadOfFreeList;
		while(pDistributeNode!=NULL)
		{
			//安全释放
			m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
			SafeDelete(pDistributeNode);
			pDistributeNode=m_pHeadOfFreeList;
		}
	}
}

//分配结点
tagDistributeNode * CDistributeNodePool::AllocNode()
{
	//获取头结点
	tagDistributeNode *pDistributeNode = m_pHeadOfFreeList; 
	if(pDistributeNode!=NULL)
		m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
	else 
	{
		//分配大块内存
		for(int nIndex=0;nIndex<BLOCK_SIZE;nIndex++)
		{
			tagDistributeNode * pNewBlock = new tagDistributeNode;
			pNewBlock->pNextDistributeNode=m_pHeadOfFreeList;
			m_pHeadOfFreeList=pNewBlock;
		}
		
		//设置结点
		pDistributeNode = m_pHeadOfFreeList;
		m_pHeadOfFreeList=pDistributeNode->pNextDistributeNode;
	}

	return pDistributeNode;
}

//释放结点
VOID CDistributeNodePool::FreeNode(void * pNode)
{
	//归还结点
	tagDistributeNode * pDeadNode =  static_cast<tagDistributeNode*>(pNode);
	pDeadNode->pNextDistributeNode = m_pHeadOfFreeList;
	m_pHeadOfFreeList = pDeadNode;
	pNode=NULL;

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDistributeManager::CDistributeManager()
{
	//设置变量
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;
	m_cbDistributeRule=0;

	//设置字典
	m_SameTableMap.InitHashTable(10003);
}

//析构函数
CDistributeManager::~CDistributeManager()
{
	//移除节点
	RemoveAll();

	//释放对象
	m_SameTableBuffer.Append(m_SameTableActive);
	for(INT_PTR nIndex=0;nIndex<m_SameTableBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_SameTableBuffer[nIndex]);
	}

	//移除元素
	m_SameTableBuffer.RemoveAll();
	m_SameTableActive.RemoveAll();
}


//插入结点
BOOL CDistributeManager::InsertDistributeNode(const tagDistributeInfo & DistributeInfo)
{
	//查找用户
	if(SearchNode(DistributeInfo.pIServerUserItem)!=NULL) return true;

	//头部判断
	if(m_pHeadNode==NULL)
	{
		//分配结点
		m_pHeadNode = m_DistributeNodePool.AllocNode();
		ASSERT(m_pHeadNode!=NULL);
		if(m_pHeadNode==NULL) return false;

		//设置变量
		m_pHeadNode->pNextDistributeNode=NULL;
		m_pHeadNode->pPrepDistributeNode=NULL;
		CopyMemory(&m_pHeadNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		m_pHeadNode->DistributeInfo.pPertainNode=m_pHeadNode;
	}
	else
	{
		//分配结点
		tagDistributeNode * pDistributeNode = m_DistributeNodePool.AllocNode();
		ASSERT(pDistributeNode!=NULL);
		if(pDistributeNode==NULL) return false;

		//设置结点
		pDistributeNode->pNextDistributeNode=NULL;
		pDistributeNode->pPrepDistributeNode=NULL;
		CopyMemory(&pDistributeNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		pDistributeNode->DistributeInfo.pPertainNode=pDistributeNode;
		
		//表头结点
		if(m_pHeadNode->pNextDistributeNode!=NULL)
		{
			m_pHeadNode->pNextDistributeNode->pPrepDistributeNode=pDistributeNode;
			pDistributeNode->pNextDistributeNode=m_pHeadNode->pNextDistributeNode;
		}
		
		pDistributeNode->pPrepDistributeNode=m_pHeadNode;
		m_pHeadNode->pNextDistributeNode=pDistributeNode;
	}

	//更新数目
	if(DistributeInfo.pIServerUserItem->IsAndroidUser())
		++m_wAndroidCount;
	else
		++m_wRealCount;
	++m_wNodeCount;

	return true;
}

//移除结点
VOID  CDistributeManager::RemoveDistributeNode(const IServerUserItem  * pIServerUserItem)
{
	//查找结点
	tagDistributeNode  *pDistributeNode=SearchNode(pIServerUserItem);
	if(pDistributeNode!=NULL) 
		RemoveDistributeNode(pDistributeNode);

	return;
}

//移除结点
VOID  CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//参数校验
	if(pDistributeNode==NULL) return;

	//查找用户
	if(SearchNode(pDistributeNode->DistributeInfo.pIServerUserItem)==NULL) return;

	//变量定义
	tagDistributeNode  *pPrepNode=pDistributeNode->pPrepDistributeNode;
	tagDistributeNode  *pNextNode=pDistributeNode->pNextDistributeNode;

	//删除结点
	if(pPrepNode!=NULL)
	{
		if(pNextNode!=NULL)
		{
			pPrepNode->pNextDistributeNode=pNextNode;
			pNextNode->pPrepDistributeNode=pPrepNode;
		}
		else
		{
			//表头链接
			if(pPrepNode->pNextDistributeNode==pDistributeNode)
				pPrepNode->pNextDistributeNode=NULL;
		}
	}
	else
	{
		if(pNextNode!=NULL)
		{
			pNextNode->pPrepDistributeNode=NULL;

			//重置表头
			m_pHeadNode=pNextNode;
		}
		else
		{
			m_pHeadNode=NULL;
		}
	}

    //更新数目
	if(pDistributeNode->DistributeInfo.pIServerUserItem->IsAndroidUser())
		--m_wAndroidCount;
	else
		--m_wRealCount;
	--m_wNodeCount;

	//安全释放
	m_DistributeNodePool.FreeNode(pDistributeNode);
}

//移除结点
VOID CDistributeManager::RemoveAll()
{
	//释放内存
	while(m_pHeadNode!=NULL) RemoveDistributeNode(m_pHeadNode);

	//重置变量
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;

	return;
}

//执行分组
WORD  CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount)
{
	//定义变量
	tagDistributeNode  * pMoveNode=NULL;
	tagDistributeNode  * pMoveStartNode=NULL;
	if(m_pHeadNode!=NULL && m_wNodeCount>1)
	{
		pMoveNode = m_pHeadNode;
		WORD wRandNodeIndex=rand()%m_wNodeCount;		
		while(wRandNodeIndex-->0)
		{
			if(pMoveNode==NULL)
			{
				pMoveNode = m_pHeadNode;
				break;
			}
			pMoveNode = pMoveNode->pNextDistributeNode; 
		}

		//设置变量
		if(pMoveNode!=NULL) pMoveStartNode = pMoveNode;
	}

	//
	if(pMoveNode==NULL) return 0;

	//获取时间戳
	DWORD dwCurrentStamp = (DWORD)time(NULL);

	//获取用户
	do
	{
		//定义变量
		BOOL bFirstSuccess=TRUE;

		//等待时间
		if(dwCurrentStamp-pMoveNode->DistributeInfo.dwInsertStamp< DISTRIBUTE_WAIT_TIMESTAMP)
			bFirstSuccess=FALSE;

		//等级过滤
		if(DistributeInfoArray.GetCount()>0 && DistributeInfoArray[0].wDistribute!=pMoveNode->DistributeInfo.wDistribute)
			bFirstSuccess=FALSE;

		//机器过滤
		if(bFirstSuccess==TRUE && DistributeInfoArray.GetCount()==wNeedCount-1 && 
			FilterRuleIsAllAndroid(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem))
			bFirstSuccess=FALSE;

		//同IP过滤
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&  
			FilterRuleExitsIPAddr(DistributeInfoArray,pMoveNode->DistributeInfo.dwClientAddr)==TRUE)
			bFirstSuccess=FALSE;

		//同桌过滤
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_LAST_TABLE)==0 &&  
			FilterRuleIsLastSameTable(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem->GetUserID())==TRUE)
			bFirstSuccess=FALSE;

		//获取成功
		if(bFirstSuccess==TRUE) DistributeInfoArray.Add(pMoveNode->DistributeInfo);

		//向前推进
		pMoveNode=pMoveNode->pNextDistributeNode;
		if(pMoveNode==NULL) pMoveNode = m_pHeadNode;

		//成功判断
		if(DistributeInfoArray.GetCount()==wNeedCount) break;

	}while(pMoveNode && pMoveNode!=pMoveStartNode);

	return (WORD)DistributeInfoArray.GetCount();
}

//获取信息
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo(DWORD dwUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo;
	if(m_SameTableMap.Lookup(dwUserID,pSameTableInfo)==FALSE)
	{
		pSameTableInfo = ActiveSameTableInfo();
		m_SameTableMap[dwUserID] = pSameTableInfo;
	}

	return pSameTableInfo;
}

//移除信息
VOID CDistributeManager::RemoveUserSameTableInfo(DWORD dwUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo;
	if(m_SameTableMap.Lookup(dwUserID,pSameTableInfo)==TRUE)
	{
		//移除信息
		m_SameTableMap.RemoveKey(dwUserID);
		RemoveSameTableInfo(pSameTableInfo);		
	}
}

//激活对象
tagSameTableInfo * CDistributeManager::ActiveSameTableInfo()
{
	//查找缓冲
	if(m_SameTableBuffer.GetCount()>0)
	{
		tagSameTableInfo * pSameTableInfo = m_SameTableBuffer.GetAt(0);
		m_SameTableBuffer.RemoveAt(0);
		m_SameTableActive.Add(pSameTableInfo);
		return pSameTableInfo;
	}

	//创建对象
	try
	{
		//创建对象
		tagSameTableInfo * pSameTableInfo = new tagSameTableInfo;
		if(pSameTableInfo==NULL) throw(TEXT("内存不足,对象创建失败!"));

		//设置对象
		ZeroMemory(pSameTableInfo,sizeof(tagSameTableInfo));
		m_SameTableActive.Add(pSameTableInfo);

		return pSameTableInfo;
	}
	catch(...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

//移除对象
VOID CDistributeManager::RemoveSameTableInfo(tagSameTableInfo * pSameTableInfo) 
{
	ZeroMemory(pSameTableInfo,sizeof(tagSameTableInfo));
	m_SameTableBuffer.Add(pSameTableInfo);

	//查找对象
	for(INT_PTR nIndex=0;nIndex<m_SameTableActive.GetCount();nIndex++)
	{
		if(m_SameTableActive[nIndex]==pSameTableInfo)
		{
			m_SameTableActive.RemoveAt(nIndex);
			break;
		}
	}
}

//查找结点
tagDistributeNode * CDistributeManager::SearchNode(const IServerUserItem  * const pIServerUserItem)
{
	if(m_pHeadNode==NULL) return NULL;

	//设置变量
	tagDistributeNode *pMoveNode=m_pHeadNode;

	//查找结点
	while(pMoveNode!=NULL)
	{
		//接口判断
		if(pMoveNode->DistributeInfo.pIServerUserItem==pIServerUserItem)
			return pMoveNode;

		//向前推进
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	return NULL;
}

//IP同址
BOOL  CDistributeManager::FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr)
{
	//查找同IP
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].dwClientAddr==dwClientAddr)
			return TRUE;
	}

	return FALSE;
}

//机器过滤
BOOL CDistributeManager::FilterRuleIsAllAndroid(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem  * const pIServerUserItem)
{
	//参数校验
	if(pIServerUserItem==NULL || DistributeInfoArray.GetCount()==0) return FALSE;

	//变量定义
	WORD wAndroidCount=0;

	//统计机器
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->IsAndroidUser()==true)
			++wAndroidCount;
	}

	return (wAndroidCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsAndroidUser();
}

//上局同桌
BOOL CDistributeManager::FilterRuleIsLastSameTable(const CDistributeInfoArray & DistributeInfoArray,DWORD dwUserID)
{
	//参数校验
	if(DistributeInfoArray.GetCount()==0) return FALSE;

	//变量定义
	tagSameTableInfo * pSameTableInfo=NULL;

	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		pSameTableInfo = GetUserSameTableInfo(DistributeInfoArray[nIndex].pIServerUserItem->GetUserID());
		if(pSameTableInfo!=NULL)
		{
			for(WORD i=0;i<pSameTableInfo->wPlayerCount;i++)
			{
				if(pSameTableInfo->wPlayerIDSet[i]==dwUserID) return TRUE;				
			}
		}
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
