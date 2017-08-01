#include "StdAfx.h"
#include "TableFrameSink.h"
#include "FvMask.h"

bool CTableFrameSink::hasRule(BYTE cbRule)
{
	return FvMask::HasAny(m_dwGameRuleIdex,_MASK_(cbRule));
}
bool CTableFrameSink::isHZGuiZe()
{
	return hasRule(GAME_TYPE_ZZ_HONGZHONG_GZ);
}
DWORD CTableFrameSink::AnalyseChiHuCardZZ(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight,bool bSelfSendCard)
{
		//变量定义
	DWORD cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//抢杠胡
	bool isQiangGangHu = false;
	if( m_wCurrentUser == INVALID_CHAIR && m_bGangStatus)
	{
		if (hasRule(GAME_TYPE_ZZ_QIANGGANGHU))
		{
			ChiHuRight |= CHR_QIANG_GANG_HU;
			isQiangGangHu = true;
		}
		else
		{
			ChiHuRight.SetEmpty();
		}

	}
	if (hasRule(GAME_TYPE_ZZ_HONGZHONG)&& isHZGuiZe())
	{
		// 起手4个红中直接胡
		if (m_GameLogic.has4MagicCard(cbCardIndex) && m_cbSendCardCount<=0)
		{
			cbChiHuKind = WIK_CHI_HU;
			ChiHuRight.SetEmpty();
			ChiHuRight |= CHR_TIAN_HU;
			return cbChiHuKind;
		}
	}

	if (hasRule(GAME_TYPE_ZZ_ZIMOHU) 
		&& !bSelfSendCard
		&& !isQiangGangHu
		)
	{
		return WIK_NULL;
	}

	if (isHZGuiZe()
		&& !bSelfSendCard
		&& !isQiangGangHu
		)
	{
		return WIK_NULL;
	}


	if( cbCurrentCard == 0 )
	{
		return WIK_NULL;
	}
	//设置变量
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	if (bSelfSendCard)
	{
		ChiHuRight |= CHR_ZI_MO;
	}

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	if (hasRule(GAME_TYPE_ZZ_QIDUI))
	{
		int iTemp = 0;
		if( m_GameLogic.IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,iTemp) ) 
		{
			cbChiHuKind = WIK_CHI_HU;
			ChiHuRight |= CHR_QI_XIAO_DUI;
			return cbChiHuKind;
		}
	}

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCurrentCard)]++;

	if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
	{
		if (m_GameLogic.has4MagicCard(cbCardIndexTemp) && m_cbSendCardCount<=0)
		{
			cbChiHuKind = WIK_CHI_HU;
			return cbChiHuKind;
		}
		if (m_GameLogic.getMagicCardCount(cbCardIndexTemp) == 0 && isHZGuiZe())
		{
			ChiHuRight |= CHR_HONGZHONG_WU;
		}
	}

	//分析扑克
	bool bValue = m_GameLogic.AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);
	if (!bValue)
	{
		ChiHuRight.SetEmpty();
		return WIK_NULL;
	}

	//258做将
	if (hasRule(GAME_TYPE_ZZ_258))
	{
		bool b258Vaild = false;
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
			BYTE cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
			if( cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8 )
			{
			}
			else
			{
				b258Vaild = true;
				break;
			}
		}
		if (!b258Vaild)
		{
			ChiHuRight.SetEmpty();
			cbChiHuKind = WIK_NULL;
			return cbChiHuKind;
		}
	}
	cbChiHuKind = WIK_CHI_HU;

	ChiHuRight |= CHR_SHU_FAN;

	return cbChiHuKind;
}

DWORD CTableFrameSink::AnalyseChiHuCardCS(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight,bool bSelfSendCard)
{
	//变量定义
	DWORD cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
//	ChiHuRight.SetEmpty();

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCard一定不为0			!!!!!!!!!
	ASSERT( cbCurrentCard != 0 );
	if( cbCurrentCard == 0 ) return WIK_NULL;

	/*
	//	特殊番型
	*/
	//七小对牌
	int nGenCount = 0;
	bool isHu = false;
	if( m_GameLogic.IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,nGenCount) )
	{
		if (nGenCount ==1)
		{
			ChiHuRight |= CHR_HAOHUA_QI_XIAO_DUI;
		}
		else if (nGenCount ==2)
		{
			ChiHuRight |= CHR_SHUANG_HAOHUA_QI_XIAO_DUI;
		}
		else
		{
			ChiHuRight |= CHR_QI_XIAO_DUI;
		}
		isHu = true;
	}
	if( m_GameLogic.IsJiangJiangHu(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) ) 
	{
		ChiHuRight |= CHR_JIANGJIANG_HU;

		isHu = true;
	}
	if(cbWeaveCount == 4 &&  m_GameLogic.IsDanDiao(cbCardIndex,cbCurrentCard))
	{
		ChiHuRight |= CHR_QUAN_QIU_REN;
		isHu = true;
	}
	
	if( !ChiHuRight.IsEmpty() )
		cbChiHuKind = WIK_CHI_HU;

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCurrentCard)]++;

	//分析扑克
	bool bValue = m_GameLogic.AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (!bValue && !isHu )
	{
		ChiHuRight.SetEmpty();
		return WIK_NULL;
	}

	cbChiHuKind = WIK_CHI_HU;
	

	//牌型分析
	for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
	{
		//变量定义
		tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

		//碰碰和
		if( m_GameLogic.IsPengPeng(pAnalyseItem) ) 
			ChiHuRight |= CHR_PENGPENG_HU;
	}

	//清一色牌
	if( m_GameLogic.IsQingYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
		ChiHuRight |= CHR_QING_YI_SE;

	//素番
	if(ChiHuRight.IsEmpty())
		ChiHuRight |= CHR_SHU_FAN;

	if (cbChiHuKind == WIK_CHI_HU && !(ChiHuRight&CHR_SHU_FAN).IsEmpty() )
	{
		//长沙麻将小胡必须258做将
		bool b258Vaild = false;
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
			BYTE cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
			if( cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8 )
			{
			}
			else
			{
				b258Vaild = true;
				break;
			}
		}
		if (!b258Vaild)
		{
			ChiHuRight.SetEmpty();
			cbChiHuKind = WIK_NULL;
			return cbChiHuKind;
		}
	}
	if (bSelfSendCard)
	{
		ChiHuRight |= CHR_ZI_MO;
	}
	return cbChiHuKind;
}
DWORD CTableFrameSink::AnalyseChiHuCardCS_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight)
{
	BYTE cbReplaceCount = 0;
	DWORD cbChiHuKind=WIK_NULL;

	//临时数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	bool bDaSiXi = false;//大四喜
	bool bBanBanHu = true;//板板胡
	BYTE cbQueYiMenColor[3] = { 1,1,1 };//缺一色
	BYTE cbLiuLiuShun = 0;//六六顺

	//计算单牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndexTemp[i];

		if (cbCardCount == 0)
		{
			continue;
		}

		if( cbCardCount == 4)
		{
			bDaSiXi = true;
		}

		if(cbCardCount == 3 )
		{
			cbLiuLiuShun ++;
		}

		BYTE cbValue = m_GameLogic.SwitchToCardData(i) & MASK_VALUE;
		if (cbValue == 2 || cbValue == 5 || cbValue == 8)
		{
			bBanBanHu = false;
		}

		BYTE cbCardColor = m_GameLogic.SwitchToCardData(i)&MASK_COLOR;
		cbQueYiMenColor[cbCardColor>>4] = 0;
	}
	if (bDaSiXi)
	{
		ChiHuRight |= CHR_XIAO_DA_SI_XI;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (bBanBanHu)
	{
		ChiHuRight |= CHR_XIAO_BAN_BAN_HU;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (cbQueYiMenColor[0] || cbQueYiMenColor[1] || cbQueYiMenColor[2])
	{
		ChiHuRight |= CHR_XIAO_QUE_YI_SE;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (cbLiuLiuShun >= 2)
	{
		ChiHuRight |= CHR_XIAO_LIU_LIU_SHUN;
		cbChiHuKind = WIK_XIAO_HU;
	}
	return cbChiHuKind;
}
DWORD CTableFrameSink::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight)
{
	if (GAME_TYPE_ZZ == m_cbGameTypeIdex)
	{
		return AnalyseChiHuCardZZ(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,ChiHuRight,m_wProvideUser == m_wCurrentUser);
	}
	if (GAME_TYPE_CS == m_cbGameTypeIdex)
	{
		return AnalyseChiHuCardCS(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,ChiHuRight,m_wProvideUser == m_wCurrentUser);
	}
	ASSERT(false);
	return 0;
}
//
void CTableFrameSink::FiltrateRight( WORD wChairId,CChiHuRight &chr )
{
	//权位增加
	//抢杠
	if( m_wCurrentUser == INVALID_CHAIR && m_bGangStatus )
	{
		chr |= CHR_QIANG_GANG_HU;
	}
	if (m_cbLeftCardCount==0)
	{
		chr |= CHR_HAI_DI_LAO;
	}
	//附加权位
	//杠上花
	if( m_wCurrentUser==wChairId && m_bGangStatus )
	{
		chr |= CHR_GANG_KAI;
		if (m_bCSGangStatusHuCout == 2)
		{
			chr |= CHR_GANG_SHUANG_KAI;
		}
	}
	//杠上炮
	if( m_bGangOutStatus && !m_bGangStatus )
	{
		chr |= CHR_GANG_SHANG_PAO;
		if (m_bCSGangStatusHuCout == 2)
		{
			chr |= CHR_GANG_SHUANG_KAI;
		}
	}
}
