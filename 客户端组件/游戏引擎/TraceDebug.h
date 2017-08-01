#pragma once
#include "GameEngineHead.h"

//输出等级
enum enGameTraceLevel
{
	TraceLevel_Game_Info				=0,									//信息消息
	TraceLevel_Game_Normal				=1,									//普通消息
	TraceLevel_Game_Warning				=2,									//警告消息
	TraceLevel_Game_Exception			=3,									//异常消息
	TraceLevel_Game_Debug				=4,									//调试消息
};

//事件输出
interface IGameTraceService
{
	//字串输出
	virtual bool TraceString(LPCTSTR pszString, enGameTraceLevel TraceLevel)=NULL;
	//字串输出
	virtual bool TraceString(enGameTraceLevel TraceLevel,LPCTSTR pszString, ...)=NULL;
};


// CTraceRichEdit
//////////////////////////////////////////////////////////////////////////
//追踪服务
class GAME_ENGINE_CLASS CGameTraceService
{
protected:
	IGameTraceService					* m_pTraceService;
	TCHAR								m_szLogFile[MAX_PATH];

public:
	CGameTraceService();

	static CGameTraceService*GetInstance();

public:
	//设置接口
	void SetInterFace(IGameTraceService *pTraceService);
	//设置文件
	void SetLogFile(LPCTSTR lpszFileName);
	//字串输出
	bool TraceString(LPCTSTR pszString, enGameTraceLevel TraceLevel);
	//字串输出
	bool TraceString(enGameTraceLevel TraceLevel,LPCTSTR pszString, ...);
};

#define TraceService	CGameTraceService::GetInstance()
//////////////////////////////////////////////////////////////////////////


