#include "StdAfx.h"
#include "PrivateGame.h"
#include "FangKaHttpUnits.h"
#include "..\游戏服务器\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
PriaveteGame::PriaveteGame()
{
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//内核接口
	m_pTableInfo=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

PriaveteGame::~PriaveteGame(void)
{
	//释放资源
	SafeDeleteArray(m_pTableInfo);

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_DISMISS_WAITE_END);
}

//接口查询
VOID* PriaveteGame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGamePrivateItem,Guid,dwQueryVer);
	QUERYINTERFACE(IPrivateEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGamePrivateItem,Guid,dwQueryVer);
	return NULL;
}

//绑定桌子
bool PriaveteGame::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//创建钩子
	CTableFramePrivate * pTableFrameHook=new CTableFramePrivate();
	pTableFrameHook->InitTableFramePrivate(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetPrivateEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFramePrivate(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_pTableInfo[wTableID].pITableFrame=pTableFrame;
	m_pTableInfo[wTableID].restValue();

	return true;
}

//初始化接口
bool PriaveteGame::InitPrivateInterface(tagPrivateManagerParameter & MatchManagerParameter)
{
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//内核组件
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//服务组件		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//创建桌子
	if (m_pTableInfo==NULL)
	{
		m_pTableInfo = new PrivateTableInfo[m_pGameServiceOption->wTableCount];
	}

	return true;
}
void PriaveteGame::OnStartService()
{
	//变量定义
	DBR_GR_Private_Info kPrivateInfo;
	ZeroMemory(&kPrivateInfo,sizeof(kPrivateInfo));
	kPrivateInfo.wKindID=m_pGameServiceOption->wKindID;
	m_pIDataBaseEngine->PostDataBaseRequest(0L,DBR_GR_PRIVATE_INFO,0L,&kPrivateInfo,sizeof(kPrivateInfo));

	m_pITimerEngine->SetTimer(IDI_DISMISS_WAITE_END,5000L,TIMES_INFINITY,0);
}

//时间事件
bool PriaveteGame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	switch(dwTimerID)
	{
	case IDI_DISMISS_WAITE_END:				//解散等待时间 10s
		{
			for(int i = 0;i<m_pGameServiceOption->wTableCount;i++)
			{
				PrivateTableInfo* pTableInfo = &m_pTableInfo[i];
				if (pTableInfo->bInEnd)
				{
					pTableInfo->fAgainPastTime += 5.0f;
					if (pTableInfo->fDismissPastTime >= AGAIN_WAITE_TIME)
					{
						ClearRoom(pTableInfo);
					}
				}
				if (pTableInfo->kDismissChairID.size())
				{
					pTableInfo->fDismissPastTime += 5.0f;
					if (pTableInfo->fDismissPastTime >= DISMISS_WAITE_TIME)
					{
						if (pTableInfo->kNotAgreeChairID.size() <= 1)
						{
							DismissRoom(pTableInfo);
						}
					}
				}
			}
			return true;
		}
	}
	
	return true;
}

//发送数据
bool PriaveteGame::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, wMainCmdID,wSubCmdID, pData, wDataSize);

	return true;
}
bool PriaveteGame::SendTableData(ITableFrame*	pITableFrame, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return pITableFrame->SendTableData(INVALID_CHAIR,wSubCmdID,pData,wDataSize,wMainCmdID);
}
void PriaveteGame::CreatePrivateCost(PrivateTableInfo* pTableInfo)
{
	if (pTableInfo->cbRoomType == Type_Private)
	{
		DBR_GR_Create_Private_Cost kNetInfo;
		kNetInfo.dwUserID = pTableInfo->dwCreaterUserID;
		kNetInfo.dwCost = pTableInfo->dwPlayCost;
		kNetInfo.dwCostType = pTableInfo->cbRoomType;
		m_pIDataBaseEngine->PostDataBaseRequest(0L,DBR_GR_CREATE_PRIVAT_COST,0L,&kNetInfo,sizeof(kNetInfo));
	}
	if (pTableInfo->cbRoomType == Type_Public)
	{
		for (int i = 0;i<pTableInfo->pITableFrame->GetSitUserCount();i++)
		{
			if (!pTableInfo->pITableFrame->GetTableUserItem(i))
			{
				continue;
			}
			DBR_GR_Create_Private_Cost kNetInfo;
			kNetInfo.dwUserID = pTableInfo->pITableFrame->GetTableUserItem(i)->GetUserID();
			kNetInfo.dwCost = pTableInfo->dwPlayCost;
			kNetInfo.dwCostType = pTableInfo->cbRoomType;
			m_pIDataBaseEngine->PostDataBaseRequest(0L,DBR_GR_CREATE_PRIVAT_COST,0L,&kNetInfo,sizeof(kNetInfo));
		}
	}

}

PrivateTableInfo* PriaveteGame::getTableInfoByRoomID(DWORD dwRoomID)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].dwRoomNum == dwRoomID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByCreaterID(DWORD dwUserID)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].dwCreaterUserID == dwUserID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByTableID(DWORD dwRoomID)
{

	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].pITableFrame && m_pTableInfo[i].pITableFrame->GetTableID() == dwRoomID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByTableFrame(ITableFrame* pTableFrame)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].pITableFrame == pTableFrame)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}

//数据库事件
bool PriaveteGame::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_PRIVATE_INFO:		//私人场信息
		{
			//参数效验
			if(wDataSize>sizeof(DBO_GR_Private_Info)) return false;

			//提取数据
			DBO_GR_Private_Info * pPrivate = (DBO_GR_Private_Info*)pData;
			m_kPrivateInfo.wKindID = pPrivate->wKindID;
			m_kPrivateInfo.lCostGold = pPrivate->lCostGold;
			memcpy(&m_kPrivateInfo.bPlayCout,pPrivate->bPlayCout,sizeof(m_kPrivateInfo.bPlayCout));
			memcpy(&m_kPrivateInfo.lPlayCost,pPrivate->lPlayCost,sizeof(m_kPrivateInfo.lPlayCost));

			break;
		}
	case DBO_GR_CREATE_PRIVATE:		//私人场信息
		{
			OnEventCreatePrivate(wRequestID,pIServerUserItem,pData,wDataSize,"");
			break;
		}
	}
	return true;
}

bool PriaveteGame::joinPrivateRoom(IServerUserItem * pIServerUserItem,ITableFrame * pICurrTableFrame)
{
	WORD wGaveInChairID = INVALID_CHAIR;
	for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
	{
		if (pICurrTableFrame->GetTableUserItem(i)==pIServerUserItem)
		{
			wGaveInChairID = i;
			break;
		}
	}
	if (wGaveInChairID!=INVALID_CHAIR)
	{
		pIServerUserItem->SetUserStatus(US_READY,pICurrTableFrame->GetTableID(),wGaveInChairID);
		return true;
	}
	WORD wChairID = INVALID_CHAIR;
	//椅子搜索
	for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
	{
		if (pICurrTableFrame->GetTableUserItem(i)==NULL)
		{
			wChairID = i;
			break;
		}
	}

	//分配用户
	if (wChairID!=INVALID_CHAIR)
	{
		//用户起立
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			return pIServerUserItem->GetTableID() == pICurrTableFrame->GetTableID();
		}

		//用户坐下
		if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
		{
			return false;
		}
		pIServerUserItem->SetUserStatus(US_READY,pICurrTableFrame->GetTableID(),wChairID);
		return true;
	}	
	return false;
}
//创建房间
bool PriaveteGame::OnEventCreatePrivate(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,std::string kChannel)
{
	//参数效验
	if(pIServerUserItem==NULL) return true;
	if(wDataSize>sizeof(DBO_GR_CreatePrivateResoult)) return false;

	//提取数据
	DBO_GR_CreatePrivateResoult* pPrivate = (DBO_GR_CreatePrivateResoult*)pData;	
	DWORD dwAgaginTable = pPrivate->dwAgaginTable;
	//报名失败
	if(pPrivate->bSucess==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pPrivate->szDescribeString,SMT_EJECT);

		return true;
	}
	if (pPrivate->bPlayCoutIdex < 0 || pPrivate->bPlayCoutIdex >= 4)
	{
		return false;
	}

	//寻找位置
	ITableFrame * pICurrTableFrame=NULL;
	PrivateTableInfo* pCurrTableInfo=NULL;
	if (dwAgaginTable != INVALID_DWORD)
	{
		pCurrTableInfo = getTableInfoByTableID(dwAgaginTable);
		if (!pCurrTableInfo)
		{
			return false;
		}
		pICurrTableFrame = pCurrTableInfo->pITableFrame;
		if (pCurrTableInfo->bInEnd == false)
		{
			joinPrivateRoom(pIServerUserItem,pICurrTableFrame);
			return true;
		}
		else
		{
			pCurrTableInfo->restAgainValue();
			sendPrivateRoomInfo(NULL,pCurrTableInfo);
		}
	}
	else
	{
		WORD wTableID = 0;
		for (wTableID=0;wTableID<m_pGameServiceOption->wTableCount;wTableID++)
		{
			//获取对象
			ASSERT(m_pTableInfo[wTableID].pITableFrame!=NULL);
			ITableFrame * pITableFrame=m_pTableInfo[wTableID].pITableFrame;
			if(m_pTableInfo[wTableID].bInEnd)
			{
				continue;
			}
			//状态判断
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame = pITableFrame;
				pCurrTableInfo = &m_pTableInfo[wTableID];
				pCurrTableInfo->restValue();
				break;
			}
		}
		if (getTableInfoByCreaterID(pIServerUserItem->GetUserID()))
		{
			return true;
		}
	}
	//桌子判断
	if(pICurrTableFrame==NULL)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("房间房间失败。"),SMT_EJECT);
		return true;
	}


	if(!joinPrivateRoom(pIServerUserItem,pICurrTableFrame))
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("房间房间失败。"),SMT_EJECT);
		return true;
	}
	int iRandNum = 1000+rand()%8900;
	while (getTableInfoByRoomID(iRandNum))
	{
		iRandNum = 1000+rand()%8900;
	}
	if (m_pGameServiceOption->wServerID >= 90)
	{
		ASSERT(false);
		return true;
	}
	iRandNum = (m_pGameServiceOption->wServerID+10)*10000+iRandNum;
	pCurrTableInfo->dwPlayCout = (DWORD)m_kPrivateInfo.bPlayCout[pPrivate->bPlayCoutIdex];
	if (pPrivate->cbRoomType == Type_Private)
	{
		pCurrTableInfo->dwPlayCost = (DWORD)m_kPrivateInfo.lPlayCost[pPrivate->bPlayCoutIdex];
	}
	else
	{
		pCurrTableInfo->dwPlayCost = (DWORD)m_kPrivateInfo.lCostGold;
	}
	pCurrTableInfo->setRoomNum(iRandNum);
	pCurrTableInfo->dwCreaterUserID = pIServerUserItem->GetUserID();
	pCurrTableInfo->kHttpChannel = kChannel;
	pCurrTableInfo->cbRoomType = pPrivate->cbRoomType;

	pCurrTableInfo->bGameRuleIdex = pPrivate->bGameRuleIdex;
	pCurrTableInfo->bGameTypeIdex = pPrivate->bGameTypeIdex;
	pCurrTableInfo->bPlayCoutIdex = pPrivate->bPlayCoutIdex;
	GetLocalTime(&pCurrTableInfo->kTotalRecord.kPlayTime);

	pICurrTableFrame->SetPrivateInfo(pCurrTableInfo->bGameTypeIdex,pCurrTableInfo->bGameRuleIdex);

	CMD_GF_Create_Private_Sucess kSucessInfo;
	kSucessInfo.lCurSocre = pPrivate->bSucess;
	kSucessInfo.dwRoomNum = pCurrTableInfo->dwRoomNum;
	SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_CREATE_PRIVATE_SUCESS,&kSucessInfo,sizeof(kSucessInfo));


	sendPrivateRoomInfo(NULL,pCurrTableInfo);
	return true;
}

	//创建私人场
bool PriaveteGame::OnTCPNetworkSubCreatePrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//参数效验
	ASSERT(wDataSize==sizeof(CMD_GR_Create_Private));
	if(wDataSize!=sizeof(CMD_GR_Create_Private)) return false;

	CMD_GR_Create_Private* pCMDInfo = (CMD_GR_Create_Private*)pData;
	pCMDInfo->stHttpChannel[CountArray(pCMDInfo->stHttpChannel)-1]=0;

	if (pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		return true;
	}

	if(pCMDInfo->cbGameType == Type_Private)
	{
		DBR_GR_Create_Private kDBRInfo;
		kDBRInfo.dwUserID = pIServerUserItem->GetUserID();
		kDBRInfo.wKindID = m_pGameServiceAttrib->wKindID;
		kDBRInfo.cbRoomType = Type_Private;
		kDBRInfo.dwCostType = Type_Private;
		kDBRInfo.dwAgaginTable = INVALID_DWORD;
		kDBRInfo.dwCost = (DWORD)m_kPrivateInfo.lPlayCost[pCMDInfo->bPlayCoutIdex];
		kDBRInfo.bPlayCoutIdex = pCMDInfo->bPlayCoutIdex;
		kDBRInfo.bGameRuleIdex = pCMDInfo->bGameRuleIdex;
		kDBRInfo.bGameTypeIdex = pCMDInfo->bGameTypeIdex;

		DBR_CreatePrivate(&kDBRInfo,dwSocketID,pIServerUserItem,pCMDInfo->stHttpChannel);
	}
	else
	{
		ITableFrame * pICurrTableFrame=NULL;
		PrivateTableInfo* pCurrTableInfo=NULL;
		WORD wTableID = 0;
		for (wTableID=0;wTableID<m_pGameServiceOption->wTableCount;wTableID++)
		{
			//获取对象
			PrivateTableInfo& pTableInfo = m_pTableInfo[wTableID];
			ASSERT(pTableInfo.pITableFrame!=NULL);
			ITableFrame * pITableFrame=pTableInfo.pITableFrame;
			if(pTableInfo.bInEnd)
			{
				continue;
			}
			if(pTableInfo.cbRoomType != Type_Public)
			{
				continue;
			}
			if (m_pTableInfo[wTableID].bGameRuleIdex != pCMDInfo->bGameRuleIdex 
				|| m_pTableInfo[wTableID].bGameTypeIdex != pCMDInfo->bGameTypeIdex )
			{
				continue;
			}
			//状态判断
			if (pITableFrame->GetNullChairCount() >= 0)
			{
				pICurrTableFrame = pITableFrame;
				pCurrTableInfo = &m_pTableInfo[wTableID];
				break;
			}
		}
		if (pICurrTableFrame == NULL)
		{
			DBR_GR_Create_Private kDBRInfo;
			kDBRInfo.dwUserID = pIServerUserItem->GetUserID();
			kDBRInfo.wKindID = m_pGameServiceAttrib->wKindID;
			kDBRInfo.cbRoomType = Type_Public;
			kDBRInfo.dwCostType = Type_Public;
			kDBRInfo.dwAgaginTable = INVALID_DWORD;
			kDBRInfo.dwCost = (DWORD)m_kPrivateInfo.lPlayCost[pCMDInfo->bPlayCoutIdex];
			kDBRInfo.bPlayCoutIdex = pCMDInfo->bPlayCoutIdex;
			kDBRInfo.bGameRuleIdex = pCMDInfo->bGameRuleIdex;
			kDBRInfo.bGameTypeIdex = pCMDInfo->bGameTypeIdex;

			DBR_CreatePrivate(&kDBRInfo,dwSocketID,pIServerUserItem,"");
		}
		else
		{
			joinPrivateRoom(pIServerUserItem,pICurrTableFrame);
		}
	}
	return true;
}
//重新加入私人场
bool PriaveteGame::OnTCPNetworkSubAgainEnter(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//参数效验
	ASSERT(wDataSize==sizeof(CMD_GR_Again_Private));
	if(wDataSize!=sizeof(CMD_GR_Again_Private)) return false;

	CMD_GR_Again_Private* pCMDInfo = (CMD_GR_Again_Private*)pData;
	pCMDInfo->stHttpChannel[CountArray(pCMDInfo->stHttpChannel)-1] = 0;

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return true;
	}
	if (!pTableInfo->bInEnd && pTableInfo->dwRoomNum != 0)
	{
		joinPrivateRoom(pIServerUserItem,pTableInfo->pITableFrame);
		return true;
	}
	DBR_GR_Create_Private kDBRInfo;
	ZeroMemory(&kDBRInfo,sizeof(kDBRInfo));
	kDBRInfo.cbRoomType = pTableInfo->cbRoomType;
	kDBRInfo.dwUserID = pIServerUserItem->GetUserID();
	kDBRInfo.wKindID = m_pGameServiceAttrib->wKindID;
	kDBRInfo.dwCost = (DWORD)m_kPrivateInfo.lPlayCost[pTableInfo->bPlayCoutIdex];
	kDBRInfo.dwAgaginTable = pIServerUserItem->GetTableID();

	DBR_CreatePrivate(&kDBRInfo,dwSocketID,pIServerUserItem,pCMDInfo->stHttpChannel);
	return true;
}
//加入私人场
bool PriaveteGame::OnTCPNetworkSubJoinPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//参数效验
	ASSERT(wDataSize==sizeof(CMD_GR_Join_Private));
	if(wDataSize!=sizeof(CMD_GR_Join_Private)) return false;

	CMD_GR_Join_Private* pCMDInfo = (CMD_GR_Join_Private*)pData;
	PrivateTableInfo* pTableInfo = getTableInfoByRoomID(pCMDInfo->dwRoomNum);
	if (!pTableInfo)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("对不起，没有找到该房间，可能房主已经退出。"),SMT_EJECT|SMT_CHAT);
		return true;
	}
	if (pTableInfo->pITableFrame->GetNullChairCount() <= 0)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("对不起，游戏人数已满，无法加入。"),SMT_EJECT|SMT_CHAT);
		return true;
	}
	if (pTableInfo->bStart || pTableInfo->bInEnd)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("对不起，游戏已经开始，无法加入。"),SMT_EJECT|SMT_CHAT);
		return true;
	}
	if (!joinPrivateRoom(pIServerUserItem,pTableInfo->pITableFrame))
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("加入房间失败。"),SMT_EJECT);
	}
	return true;
}
bool PriaveteGame::OnTCPNetworkSubDismissPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	CTraceService::TraceString(TEXT("00000"),TraceLevel_Normal);

	//参数效验
	ASSERT(wDataSize==sizeof(CMD_GR_Dismiss_Private));
	if(wDataSize!=sizeof(CMD_GR_Dismiss_Private)) return false;
	CTraceService::TraceString(TEXT("1111"),TraceLevel_Normal);

	CMD_GR_Dismiss_Private* pCMDInfo = (CMD_GR_Dismiss_Private*)pData;

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		CTraceService::TraceString(TEXT("222222"),TraceLevel_Normal);

		return false;
	}
	if (!pTableInfo->bStart && pTableInfo->dwCreaterUserID != pIServerUserItem->GetUserID())
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		return true;
	}
	if (!pTableInfo)
	{
		return true;
	}
	if (pTableInfo->kDismissChairID.size() == 0 && !pCMDInfo->bDismiss)
	{
		return true;
	}
	if(pCMDInfo->bDismiss)
	{
		for (int i = 0;i<(int)pTableInfo->kDismissChairID.size();i++)
		{
			if (pTableInfo->kDismissChairID[i] == pIServerUserItem->GetChairID())
			{
				return true;
			}
		}
		pTableInfo->kDismissChairID.push_back(pIServerUserItem->GetChairID());
	}
	else
	{
		for (int i = 0;i<(int)pTableInfo->kNotAgreeChairID.size();i++)
		{
			if (pTableInfo->kNotAgreeChairID[i] == pIServerUserItem->GetChairID())
			{
				return true;
			}
		}
		pTableInfo->kNotAgreeChairID.push_back(pIServerUserItem->GetChairID());
	}
	CMD_GF_Private_Dismiss_Info kNetInfo;
	kNetInfo.dwDissUserCout = pTableInfo->kDismissChairID.size();
	kNetInfo.dwNotAgreeUserCout = pTableInfo->kNotAgreeChairID.size();
	for (int i = 0;i<(int)pTableInfo->kDismissChairID.size();i++)
	{
		kNetInfo.dwDissChairID[i] = pTableInfo->kDismissChairID[i];
	}
	for (int i = 0;i<(int)pTableInfo->kNotAgreeChairID.size();i++)
	{
		kNetInfo.dwNotAgreeChairID[i] = pTableInfo->kNotAgreeChairID[i];
	}
	SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GR_PRIVATE_DISMISS,&kNetInfo,sizeof(kNetInfo));

	bool bClearDismissInfo = false;
	if (pTableInfo->kNotAgreeChairID.size() >= 2)
	{
		bClearDismissInfo = true;
	}

	if (!pTableInfo->bStart || (int)kNetInfo.dwDissUserCout >= (int)pTableInfo->pITableFrame->GetSitUserCount()-1)
	{
		bClearDismissInfo = true;

		pTableInfo->pITableFrame->SendGameMessage(TEXT("房间已被解散！"),SMT_EJECT);

		DismissRoom(pTableInfo);

		ClearRoom(pTableInfo);

	}
	if (bClearDismissInfo)
	{
		pTableInfo->kNotAgreeChairID.clear();
		pTableInfo->kDismissChairID.clear();
		kNetInfo.dwDissUserCout = pTableInfo->kDismissChairID.size();
		kNetInfo.dwNotAgreeUserCout = pTableInfo->kNotAgreeChairID.size();
		SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GR_PRIVATE_DISMISS,&kNetInfo,sizeof(kNetInfo));
	}
	return true;
}
//比赛事件
bool PriaveteGame::OnEventSocketPrivate(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_PRIVATE_INFO:	//私人场信息
		{
			SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_PRIVATE_INFO,&m_kPrivateInfo,sizeof(m_kPrivateInfo));
			return true;
		}
	case SUB_GR_CREATE_PRIVATE:	//创建私人场
		{
			return OnTCPNetworkSubCreatePrivate(pData,wDataSize,pIServerUserItem,dwSocketID);
		}
	case SUB_GR_RIVATE_AGAIN:	//重新加入私人场
		{
			return OnTCPNetworkSubAgainEnter(pData,wDataSize,pIServerUserItem,dwSocketID);
		}
	case SUB_GR_JOIN_PRIVATE:	//加入私人场
		{
			return OnTCPNetworkSubJoinPrivate(pData,wDataSize,pIServerUserItem,dwSocketID);	
		}
	case SUB_GR_PRIVATE_DISMISS:	//解散
		{
			CTraceService::TraceString(TEXT("ccccccccccccc"),TraceLevel_Normal);

			return OnTCPNetworkSubDismissPrivate(pData,wDataSize,pIServerUserItem,dwSocketID);	
		}
	}
	return true;
}

//重置桌位
void PriaveteGame::DismissRoom(PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo!=NULL);
	if (pTableInfo==NULL) return;

	ITableFrame* pTableFrame=pTableInfo->pITableFrame;
	ASSERT(pTableFrame!=NULL);
	if (pTableFrame==NULL) return;

	if (pTableInfo->bStart)
	{
		CMD_GF_Private_End_Info kNetInfo;
		kNetInfo.lPlayerWinLose.resize(pTableFrame->GetChairCount());
		kNetInfo.lPlayerAction.resize(pTableFrame->GetChairCount()*MAX_PRIVATE_ACTION);
		for (int i = 0;i<pTableFrame->GetChairCount();i++)
		{
			kNetInfo.lPlayerWinLose[i] = pTableInfo->lPlayerWinLose[i];
			for (int m = 0;m<MAX_PRIVATE_ACTION;m++)
			{
				kNetInfo.lPlayerAction[i*MAX_PRIVATE_ACTION+m] = pTableInfo->lPlayerAction[i][m];
			}
		}
		datastream kDataStream;
		kNetInfo.StreamValue(kDataStream,true);
		SendTableData(pTableFrame,MDM_GR_PRIVATE,SUB_GF_PRIVATE_END,&kDataStream[0],kDataStream.size());
	}

	//强制解散游戏
	if (pTableFrame->IsGameStarted()==true)
	{
		bool bSuccess=pTableFrame->DismissGame();
		if (bSuccess==false)
		{
			CTraceService::TraceString(TEXT("PriaveteGame 解散游戏"),TraceLevel_Exception);
			return;
		}
	}

	if (pTableInfo->bStart)
	{
		pTableInfo->bInEnd = true;
		pTableInfo->dwCreaterUserID = 0;

		datastream kDataStream;
		pTableInfo->kTotalRecord.StreamValue(kDataStream,true);

		m_pIDataBaseEngine->PostDataBaseRequest(INVALID_DWORD,DBR_GR_PRIVATE_GAME_RECORD,0,&kDataStream[0],kDataStream.size());	

		pTableInfo->bStart = false;
		sendPrivateRoomInfo(NULL,pTableInfo);
	}
	else
	{
		ClearRoom(pTableInfo);
	}
	return;
}
void PriaveteGame::ClearRoom(PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo!=NULL);
	if (pTableInfo==NULL) return;

	ITableFrame* pTableFrame=pTableInfo->pITableFrame;
	ASSERT(pTableFrame!=NULL);
	if (pTableFrame==NULL) return;

	//该桌用户全部离开
	for (int i=0;i<pTableFrame->GetChairCount();i++)
	{
		IServerUserItem* pUserItem=pTableFrame->GetTableUserItem(i);
		if(pUserItem&&pUserItem->GetTableID()!=INVALID_TABLE)
		{
			pTableFrame->PerformStandUpActionEx(pUserItem);
		}
	}
	pTableInfo->restValue();
}
void PriaveteGame::DBR_CreatePrivate(DBR_GR_Create_Private* kInfo,DWORD dwSocketID,IServerUserItem* pIServerUserItem,std::string kHttpChannel)
{
	if (kHttpChannel != "")
	{
		 int iCout = FangKaHttpUnits::UseCard(pIServerUserItem->GetUserID(),0,kHttpChannel);
		 DBO_GR_CreatePrivateResoult kNetInfo;
		 ZeroMemory(&kNetInfo,sizeof(kNetInfo));
		 kNetInfo.dwAgaginTable = kInfo->dwAgaginTable;
		 kNetInfo.bGameRuleIdex = kInfo->bGameRuleIdex;
		 kNetInfo.bGameTypeIdex = kInfo->bGameTypeIdex;
		 kNetInfo.bPlayCoutIdex = kInfo->bPlayCoutIdex;
		 kNetInfo.cbRoomType = Type_Private;

		 if (iCout < (int)kInfo->dwCost)
		 {
			 kNetInfo.bSucess = false;
			 lstrcpyn(kNetInfo.szDescribeString,TEXT("房卡不够"),CountArray(kNetInfo.szDescribeString));
		 }
		 else
		 {
			 kNetInfo.bSucess = true;
			 kNetInfo.lCurSocre = iCout;
		 }
		 OnEventCreatePrivate(DBO_GR_CREATE_PRIVATE,pIServerUserItem,&kNetInfo,sizeof(kNetInfo),kHttpChannel);
	}
	else
	{
		m_pIDataBaseEngine->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_CREATE_PRIVAT,dwSocketID,kInfo,sizeof(DBR_GR_Create_Private));
	}
}
//用户积分
bool PriaveteGame::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,cbReason);
	return true;
}

//用户状态
bool PriaveteGame::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//用户权限
bool PriaveteGame::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,bGameRight);
	}

	return true;
}

//用户登录
bool PriaveteGame::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	return true;
}

//用户登出
bool PriaveteGame::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//进入事件
bool PriaveteGame::OnEventEnterPrivate(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//手机用户
	if(bIsMobile == true)
	{
		//处理消息
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//构造数据
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));		
	}
	else
	{
		//处理消息
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//构造数据
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}
	return true;
}
bool PriaveteGame::AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pTbableFrame);
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}
	if (dwChairID >= 100 || bActionIdex >= MAX_PRIVATE_ACTION)
	{
		return true;
	}
	pTableInfo->lPlayerAction[dwChairID][bActionIdex] ++;
	return true;
}

//用户
bool PriaveteGame::OnEventUserJoinPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_PRIVATE_INFO,&m_kPrivateInfo,sizeof(m_kPrivateInfo));

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	//判断状态
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		sendPrivateRoomInfo(pIServerUserItem,getTableInfoByTableID(pIServerUserItem->GetTableID()));
	}
	return true;
}

//用户退赛
bool PriaveteGame::OnEventUserQuitPrivate(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return true;
}

//游戏开始
bool PriaveteGame::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}
	pTableInfo->dwStartPlayCout ++;
	pTableInfo->bStart = true;
	pTableInfo->newRandChild();

	sendPrivateRoomInfo(NULL,pTableInfo);
	return true;
}

//游戏结束
bool PriaveteGame::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	if (!pTableInfo)
	{
		return true;
	}

	if (pTableInfo->cbRoomType == Type_Private)
	{
		if (pTableInfo->dwFinishPlayCout == 0)
		{
			if (pTableInfo->kHttpChannel != "")
			{
				FangKaHttpUnits::UseCard(pTableInfo->dwCreaterUserID,(int)m_kPrivateInfo.lPlayCost[pTableInfo->bPlayCoutIdex],pTableInfo->kHttpChannel);
			}
			else
			{
				CreatePrivateCost(pTableInfo);
			}
		}
		pTableInfo->dwFinishPlayCout ++;
		sendPrivateRoomInfo(NULL,pTableInfo);
		if (pTableInfo->dwFinishPlayCout >= pTableInfo->dwPlayCout)
		{
			DismissRoom(pTableInfo);
		}
	}
	else if (pTableInfo->cbRoomType == Type_Public)
	{
		CreatePrivateCost(pTableInfo);
		DismissRoom(pTableInfo);
		ClearRoom(pTableInfo);
	}
	return true;
}
bool PriaveteGame::WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,datastream& kData)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	if (!pTableInfo)
	{
		return true;
	}
	pTableInfo->writeSocre( ScoreInfoArray,wScoreCount,kData);

	return true;
}
void PriaveteGame::sendPrivateRoomInfo(IServerUserItem * pIServerUserItem,PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return;
	}

	CMD_GF_Private_Room_Info kNetInfo;
	kNetInfo.cbRoomType = pTableInfo->cbRoomType;
	kNetInfo.bStartGame = pTableInfo->bStart;
	kNetInfo.dwRoomNum = pTableInfo->dwRoomNum;
	kNetInfo.dwPlayCout = pTableInfo->dwStartPlayCout;
	kNetInfo.dwCreateUserID = pTableInfo->dwCreaterUserID;
	kNetInfo.bGameRuleIdex = pTableInfo->bGameRuleIdex;
	kNetInfo.bGameTypeIdex = pTableInfo->bGameTypeIdex;
	kNetInfo.bPlayCoutIdex = pTableInfo->bPlayCoutIdex;
	kNetInfo.dwPlayTotal = pTableInfo->dwPlayCout;
	kNetInfo.kWinLoseScore.resize(pTableInfo->pITableFrame->GetChairCount());
	for (WORD i = 0;i<pTableInfo->pITableFrame->GetChairCount();i++)
	{
		kNetInfo.kWinLoseScore[i] =  (int)pTableInfo->lPlayerWinLose[i];
	}

	datastream kDataStream;
	kNetInfo.StreamValue(kDataStream,true);
	
	if (pIServerUserItem)
	{
		SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GF_PRIVATE_ROOM_INFO,&kDataStream[0],kDataStream.size());
	}
	else
	{
		SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GF_PRIVATE_ROOM_INFO,&kDataStream[0],kDataStream.size());
	}
}
//断线
bool PriaveteGame::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return false;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return false;
	}
	if (pTableInfo->dwCreaterUserID == pIServerUserItem->GetUserID() && !pTableInfo->bInEnd)
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		pTableInfo->pITableFrame->PerformStandUpActionEx(pIServerUserItem);
	}
	return true;
}
//用户坐下
bool PriaveteGame::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool PriaveteGame::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//移除分组
	return true;
}

 //用户同意
bool PriaveteGame::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return true;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		return false;
	}
	return true; 
}

bool PriaveteGame::OnEventReqStandUP(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return true;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}
	if (pTableInfo->bStart && !pTableInfo->bInEnd)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("游戏已经开始，无法退出。"),SMT_EJECT);
		return true;
	}
	if (getTableInfoByCreaterID(pIServerUserItem->GetUserID()) && !pTableInfo->bInEnd)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("您已返回大厅,房间将会继续保留。"),SMT_CLOSE_GAME);
		return true;
	}
	pTableInfo->pITableFrame->PerformStandUpActionEx(pIServerUserItem);
	return true;
}
bool PriaveteGame::OnEventClientReady(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return true;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return true;
	}
	sendPrivateRoomInfo(pIServerUserItem,pTableInfo);

	if (pTableInfo->pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		pIServerUserItem->SetUserStatus(US_READY,
			pTableInfo->pITableFrame->GetTableID(),wChairID);
	}

	if (pTableInfo->kDismissChairID.size())
	{
		CMD_GF_Private_Dismiss_Info kNetInfo;
		kNetInfo.dwDissUserCout = pTableInfo->kDismissChairID.size();
		kNetInfo.dwNotAgreeUserCout = pTableInfo->kNotAgreeChairID.size();
		for (int i = 0;i<(int)pTableInfo->kDismissChairID.size();i++)
		{
			kNetInfo.dwDissChairID[i] = pTableInfo->kDismissChairID[i];
		}
		for (int i = 0;i<(int)pTableInfo->kNotAgreeChairID.size();i++)
		{
			kNetInfo.dwNotAgreeChairID[i] = pTableInfo->kNotAgreeChairID[i];
		}
		SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GR_PRIVATE_DISMISS,&kNetInfo,sizeof(kNetInfo));
	}

	return true;
}