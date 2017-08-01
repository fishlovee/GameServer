#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchListControl::CMatchListControl()
{
}

//析构函数
CMatchListControl::~CMatchListControl()
{
}

//配置列表
VOID CMatchListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("比赛名称"),LVCFMT_LEFT,90);
	InsertColumn(nColIndex++,TEXT("比赛类型"),LVCFMT_LEFT,90);
	InsertColumn(nColIndex++,TEXT("扣费类型"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("报名费用"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("报名条件"),LVCFMT_CENTER,60);

	return;
}

//子项排序
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption1=(tagGameMatchOption *)lParam1;
	tagGameMatchOption * pGameMatchOption2=(tagGameMatchOption *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//比赛名称
		{
			return lstrcmp(pGameMatchOption1->szMatchName,pGameMatchOption2->szMatchName);
		}
	case 1:		//比赛类型
		{
			return pGameMatchOption1->cbMatchType>pGameMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
		}
	//case 1:		//开始日期
	//	{
	//		//构造时间
	//		CTime StartTime1(pGameMatchOption1->MatchStartTime);
	//		CTime StartTime2(pGameMatchOption2->MatchStartTime);
	//		return StartTime1>StartTime2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;;
	//	}
	//case 2:		//结束日期
	//	{
	//		//构造时间
	//		CTime EndTime1(pGameMatchOption1->MatchEndTime);
	//		CTime EndTime2(pGameMatchOption2->MatchEndTime);
	//		return EndTime1>EndTime2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;;
	//	}
	case 2:		//扣费类型
		{
			return pGameMatchOption1->cbMatchFeeType<pGameMatchOption2->cbMatchFeeType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//比赛费用
		{
			return pGameMatchOption1->lMatchFee>pGameMatchOption2->lMatchFee?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 4:		//报名条件
		{
			return pGameMatchOption1->cbMemberOrder>pGameMatchOption2->cbMemberOrder?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	//case 6:		//淘汰分数
	//	{
	//		return pGameMatchOption1->lMatchCullScore>pGameMatchOption2->lMatchCullScore?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//	}
	//case 7:		//游戏局数
	//	{
	//		return pGameMatchOption1->wMatchPlayCount>pGameMatchOption2->wMatchPlayCount?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//	}
	//case 8:		//奖励内容
	//	{
	//		return lstrcmp(pGameMatchOption1->szMatchAwardContent,pGameMatchOption2->szMatchAwardContent);
	//	}
	//case 9:		//比赛内容
	//	{
	//		return lstrcmp(pGameMatchOption1->szMatchContent,pGameMatchOption2->szMatchContent);
	//	}
	}

	return 0;
}

//获取颜色
VOID CMatchListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//插入列表
bool CMatchListControl::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pGameMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameMatchOption);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//插入索引
WORD CMatchListControl::GetInsertIndex(tagGameMatchOption * pGameMatchOption)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameMatchOption * pGameMatchTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameMatchTemp=(tagGameMatchOption *)GetItemData(i);

		//比赛名称
		if (lstrcmp(pGameMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CMatchListControl::GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//比赛名称
		{
			return pGameMatchOption->szMatchName;
		}
	case 1:		//比赛类型
		{
			//变量定义
			LPCTSTR pszMatchType[] = {TEXT("定时赛"),TEXT("即时赛")};

			//判断索引
			if(pGameMatchOption->cbMatchType<CountArray(pszMatchType))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMatchType[pGameMatchOption->cbMatchType]);
			}

			return szDescribe;
		}
	case 2:		//比赛费用
		{
			if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_GOLD)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("金币"));
			}
			else if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_MEDAL)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("奖牌"));
			}

			return szDescribe;
		}
	case 3:		//比赛费用
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchFee);
			return szDescribe;
		}
	case 4:		//报名条件
		{
			//变量定义
			LPCTSTR pszMemberOrder[] = {TEXT("所有玩家"),TEXT("蓝钻会员"),TEXT("黄钻会员"),TEXT("白钻会员"),TEXT("红钻会员"),TEXT("VIP会员"),TEXT("晋级人员")};

			//判断索引
			if(pGameMatchOption->cbMemberOrder<CountArray(pszMemberOrder))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMemberOrder[pGameMatchOption->cbMemberOrder]);		
			}

			return szDescribe;
		}
	//case 1:		//开始日期
	//	{
	//		CTime StartTime= CTime(pGameMatchOption->MatchStartTime);
	//		lstrcpyn(szDescribe,StartTime.Format(TEXT("%Y-%m-%d %H:%M:%S")),CountArray(szDescribe));
	//		return szDescribe;
	//	}
	//case 2:		//结束日期
	//	{
	//		CTime EndTime= CTime(pGameMatchOption->MatchEndTime);
	//		lstrcpyn(szDescribe,EndTime.Format(TEXT("%Y-%m-%d  %H:%M:%S")),CountArray(szDescribe));
	//		return szDescribe;
	//	}
	//case 3:		//比赛费用
	//	{
	//		if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_GOLD)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("金币"));
	//		}
	//		else if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_MEDAL)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("奖牌"));
	//		}
	//		return szDescribe;
	//	}
	//case 4:		//比赛费用
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchFee);
	//		return szDescribe;
	//	}
	//case 5:		//初始分数
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchInitScore);
	//		return szDescribe;
	//	}
	//case 6:		//淘汰分数
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchCullScore);
	//		return szDescribe;
	//	}
	//case 7:		//游戏局数
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->wMatchPlayCount);
	//		return szDescribe;
	//	}
	//case 8:		//奖励内容
	//	{
	//		return pGameMatchOption->szMatchAwardContent;
	//	}
	//case 9:		//比赛内容
	//	{
	//		return pGameMatchOption->szMatchContent;
	//	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
