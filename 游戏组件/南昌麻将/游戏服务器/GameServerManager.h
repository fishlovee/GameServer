#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

//游戏服务器管理类
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
{
	//控件变量
protected:
	CDlgCustomRule						m_DlgCustomRule;				//自定规则
	//变量定义
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//服务属性

	//组件变量
protected:
	CGameServiceManagerHelper			m_AndroidServiceHelper;			//机器人服务

	//函数定义
public:
	//构造函数
	CGameServiceManager(void);
	//析构函数
	virtual ~CGameServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID Release() { }
	//是否有效
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CGameServiceManager))?true:false; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//创建桌子
	virtual void * CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer);
	//创建机器
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//创建数据
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer) { return NULL; }
	
	//获取属性
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//修改参数
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

	//配置接口
public:
	//获取配置
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//默认配置
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//创建窗口
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

};

//////////////////////////////////////////////////////////////////////////

#endif
