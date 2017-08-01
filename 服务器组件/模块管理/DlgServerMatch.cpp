#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"


//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_LOAD_MATCH, OnBnClickedLoadMatch)//确定加载比赛

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)//确定加载比赛
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//标识变量
	m_wKindID=0;

	//模块参数
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));

	return;
}

//析构函数
CDlgServerMatch::~CDlgServerMatch()
{
}

//初始窗口
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//加载比赛
	LoadDBMatchItem();

	return TRUE;  
}

//数据交换
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{
	//比赛列表
	__super::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//打开比赛
bool CDlgServerMatch::OpenGameMatch(WORD wKindID)
{
	//设置变量
	m_wKindID=wKindID;

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}


//加载房间
bool CDlgServerMatch::LoadDBMatchItem()
{
	//重置列表
	m_MatchListControl.DeleteAllItems();

	//设置按钮
	GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);

	//加载信息
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameMatchOption(m_wKindID,m_MatchOptionBuffer)==true)
	{
		//变量定义
		tagGameMatchOption * pGameMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchOptionBuffer.m_GameMatchOptionArray.GetCount();nIndex++)
		{
			//获取对象
			pGameMatchOption=m_MatchOptionBuffer.m_GameMatchOptionArray[nIndex];

			//插入列表
			ASSERT(pGameMatchOption!=NULL);
			if (pGameMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}
		
		return true;
	}

	return false;
}

//加载房间
VOID CDlgServerMatch::OnBnClickedLoadMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//加载奖励
	CServerInfoManager ServerInfoManager;
	pGameMatchOption->wRewardCount=ServerInfoManager.LoadGameMatchReward(pGameMatchOption->dwMatchID,pGameMatchOption->dwMatchNO);

	//设置变量
	CopyMemory(&m_GameMatchOption,pGameMatchOption,sizeof(m_GameMatchOption));

	//结束对话框
	EndDialog(IDC_LOAD_SERVER);
}

//双击列表
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadMatch();
	}

	return;
}

//选择改变
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//获取选择
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//设置列表
	if (Position!=NULL)
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
