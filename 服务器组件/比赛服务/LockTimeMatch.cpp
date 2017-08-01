#include "StdAfx.h"
#include "LockTimeMatch.h"
#include "..\游戏服务器\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//无效值

//////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+100)			//分配用户

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CLockTimeMatch::CLockTimeMatch()
{
	//状态变量
	m_MatchStatus=MatchStatus_Free;

	//比赛配置
	m_pMatchOption=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//内核接口
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

CLockTimeMatch::~CLockTimeMatch(void)
{
	//释放资源
	SafeDeleteArray(m_ppITableFrame);

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

	//移除元素
	m_DistributeManage.RemoveAll();
}

//接口查询
VOID* CLockTimeMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//绑定桌子
bool CLockTimeMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//创建钩子
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//初始化接口
bool CLockTimeMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//服务配置
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
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

	//分组设置
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//创建桌子
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	//开始间隔
	DWORD dwStartInterval = GetMatchStartInterval();
	if(dwStartInterval>0)
	{
		//开始检测	
		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,dwStartInterval*1000L,TIMES_INFINITY,NULL);
	}

	return true;
}

//时间事件
bool CLockTimeMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	switch(dwTimerID)
	{
	case IDI_DISTRIBUTE_USER:				//分配用户
		{
			//执行分组
			PerformDistribute();

			return true;
		}
	case IDI_CHECK_START_MATCH:				//开始比赛
		{
			//设置状态
			m_MatchStatus=MatchStatus_Playing;

			//发送状态
			BYTE cbMatchStatus=MS_MATCHING;
			m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

			//关闭定时器
			m_pITimerEngine->KillTimer(IDI_CHECK_START_MATCH);

			//检测比赛结束
			m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,0);

			//执行分组
			m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);

			SendMatchInfo(NULL);

			return true;
		}
	case IDI_CHECK_END_MATCH:				//结束比赛
		{
			//获取时间
			CTime CurTime=CTime::GetCurrentTime();
			SYSTEMTIME MatchEndTime=m_pMatchOption->MatchType.LockTime.MatchEndTime;

			//比赛完成
			if (CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
				MatchEndTime.wHour*3600+MatchEndTime.wMinute*60+MatchEndTime.wSecond)
			{
				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
				m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

				//设置状态
				m_MatchStatus=MatchStatus_End;	

				//发送状态
				BYTE cbMatchStatus=MS_MATCHEND;
				m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

				//解散游戏
				for(INT i=0; i<m_pGameServiceOption->wTableCount; i++)
				{
					//获取对象
					ITableFrame * pITableFrame=m_ppITableFrame[i];					
					ASSERT(pITableFrame!=NULL);
					if (pITableFrame==NULL) continue;

					//解散游戏
					if(pITableFrame->IsGameStarted())
					{
						pITableFrame->DismissGame();
					}
				}

				//比赛结束
				if(CurTime >= MatchEndTime)
				{
					//比赛结束
					DBR_GR_MatchOver MatchOver;
					ZeroMemory(&MatchOver,sizeof(MatchOver));

					//构造数据				
					MatchOver.cbMatchType=MATCH_TYPE_LOCKTIME;
					MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
					MatchOver.dwMatchNO=m_pMatchOption->dwMatchNO;
					MatchOver.wServerID=m_pGameServiceOption->wServerID;
					MatchOver.MatchEndTime=m_pMatchOption->MatchType.LockTime.MatchEndTime;
					MatchOver.MatchStartTime=m_pMatchOption->MatchType.LockTime.MatchStartTime;

					//投递请求
					m_pIDataBaseEngine->PostDataBaseRequest(0,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
					
					return true;
				}

				//发送结束消息
				TCHAR szMessage[128]=TEXT("今天的比赛到此结束，请您明天再来参加！");
				m_pIGameServiceFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);

				//比赛检测
				DWORD dwStartInterval = GetMatchStartInterval();
				if(dwStartInterval>0)
				{
					m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,dwStartInterval*1000L,TIMES_INFINITY,NULL);
				}
			}
			return true;
		}
	}
	return true;
}

//数据库事件
bool CLockTimeMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch(wRequestID)
	{
	case DBO_GR_MATCH_RANK_LIST:			//比赛排行
		{
			//参数校验
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//提取数据
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//变量定义
			bool bRewardUser=true;
			tagMatchRankInfo * pMatchRankInfo=NULL;
			IServerUserItem * pIRewardUserItem=NULL;

			//发放奖励
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//查找玩家
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if (pIRewardUserItem==NULL || pIRewardUserItem->IsAndroidUser()==true) continue;

				//写入奖励
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);

				//设置标识
				pIRewardUserItem->SetMatchData(&bRewardUser);
			}

			//在线用户
			WORD wUserIndex=0;			
			while (true)
			{
				pIRewardUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
				if (pIRewardUserItem==NULL) break;
				if (pIRewardUserItem->IsAndroidUser()==true) continue;
				if (pIRewardUserItem->GetMatchData()==&bRewardUser)
				{
					pIRewardUserItem->SetMatchData(NULL);
					continue;
				}

				//发送结束消息
				TCHAR szMessage[128]=TEXT("本次比赛到此结束，请您前往网站查看系统排名或移驾其他比赛房间参与比赛！");
				m_pIGameServiceFrame->SendGameMessage(pIRewardUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);	
			}			

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:	//奖励结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBR_GR_MatchRewardResult)) return false;

			//提取数据
			DBR_GR_MatchRewardResult * pMatchRewardResult = (DBR_GR_MatchRewardResult*)pData;

			//奖励成功
			if(pMatchRewardResult->bResultCode==true)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchRewardResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pMatchRewardResult->dwCurrExperience;

				//发送数据
				m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//比赛事件
bool CLockTimeMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	return true;
}
//用户积分
bool CLockTimeMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserScore UserScore;
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//变量定义
	CMD_GR_MobileUserScore MobileUserScore;

	//构造数据
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;

	//构造积分
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));

	//即时写分
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//比赛信息
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.dwMatchNO=m_pMatchOption->dwMatchNO;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);
	}

	return true;
}

//用户状态
bool CLockTimeMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//用户权限
bool CLockTimeMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,bGameRight);
	}

	return true;
}

//用户登录
bool CLockTimeMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//发送状态
	if(m_MatchStatus>MatchStatus_Free)
	{
		//发送状态
		BYTE cbMatchStatus=(m_MatchStatus==MatchStatus_Playing)?MS_MATCHING:MS_MATCHEND;
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

		SendMatchInfo(NULL);
	}

	return true;
}

//用户登出
bool CLockTimeMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//构造结构
	SendMatchInfo(NULL);
	return true;
}

//进入事件
bool CLockTimeMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//判断时间
	TCHAR szDescribe[256];
	WORD wMessageType;
	if (VerifyMatchTime(szDescribe,CountArray(szDescribe),wMessageType)==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,wMessageType|SMT_GLOBAL,dwUserIP==0);
		return true;
	}

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
		LogonMobile.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonMobile.dwMatchNO=m_pMatchOption->dwMatchNO;
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
		LogonUserID.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonUserID.dwMatchNO=m_pMatchOption->dwMatchNO;	
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//用户参赛
bool CLockTimeMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//判断状态
	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		return false;
	}

	//比赛未开始
	if(m_MatchStatus==MatchStatus_Free)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("比赛尚未开始，请稍后再来！"),SMT_EJECT);

		return false;
	}

	//比赛结束
	if(m_MatchStatus==MatchStatus_End)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("比赛已经结束，请您移驾至其他比赛房间参与比赛！"),SMT_EJECT);
		return true;
	}

	//加入分组
	if(InsertDistribute(pIServerUserItem)==false) return false;
	
	//发送成绩
	SendMatchUserScore(pIServerUserItem);

	//发送消息
	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("系统正在努力为您分配座位，请稍后。。。"),SMT_CHAT);

	SendMatchInfo(NULL);

	return true;
}

//用户退赛
bool CLockTimeMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//移除分组
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	//移除同桌信息
	m_DistributeManage.RemoveUserSameTableInfo(pIServerUserItem->GetUserID());

	return true;
}

//游戏开始
bool CLockTimeMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//发送比赛信息
	SendTableUserMatchInfo(pITableFrame, INVALID_CHAIR);

	//构造数据包
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//变量定义
	IServerUserItem *pIServerUserItem=NULL;
	IServerUserItem *pITableUserItem=NULL;	

	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		pIServerUserItem=pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem!=NULL);
		if (pIServerUserItem==NULL) continue;

		//构造提示消息
		TCHAR szMessage[64]=TEXT("");
		_sntprintf(szMessage, CountArray(szMessage),TEXT("本局比赛是您的第 %d 局。"),pIServerUserItem->GetUserPlayCount()+1);
		lstrcpyn(SystemMessage.szString,szMessage,CountArray(SystemMessage.szString));
		SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
		SystemMessage.wType=SMT_CHAT;

		//发送提示数据
		WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
		pITableFrame->SendTableData(pIServerUserItem->GetChairID(),SUB_GF_SYSTEM_MESSAGE, &SystemMessage,wSendSize,MDM_GF_FRAME);

		//清除同桌
		ClearSameTableUser(pIServerUserItem->GetUserID());

		for (INT j=0; j<pITableFrame->GetChairCount(); j++)
		{
			//获取用户
			pITableUserItem=pITableFrame->GetTableUserItem(j);
			if(pITableUserItem==NULL) continue;
			if(pITableUserItem==pIServerUserItem) continue;

			//插入同桌用户
			InserSameTableUser(pIServerUserItem->GetUserID(),pITableUserItem->GetUserID());
		}
	}

	return true;
}

//游戏结束
bool CLockTimeMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	//结束处理
	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		IServerUserItem *pIUserItem=pITableFrame->GetTableUserItem(i);
		ASSERT(pIUserItem!=NULL);
		if (pIUserItem==NULL) continue;

		//正常结束
		if(cbReason!=REMOVE_BY_ENDMATCH)
		{
			//达到淘汰线
			if (m_pMatchOption->MatchType.LockTime.lMatchCullScore>0 && pIUserItem->GetUserScore()<m_pMatchOption->MatchType.LockTime.lMatchCullScore)
			{
				//变量定义
				TCHAR szMessage[128]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("由于您的比赛币低于 %d，您将不能继续游戏！"),m_pMatchOption->MatchType.LockTime.lMatchCullScore);

				//发送消息
				m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_EJECT|SMT_CHAT);

				//构造结构
				DBR_GR_MatchEliminate MatchEliminate;
				MatchEliminate.cbMatchType=MATCH_TYPE_LOCKTIME;
				MatchEliminate.dwMatchID=m_pMatchOption->dwMatchID;
				MatchEliminate.dwMatchNO=m_pMatchOption->dwMatchNO;
				MatchEliminate.dwUserID=pIUserItem->GetUserID();

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(0,DBR_GR_MATCH_ELIMINATE,0,&MatchEliminate,sizeof(MatchEliminate));

				//发送数据
				m_pIGameServiceFrame->SendData(pIUserItem,MDM_GR_MATCH,SUB_GR_MATCH_ELIMINATE,NULL,0);

				continue;
			}

			//插入分组
			if(InsertDistribute(pIUserItem)==true)
			{
				m_pIGameServiceFrame->SendGameMessage(pIUserItem,TEXT("本局比赛结束，系统正在努力为您分配座位，请稍后..."),SMT_CHAT|SMT_TABLE_ROLL);
			}

			//完成局数
			if(pIUserItem->GetUserPlayCount()-pIUserItem->GetUserInfo()->dwFleeCount==m_pMatchOption->MatchType.LockTime.wMatchPlayCount)
			{
				TCHAR szMessage[128]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("您已完成%d局比赛，可以继续当前比赛或选择退出等待本次比赛活动结束后排名！"),m_pMatchOption->MatchType.LockTime.wMatchPlayCount);
				m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_CHAT|SMT_TABLE_ROLL);
			}
		}
	}

	return true;
}

//用户坐下
bool CLockTimeMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool CLockTimeMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//移除分组
	return RemoveDistribute(pIServerUserItem);
}

 //用户同意
bool CLockTimeMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return true; 
}

//参赛权限
bool CLockTimeMatch::VerifyUserEnterMatch(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//机器人不受限制
	if (pIServerUserItem->IsAndroidUser()==true)
		return true;

	//变量定义
	WORD wMessageType;
	TCHAR szMessage[128]=TEXT("");

	//结束时间已到	
	if (VerifyMatchTime(szMessage,CountArray(szMessage),wMessageType)==false)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,wMessageType);

		return false;
	}

	//达到淘汰线
	if (m_pMatchOption->MatchType.LockTime.lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pMatchOption->MatchType.LockTime.lMatchCullScore)
	{
		_sntprintf(szMessage,CountArray(szMessage),TEXT("由于您的比赛币低于 %d，您将不能继续游戏！"),m_pMatchOption->MatchType.LockTime.lMatchCullScore);

		//发送消息
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}

		return false;
	}

	return true;
}

//开始间隔
DWORD CLockTimeMatch::GetMatchStartInterval()
{
	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchStartTime(m_pMatchOption->MatchType.LockTime.MatchStartTime);
	CTime MatchEndTime(m_pMatchOption->MatchType.LockTime.MatchEndTime);

	//变量定义
	LONGLONG dwTimeInterval=1;
	DWORD dwCurrStamp,dwStartStamp,dwEndStamp;

	//比赛未开始
	if(CurTime<MatchStartTime)
	{
		CTimeSpan TimeSpan = MatchStartTime-CurTime;
		dwTimeInterval = TimeSpan.GetTotalSeconds();
	}

	//比赛已结束
	if(CurTime>=MatchEndTime) dwTimeInterval=0;

	//计算时间
	dwCurrStamp = CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();
	dwStartStamp = m_pMatchOption->MatchType.LockTime.MatchStartTime.wHour*3600+m_pMatchOption->MatchType.LockTime.MatchStartTime.wMinute*60+m_pMatchOption->MatchType.LockTime.MatchStartTime.wSecond;
	dwEndStamp = m_pMatchOption->MatchType.LockTime.MatchEndTime.wHour*3600+m_pMatchOption->MatchType.LockTime.MatchEndTime.wMinute*60+m_pMatchOption->MatchType.LockTime.MatchEndTime.wSecond;

	//比赛未开始
	if(dwCurrStamp<dwStartStamp) 
	{
		dwTimeInterval = dwStartStamp-dwCurrStamp;
	}

	//比赛已结束
	if(dwCurrStamp>=dwEndStamp)
	{
		dwTimeInterval = 24*3600-(dwCurrStamp-dwStartStamp);		
	}

	return (DWORD)dwTimeInterval;
}

//分配用户
bool CLockTimeMatch::PerformDistribute()
{
	//状态判断
	if(m_MatchStatus!=MatchStatus_Playing) return true;

	//人数校验
	if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinDistributeUser) return false;

	//分配用户
	while(true)
	{
		//变量定义
		CDistributeInfoArray DistributeInfoArray;

        //获取用户
		//WORD wRandCount = __max(m_pGameServiceOption->wMaxDistributeUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pGameServiceAttrib->wChairCount/*+rand()%wRandCount*/;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if(wDistributeCount < wChairCount/*m_pGameServiceOption->wMinPartakeGameUser*/) break;

		//寻找位置
		ITableFrame * pICurrTableFrame=NULL;
		for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//获取对象
			ASSERT(m_ppITableFrame[i]!=NULL);
			ITableFrame * pITableFrame=m_ppITableFrame[i];

			//状态判断
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame=pITableFrame;
				break;
			}
		}

		//桌子判断
		if(pICurrTableFrame==NULL) break;

		//玩家坐下
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//变量定义
			WORD wChairID=INVALID_CHAIR;

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
				//获取用户
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//用户起立
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=pIServerUserItem->GetTableID();
					ITableFrame * pITableFrame = m_ppITableFrame[wTableID];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//用户坐下
				if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//坐下结果
		if(bSitSuccess)
		{
			//移除结点
			while(DistributeInfoArray.GetCount()>0)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[0].pPertainNode);
				DistributeInfoArray.RemoveAt(0);
			}
		}
		else
		{
			for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
			{
				if(nSitFailedIndex==nIndex)
				{
					m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
				}
				else
				{
					//用户起立
					if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
					{
						WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
						m_ppITableFrame[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
					}
				}
			}
		}		
	}

	return true;
}

//插入用户
bool CLockTimeMatch::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//比赛状态
 	if(m_MatchStatus!=MatchStatus_Playing) return false;

	//状态判断
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//参赛校验
	if(VerifyUserEnterMatch(pIServerUserItem)==false) return false;

	//变量定义
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//设置变量
	DistributeInfo.pPertainNode=NULL;	
	DistributeInfo.dwInsertStamp = (DWORD)time(NULL);
	DistributeInfo.pIServerUserItem=pIServerUserItem;   

	//插入节点
	if(m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	return true;
}

//清除玩家
void CLockTimeMatch::ClearSameTableUser(DWORD dwUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		pSameTableInfo->wPlayerCount=0;
		ZeroMemory(pSameTableInfo->wPlayerIDSet,sizeof(pSameTableInfo->wPlayerIDSet));
	}
}

//插入用户
void CLockTimeMatch::InserSameTableUser(DWORD dwUserID,DWORD dwTableUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		//添加用户
		pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++] = dwTableUserID;
	}

	return;
}

//事件校验
bool CLockTimeMatch::VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType)
{
	//变量定义
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchStartTime(m_pMatchOption->MatchType.LockTime.MatchStartTime);
	CTime MatchEndTime(m_pMatchOption->MatchType.LockTime.MatchEndTime);

	//设置类型
	wMessageType = SMT_EJECT|SMT_CHAT;

	////比赛未开始
	//if(CurTime<MatchStartTime)
	//{
	//	wMessageType |= SMT_CLOSE_ROOM;
	//	_sntprintf(pszMessage,wMaxCount,TEXT("比赛尚未开始,请您于%d年%d月%d日-%d时%d分%d秒前来参加比赛！"),MatchStartTime.GetYear(),MatchStartTime.GetMonth(),MatchStartTime.GetDay(),MatchStartTime.GetHour(),MatchStartTime.GetMinute(),MatchStartTime.GetSecond());
	//	return false;
	//}

	//比赛已结束
	if(CurTime>=MatchEndTime)
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("比赛已经结束,请您移驾至其他比赛房间参与比赛吧！"));
		return false;
	}

	////比赛未开始
	//if(CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond()<
	//  m_pMatchOption->MatchType.LockTime.MatchStartTime.wHour*3600+m_pMatchOption->MatchType.LockTime.MatchStartTime.wMinute*60+m_pMatchOption->MatchType.LockTime.MatchStartTime.wSecond)
	//{
	//	wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
	//	_sntprintf(pszMessage,wMaxCount,TEXT("今天的比赛尚未开始,请您于%d时%d分%d秒前来参加比赛！"),MatchStartTime.GetHour(),MatchStartTime.GetMinute(),MatchStartTime.GetSecond());
	//	return false;
	//}

	//比赛已结束
	if(CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond()>= m_pMatchOption->MatchType.LockTime.MatchEndTime.wHour*3600+m_pMatchOption->MatchType.LockTime.MatchEndTime.wMinute*60+m_pMatchOption->MatchType.LockTime.MatchEndTime.wSecond)
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("今天的比赛已经结束,请您于明天%d时%d分%d秒前来参加比赛！"),MatchStartTime.GetHour(),MatchStartTime.GetMinute(),MatchStartTime.GetSecond());
		return false;
	}

	return true;
}

//发送用户分数
bool CLockTimeMatch::SendMatchUserScore(IServerUserItem * pIServerUserItem)
{
	//发送数据
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lScore=pUserInfo->lScore;
	//UserScore.UserScore.lIngot=pUserInfo->lIngot;

	return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//移除玩家
bool CLockTimeMatch::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//验证参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//移除用户
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}


//获取名次
WORD CLockTimeMatch::GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame)
{
	//参数效验
	ASSERT(pUserItem!=NULL);
	if(pUserItem==NULL) return INVALID_WORD;
	if (!pITableFrame)
	{
		return INVALID_WORD;
	}
	//定义变量
	SCORE lUserScore=pUserItem->GetUserScore();
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	ASSERT(pUserItem->GetTableID()==pITableFrame->GetTableID());
	if(pUserItem->GetTableID()!=pITableFrame->GetTableID()) return INVALID_WORD;

	for(WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
	{
		pLoopUserItem=pITableFrame->GetTableUserItem(i);
		if(pLoopUserItem==NULL) continue;

		if(pLoopUserItem!=pUserItem && pLoopUserItem->GetUserScore()>lUserScore)
			wRank++;
	}
	return wRank;
}
//发送比赛信息
void CLockTimeMatch::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//验证参数
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//构造数据
	CMD_GR_Match_Info MatchInfo;
	ZeroMemory(&MatchInfo,sizeof(MatchInfo));

	//设置标题
	_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：累计积分"));
	_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_pMatchOption->MatchType.LockTime.lMatchInitScore);
	_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：定时开赛"));

	for(WORD i=0; i<pITableFrame->GetChairCount(); i++)
	{
		//获取用户
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//设置局数
		MatchInfo.wGameCount=(WORD)pIServerUserItem->GetUserPlayCount()+1;
		MatchInfo.wRank=GetUserRank(pIServerUserItem,pITableFrame);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			pITableFrame->SendTableData(i,SUB_GR_MATCH_INFO,&MatchInfo,sizeof(MatchInfo),MDM_GR_MATCH);
		}
	}

	return;
}

//写入奖励
bool CLockTimeMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//变量定义
	CMD_GR_MatchResult MatchResult;
	ZeroMemory(&MatchResult,sizeof(MatchResult));

	//比赛奖励
	MatchResult.lGold=pMatchRankInfo->lRewardGold;
	MatchResult.dwIngot=pMatchRankInfo->dwRewardIngot;
	MatchResult.dwExperience=pMatchRankInfo->dwRewardExperience;

	//获奖提示
	_sntprintf(MatchResult.szDescribe,CountArray(MatchResult.szDescribe),TEXT("%s, 恭喜您在%s中获得第%d名,奖励如下："),
	pIServerUserItem->GetNickName(),m_pMatchOption->szMatchName,pMatchRankInfo->wRankID);

	//更新用户信息
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	//pUserInfo->lIngot+=pMatchRankInfo->dwRewardIngot;
	pUserInfo->dwExperience+=pMatchRankInfo->dwRewardExperience;
	OnEventUserItemScore(pIServerUserItem, 0);

	//发送数据
	if (pIServerUserItem->IsAndroidUser()==false)
	{
		m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
	}

	//写入记录
	if(m_pIDataBaseEngine!=NULL)
	{
		//变量定义
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		
		//构造结构
		MatchReward.dwUserID=pIServerUserItem->GetUserID();	
		MatchReward.lRewardGold=pMatchRankInfo->lRewardGold;
		MatchReward.dwRewardIngot=pMatchRankInfo->dwRewardIngot;
		MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
		MatchReward.dwClientAddr=pIServerUserItem->GetClientAddr();

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	}

	return true;
}
//发送数据
bool CLockTimeMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{
	CMD_GR_Match_Num MatchNum;
	ZeroMemory(&MatchNum,sizeof(MatchNum));

	MatchNum.dwWaitting=m_pIServerUserManager->GetUserItemCount();
	MatchNum.dwTotal=m_pGameServiceAttrib->wChairCount;

	if(pIServerUserItem==NULL)
	{
		m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		return true;
	}

	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
