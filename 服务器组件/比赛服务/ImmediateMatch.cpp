#include "StdAfx.h"
#include "ImmediateMatch.h"
#include "..\游戏服务器\DataBasePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//无效值

//////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+100)		//分配用户

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CImmediateMatch::CImmediateMatch()
{
	//比赛配置
	m_pMatchOption=NULL;	
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//设置指针
	m_ppITableFrame=NULL;
	m_pCurMatchGroup=NULL;

	//内核接口
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
	m_pIServerUserItemSink=NULL;
}

CImmediateMatch::~CImmediateMatch(void)
{
	SafeDeleteArray(m_ppITableFrame);
	
	m_LoopTimer.RemoveAll();
	SafeDelete(m_pCurMatchGroup);
	m_MatchGroup.Append(m_OverMatchGroup);
	for (int i=0; i<m_MatchGroup.GetCount(); i++)
	{
		SafeDelete(m_MatchGroup[i]);
	}

	m_MatchGroup.RemoveAll();
	m_OverMatchGroup.RemoveAll();
}

//接口查询
VOID* CImmediateMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);	
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CImmediateMatch::OnStartService()
{
	//创建分组
	if (m_pCurMatchGroup==NULL)
	{
		m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);
	}

	//最多同时9组编号
	for (int i=0;i<79;i++)
	{
		m_LoopTimer.Add(i);
	}
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);
//#ifdef _DEBUG
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,1000,TIMES_INFINITY,0);
//#else
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,3000,TIMES_INFINITY,0);
//#endif

	InsertNullTable();
}

//绑定桌子
bool CImmediateMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
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
bool CImmediateMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
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

	//创建桌子数组
	CImmediateGroup::m_wChairCount=m_pGameServiceAttrib->wChairCount;
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	return true;
}

//时间事件
bool CImmediateMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	if(dwTimerID>=IDI_GROUP_TIMER_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//查看是哪场比赛的定时器
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_TIMER_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//定时检测一下 那些比赛人数不够被卡的问题
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//参赛用户小于椅子数量 就要结束
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CImmediateGroup * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	}

	return true;
}


//数据库事件
bool CImmediateMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//报名结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//报名失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//加入比赛
			if(m_pCurMatchGroup->OnUserSignUpMatch(pIServerUserItem)==false)
			{
				SendRoomMessage(pIServerUserItem, TEXT("报名失败"),SMT_CHAT);
				return false;
			}

			//报名成功
			m_OnMatchUserItem.Add(pIServerUserItem);
			SendMatchInfo(NULL);
			if(m_pIGameServiceFrame!=NULL)
			{
				SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//退赛结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//退费失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//设置状态
			pIServerUserItem->SetSignUpStatus(MatchStatus_Null);

			//变量定义
			LPCTSTR pszMatchFeeType[]={TEXT("游戏币"),TEXT("元宝")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				//退费通知
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("退赛成功，退还报名费%I64d%s！"),m_pMatchOption->lMatchFee,pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}	
	case DBO_GR_MATCH_RANK_LIST:			//比赛排行
		{
			//参数校验
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//提取数据
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//变量定义
			IServerUserItem * pIRewardUserItem=NULL;
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//发放奖励
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//查找玩家
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if(pIRewardUserItem==NULL || pIRewardUserItem->IsAndroidUser()==true) continue;

				//写入奖励
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);
			}

			//变量定义
			CImmediateGroup * pImmediateGroup=NULL;

			//查找分组
			for (int i=0;i<m_MatchGroup.GetCount();i++)
			{
				if(m_MatchGroup[i]->m_dwMatchNO==pMatchRankList->dwMatchNO)
				{
					pImmediateGroup=m_MatchGroup[i];

					if(pImmediateGroup!=NULL)
					{
						//全部起立
						pImmediateGroup->RearrangeUserSeat();

						//发送可报名状态
						BYTE cbMatchStatus=MatchStatus_Null;
						SendGroupData(MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pImmediateGroup);

						//还原编号
						m_LoopTimer.Add(pImmediateGroup->m_LoopTimer);

						//参赛玩家退赛
						POSITION pos=pImmediateGroup->m_OnMatchUserMap.GetStartPosition();
						IServerUserItem *pITempUserItem=NULL;
						DWORD dwUserID=0;
						WORD wQuitUserCount=0;
						WORD wMatchUserCount=(WORD)pImmediateGroup->m_OnMatchUserMap.GetCount();
						while(pos!=NULL)
						{
							pImmediateGroup->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pITempUserItem);
							if (pITempUserItem!=NULL)
							{
								pITempUserItem->SetSignUpStatus(MatchStatus_Null);
								if (true==OnEventUserQuitMatch(pITempUserItem,0))
									wQuitUserCount++;

								//比赛结束后代打玩家未返回，则踢出房间
								bool bAndroidUser=pITempUserItem->IsAndroidUser();
								//bool bTrusteeUser=pITempUserItem->IsTrusteeUser();
								bool bClientReady=pITempUserItem->IsClientReady();
								//if (bAndroidUser==false && bTrusteeUser==true && bClientReady==false)
								if (bAndroidUser==false && bClientReady==false)
								{
									//离开桌子
									WORD wTableID=pITempUserItem->GetTableID();
									if (wTableID!=INVALID_TABLE)
									{
										//状态判断
										ASSERT (pITempUserItem->GetUserStatus()!=US_PLAYING);
										SendGameMessage(pITempUserItem,TEXT(""),SMT_CLOSE_GAME);
										
										//查找桌子并弹起
										for(int m=0;m<pImmediateGroup->m_MatchTableArray.GetCount();m++)
										{
											if(pITempUserItem->GetTableID()==pImmediateGroup->m_MatchTableArray[m]->wTableID)
											{
												pImmediateGroup->m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pITempUserItem);
												break;
											}
										}
									}

									//离开房间
									if (pITempUserItem->GetBindIndex()==INVALID_WORD)
									{
										pITempUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
									}
								}
							}
						}
						
						//校验退赛结果
						ASSERT(wQuitUserCount==wMatchUserCount);
						if (wQuitUserCount!=wMatchUserCount)
						{
							CTraceService::TraceString(TEXT("比赛结束后，退赛人数异常"),TraceLevel_Exception);
						}

						//回收全部桌子
						for (int j=0;pImmediateGroup->m_MatchTableArray.GetCount();)
						{
							tagTableFrameInfo* pTempTable=pImmediateGroup->m_MatchTableArray[j];
							pImmediateGroup->m_MatchTableArray.RemoveAt(j);
							SafeDelete(pTempTable);
						}
					}
					//移除对象
					m_MatchGroup.RemoveAt(i);
					m_OverMatchGroup.Add(pImmediateGroup);

					break;
				}
			}


			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//奖励结果
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
				SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//命令消息
bool CImmediateMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//比赛费用
		{
			//参数效验
			ASSERT(wDataSize==sizeof(SCORE));
			if(wDataSize!=sizeof(SCORE)) return false;

			//提取数据
			SCORE lMatchFee=*(SCORE*)pData;
			if(lMatchFee!=m_pMatchOption->lMatchFee)return false;

			//变量定义
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//构造结构
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();
			MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//退出比赛
		{
			//参数效验
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			OnEventUserQuitMatch(pIServerUserItem,0,0,dwSocketID);

			return true;
		}
	}
	return true;
}

//用户登录
bool CImmediateMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	return SendMatchInfo(pIServerUserItem);
}

//用户登出
bool CImmediateMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//进入事件
bool CImmediateMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
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
bool CImmediateMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("用户指针为空!"),TraceLevel_Warning);
		return true;
	}

	if(CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule))
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，由于系统维护，该房间禁止用户报名比赛！。"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	WORD wTableCount=GetNullTable();
	if(wTableCount<m_pMatchOption->MatchType.Immediate.wStartUserCount/CImmediateGroup::m_wChairCount)
	{
		CString str;
		str.Format( TEXT("抱歉，当前桌子不够请等待。还差 %d 张桌子可以报名"),m_pMatchOption->MatchType.Immediate.wStartUserCount/CImmediateGroup::m_wChairCount-wTableCount);
		SendRoomMessage(pIServerUserItem,str,SMT_CHAT);
		return true;
	}
	if(m_pCurMatchGroup==NULL || m_pCurMatchGroup->m_enMatchStatus==MatchStatus_Wait)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，该房间参赛人数已满，请稍后再报名或者进入另一房间比赛。"),SMT_CHAT);
		return true;
	}

	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]->GetUserID()==pIServerUserItem->GetUserID())
		{
			SendRoomMessage(pIServerUserItem, TEXT("您已经成功报名，不能重复报名！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	if(m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，比赛编号不够请稍等。"),SMT_CHAT);
		return true;
	}

	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("对不起，您当前的状态不允许参加比赛！"),SMT_CHAT);
		return true;
	}

	if (m_pMatchOption->MatchType.Immediate.dwInitalScore==0&&pIServerUserItem->GetUserScore()<m_pMatchOption->MatchType.Immediate.dwInitalBase*m_pMatchOption->MatchType.Immediate.wMinEnterGold+m_pMatchOption->lMatchFee)
	{
		TCHAR szMsg[256]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg),TEXT("对不起，您当前的金币低于了%d，不能参加比赛！"),
			m_pMatchOption->MatchType.Immediate.dwInitalBase*m_pMatchOption->MatchType.Immediate.wMinEnterGold+m_pMatchOption->lMatchFee);
		SendRoomMessage(pIServerUserItem, szMsg,SMT_CHAT|SMT_EJECT);
		return true;
	}

	//真人扣报名费
	if(m_pMatchOption->lMatchFee>0 && pIServerUserItem->IsAndroidUser()==false)
	{
		ASSERT(m_pIGameServiceFrame!=NULL);
		if(m_pIGameServiceFrame==NULL)
		{
			CTraceService::TraceString(TEXT("服务框架指针为空!"),TraceLevel_Warning);
			return true;
		}

		//构造结构
		CMD_GR_Match_Fee MatchFee;
		MatchFee.lMatchFee=m_pMatchOption->lMatchFee;

		//构造提示
		if(MatchFee.lMatchFee>0)
		{
			//变量定义
			LPCTSTR pszMatchFeeType[]={TEXT("游戏币"),TEXT("元宝")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				_sntprintf(MatchFee.szNotifyContent,CountArray(MatchFee.szNotifyContent),TEXT("参赛将扣除报名费 %I64d %s，确认要参赛吗？"),m_pMatchOption->lMatchFee,pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
			}
		}

		//发送消息
		WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

		return true;
	}

	//机器人报名
	if(m_pMatchOption->lMatchFee==0 || pIServerUserItem->IsAndroidUser()==true)
	{
		//机器数判断
		if(pIServerUserItem->IsAndroidUser()==true && (m_pCurMatchGroup->m_wAndroidUserCount >= 
		   m_pMatchOption->MatchType.Immediate.wAndroidUserCount))
		{
			return true;
		}

		return OnEventSocketMatch(SUB_GR_MATCH_FEE,&m_pMatchOption->lMatchFee,sizeof(SCORE),pIServerUserItem,dwSocketID);
	}

	//报名成功
	m_OnMatchUserItem.Add(pIServerUserItem);
	SendMatchInfo(NULL);
	if(m_pIGameServiceFrame!=NULL)
	{
		SCORE lScore=SCORE(m_pMatchOption->lMatchFee)*-1;
		/*if(m_pMatchOption->MatchType.Immediate.dwInitalScore==0)
		{
			pIServerUserItem->GetUserInfo()->lScore+=lScore;
			m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_FEE);
		}*/
		SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
	}

	return true;
}

//用户退赛
bool CImmediateMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//正在游戏状态
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//正在比赛阶段
	BYTE cbUserEnlist=pIServerUserItem->GetSignUpStatus();
	if (cbUserEnlist==MS_MATCHING)
	{
		return true;
	}

	//否则玩家退赛
	if(RemoveMatchUserItem(pIServerUserItem))
	{
		//从正在比赛的组中删除该玩家
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CImmediateGroup *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem))
			{
				//if(pMatch->m_enMatchStatus!=MatchStatus_Signup && pMatch->m_enMatchStatus!=MatchStatus_Wait)
				//{
				//	SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				//}
				if (pMatch->m_enMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				}
				return true;
			}
		}

		//从正在报名的组中删除该玩家
		if(m_pCurMatchGroup->OnUserQuitMatch(pIServerUserItem))
		{
			//退还报名费
			if(m_pMatchOption->lMatchFee>0 && !pIServerUserItem->IsAndroidUser())
			{
				//变量定义
				DBR_GR_MatchSignup MatchSignup;
				MatchSignup.dwUserID=pIServerUserItem->GetUserID();

				//构造结构
				MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
				MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
				lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchSignup,sizeof(MatchSignup));
			}

			if (m_pCurMatchGroup->m_enMatchStatus!=MatchStatus_Null)
			{
				SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
			}

			SendMatchInfo(NULL);
			return true;
		}
		SendMatchInfo(NULL);
	}

	return false;
}

//用户积分
bool CImmediateMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//获取对象
	CImmediateGroup * pImmediateGroup = static_cast<CImmediateGroup *>(pIServerUserItem->GetMatchData());
	if(pImmediateGroup==NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,cbReason);
	}

	//发送分数
	SendGroupUserScore(pIServerUserItem,pImmediateGroup);

	//变量定义
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

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
		WriteGameScore.dwMatchNO=pImmediateGroup->m_dwMatchNO;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);
	}

	return true;
}

//用户状态
bool CImmediateMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//清除数据
	if(pIServerUserItem->GetUserStatus()==US_FREE) pIServerUserItem->SetMatchData(NULL);

	//回调接口
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//用户权限
bool CImmediateMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,bGameRight);
	}

	return true;
}

//报名人满，开始比赛
bool CImmediateMatch::OnEventMatchStart(CImmediateGroup *pMatch)
{
	ASSERT(pMatch==m_pCurMatchGroup&&m_LoopTimer.GetCount()>0);
	
	//编号不够 不能报名的
	pMatch->m_LoopTimer=m_LoopTimer[0];
	m_LoopTimer.RemoveAt(0);
	m_MatchGroup.Add(pMatch);

	//准备新的一组比赛
	m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);
	
	SendMatchInfo(NULL);
	BYTE cbMatchStatus=MS_MATCHING;
	SendGroupData( MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pMatch);

	return true;
}


//比赛结束
bool CImmediateMatch::OnEventMatchOver(CImmediateGroup *pMatch)
{
	//比赛结束
	if(pMatch!=NULL)
	{
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver,sizeof(MatchOver));

		//构造数据						
		MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
		MatchOver.dwMatchNO=pMatch->m_dwMatchNO;		
		MatchOver.cbMatchType=MATCH_TYPE_IMMEDIATE;
		MatchOver.wServerID=m_pGameServiceOption->wServerID;
		
		//获取时间
		MatchOver.MatchStartTime=pMatch->m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(0,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
	}

	return true;
}

//关闭定时器
bool CImmediateMatch::KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)
{
	m_pITimerEngine->KillTimer(dwTimerID);

	return false;
}

//设置定时器
bool CImmediateMatch::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)
{
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}


//发送数据
bool CImmediateMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatchGroup->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_pMatchOption->MatchType.Immediate.wStartUserCount;
	if(pIServerUserItem==NULL)
	{
		m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		return true;
	}

	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbMatchStatus=pIServerUserItem->GetSignUpStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
			return true;
		}
	}

	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
	if((SCORE)m_pMatchOption->MatchType.Immediate.dwInitalScore!=0)
	{
		pUserScore->lScore=(LONG)m_pMatchOption->MatchType.Immediate.dwInitalScore;
	}

	return SendGroupUserScore(pIServerUserItem, NULL);
}


//发送信息
bool CImmediateMatch::SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		SendGameMessage(pUserItem,pStrMessage, SMT_CHAT|SMT_TABLE_ROLL);
	}
	return true;
}


//发送用户分数
bool CImmediateMatch::SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
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
	//UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;	

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;

	if(pMatch!=NULL)
	{
		POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pUserItem=NULL;
		DWORD dwUserID=0;
		while(pos!=NULL)
		{
			pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));	
		}
		return true;
	}

	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}


//发送状态
bool  CImmediateMatch::SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsClientReady()==true);
	if (pIServerUserItem->IsClientReady()==false) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

	//构造数据
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.UserStatus.wTableID=pUserData->wTableID;
	UserStatus.UserStatus.wChairID=pUserData->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserData->cbUserStatus;

	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		if(pUserItem->IsClientReady())
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));	
	}
	return true;
}


//发送游戏消息
bool CImmediateMatch::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true)&&(pIServerUserItem->IsAndroidUser()==false))
	{
		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;
		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}


//发送消息
bool CImmediateMatch::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD && false==pIServerUserItem->IsAndroidUser())
	{
		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;

		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//发送数据到一组用户
bool CImmediateMatch::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		m_pIGameServiceFrame->SendData(pUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);	
	}
	return true;
}

//发送数据
bool CImmediateMatch::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, wMainCmdID,wSubCmdID, pData, wDataSize);
    
	return true;
}

//为首轮插入空桌子
void CImmediateMatch::InsertNullTable()
{
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//超过就退出
		WORD nCurTableCount=(WORD)m_pCurMatchGroup->m_MatchTableArray.GetCount();
		if(nCurTableCount>=m_pMatchOption->MatchType.Immediate.wStartUserCount/m_pGameServiceAttrib->wChairCount)break;

		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			tagTableFrameInfo * pTableFrameInfo=new tagTableFrameInfo;
			ZeroMemory(pTableFrameInfo,sizeof(tagTableFrameInfo));			
			pTableFrameInfo->wTableID=i;
			pTableFrameInfo->pTableFrame=m_ppITableFrame[i];
			pTableFrameInfo->dwBaseScore=m_pMatchOption->MatchType.Immediate.dwInitalBase;
			ITableFrameHook * pFrameHook=QUERY_OBJECT_PTR_INTERFACE(m_ppITableFrame[i]->GetTableFrameHook(),ITableFrameHook);
			pFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(m_pCurMatchGroup,IUnknownEx));
			m_pCurMatchGroup->AddMatchTable(pTableFrameInfo);
		}
	}
}

//获取一个空闲的机器人
IAndroidUserItem * CImmediateMatch::GetFreeAndroidUserItem()
{

	if(m_pAndroidUserManager->GetAndroidCount()==0) return NULL;

	//机器状态
	tagAndroidUserInfo AndroidSimulate;
	m_pAndroidUserManager->GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE|ANDROID_PASSIVITY|ANDROID_INITIATIVE);

	if(AndroidSimulate.wFreeUserCount==0) 
		return NULL;
	else
	{
		//避免已报名的机器人重复安排
		for(WORD i=0; i<AndroidSimulate.wFreeUserCount;i++)
		{
			if(AndroidSimulate.pIAndroidUserFree[i]->GetMeUserItem()->GetSignUpStatus()==MatchStatus_Null)
			{
				ASSERT(AndroidSimulate.pIAndroidUserFree[i]->GetUserID()==AndroidSimulate.pIAndroidUserFree[i]->GetMeUserItem()->GetUserID());
				return AndroidSimulate.pIAndroidUserFree[i];
			}
		}
	}

	return NULL;
}

//用户淘汰
bool CImmediateMatch::OnUserOut(IServerUserItem *pUserItem,LPCTSTR szReason,WORD wLen,WORD wRank,CImmediateGroup *pMatch)
{
	if(pUserItem==NULL)
		return false;

	pUserItem->SetSignUpStatus(MatchStatus_Null);

	//清除比赛后的一些用户
	for (INT_PTR i=0;i<pMatch->m_FreeUserArray.GetCount();i++)
	{
		if(pMatch->m_FreeUserArray[i]->GetUserID()==pUserItem->GetUserID())
		{
			pMatch->m_FreeUserArray.RemoveAt(i);
			break;
		}
	}

	if(wLen>0 && szReason!=NULL)
	{
		CString str(szReason, wLen);
		SendGameMessage(pUserItem,str,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
	}

	//移除参赛用户
	RemoveMatchUserItem(pUserItem);

	BYTE cbMatchStatus=MatchStatus_Null;
	m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	if(pMatch==m_pCurMatchGroup)
	{
		if(pUserItem!=NULL && !pUserItem->IsAndroidUser())
		{
			if(m_pMatchOption->lMatchFee>0)
			{
				//变量定义
				DBR_GR_MatchSignup MatchSignup;
				ZeroMemory(&MatchSignup,sizeof(MatchSignup));

				//构造结构
				MatchSignup.dwUserID=pUserItem->GetUserID();
				MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
				MatchSignup.dwInoutIndex=pUserItem->GetInoutIndex();
				MatchSignup.dwClientAddr=pUserItem->GetClientAddr();
				MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
				lstrcpyn(MatchSignup.szMachineID,pUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,0,&MatchSignup,sizeof(MatchSignup));

				//发送消息				
				LPCTSTR pszMatchFeeType[]={TEXT("游戏币"),TEXT("元宝")};
				if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
				{
					//构造消息
					TCHAR szMessage[128]=TEXT("");
					_sntprintf(szMessage,CountArray(szMessage),TEXT("该房间即将关闭，请到其他房间报名参赛。退还报名费 %I64d %s。"),m_pMatchOption->lMatchFee,pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);

					//发送消息
					m_pIGameServiceFrame->SendRoomMessage(pUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM);
				}
			}
		}
		CServerRule::SetForfendRoomEnter(static_cast<DWORD>(m_pGameServiceOption->dwServerRule),true);
	}
	else if(m_pIDataBaseEngine!=NULL)
	{
		/*DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));

		MatchReward.dwMatchID=m_pMatchOption->dwMatchID;
		MatchReward.dwUserID=pUserItem->GetUserID();
		MatchReward.dwMatchNO=pMatch->m_dwMatchNO;
		MatchReward.wRank=wRank;
		MatchReward.lMatchScore=pUserItem->GetUserScore();
		MatchReward.dwClientAddr=pUserItem->GetClientAddr();

		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD, 0, &MatchReward, sizeof(MatchReward));*/

	}
	return true;
}

//写入奖励
bool CImmediateMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//写入判断
	bool bWriteReward=(pMatchRankInfo->lRewardGold+pMatchRankInfo->dwRewardIngot+pMatchRankInfo->dwRewardExperience)>0;

	//写入奖励
	if(bWriteReward==true)
	{
		//变量定义
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult,sizeof(MatchResult));

		//比赛奖励
		MatchResult.lGold+=pMatchRankInfo->lRewardGold;
		MatchResult.dwIngot+=pMatchRankInfo->dwRewardIngot;
		MatchResult.dwExperience+=pMatchRankInfo->dwRewardExperience;

		//获奖提示
		_sntprintf(MatchResult.szDescribe,CountArray(MatchResult.szDescribe),TEXT("%s, 恭喜您在%s中获得第%d名,奖励如下："),
			pIServerUserItem->GetNickName(),m_pMatchOption->szMatchName,pMatchRankInfo->wRankID);

		//更新用户信息
		tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
		//pUserInfo->lIngot+=pMatchRankInfo->dwRewardIngot;
		pUserInfo->dwExperience+=pMatchRankInfo->dwRewardExperience;
		SendGroupUserScore(pIServerUserItem, NULL);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
		}
	}
	else
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("比赛已结束，恭喜您获得第%d名，欢迎您参加其他场次的比赛！"),pMatchRankInfo->wRankID);
		SendGameMessage(pIServerUserItem, szMessage, SMT_CLOSE_GAME);
		SendRoomMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT);

		return true;
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

//获取空桌子
WORD CImmediateMatch::GetNullTable()
{
	WORD wTableCount=0;
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			wTableCount++;
		}
	}
	return wTableCount;
}

//删除用户
bool CImmediateMatch::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//移除参赛用户
bool CImmediateMatch::RemoveMatchUserItem(IServerUserItem *pIServerUserItem)
{
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];
		
		//用户判断
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());

			m_OnMatchUserItem.RemoveAt(i);
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
