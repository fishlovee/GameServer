#ifndef DLG_MATCH_HEAD_FILE
#define DLG_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "ModuleManagerHead.h"
#include "ServerInfoManager.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

//主对话框
class MODULE_MANAGER_CLASS CDlgServerMatch : public CDialog
{
	//标识变量
protected:
	WORD							m_wKindID;								//类型标识

	//列表变量
protected:
	CMatchOptionBuffer				m_MatchOptionBuffer;					//模块信息
	CMatchListControl				m_MatchListControl;						//比赛列表

	//配置变量
public:
	tagGameMatchOption				m_GameMatchOption;						//比赛配置

	//函数定义
public:
	//构造函数
	CDlgServerMatch();
	//析构函数
	virtual ~CDlgServerMatch();

	//继承函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//数据交换
	virtual void DoDataExchange(CDataExchange* pDX);

	//配置函数
public:
	//打开比赛
	bool OpenGameMatch(WORD wKindID);

	//功能函数
protected:
	//加载房间
	bool LoadDBMatchItem();

	//按钮消息
protected:
	//加载房间
	VOID OnBnClickedLoadMatch();

	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//选择改变
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////
#endif