#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Check(pDX,IDC_CHECK_ZZ,m_CustomConfig.cbGameType1);
	DDX_Check(pDX,IDC_CHECK_CS,m_CustomConfig.cbGameType2);
	DDX_Check(pDX,IDC_CHECK1,m_CustomConfig.cbGameRule[0]);
	DDX_Check(pDX,IDC_CHECK2,m_CustomConfig.cbGameRule[1]);
	DDX_Check(pDX,IDC_CHECK3,m_CustomConfig.cbGameRule[2]);
	DDX_Check(pDX,IDC_CHECK4,m_CustomConfig.cbGameRule[3]);
	DDX_Check(pDX,IDC_CHECK5,m_CustomConfig.cbGameRule[4]);
	DDX_Check(pDX,IDC_CHECK6,m_CustomConfig.cbGameRule[5]);
	DDX_Check(pDX,IDC_CHECK7,m_CustomConfig.cbGameRule[6]);
	DDX_Check(pDX,IDC_CHECK8,m_CustomConfig.cbGameRule[7]);
	DDX_Check(pDX,IDC_CHECK9,m_CustomConfig.cbGameRule[8]);
	DDX_Check(pDX,IDC_CHECK10,m_CustomConfig.cbGameRule[9]);

}

//初始化函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//设置变量
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return false;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

	//更新界面
	if( m_hWnd )
		UpdateData(TRUE);

	return true;
}

//保存数据
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//更新界面
	if( m_hWnd )
		UpdateData(TRUE);

	//设置变量
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	return true;
}

//保存数据
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//设置变量
	m_wCustomSize=wCustomSize;
	m_pcbCustomRule=pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if( !m_pcbCustomRule ) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

	m_CustomConfig.DefaultCustomRule();
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	//更新界面
	if( m_hWnd )
		UpdateData(TRUE);

	return true;
}
