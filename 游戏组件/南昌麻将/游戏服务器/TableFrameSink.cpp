#include "StdAfx.h"
#include "TableFrameSink.h"
#include "FvMask.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define IDI_TIMER_XIAO_HU			1 //小胡
#define IDI_TIMER_PLAYER_ACTION		2 //出牌时间
#define IDI_TIMER_HAIDI_ACTION		3 //海底捞时间

#define TIME_XIAO_HU				3
#define TIME_PLAYER_ACTION			30
#define TIME_PLAYER_ACTION_TRUSTEE	2
#define TIME_HAIDI_ACTION	3
//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_pITableFrame = NULL;
	m_dwGameRuleIdex = 0;
	m_cbGameTypeIdex = 0;
	m_wBankerUser=INVALID_CHAIR;
	//游戏变量
	RepositionSink();
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL)
		return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	m_cbGameTypeIdex = GAME_TYPE_ZZ;
	if (pCustomConfig->cbGameType1)
	{
		m_cbGameTypeIdex = GAME_TYPE_ZZ;
	}
	if (pCustomConfig->cbGameType2)
	{
		m_cbGameTypeIdex = GAME_TYPE_CS;
	}
	for (int i = 0;i<GAME_TYPE_ZZ_HONGZHONG_GZ;i++)
	{
		if (pCustomConfig->cbGameRule[i])
		{
			FvMask::Add(m_dwGameRuleIdex,_MASK_(i+1));
		}
	}

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardData_EX = 0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//发牌信息
	m_cbNiaoCount = 0;
	m_cbNiaoPick = INVALID_BYTE;
	m_cbSendCardData=0;
	m_cbHaiDiSendCardData = 0;
	m_cbSendCardData_EX=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbRepertoryCard_HZ,sizeof(m_cbRepertoryCard_HZ));

	//运行变量
	m_cbProvideCard=0;
	m_wHaiDiCout = 0;
	m_wHaiDiUser=INVALID_CHAIR;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wCSGangHuUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;
	m_bGangStatus_CS = false;
	m_bCSGangStatusHuCout = 0;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	
	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));
	
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_ChiHuRight[i].SetEmpty();
		m_StartHuRight[i].SetEmpty();
		ZeroMemory(m_WeaveItemArray[i],sizeof(m_WeaveItemArray[i]));
	}
	//组合扑克
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	ZeroMemory( m_lStartHuScore,sizeof(m_lStartHuScore) );
	ZeroMemory( m_cbCardDataNiao,sizeof(m_cbCardDataNiao) );
	ZeroMemory( m_cbMasterCheckCard,sizeof(m_cbMasterCheckCard) );
	ZeroMemory( m_cbCanHu,sizeof(m_cbCanHu) );
	ZeroMemory( m_bHasCSGang,sizeof(m_bHasCSGang) );
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		m_cbMasterZhaNiao[i] = INVALID_BYTE;
	}

	if (m_pITableFrame)
	{
		m_pITableFrame->KillGameTimer(IDI_TIMER_PLAYER_ACTION);
		m_pITableFrame->KillGameTimer(IDI_TIMER_XIAO_HU);
	}
	return;
}


//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	RepositionSink();
	if (m_wBankerUser == INVALID_CHAIR)
	{
		m_wBankerUser = 0;
	}
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		m_cbMasterZhaNiao[i] = INVALID_BYTE;
	}

	if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
	{
		Shuffle(m_cbRepertoryCard_HZ,MAX_REPERTORY_HZ);
	}
	else
	{
		Shuffle(m_cbRepertoryCard,MAX_REPERTORY);
	}
	if (m_cbGameTypeIdex == GAME_TYPE_ZZ)
	{
		GameStart_ZZ();
	}
	else if (m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		GameStart_CS();
	}
	return true;
}

void CTableFrameSink::Shuffle(BYTE* pRepertoryCard,int nCardCount)
{
	m_cbLeftCardCount = nCardCount;
	m_GameLogic.RandCardData(pRepertoryCard,nCardCount);

	//test
	//BYTE byTest[] = {
	//	0x12,0x12,0x12,0x12,0x13,0x21,0x22,0x19,0x19,0x25,0x25,0x25,				
	//};
	//int iCout = CountArray(byTest);
	//for (int i = 0;i<iCout;i++)
	//{
	//	pRepertoryCard[m_cbLeftCardCount-1-i] = byTest[i];
	//}
	//m_wBankerUser = 2;
	//end test

	//分发扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( m_pITableFrame->GetTableUserItem(i) != NULL )
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}
	}
}

void CTableFrameSink::GameStart_ZZ()
{
	//混乱扑克
	LONG lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));

	//设置变量
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;

	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
	GameStart.cbXiaoHuTag = 0;

	bool bHasHu = false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//胡牌判断
		CChiHuRight chr;
		m_cbUserAction[i] |= AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],m_cbWeaveItemCount[i],0,chr);
		if (m_cbUserAction[i]&WIK_CHI_HU && !m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
		{
			bHasHu = true;
			m_wCurrentUser=i;
		}
		else
		{
			m_cbUserAction[i] = WIK_NULL;
		}
	}


	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置变量
		GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		if( m_pITableFrame->GetTableUserItem(i)->IsAndroidUser() )
		{
			BYTE bIndex = 1;
			for( WORD j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}

			CMD_S_AndroidRule kAndroidRule;
			kAndroidRule.cbGameTypeIdex = m_cbGameTypeIdex;
			kAndroidRule.dwGameRuleIdex = m_dwGameRuleIdex;

			m_pITableFrame->SendTableData(i,SUB_S_ANDROID_RULE,&kAndroidRule,sizeof(kAndroidRule));
			m_pITableFrame->SendLookonData(i,SUB_S_ANDROID_RULE,&kAndroidRule,sizeof(kAndroidRule));

		}

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	if (hasRule(GAME_TYPE_ZZ_HONGZHONG)&& isHZGuiZe() && bHasHu)
	{
		SendOperateNotify();
	}
	else
	{
		starGameRecord();
		sendMasterHandCard();
		m_bSendStatus = true;
		DispatchCardData(m_wCurrentUser);
	}

}
void CTableFrameSink::GameStart_CS()
{
	//混乱扑克
	LONG lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));

	//设置变量
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;

	bool bInXaoHu = false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//胡牌判断
		CChiHuRight chr;
		m_cbUserAction[i] |= AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[i],chr);
		if (m_cbUserAction[i] != WIK_NULL && !m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
		{
			bInXaoHu = true;
		}
		else
		{
			m_cbUserAction[i] = WIK_NULL;
		}
	}

	if (bInXaoHu)
	{
		m_pITableFrame->SetGameStatus(GS_MJ_XIAOHU);
		GameStart.cbXiaoHuTag =1;
	}
	else
	{
		m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
		GameStart.cbXiaoHuTag = 0;
	}

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置变量
		GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		if( m_pITableFrame->GetTableUserItem(i)->IsAndroidUser() )
		{
			BYTE bIndex = 1;
			for( WORD j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}
		}

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	sendMasterHandCard();
	if (GameStart.cbXiaoHuTag == 1)
	{
		SendOperateNotify();
	}
	else
	{
		starGameRecord();
		m_bSendStatus = true;
		DispatchCardData(m_wCurrentUser);
	}
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//设置中鸟数据
			ZeroMemory(GameEnd.cbCardDataNiao,sizeof(GameEnd.cbCardDataNiao));
			GameEnd.cbNiaoPick = m_cbNiaoPick;
			GameEnd.cbNiaoCount = m_cbNiaoCount;
			ASSERT(m_cbNiaoCount < MAX_COUNT);
			for (int i = 0;i<MAX_NIAO_CARD && i< m_cbNiaoCount&& m_cbNiaoCount < MAX_COUNT;i++)
			{
				GameEnd.cbCardDataNiao[i] = m_cbCardDataNiao[i];
			}
			
			//结束信息
			WORD wWinner = INVALID_CHAIR;
			BYTE cbLeftUserCount = 0;			//判断是否流局
			bool bUserStatus[GAME_PLAYER];		//
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
				m_ChiHuRight[i].GetRightData( &GameEnd.dwChiHuRight[i],MAX_RIGHT_COUNT );
				m_StartHuRight[i].GetRightData( &GameEnd.dwStartHuRight[i],MAX_RIGHT_COUNT );
				
				//流局玩家数
				if( m_ChiHuRight[i].IsEmpty() ) cbLeftUserCount++;
				//当前玩家状态
				if( NULL != m_pITableFrame->GetTableUserItem(i) ) bUserStatus[i] = true;
				else bUserStatus[i] = false;
			}
			
			LONGLONG lGangScore[GAME_PLAYER];
			ZeroMemory(&lGangScore,sizeof(lGangScore));
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				for( BYTE j = 0; j < m_GangScore[i].cbGangCount; j++ )
				{
					for( WORD k = 0; k < GAME_PLAYER; k++ )
						lGangScore[k] += m_GangScore[i].lScore[j][k];
				}
			}

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				m_lGameScore[i] += lGangScore[i];
				m_lGameScore[i] += m_lStartHuScore[i];

			}

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			int	lGameTaxs[GAME_PLAYER];				//
			ZeroMemory(&lGameTaxs,sizeof(lGameTaxs));
			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if( NULL == m_pITableFrame->GetTableUserItem(i) ) continue;

				//设置积分
				if (m_lGameScore[i]>0L)
				{
					lGameTaxs[i] = m_pITableFrame->CalculateRevenue(i,m_lGameScore[i]);
					m_lGameScore[i] -= lGameTaxs[i];
				}
				
				BYTE ScoreKind;
				if( m_lGameScore[i] > 0L ) ScoreKind = SCORE_TYPE_WIN;
				else if( m_lGameScore[i] < 0L ) ScoreKind = SCORE_TYPE_LOSE;
				else ScoreKind = SCORE_TYPE_DRAW;

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = lGameTaxs[i];
				ScoreInfoArray[i].cbType   = ScoreKind;

			}

			//写入积分
			datastream kDataStream;
			m_kGameRecord.StreamValue(kDataStream,true);
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray),kDataStream);

			CopyMemory( GameEnd.lGameScore,m_lGameScore,sizeof(GameEnd.lGameScore) );
			CopyMemory( GameEnd.lGangScore,lGangScore,sizeof(GameEnd.lGangScore) );
			CopyMemory( GameEnd.wProvideUser,m_wProvider,sizeof(GameEnd.wProvideUser) );
			CopyMemory( GameEnd.lGameTax,lGameTaxs,sizeof(GameEnd.lGameTax));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				CopyMemory( GameEnd.wLostFanShu[i],m_wLostFanShu[i],sizeof(m_wLostFanShu[i]) );
				CopyMemory(GameEnd.WeaveItemArray[i],m_WeaveItemArray[i],sizeof(m_WeaveItemArray[i]));
			}
			CopyMemory( GameEnd.lStartHuScore,m_lStartHuScore,sizeof(GameEnd.lStartHuScore) );

			

			//组合扑克
			CopyMemory(GameEnd.cbWeaveCount,m_cbWeaveItemCount,sizeof(GameEnd.cbWeaveCount));

			//发送结束信息
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );

			CMD_S_GameEnd_LastCard kNetLastCard;
			kNetLastCard.cbCardCout = m_cbLeftCardCount;
			if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
			{
				CopyMemory(kNetLastCard.cbCardData,m_cbRepertoryCard_HZ,m_cbLeftCardCount);
			}
			else
			{
				CopyMemory(kNetLastCard.cbCardData,m_cbRepertoryCard,m_cbLeftCardCount);
			}
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GAME_END_LAST_CARD,&kNetLastCard,sizeof(kNetLastCard));
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GAME_END_LAST_CARD,&kNetLastCard,sizeof(kNetLastCard));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_NETWORK_ERROR:		//网络错误
	case GER_USER_LEAVE:		//用户强退
		{
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);
			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_XIAOHU:
	case GS_MJ_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			for (int i = 0;i<GAME_PLAYER;i++)
			{
				CopyMemory(StatusPlay.cbDiscardCard[i],m_cbDiscardCard[i],sizeof(BYTE)*60);
			}
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));
			CopyMemory(StatusPlay.bHasCSGang,m_bHasCSGang,sizeof(StatusPlay.bHasCSGang));
			
			//扑克数据
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;

			//发送场景
			m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			SendOperateNotify(wChiarID);
			sendMasterHandCard();
			if (m_wHaiDiUser != INVALID_CHAIR)
			{
				SendUseHaiDi(m_wHaiDiUser);
			}
			return true;
		}
	}

	return false;
}
bool CTableFrameSink::isUseTuoGuan()
{
	if (m_pITableFrame->GetGameServiceOption()->wServerType == GAME_GENRE_EDUCATE)
	{
		return false;
	}
	return true;
}
void CTableFrameSink::setActionTimeStart(bool bStart,bool Trustee)
{
	if (!isUseTuoGuan())
	{
		return;
	}
	int iTime = TIME_PLAYER_ACTION;
	if (Trustee)
	{
		iTime = TIME_PLAYER_ACTION_TRUSTEE;
	}
	if (bStart)
	{
		m_pITableFrame->SetGameTimer(IDI_TIMER_PLAYER_ACTION,iTime*1000,1,0);
	}
	else
	{
		m_pITableFrame->KillGameTimer(IDI_TIMER_PLAYER_ACTION);
	}
}
//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_TIMER_XIAO_HU:  //小胡结束
		{
			m_pITableFrame->KillGameTimer(IDI_TIMER_XIAO_HU);
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

			starGameRecord();
			m_bSendStatus = true;
			DispatchCardData(m_wCurrentUser);
			return true;
		}
	case IDI_TIMER_HAIDI_ACTION:  //海底捞
		{
			m_pITableFrame->KillGameTimer(IDI_TIMER_HAIDI_ACTION);
			OnUserOutCard(m_wCurrentUser,m_cbHaiDiSendCardData);
			m_cbHaiDiSendCardData = 0;
			return true;
		}
	case IDI_TIMER_PLAYER_ACTION:  //玩家一轮
		{
			m_pITableFrame->KillGameTimer(IDI_TIMER_XIAO_HU);
			if (m_wHaiDiUser != INVALID_CHAIR)
			{
				OnUseHaiDiCard(m_wHaiDiUser,0);
				return true;
			}
			if (m_wCurrentUser == INVALID_CHAIR)
			{
				for (WORD i = 0;i<GAME_PLAYER;i++)
				{
					if (m_cbUserAction[i] == WIK_NULL)
					{
						continue;
					}
					setChairTrustee(i,true);
					OnUserOperateCard(i,WIK_NULL,0);
				}
			}
			else
			{
				setChairTrustee(m_wCurrentUser,true);
				std::vector<BYTE> kCard;
				BYTE cbActSendCard = INVALID_BYTE;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[m_wCurrentUser],kCard);
				if (kCard.size() == 0)
				{
					OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);
					return true;
				}
				for (int i = 0;i<(int)kCard.size();i++)
				{
					if (kCard[i] == m_cbSendCardData)
					{
						cbActSendCard = m_cbSendCardData;
					}
				}
				if (cbActSendCard == INVALID_BYTE)
				{
					cbActSendCard = kCard[0];
				}
				OnUserOutCard(m_wCurrentUser,cbActSendCard);
			}

			return true;
		}
	}
	return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			if (m_cbHaiDiSendCardData != 0)
			{
				return true;
			}
			if (m_bHasCSGang[pIServerUserItem->GetChairID()])
			{
				return true;
			}
			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData);
		}
	case SUB_C_OUT_CARD_CSGANG:
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard_CSGang));
			if (wDataSize!=sizeof(CMD_C_OutCard_CSGang)) return false;
			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			//消息处理
			CMD_C_OutCard_CSGang * pOutCard=(CMD_C_OutCard_CSGang *)pDataBuffer;
			return OnUserOutCardCSGang(pIServerUserItem->GetChairID(),pOutCard->cbCardData1,pOutCard->cbCardData2);
		}
	case SUB_C_USE_HAIDI:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_Use_HaiDi));
			if (wDataSize!=sizeof(CMD_C_Use_HaiDi)) return false;
			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			if (m_wHaiDiUser != pIServerUserItem->GetChairID())
			{
				return true;
			}
			//消息处理
			CMD_C_Use_HaiDi * pUseHaiDi=(CMD_C_Use_HaiDi *)pDataBuffer;
			OnUseHaiDiCard(m_wHaiDiUser,pUseHaiDi->cbUseHaiDi);
			return true;
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pIServerUserItem->GetChairID(),pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			setChairTrustee(pIServerUserItem->GetChairID(),pTrustee->bTrustee);

			return true;
		}
	case SUB_C_XIAOHU:
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_XiaoHu));
			if (wDataSize!=sizeof(CMD_C_XiaoHu)) return false;
			//消息处理
			CMD_C_XiaoHu * pXiaoHu=(CMD_C_XiaoHu *)pDataBuffer;

			if (m_pITableFrame->GetGameStatus() != GS_MJ_XIAOHU)
			{
				return true;
			}
			WORD wChairID = pIServerUserItem->GetChairID();
			OnUserXiaoHu(wChairID,pXiaoHu->cbOperateCode);
			return true;
		}
	case SUB_C_MASTER_LEFTCARD:
		{
			WORD wChairID = pIServerUserItem->GetChairID();
			sendMasterLeftCard(wChairID);
			return true;
		}
	case SUB_C_MASTER_CHEAKCARD:
		{

			MaterCheckCard *pMaterCheckCard =(MaterCheckCard *)pDataBuffer;
			if(wDataSize != sizeof(MaterCheckCard)) return false;

			WORD wChairID = pIServerUserItem->GetChairID();
			m_cbMasterCheckCard[wChairID] = pMaterCheckCard->cbCheakCard;
			return true;
		}
	case SUB_C_MASTER_ZHANIAO:
		{

			MaterNiaoCout *pMaterCheckCard =(MaterNiaoCout *)pDataBuffer;
			if(wDataSize != sizeof(MaterNiaoCout)) return false;

			BYTE cbNiaoCout = pMaterCheckCard->cbNiaoCout;
			if (cbNiaoCout > GetNiaoCardNum())
			{
				cbNiaoCout = GetNiaoCardNum();
			}
			WORD wChairID = pIServerUserItem->GetChairID();
			m_cbMasterZhaNiao[wChairID] = cbNiaoCout;

			return true;
		}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	if (m_pITableFrame->GetGameStatus() >= GAME_STATUS_PLAY)
	{
		setChairTrustee(wChairID,true);
	}
	return true;
}
//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	if (m_pITableFrame->GetGameStatus() >= GAME_STATUS_PLAY)
	{
		setChairTrustee(wChairID,false);
	}
	return true;
}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser==false)
	{
	}

	return true;
}
void CTableFrameSink::SetPrivateInfo(BYTE bGameTypeIdex,DWORD	bGameRuleIdex)
{
	m_cbGameTypeIdex = bGameTypeIdex;
	m_dwGameRuleIdex = bGameRuleIdex;
}

void CTableFrameSink::SetCreateUserID(DWORD dwUserID)
{
	IServerUserItem* pUserItem = m_pITableFrame->SearchUserItem(dwUserID);

	if (pUserItem)
	{
		m_wBankerUser = pUserItem->GetChairID();
	}
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return true;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return true;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return true;
	}

	//设置变量
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangStatus_CS = false;
		m_bGangOutStatus = true;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;
	m_cbOutCardData_EX = 0;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;
	addGameRecordAction(OutCard);

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//用户切换
	m_wCurrentUser=(wChairID+GAME_PLAYER-1)%GAME_PLAYER;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//派发扑克
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}
bool CTableFrameSink::OnUseHaiDiCard(WORD wChairID, BYTE cbUseHaiDi)
{
	if (cbUseHaiDi)
	{
		if (DispatchCardData(wChairID,true))
		{
			return true;
		}
	}
	m_wHaiDiCout++;
	if (m_wHaiDiCout >= 4)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);
	}
	else
	{
		m_wHaiDiUser=(m_wHaiDiUser+GAME_PLAYER-1)%GAME_PLAYER;
		SendUseHaiDi(m_wHaiDiUser);
	}
	return true;
}
bool CTableFrameSink::OnUserOutCardCSGang(WORD wChairID, BYTE cbCardData1,BYTE cbCardData2)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData1)==true && m_GameLogic.IsValidCard(cbCardData2)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return true;
	if (m_GameLogic.IsValidCard(cbCardData1)==false || m_GameLogic.IsValidCard(cbCardData2)==false) return true;

	//设置变量
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangStatus_CS = false;
		m_bGangOutStatus = true;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//出牌记录
	m_cbOutCardCount+=2;;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData1;
	m_cbOutCardData_EX = cbCardData2;
	//构造数据
	CMD_S_OutCard_CSGang OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData1=cbCardData1;
	OutCard.cbOutCardData2=cbCardData2;
//	addGameRecordAction(OutCard);

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD_CSGANG,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD_CSGANG,&OutCard,sizeof(OutCard));

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData1;

	//用户切换
	bool bAroseAction = false;
	m_wCurrentUser=(wChairID+GAME_PLAYER-1)%GAME_PLAYER;
	for (int i = 0;i<m_wCurrentUser;i++)
	{
		if (m_wCurrentUser == wChairID)
		{
			break;
		}
		m_wCSGangHuUser = m_wCurrentUser;
		//响应判断
		bAroseAction=EstimateUserRespondCSGang(wChairID,cbCardData1,cbCardData2);
		if (bAroseAction)
		{
			break;
		}
	}
	if (bAroseAction==false)
	{
		DispatchCardData(m_wCurrentUser);
	}
	return true;
}
//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, DWORD cbOperateCode, BYTE cbOperateCard)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()!=GS_MJ_FREE);
	if (m_pITableFrame->GetGameStatus()==GS_MJ_FREE)
		return true;

	//效验用户	注意：机器人有可能发生此断言
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		if (m_bResponse[wChairID]==true) 
			return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//变量定义
		WORD wTargetUser=wChairID;
		DWORD cbTargetAction=cbOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		m_cbOperateCard[wChairID]=m_cbProvideCard;

		//执行判断
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//获取动作
			DWORD cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			if(FvMask::HasAny(m_cbUserAction[wChairID],WIK_CHI_HU))
			{
				m_cbCanHu[wChairID] = 1;
			}

			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			if (m_wCSGangHuUser != INVALID_CHAIR)
			{
				//用户切换
				bool bAroseAction = false;
				m_wCurrentUser=(wChairID+GAME_PLAYER-1)%GAME_PLAYER;
				for (int i = 0;i<m_wCurrentUser;i++)
				{
					if (m_wCurrentUser == wChairID)
					{
						break;
					}
					m_wCSGangHuUser = m_wCurrentUser;
					//响应判断
					bAroseAction=EstimateUserRespondCSGang(wChairID,m_cbSendCardData,m_cbSendCardData_EX);
					if (bAroseAction)
					{
						break;
					}
				}
				if (bAroseAction==false)
				{
					DispatchCardData(m_wCurrentUser);
				}
			}
			else
			{
				//发送扑克
				DispatchCardData(m_wResumeUser);
			}

			return true;
		}

		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//出牌变量
		m_cbOutCardData=0;
		m_cbOutCardData_EX = 0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//胡牌操作
		if (cbTargetAction==WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard=cbTargetCard;

			int nChiHuCount = 0;
			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER;i!=m_wProvideUser;i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//过虑判断
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//胡牌判断
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
				m_dwChiHuKind[i] = AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);

				//插入扑克
				if (m_dwChiHuKind[i]!=WIK_NULL) 
				{
					nChiHuCount ++;
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					ProcessChiHuUser( i,false );
				}
			}
			if (nChiHuCount >1)//一炮多响 放炮的为庄家
			{
				m_wBankerUser = m_wProvideUser;
			}

			OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );

			return true;
		}

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//组合扑克
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//删除扑克
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//上牌操作
		{
			//删除扑克
			BYTE cbRemoveCard[3];
			m_GameLogic.GetWeaveCard(WIK_LEFT,cbTargetCard,cbRemoveCard);
			VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
			VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

			break;
		}
		case WIK_RIGHT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_RIGHT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_CENTER:	//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_CENTER,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_CS_GANG:
		case WIK_BU_ZHANG:
		case WIK_GANG:		//杠牌操作
			{
				//删除扑克,被动动作只存在放杠
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) );

				break;
			}
		default:
			ASSERT( FALSE );
			return false;
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
		addGameRecordAction(OperateResult);
		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//设置用户
		m_wCurrentUser=wTargetUser;

		NoticeGangState(m_wCurrentUser,true);

		//杠牌处理
		if (cbTargetAction==WIK_GANG)
		{
			ASSERT(m_cbGameTypeIdex != GAME_TYPE_CS);
			
			SCORE  nActionScore = m_pGameServiceOption->lCellScore*GANG_FEN;
			//发送信息
			CMD_S_GangScore gs;
			ZeroMemory( &gs,sizeof(gs) );
			gs.wChairId = wTargetUser;
			gs.cbXiaYu = FALSE;
			gs.lGangScore[wTargetUser] = nActionScore;
			gs.lGangScore[m_wProvideUser] = -nActionScore;
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

			//杠得分
			BYTE cbIndex = m_GangScore[wTargetUser].cbGangCount++;
			m_GangScore[wTargetUser].lScore[cbIndex][wTargetUser] = nActionScore;
			m_GangScore[wTargetUser].lScore[cbIndex][m_wProvideUser] = -nActionScore;
			
			m_bGangStatus = true;
			DispatchCardData(wTargetUser);
		}
		if (cbTargetAction == WIK_CS_GANG)
		{
			ASSERT(m_cbGameTypeIdex == GAME_TYPE_CS);

			m_bHasCSGang[wTargetUser] = true;
			m_bGangStatus_CS = true;
			m_bGangStatus = true;
			DispatchCardData_CSGang(wTargetUser);
		}
		if (cbTargetAction == WIK_BU_ZHANG)
		{
			ASSERT(m_cbGameTypeIdex == GAME_TYPE_CS);

			m_bGangStatus = false;
			DispatchCardData(wTargetUser);
		}
		return true;
	}

	//主动动作
	if (m_wCurrentUser==wChairID)
	{

		if (cbOperateCode==WIK_NULL && m_bHasCSGang[wChairID] && m_cbGameTypeIdex == GAME_TYPE_CS)
		{
			OnUserOutCard(wChairID,m_cbSendCardData);
			return true;
		}
		//效验操作
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//扑克效验
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||
			(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)
			&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) 
			return true;

		//设置变量
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;

		//执行动作
		switch (cbOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				ASSERT(m_cbGameTypeIdex != GAME_TYPE_CS);
				bool bPublic = false;
				if (!checkSelfGang(wChairID,cbOperateCard,bPublic))
				{
					return false;
				}
				m_bGangStatus = true;

				//杠牌得分
				LONGLONG lScore = bPublic?m_pGameServiceOption->lCellScore*ZIMO_GANG_FEN :m_pGameServiceOption->lCellScore * ANGANG_FEN;
				BYTE cbGangIndex = m_GangScore[wChairID].cbGangCount++;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if(  i == wChairID ) continue;

					m_GangScore[wChairID].lScore[cbGangIndex][i] = -lScore;
					m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += lScore;
				}

				//发送信息
				CMD_S_GangScore gs;
				gs.wChairId = wChairID;
				ZeroMemory( &gs,sizeof(gs) );
				gs.cbXiaYu = bPublic?FALSE:TRUE;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( i == wChairID ) continue;

					gs.lGangScore[i] = -lScore;
					gs.lGangScore[wChairID] += lScore;
				}
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
				
				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;
				addGameRecordAction(OperateResult);

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				//效验动作
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID);
				}
				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//吃牌权位
				if (m_cbOutCardCount==0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];

				if (m_cbSendCardData_EX == 0 && m_cbProvideCard !=0)
				{
					m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1);
				}

				m_dwChiHuKind[wChairID] = AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,
					cbWeaveItemCount,m_cbProvideCard,m_ChiHuRight[wChairID]);

				if (m_cbProvideCard !=0)
				{
					m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;
				}

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;

				ProcessChiHuUser( wChairID,false );

				
				OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );

				return true;
			}
		case WIK_CS_GANG:
			{
				ASSERT(m_cbGameTypeIdex == GAME_TYPE_CS);
				bool bPublic = false;
				if (!checkSelfGang(wChairID,cbOperateCard,bPublic))
				{
					return false;
				}
				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=WIK_CS_GANG;
				OperateResult.cbOperateCard=cbOperateCard;
				addGameRecordAction(OperateResult);

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				m_bHasCSGang[wChairID] = true;
				m_bGangStatus_CS = true;
				m_bGangStatus = true;
				//效验动作
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
				{
					DispatchCardData_CSGang(wChairID);
				}
				return true;
			}
		
		case WIK_BU_ZHANG:
			{
				ASSERT(m_cbGameTypeIdex == GAME_TYPE_CS);

				bool bPublic = false;
				if (!checkSelfGang(wChairID,cbOperateCard,bPublic))
				{
					return false;
				}
				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=WIK_BU_ZHANG;
				OperateResult.cbOperateCard=cbOperateCard;
				addGameRecordAction(OperateResult);

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				m_bGangStatus = false;
				DispatchCardData(wChairID);
				return true;
			}
		}
		NoticeGangState(m_wCurrentUser,true);
		return true;
	}

	return false;
}

bool CTableFrameSink::checkSelfGang(WORD wChairID,BYTE cbOperateCard,bool& bPublic)
{
	//变量定义
	BYTE cbWeaveIndex=0xFF;
	BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

	//杠牌处理
	if (m_cbCardIndex[wChairID][cbCardIndex]==1)
	{
		//寻找组合
		for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
		{
			DWORD cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
			DWORD cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				bPublic=true;
				cbWeaveIndex=i;
				break;
			}
		}
		//效验动作
		ASSERT(cbWeaveIndex!=0xFF);
		if (cbWeaveIndex==0xFF) return false;

		//组合扑克
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = WIK_GANG;
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
	}
	else
	{
		//扑克效验
		ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
		if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
			return false;

		//设置变量
		bPublic=false;
		cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
		m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind= WIK_GANG;
		m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
	}
	//删除扑克
	m_cbCardIndex[wChairID][cbCardIndex]=0;
	return true;
}

//发送操作
bool CTableFrameSink::SendOperateNotify(WORD wChairID)
{
	bool bTrustee = true;
	bool bTimeAction = false;
	//发送提示
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (wChairID != INVALID_CHAIR && wChairID != i)
		{
			continue;
		}
		if (m_cbUserAction[i]==WIK_NULL)
		{
			continue;
		}
		//构造数据
		CMD_S_OperateNotify OperateNotify;
		OperateNotify.wResumeUser=m_wResumeUser;
		OperateNotify.cbActionCard=m_cbProvideCard;
		OperateNotify.cbActionMask=m_cbUserAction[i];

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		bTimeAction = true;
		if (m_bTrustee[i] == false)
		{
			bTrustee = false;
		}
	}
	if (bTimeAction)
	{
		setActionTimeStart(true,bTrustee);
	}
	return true;
}
void CTableFrameSink::NoticeGangState(WORD iChairID,bool bSendNotice)
{
	//杠牌判断
	if (getLeftCardNum()>0)
	{
		tagGangCardResult GangCardResult;
		GangCardResult.cbCardData[0] = m_cbSendCardData;
		DWORD  cbTemp =m_GameLogic.AnalyseGangCard(m_cbCardIndex[iChairID],m_WeaveItemArray[iChairID],m_cbWeaveItemCount[iChairID],
			GangCardResult,m_cbGameTypeIdex);

		if (cbTemp == WIK_GANG && m_cbGameTypeIdex == GAME_TYPE_ZZ)
		{
			m_cbUserAction[iChairID] |= WIK_GANG;
		}
		else if(cbTemp == WIK_GANG && m_cbGameTypeIdex == GAME_TYPE_CS)
		{
			m_cbUserAction[iChairID] |= WIK_BU_ZHANG;

			if (getLeftCardNum() >=2 && !m_pITableFrame->GetTableUserItem(iChairID)->IsAndroidUser())
			{
				m_cbUserAction[iChairID] |= WIK_CS_GANG;
			}
		}
	}
	if (bSendNotice && m_cbUserAction[iChairID] != WIK_NULL)
	{
		SendOperateNotify(iChairID);
	}
}
//派发扑克
void CTableFrameSink::SendUseHaiDi(WORD wCurrentUser)
{
	CMD_S_UseHaiDi kNetInfo;
	kNetInfo.wCurrentUser = wCurrentUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USE_HAIDI_CARD,&kNetInfo,sizeof(kNetInfo));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USE_HAIDI_CARD,&kNetInfo,sizeof(kNetInfo));

	setActionTimeStart(true,m_bTrustee[wCurrentUser]);
}
//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bHaiDi)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}
	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData_EX!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData_EX;
	}
	m_wOutCardUser=INVALID_CHAIR;
	m_wCSGangHuUser = INVALID_CHAIR;

	//荒庄结束
	if (getLeftCardNum()==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);
		return true;
	}
	if (!bHaiDi && getLeftCardNum()==1 && m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		m_wHaiDiUser = wCurrentUser;
		m_wCurrentUser = INVALID_CHAIR;
		SendUseHaiDi(m_wHaiDiUser);
		return true;
	}
	//设置变量
	m_cbOutCardData=0;
	m_cbOutCardData_EX = 0;
	m_wCurrentUser=wCurrentUser;

	//杠后炮
	if( m_bGangOutStatus )
	{
		m_bGangOutStatus = false;
	}

	//发牌处理
	if (m_bSendStatus==true)
	{
		//发送扑克
		m_cbSendCardCount++;
		m_cbSendCardData = getSendCardData(m_wCurrentUser);
		m_cbSendCardData_EX = 0;

		//胡牌判断
		CChiHuRight chr;
		m_wProvideUser = wCurrentUser;
		m_cbUserAction[wCurrentUser]|= AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,chr);

		//加牌
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_cbProvideCard = m_cbSendCardData;

		if (!(m_bHasCSGang[wCurrentUser] && m_cbGameTypeIdex == GAME_TYPE_CS))
		{
			NoticeGangState(wCurrentUser,false);
		}
	}

	m_cbCanHu[wCurrentUser] = 0;
	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.bTail = false;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;
	addGameRecordAction(SendCard);
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	setActionTimeStart(true,m_bTrustee[wCurrentUser]);
	if (m_bHasCSGang[wCurrentUser] && m_cbGameTypeIdex == GAME_TYPE_CS && m_cbUserAction[wCurrentUser] != WIK_CHI_HU)
	{
		OnUserOutCard(wCurrentUser,m_cbSendCardData);
	}
	else if(m_cbUserAction[wCurrentUser] == WIK_NULL && getLeftCardNum() == 0 && m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		m_cbHaiDiSendCardData = m_cbSendCardData;
		m_pITableFrame->SetGameTimer(IDI_TIMER_HAIDI_ACTION,TIME_HAIDI_ACTION*1000,1,0);
	}
	return true;
}

void CTableFrameSink::setChairTrustee(WORD iChairID,bool bTrustee)
{
	if (!isUseTuoGuan())
	{
		return;
	}
	if (iChairID >= GAME_PLAYER)
	{
		return;
	}
	m_bTrustee[iChairID]=bTrustee;

	CMD_S_Trustee Trustee;
	Trustee.bTrustee = bTrustee;
	Trustee.wChairID = iChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
}
bool CTableFrameSink::DispatchCardData_CSGang( WORD wCurrentUser )
{
	ASSERT(m_cbGameTypeIdex == GAME_TYPE_CS);
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	ASSERT(getLeftCardNum()>=2);
	if (wCurrentUser==INVALID_CHAIR || getLeftCardNum()<2 || m_cbGameTypeIdex != GAME_TYPE_CS)
		return false;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}
	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData_EX!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData_EX;
	}
	//设置变量
	m_cbOutCardData=0;
	m_cbOutCardData_EX =0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;

	//杠后炮
	if( m_bGangOutStatus )
	{
		m_bGangOutStatus = false;
	}


	//发送扑克
	m_cbSendCardCount +=2;
	m_cbSendCardData = getSendCardData(m_wCurrentUser);
	m_cbSendCardData_EX = getSendCardData(m_wCurrentUser);
	//胡牌判断
	CChiHuRight chr1,chr2;
	m_wProvideUser = wCurrentUser;
	DWORD cbAction1 = AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,chr1);
	DWORD cbAction2 = AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData_EX,chr2);

	m_bCSGangStatusHuCout = 0;

	if (cbAction1 != WIK_NULL)
	{
		m_cbUserAction[wCurrentUser]|=cbAction1;
		m_cbProvideCard = m_cbSendCardData;
		m_bCSGangStatusHuCout++;
	}
	if (cbAction2 != WIK_NULL)
	{
		m_cbUserAction[wCurrentUser]|=cbAction2;
		m_cbProvideCard = m_cbSendCardData_EX;
		m_bCSGangStatusHuCout++;
	}
 	//加牌
	// 	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
 
 	//设置变量

	m_cbCanHu[wCurrentUser] = 0;
	//构造数据
	CMD_S_SendCard_CSGang SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData1=(m_bSendStatus==true)?m_cbSendCardData:0x00;
	SendCard.cbCardData2=(m_bSendStatus==true)?m_cbSendCardData_EX:0x00;
//	addGameRecordAction(SendCard);
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD_CSGANG,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD_CSGANG,&SendCard,sizeof(SendCard));

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	bool bHaiDiLao = false;
	if (m_cbGameTypeIdex == GAME_TYPE_CS && getLeftCardNum() == 0)
	{
		bHaiDiLao = true;
	}
	//动作判断
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//用户过滤
		if (wCenterUser==i) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard && !m_bHasCSGang[i] && !bHaiDiLao)
		{
			////碰牌判断
			m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

			//杠牌判断
			if (getLeftCardNum()>0) 
			{
				DWORD cbTemp = m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
				if (cbTemp == WIK_GANG && m_cbGameTypeIdex == GAME_TYPE_ZZ)
				{
					m_cbUserAction[i] |= WIK_GANG;
				}
				else if(cbTemp == WIK_GANG && m_cbGameTypeIdex == GAME_TYPE_CS)
				{
					m_cbUserAction[i] |= WIK_BU_ZHANG;

					if (getLeftCardNum() >=2 &&!m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
					{
						m_cbUserAction[i] |= WIK_CS_GANG;
					}
				}
			}
		}

		if (m_cbCanHu[i] != 1)
		{
			//吃胡判断
			CChiHuRight chr;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			m_cbUserAction[i]|= AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
		}

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//长沙麻将吃操作
	if (m_cbGameTypeIdex == GAME_TYPE_CS && !m_bHasCSGang[m_wCurrentUser] && !bHaiDiLao && EstimatKind == EstimatKind_OutCard)
	{
		m_cbUserAction[m_wCurrentUser] |= m_GameLogic.EstimateEatCard(m_cbCardIndex[m_wCurrentUser],cbCenterCard);

		//结果判断
		if (m_cbUserAction[m_wCurrentUser]!=WIK_NULL) 
			bAroseAction=true;
	}
	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();
		return true;
	}

	return false;
}

bool CTableFrameSink::EstimateUserRespondCSGang( WORD wCenterUser, BYTE cbCenterCard1,BYTE cbCenterCard2 )
{
	//变量定义
	bool bAroseAction=false;
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		return bAroseAction;
	}
	if (m_wCurrentUser == wCenterUser)
	{
		return bAroseAction;
	}
	if (m_cbCanHu[m_wCurrentUser] == 1)
	{
		return bAroseAction;
	}
	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	m_bCSGangStatusHuCout = 0;
	BYTE cbCenterCard = 0;
	//用户过滤
	//吃胡判断
	CChiHuRight chr;
	BYTE cbWeaveCount=m_cbWeaveItemCount[m_wCurrentUser];
	DWORD actionTemp1 = AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],m_WeaveItemArray[m_wCurrentUser],cbWeaveCount,cbCenterCard1,chr);
	if (actionTemp1 != WIK_NULL)
	{
		m_cbUserAction[m_wCurrentUser]|= actionTemp1;
		cbCenterCard = cbCenterCard1;
		m_bCSGangStatusHuCout++;
	}
	DWORD actionTemp2 = AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],m_WeaveItemArray[m_wCurrentUser],cbWeaveCount,cbCenterCard2,chr);
	if (actionTemp2 != WIK_NULL)
	{
		m_cbUserAction[m_wCurrentUser]|= actionTemp2;
		cbCenterCard = cbCenterCard2;
		m_bCSGangStatusHuCout++;
	}
	//结果判断
	if (m_cbUserAction[m_wCurrentUser]!=WIK_NULL) 
		bAroseAction=true;

	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();
		return true;
	}

	return false;
}

//
void CTableFrameSink::ProcessChiHuUser( WORD wChairId, bool bGiveUp )
{
	if( !bGiveUp )
	{
		//引用权位
		CChiHuRight &chr = m_ChiHuRight[wChairId];

		FiltrateRight( wChairId,chr );
		WORD wFanShu = 0;
		int nFanNum = 0;
		if ( m_cbGameTypeIdex == GAME_TYPE_ZZ)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_ZZ(chr);
			nFanNum = 1;
		}
		if ( m_cbGameTypeIdex == GAME_TYPE_CS)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_CS(chr);
			nFanNum = 1;
			if (wFanShu>6)
			{
				nFanNum = wFanShu/6;
			}
		}
		
		LONGLONG lChiHuScore = wFanShu*m_pGameServiceOption->lCellScore;
		int nNiaoFen = m_pGameServiceOption->lCellScore;

		if(hasRule(GAME_TYPE_ZZ_HONGZHONG)&&isHZGuiZe())
		{
			lChiHuScore = 2*m_pGameServiceOption->lCellScore;
			nNiaoFen = 2*m_pGameServiceOption->lCellScore;
		}

		if (m_cbGameTypeIdex != GAME_TYPE_CS)
		{
			//抢杠和杠上炮
			checkGangScore(wChairId,chr);
		}

		if( m_wProvideUser != wChairId )
		{
			m_wLostFanShu[m_wProvideUser][wChairId] = (WORD)lChiHuScore;
		}
		else
		{
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				m_wLostFanShu[i][wChairId] = (WORD)lChiHuScore;
			}
		}

		if (m_cbNiaoPick == INVALID_BYTE)
		{
			m_cbNiaoPick = setNiaoCard(chr,wChairId);//结束后设置鸟牌
		}

		if( m_wProvideUser == wChairId )
		{
			if (hasRule(GAME_TYPE_CS_ZHUANGXIANFEN)&& wChairId== m_wBankerUser)
			{
				//庄闲分
				lChiHuScore +=nFanNum;
			}


			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				if (hasRule(GAME_TYPE_CS_ZHUANGXIANFEN) && i == m_wBankerUser)
				{

					m_lGameScore[i] -= lChiHuScore+nFanNum;
					m_lGameScore[wChairId] += lChiHuScore+nFanNum;
				}
				else
				{
					//胡牌分
					m_lGameScore[i] -= lChiHuScore;
					m_lGameScore[wChairId] += lChiHuScore;
				}
				//中鸟分

				if (hasRule(GAME_TYPE_ZZ_ZHANIAO_DOUBLE))
				{
					m_lGameScore[i] -= m_cbNiaoPick*lChiHuScore;
					m_lGameScore[wChairId] += m_cbNiaoPick*lChiHuScore;
				}
				else
				{
					m_lGameScore[i] -= m_cbNiaoPick* nNiaoFen;
					m_lGameScore[wChairId] += m_cbNiaoPick* nNiaoFen;
				}
				
			}
		}
		//点炮
		else
		{
			if (hasRule(GAME_TYPE_CS_ZHUANGXIANFEN)&& wChairId== m_wBankerUser)
			{
				//庄闲分
				lChiHuScore +=nFanNum;
			}
			else if(hasRule(GAME_TYPE_CS_ZHUANGXIANFEN)&& m_wProvideUser== m_wBankerUser)
			{
				//庄闲分
				lChiHuScore +=nFanNum;
			}
			else if (hasRule(GAME_TYPE_ZZ_HONGZHONG)&& isHZGuiZe())
			{
				lChiHuScore *=3;

				if(!(chr&CHR_QIANG_GANG_HU).IsEmpty())
				{
					nNiaoFen*=3;
				}
			}

			m_lGameScore[m_wProvideUser] -= lChiHuScore;
			m_lGameScore[wChairId] += lChiHuScore;

			//中鸟分
			if (hasRule(GAME_TYPE_ZZ_ZHANIAO_DOUBLE))
			{
				m_lGameScore[m_wProvideUser] -= m_cbNiaoPick*lChiHuScore;
				m_lGameScore[wChairId] += m_cbNiaoPick*lChiHuScore;
			}
			else
			{
				m_lGameScore[m_wProvideUser] -= m_cbNiaoPick* nNiaoFen;
				m_lGameScore[wChairId] += m_cbNiaoPick* nNiaoFen;
			}
		}

		//设置变量
		m_wProvider[wChairId] = m_wProvideUser;
		m_bGangStatus = false;
		m_bGangOutStatus = false;

		m_wBankerUser = wChairId;

		//发送消息
		CMD_S_ChiHu ChiHu;
		ChiHu.wChiHuUser = wChairId;
		ChiHu.wProviderUser = m_wProvideUser;
		ChiHu.lGameScore = m_lGameScore[wChairId];
		ChiHu.cbCardCount = m_GameLogic.GetCardCount( m_cbCardIndex[wChairId] );
		ChiHu.cbChiHuCard = m_cbProvideCard;
		addGameRecordAction(ChiHu);

		m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );
		m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );

	}

	return;
}

void CTableFrameSink::checkGangScore(WORD wChairId,const CChiHuRight &chr)
{
	if( !(chr&CHR_QIANG_GANG_HU).IsEmpty() )
	{
		m_GangScore[m_wProvideUser].cbGangCount--;
	}
}


BYTE CTableFrameSink::GetNiaoCardNum()
{
	BYTE nNum = 0;
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO1))
	{
		nNum = 1;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO2))
	{
		nNum = 2;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO3))
	{
		nNum = 3;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO4))
	{
		nNum = 4;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO5))
	{
		nNum = 5;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO6))
	{
		nNum = 6;
	}
	return nNum;
}

int CTableFrameSink::setNiaoCard(CChiHuRight chr,BYTE iChairID)
{
	ZeroMemory(m_cbCardDataNiao,sizeof(m_cbCardDataNiao));
	m_cbNiaoCount = GetNiaoCardNum();

	if(!(chr&CHR_HONGZHONG_WU).IsEmpty())
	{
		BYTE nHongZhongNum =  m_GameLogic.getMagicCardCount(m_cbCardIndex[iChairID]);
		if (nHongZhongNum == 0&&isHZGuiZe())
		{
			//没红中多加一个鸟
			m_cbNiaoCount++;
		}
	}
	
	if (m_cbNiaoCount>m_cbLeftCardCount)
	{
		m_cbNiaoCount = m_cbLeftCardCount;
	}
	if (m_cbNiaoCount == 0)
	{
		m_cbNiaoPick = 0;
		return 0;
	}
	BYTE cbMasterZhaNiao = m_cbMasterZhaNiao[iChairID];
	if (cbMasterZhaNiao != INVALID_BYTE)
	{
		if (cbMasterZhaNiao > m_cbNiaoCount)
		{
			cbMasterZhaNiao = m_cbNiaoCount;
		}
		BYTE cbTempVaild = 0;
		BYTE cbTempNiaoCardCout = 0;
		for (int i = 0;i<m_cbLeftCardCount && cbTempNiaoCardCout < m_cbNiaoCount;i++)
		{
			BYTE cbCard = 0;
			if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
			{
				cbCard = m_cbRepertoryCard_HZ[i];
			}
			else
			{
				cbCard = m_cbRepertoryCard[i];
			}
			BYTE nValue = cbCard&MASK_VALUE;
			if (nValue == 1 || nValue == 5 || nValue == 9 || m_GameLogic.IsMagicCard(cbCard))
			{
				if (cbTempVaild < cbMasterZhaNiao)
				{
					m_cbCardDataNiao[cbTempNiaoCardCout] = cbCard;
					cbTempNiaoCardCout++;
					cbTempVaild++;
				}
			}
		}
		for (int i = 0;i<m_cbLeftCardCount && cbTempNiaoCardCout < m_cbNiaoCount;i++)
		{
			BYTE cbCard = 0;
			if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
			{
				cbCard = m_cbRepertoryCard_HZ[i];
			}
			else
			{
				cbCard = m_cbRepertoryCard[i];
			}
			BYTE nValue = cbCard&MASK_VALUE;
			if (nValue == 1 || nValue == 5 || nValue == 9 || m_GameLogic.IsMagicCard(cbCard))
			{
			}
			else
			{
				m_cbCardDataNiao[cbTempNiaoCardCout] = cbCard;
				cbTempNiaoCardCout++;
			}
		}
	}
	else
	{
		BYTE cbCardIndexTemp[MAX_INDEX];
		m_cbLeftCardCount-= m_cbNiaoCount;
		if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
		{
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard_HZ[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		else
		{
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		m_GameLogic.SwitchToCardData(cbCardIndexTemp,m_cbCardDataNiao);
	}
	m_cbNiaoPick = m_GameLogic.GetPickNiaoCount(m_cbCardDataNiao,m_cbNiaoCount);

	return m_cbNiaoPick;
}



void CTableFrameSink::OnUserXiaoHu(WORD wChairID,DWORD dwCode)
{
	m_dwChiHuKind[wChairID] = AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[wChairID],m_StartHuRight[wChairID]);

	if (m_dwChiHuKind[wChairID] == dwCode  && dwCode == WIK_XIAO_HU)
	{
		LONGLONG lStartHuScore = 0;
		int wFanShu = m_GameLogic.GetChiHuActionRank_CS(m_StartHuRight[wChairID]);
		lStartHuScore  = wFanShu*m_pGameServiceOption->lCellScore;
		m_lStartHuScore[wChairID] += lStartHuScore*3;

		for (int i=0;i<GAME_PLAYER;i++)
		{
			if (i == wChairID)continue;
			m_lStartHuScore[i] -= lStartHuScore;
		}
	}
	if (dwCode !=WIK_NULL)
	{
		CMD_S_XiaoHu kXiaoHu;
		ZeroMemory(kXiaoHu.cbCardData,sizeof(kXiaoHu.cbCardData));
		getXiaoHuCard(wChairID,kXiaoHu);
		m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_XIAO_HU,&kXiaoHu,sizeof(kXiaoHu) );
		m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_XIAO_HU,&kXiaoHu,sizeof(kXiaoHu) );
	}

	m_cbUserAction[wChairID] = WIK_NULL;

	bool bEnd = true;
	for (int i = 0;i < GAME_PLAYER;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
		{
			continue;
		}
		if (m_cbUserAction[i] != WIK_NULL)
		{
			bEnd = false;
		}
	}
	if (bEnd)
	{
		m_pITableFrame->SetGameTimer(IDI_TIMER_XIAO_HU,(TIME_XIAO_HU)*1000,1,0);
	}
}

void CTableFrameSink::getXiaoHuCard( WORD wChairID,CMD_S_XiaoHu& kXiaoHu)
{
	m_StartHuRight[wChairID].GetRightData( &kXiaoHu.dwXiaoCode,MAX_RIGHT_COUNT );
	
	//临时数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,m_cbCardIndex[wChairID],sizeof(cbCardIndexTemp));

	int cbLiuLiuShun = 0;
	kXiaoHu.cbCardCount = 0;
	kXiaoHu.wXiaoHuUser = wChairID;
	//计算单牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndexTemp[i];

		if (cbCardCount ==0)
		{
			continue;
		}

		if( cbCardCount == 4 && kXiaoHu.dwXiaoCode&CHR_XIAO_DA_SI_XI  && kXiaoHu.cbCardCount <4)
		{
			kXiaoHu.cbCardCount = 4;
			for (BYTE j=0;j<kXiaoHu.cbCardCount;j++)
			{
				 kXiaoHu.cbCardData[j]=m_GameLogic.SwitchToCardData(i);
			}
		}

		if(cbCardCount == 3 && kXiaoHu.dwXiaoCode&CHR_XIAO_LIU_LIU_SHUN && kXiaoHu.cbCardCount <6)
		{
			kXiaoHu.cbCardCount +=3;
			for (BYTE j=cbLiuLiuShun*3;j<kXiaoHu.cbCardCount;j++)
			{
				 kXiaoHu.cbCardData[j]=m_GameLogic.SwitchToCardData(i);
			}

			cbLiuLiuShun ++;
		}
	}

	if (kXiaoHu.cbCardCount == 0)
	{
		kXiaoHu.cbCardCount = m_GameLogic.SwitchToCardData(cbCardIndexTemp,kXiaoHu.cbCardData);
		ASSERT(kXiaoHu.cbCardCount == MAX_COUNT -1);
	}

}

int CTableFrameSink::getLeftCardNum()
{
	int nLeftCardNum = 0;
	if (hasRule(GAME_TYPE_ZZ_LIUJU_4CARD))
	{
		nLeftCardNum = m_cbLeftCardCount-4;
	}
	else
	{
		nLeftCardNum = m_cbLeftCardCount;
	}
	return nLeftCardNum;
}

//////////////////////////////////////////////////////////////////////////
