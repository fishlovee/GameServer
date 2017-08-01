#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//连接信息
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->szDataBaseAddr,pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LOGON_GAMEID:			//标识登录
		{
			return OnRequestLogonGameID(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_ACCOUNTS:			//帐号登录
		{
			return OnRequestLogonAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnRequestRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_VISITOR_ACCOUNTS:		//游客登录
		{
			return OnRequestLogonVisitor(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_MACHINE:			//修改机器
		{
			return OnRequestModifyMachine(dwContextID,pData,wDataSize);
		}
	case DBR_GP_PUBLIC_NOTIC:			//自定义数据
		{
			return OnRequestPublicNotic(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_LOGON_PASS:		//修改密码
		{
			return OnRequestModifyLogonPass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INSURE_PASS:		//修改密码
		{
			return OnRequestModifyInsurePass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_ACCOUNTS:		//修改帐号
		{
			return OnRequestModifyAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_SPREADER:		//修改密码
		{
			return OnRequestModifySpreader(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_UNDER_WRITE:		//修改签名
		{
			return OnRequestModifyUnderWrite(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_SYSTEM_FACE:		//修改头像
		{
			return OnRequestModifySystemFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_CUSTOM_FACE:		//修改头像
		{
			return OnRequestModifyCustomFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			return OnRequestModifyIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_INGAME_SERVERID:		//游戏状态
		{
			return OnRequestUserInGameServerID(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SAVE_SCORE:		//存入游戏币
		{
			return OnRequestUserSaveScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TAKE_SCORE:		//提取游戏币
		{
			return OnRequestUserTakeScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TRANSFER_SCORE:	//转账游戏币
		{
			return OnRequestUserTransferScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INDIVIDUAL:		//查询资料
		{
			return OnRequestQueryIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_ACCOUNTINFO:		//查询资料
		{
			return OnRequestQueryAccountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INSURE_INFO:		//查询银行
		{
			return OnRequestQueryInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_USER_INFO:		//查询用户
		{
			return OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_GAME_LIST:			//加载列表
		{
			return OnRequestLoadGameList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ONLINE_COUNT_INFO:		//在线信息
		{
			return OnRequestOnLineCountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKIN_QUERY_INFO:		//查询签到
		{
			return OnRequestCheckInQueryInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_BEGINNER_QUERY_INFO:		//查询签到
		{
			return OnRequestBeginnerQueryInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_BEGINNER_DONE:		//查询签到
		{
			return OnRequestBeginnerDone(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_ADDRANK_CONFIG:		
		{
			return OnRequestAddBankAwardConfig(dwContextID,pData,wDataSize);
		}
	case DBO_GP_ADDRANK_UP_AWARD:		
		{
			return OnRequestAddBackUpAward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GET_ADDRANK:		
		{
			return OnRequestGetAddBank(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_CHECKIN_REWARD:	//签到奖励
		{
			return OnRequestCheckInReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKIN_DONE:			//执行签到
		{
			return OnRequestCheckInDone(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKIN_AWARD:			//执行签到
		{
			return OnRequestCheckAward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_BASEENSURE:		//加载低保
		{
			return OnRequestLoadBaseEnsure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_BEGINNER:		//加载低保
		{
			return OnRequestBeginnerConfig(dwContextID,pData,wDataSize);
		}
	case DBR_GP_BASEENSURE_TAKE:		//领取低保
		{
			return OnRequestTakeBaseEnsure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MATCH_SIGNUP:			//比赛报名
		{
			return OnRequestMatchSignup(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MATCH_UNSIGNUP:			//取消报名
		{
			return OnRequestMatchUnSignup(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MATCH_AWARD:			//取消报名
		{
			return OnRequestLoadMatchReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GET_EXCHANGE_HUAFEI:
		{
			 return OnRequestGetExchangeHuaFeiList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GET_SHOPINFO:
		{
			return OnRequestGetShopInfoList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GAME_RECORD_LIST:
		{
			return OnRequestGameRecordList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GAME_TOTAL_RECORD:
		{
			return OnRequestGameTotalRecord(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_SPREADER:
		{
			return OnRequestQuerySpreaterNickName(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ADD_SPREADER:
		{
			return OnRequestAddSpreater(dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_LogonGameID));
		if (wDataSize!=sizeof(DBR_GP_LogonGameID)) return false;

		//执行查询
		DBR_GP_LogonGameID * pLogonGameID=(DBR_GP_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonGameID->cbNeeValidateMBCard);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonAccounts)) return false;

		//请求处理
		DBR_GP_LogonAccounts * pLogonAccounts=(DBR_GP_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonAccounts->cbNeeValidateMBCard);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//注册处理
bool CDataBaseEngineSink::OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//请求处理
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if(!pBindParameter) return false;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pRegisterAccounts->szSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@strInsurePass"),pRegisterAccounts->szInsurePass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pRegisterAccounts->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pRegisterAccounts->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_LogonVisitor));
	if (wDataSize!=sizeof(DBR_GP_LogonVisitor)) return false;

	//执行查询
	DBR_GP_LogonVisitor * pLogonVisitor=(DBR_GP_LogonVisitor *)pData;

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&pLogonVisitor->dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pLogonVisitor->szComputerID);

	LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyVisitorID"),true);

	if (lReturnValue==0L)
	{
		TCHAR		szAccounts[LEN_NICKNAME];				//登录帐号
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),szAccounts,CountArray(szAccounts));

		DBR_GP_RegisterAccounts kRegister;
		ZeroMemory(&kRegister,sizeof(kRegister));
		kRegister.pBindParameter = pLogonVisitor->pBindParameter;
		lstrcpyn(kRegister.szAccounts,szAccounts,CountArray(kRegister.szAccounts));
		lstrcpyn(kRegister.szNickName,pLogonVisitor->szNickName,CountArray(kRegister.szNickName));
		lstrcpyn(kRegister.szLogonPass,pLogonVisitor->szPassWord,CountArray(kRegister.szLogonPass));
		lstrcpyn(kRegister.szInsurePass,pLogonVisitor->szPassWordBank,CountArray(kRegister.szInsurePass));
		lstrcpyn(kRegister.szMachineID,pLogonVisitor->szComputerID,CountArray(kRegister.szMachineID));
		kRegister.dwClientAddr = pLogonVisitor->dwClientIP;
		kRegister.wFaceID = pLogonVisitor->wFaceID;
		kRegister.cbGender = pLogonVisitor->cbGender;

		OnRequestRegisterAccounts(dwContextID,&kRegister,sizeof(kRegister));
	}
	else
	{

		DBO_MB_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode=lReturnValue;
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonFailure.szDescribeString,CountArray(LogonFailure.szDescribeString));
		//发送结果
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		return true;
	}
	return false;
}
//I D 登录
bool CDataBaseEngineSink::OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_LogonGameID));
		if (wDataSize!=sizeof(DBR_MB_LogonGameID)) return false;

		//执行查询
		DBR_MB_LogonGameID * pLogonGameID=(DBR_MB_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonGameID->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_LogonAccounts));
		if (wDataSize!=sizeof(DBR_MB_LogonAccounts)) return false;

		//请求处理
		DBR_MB_LogonAccounts * pLogonAccounts=(DBR_MB_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonAccounts->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//帐号注册
bool CDataBaseEngineSink::OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_MB_RegisterAccounts)) return false;

		//请求处理
		DBR_MB_RegisterAccounts * pRegisterAccounts=(DBR_MB_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@strInsurePass"),pRegisterAccounts->szInsurePass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterAccounts->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//修改机器
bool CDataBaseEngineSink::OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyMachine));
		if (wDataSize!=sizeof(DBR_GP_ModifyMachine)) return false;

		//请求处理
		DBR_GP_ModifyMachine * pModifyMachine=(DBR_GP_ModifyMachine *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//绑定操作
		if (pModifyMachine->cbBind==TRUE)
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_MoorMachine"),false);
		}
		else
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnMoorMachine"),false);
		}

		//结果处理
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifySystemFace));
		if (wDataSize!=sizeof(DBR_GP_ModifySystemFace)) return false;

		//请求处理
		DBR_GP_ModifySystemFace * pModifySystemFace=(DBR_GP_ModifySystemFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifySystemFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifySystemFace->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifySystemFace->wFaceID);

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifySystemFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SystemFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyCustomFace));
		if (wDataSize!=sizeof(DBR_GP_ModifyCustomFace)) return false;

		//请求处理
		DBR_GP_ModifyCustomFace * pModifyCustomFace=(DBR_GP_ModifyCustomFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyCustomFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//变量定义
		SAFEARRAYBOUND SafeArrayBound;
		ZeroMemory(&SafeArrayBound,sizeof(SafeArrayBound));

		//设置变量
		SafeArrayBound.lLbound=0L;
		SafeArrayBound.cElements=sizeof(pModifyCustomFace->dwCustomFace);

		//变量定义
		BYTE * pcbCustomFace=(BYTE *)pModifyCustomFace->dwCustomFace;
		SAFEARRAY * pSafeArray=SafeArrayCreate(VT_UI1,1,&SafeArrayBound);

		//创建数组
		for (LONG nIndex=0;nIndex<sizeof(pModifyCustomFace->dwCustomFace);nIndex++)
		{
			SafeArrayPutElement(pSafeArray,&nIndex,pcbCustomFace+nIndex);
		}

		//设置变量
		VARIANT VarChunk;
		VariantClear(&VarChunk);
		VarChunk.parray=pSafeArray;
		VarChunk.vt=VT_ARRAY|VT_UI1;

		//获取对象
		ASSERT(m_AccountsDBModule.GetInterface()!=NULL);
		IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyCustomFace->szPassword);

		//头像信息
		pIDataBase->AddParameter(TEXT("@dwCustomFace"),adLongVarBinary,adParamInput,sizeof(pModifyCustomFace->dwCustomFace),CDBVarValue(VarChunk));

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyCustomFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//释放资源
		SafeArrayDestroy(pSafeArray);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CustomFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=INVALID_WORD;
			UserFaceInfo.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			
			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//请求处理
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyLogonPass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//请求处理
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改帐号
bool CDataBaseEngineSink::OnRequestModifyAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyAccounts));
		if (wDataSize!=sizeof(DBR_GP_ModifyAccounts)) return false;

		//请求处理
		DBR_GP_ModifyAccounts * pModifyInsurePass=(DBR_GP_ModifyAccounts *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesAccounts"),pModifyInsurePass->szDesAccount);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyAccount"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//修改推荐人
bool CDataBaseEngineSink::OnRequestModifySpreader(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifySpreader));
		if (wDataSize!=sizeof(DBR_GP_ModifySpreader)) return false;

		//请求处理
		DBR_GP_ModifySpreader * pModifyUnderWrite=(DBR_GP_ModifySpreader *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyUnderWrite->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyUnderWrite->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyUnderWrite->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pModifyUnderWrite->szSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifySpreader"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);		

		DBO_GP_SpreaderResoult OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造变量
		OperateSuccess.lResultCode=lResultCode;
		if (lResultCode == DB_SUCCESS)
		{
			OperateSuccess.lScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("DestScore"));
		}
		lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SPREADER_RESOULT,dwContextID,&OperateSuccess,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//修改签名
bool CDataBaseEngineSink::OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyUnderWrite));
		if (wDataSize!=sizeof(DBR_GP_ModifyUnderWrite)) return false;

		//请求处理
		DBR_GP_ModifyUnderWrite * pModifyUnderWrite=(DBR_GP_ModifyUnderWrite *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyUnderWrite->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyUnderWrite->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyUnderWrite->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyUnderWrite->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUnderWrite"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改资料
bool CDataBaseEngineSink::OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//请求处理
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyIndividual->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strHeadHttp"),pModifyIndividual->szHeadHttp);
		m_AccountsDBAide.AddParameter(TEXT("@strUserChannel"),pModifyIndividual->szUserChannel);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//修改资料
bool CDataBaseEngineSink::OnRequestUserInGameServerID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryUserInGameServerID));
		if (wDataSize!=sizeof(DBR_GP_QueryUserInGameServerID)) return false;

		//请求处理
		DBR_GP_QueryUserInGameServerID * pModifyIndividual=(DBR_GP_QueryUserInGameServerID *)pData;


		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserInGameServerID"),true);

		if (lResultCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserInGameServerID kGPNetInfo;
			ZeroMemory(&kGPNetInfo,sizeof(kGPNetInfo));

			//构造变量
			kGPNetInfo.LockKindID=m_AccountsDBAide.GetValue_DWORD(TEXT("LockKindID"));
			kGPNetInfo.LockServerID=m_AccountsDBAide.GetValue_DWORD(TEXT("LockServerID"));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INGAME_SERVER_ID,dwContextID,&kGPNetInfo,sizeof(kGPNetInfo));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

	return true;
}
bool CDataBaseEngineSink::OnRequestPublicNotic(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_PublicNotic));
		if (wDataSize!=sizeof(DBR_GP_PublicNotic)) return false;

		//请求处理
		DBR_GP_PublicNotic * pDBNetInfo=(DBR_GP_PublicNotic *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strKeyName"),pDBNetInfo->szKeyName);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_PublicNotic"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		if (lResultCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_PublicNotice kGPNetInfo;
			ZeroMemory(&kGPNetInfo,sizeof(kGPNetInfo));

			//构造变量
			kGPNetInfo.lResultCode=lResultCode;
			lstrcpyn(kGPNetInfo.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(kGPNetInfo.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(kGPNetInfo.szDescribeString);
			WORD wHeadSize=sizeof(kGPNetInfo)-sizeof(kGPNetInfo.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PUBLIC_NOTICE,dwContextID,&kGPNetInfo,wHeadSize+wDataSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//构造变量
			OperateFailure.lResultCode=lResultCode;
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//存入游戏币
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserSaveScore));
		if (wDataSize!=sizeof(DBR_GP_UserSaveScore)) return false;

		//请求处理
		DBR_GP_UserSaveScore * pUserSaveScore=(DBR_GP_UserSaveScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),(int)pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserSaveScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserSaveScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//提取游戏币
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserTakeScore));
		if (wDataSize!=sizeof(DBR_GP_UserTakeScore)) return false;

		//请求处理
		DBR_GP_UserTakeScore * pUserTakeScore=(DBR_GP_UserTakeScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTakeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTakeScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//转账游戏币
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserTransferScore));
		if (wDataSize!=sizeof(DBR_GP_UserTransferScore)) return false;

		//请求处理
		DBR_GP_UserTransferScore * pUserTransferScore=(DBR_GP_UserTransferScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTransferScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pUserTransferScore->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@lTransferScore"),pUserTransferScore->lTransferScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTransferScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pUserTransferScore->szNickName);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//查询个人信息
bool CDataBaseEngineSink::OnRequestQueryAccountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryAccountInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryAccountInfo)) return false;

		//请求处理
		DBR_GP_QueryAccountInfo * pQueryAccountInfo=(DBR_GP_QueryAccountInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryAccountInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryAccountInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserAccountInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserAccountInfo kUserAccountInfo;
			ZeroMemory(&kUserAccountInfo,sizeof(kUserAccountInfo));

			//属性资料
			kUserAccountInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			kUserAccountInfo.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			kUserAccountInfo.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			kUserAccountInfo.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			kUserAccountInfo.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
			kUserAccountInfo.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			kUserAccountInfo.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			kUserAccountInfo.dwSpreaderID=m_AccountsDBAide.GetValue_DWORD(TEXT("SpreaderID"));

			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),kUserAccountInfo.szAccounts,CountArray(kUserAccountInfo.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),kUserAccountInfo.szNickName,CountArray(kUserAccountInfo.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("LogonIP"),kUserAccountInfo.szLogonIp,CountArray(kUserAccountInfo.szLogonIp));

			//用户成绩
			kUserAccountInfo.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			kUserAccountInfo.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));

			//用户资料
			kUserAccountInfo.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			kUserAccountInfo.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),kUserAccountInfo.szUnderWrite,CountArray(kUserAccountInfo.szUnderWrite));

			//会员资料
			kUserAccountInfo.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),kUserAccountInfo.MemberOverDate);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_ACCOUNTINFO,dwContextID,&kUserAccountInfo,sizeof(kUserAccountInfo));
		}
		else
		{
			//错误处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),szDescribeString,false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//查询资料
bool CDataBaseEngineSink::OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryIndividual));
		if (wDataSize!=sizeof(DBR_GP_QueryIndividual)) return false;

		//请求处理
		DBR_GP_QueryIndividual * pQueryIndividual=(DBR_GP_QueryIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserIndividual UserIndividual;
			ZeroMemory(&UserIndividual,sizeof(UserIndividual));

			//用户信息
			UserIndividual.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserIndividual.szUserNote,CountArray(UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserIndividual.szCompellation,CountArray(UserIndividual.szCompellation));

			//电话号码
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.szSeatPhone,CountArray(UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.szMobilePhone,CountArray(UserIndividual.szMobilePhone));

			//联系资料
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.szQQ,CountArray(UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.szEMail,CountArray(UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.szDwellingPlace,CountArray(UserIndividual.szDwellingPlace));
			m_AccountsDBAide.GetValue_String(TEXT("HeadHttp"),UserIndividual.szHeadHttp,CountArray(UserIndividual.szHeadHttp));
			m_AccountsDBAide.GetValue_String(TEXT("UserChannel"),UserIndividual.szUserChannel,CountArray(UserIndividual.szUserChannel));
			lstrcpyn(UserIndividual.szLogonIP,szClientAddr,CountArray(UserIndividual.szLogonIP));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL,dwContextID,&UserIndividual,sizeof(UserIndividual));
		}
		else
		{
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询银行
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryInsureInfo)) return false;

		//请求处理
		DBR_GP_QueryInsureInfo * pQueryInsureInfo=(DBR_GP_QueryInsureInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserInsureInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//银行信息
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TransferPrerequisite"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询用户
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureUserInfo));
	if (wDataSize!=sizeof(DBR_GP_QueryInsureUserInfo)) return false;

	//请求处理
	DBR_GP_QueryInsureUserInfo * pQueryTransferUserInfo=(DBR_GP_QueryInsureUserInfo *)pData;

	try
	{
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pQueryTransferUserInfo->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pQueryTransferUserInfo->szNickName);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferUserInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//银行信息
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("NickName"), TransferUserInfo.szNickName, CountArray(TransferUserInfo.szNickName));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}
}

//加载列表
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//加载类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);

		//发送种类
		wPacketSize=0;
		DBO_GP_GameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameType))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameType=(DBO_GP_GameType *)(cbBuffer+wPacketSize);
			pGameType->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameType);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);

		//发送类型
		wPacketSize=0;
		DBO_GP_GameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameKind))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameKind=(DBO_GP_GameKind *)(cbBuffer+wPacketSize);
			pGameKind->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			pGameKind->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wGameID=m_PlatformDBAide.GetValue_WORD(TEXT("GameID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));
			m_PlatformDBAide.GetValue_String(TEXT("ProcessName"),pGameKind->szProcessName,CountArray(pGameKind->szProcessName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取节点
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);

		//发送节点
		wPacketSize=0;
		DBO_GP_GameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameNode))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameNode=(DBO_GP_GameNode *)(cbBuffer+wPacketSize);
			pGameNode->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameNode);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取定制
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePageItem"),true);

		//发送定制
		wPacketSize=0;
		DBO_GP_GamePage * pGamePage=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePage))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePage=(DBO_GP_GamePage *)(cbBuffer+wPacketSize);
			pGamePage->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGamePage->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGamePage->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGamePage->wPageID=m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGamePage->wOperateType=m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"),pGamePage->szDisplayName,CountArray(pGamePage->szDisplayName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePage);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=TRUE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=FALSE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//在线信息
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//效验数据
		ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

		//机器标识
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//构造信息
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			_sntprintf(&szOnLineCountKind[nLength],CountArray(szOnLineCountKind)-nLength,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[i].wKindID,
				pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);

		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载奖励
bool CDataBaseEngineSink::OnRequestCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadCheckInReward"),true);

		//构造结构
		DBO_GP_CheckInReward CheckInReward;
		ZeroMemory(&CheckInReward,sizeof(CheckInReward));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			WORD wDayIndex=0;
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				wDayIndex = m_AccountsDBAide.GetValue_WORD(TEXT("DayID"));
				if(wDayIndex<=LEN_SIGIN)
				{
					CheckInReward.lRewardGold[wDayIndex-1] = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RewardGold")); 
					CheckInReward.lRewardType[wDayIndex-1] = m_AccountsDBAide.GetValue_BYTE(TEXT("RewardType")); 
					CheckInReward.lRewardDay[wDayIndex-1]  = m_AccountsDBAide.GetValue_BYTE(TEXT("RewardDay")); 
				}

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_REWARD,dwContextID,&CheckInReward,sizeof(CheckInReward));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载奖励
bool CDataBaseEngineSink::OnRequestBeginnerConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadBeginnerConfig"),true);

		//构造结构
		DBO_GP_BeginnerCofig kDBInfo;
		ZeroMemory(&kDBInfo,sizeof(kDBInfo));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			WORD wDayIndex=0;
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				wDayIndex = m_AccountsDBAide.GetValue_WORD(TEXT("DayID"));
				if(wDayIndex<=LEN_BEGINNER)
				{
					kDBInfo.lRewardGold[wDayIndex-1] = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RewardGold")); 
					kDBInfo.lRewardType[wDayIndex-1] = m_AccountsDBAide.GetValue_BYTE(TEXT("RewardType")); 
				}

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_CONFIG,dwContextID,&kDBInfo,sizeof(kDBInfo));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}
bool CDataBaseEngineSink::OnRequestBeginnerQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_BeginnerQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_BeginnerQueryInfo)) return false;

		//请求处理
		DBR_GP_BeginnerQueryInfo * pNetInfo=(DBR_GP_BeginnerQueryInfo *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pNetInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pNetInfo->szPassword);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_BeginnerQueryInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_BeginnerInfo CheckInInfo;
			ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

			//银行信息
			CheckInInfo.wSeriesDate=m_AccountsDBAide.GetValue_WORD(TEXT("SeriesDate"));
			CheckInInfo.bTodayChecked = (bool)m_AccountsDBAide.GetValue_BYTE(TEXT("TodayCheckIned"));
			CheckInInfo.bLastCheckIned = (bool)m_AccountsDBAide.GetValue_BYTE(TEXT("LastCheckIned"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_INFO,dwContextID,&CheckInInfo,sizeof(CheckInInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GP_BeginnerResult kResoult;
			kResoult.bSuccessed=false;
			lstrcpyn(kResoult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(kResoult.szNotifyContent));

			//发送结果
			WORD wSendSize=sizeof(kResoult)-sizeof(kResoult.szNotifyContent)+CountStringBuffer(kResoult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_RESULT,dwContextID,&kResoult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}
bool CDataBaseEngineSink::OnRequestBeginnerDone(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_BeginnerDone));
		if (wDataSize!=sizeof(DBR_GP_BeginnerDone)) return false;

		//请求处理
		DBR_GP_BeginnerDone * pNetInfo=(DBR_GP_BeginnerDone *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pNetInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pNetInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pNetInfo->szPassword);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_BeginnerDone"),true);

		//变量定义
		DBO_GP_BeginnerResult kNetInfo;
		ZeroMemory(&kNetInfo,sizeof(kNetInfo));
		kNetInfo.lAwardType = 0;

		//读取分数
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			kNetInfo.lAwardType = m_AccountsDBAide.GetValue_LONGLONG(TEXT("AwardType"));
			kNetInfo.lAwardCout = m_AccountsDBAide.GetValue_LONGLONG(TEXT("AwardCout"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


		kNetInfo.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(kNetInfo.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(kNetInfo.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(kNetInfo)-sizeof(kNetInfo.szNotifyContent)+CountStringBuffer(kNetInfo.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_RESULT,dwContextID,&kNetInfo,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BEGINNER_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}
//
bool CDataBaseEngineSink::OnRequestAddBankAwardConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_GetAddBankConfig));
		if (wDataSize!=sizeof(DBR_GP_GetAddBankConfig)) return false;

		//请求处理
		DBR_GP_GetAddBankConfig * pNetInfo=(DBR_GP_GetAddBankConfig *)pData;
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@iRankIdex"),pNetInfo->iIdex);

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_AddPointConfig"),true);

		//构造结构
		DBO_GP_AddRankAwardConfig kDBInfo;
		ZeroMemory(&kDBInfo,sizeof(DBO_GP_AddRankAwardConfig));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			WORD wDayIndex=0;
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				wDayIndex = m_AccountsDBAide.GetValue_WORD(TEXT("DayID"))-1;
				if(wDayIndex<=LEN_ADDRANK && wDayIndex >= 0)
				{
					kDBInfo.iIdex = pNetInfo->iIdex; 
					kDBInfo.kRewardGold[wDayIndex] = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RewardGold")); 
					kDBInfo.kRewardType[wDayIndex] = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RewardType")); 
				}
				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ADDRANK_AWARD_CONFIG,dwContextID,&kDBInfo,sizeof(DBO_GP_AddRankAwardConfig));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}
//
bool CDataBaseEngineSink::OnRequestAddBackUpAward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		m_AccountsDBAide.ResetParameter();

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_UserAddPoint"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	return true;
}
//
bool CDataBaseEngineSink::OnRequestGetAddBank(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_GetAddBank));
		if (wDataSize!=sizeof(DBR_GP_GetAddBank)) return false;

		//请求处理
		DBR_GP_GetAddBank * pNetInfo=(DBR_GP_GetAddBank *)pData;
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pNetInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pNetInfo->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@iRankIdex"),pNetInfo->iIdex);

		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_UserGetAddRank"),true);

		//构造结构
		DBO_GP_BackAddBank kDBInfo;
		kDBInfo.iRankIdex = pNetInfo->iIdex;

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			WORD wDayIndex=0;
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				TCHAR		szNickName[LEN_NICKNAME];				//登录帐号
				m_AccountsDBAide.GetValue_String(TEXT("NickName"),szNickName,CountArray(szNickName));
				kDBInfo.kNickName.push_back(szNickName);
				kDBInfo.kUserID.push_back(m_AccountsDBAide.GetValue_INT(TEXT("UserID"))); 
				kDBInfo.kUserPoint.push_back(m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserPoint"))); 
				kDBInfo.kFaceID.push_back(m_AccountsDBAide.GetValue_INT(TEXT("FaceID")));  
				kDBInfo.kCustomID.push_back(m_AccountsDBAide.GetValue_INT(TEXT("CustomID")));  
				
				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			datastream kTempDataStream;
			kDBInfo.StreamValue(kTempDataStream,true);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ADDRANK_RANK_BACK,dwContextID,kTempDataStream.data(),kTempDataStream.size());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}
//查询签到
bool CDataBaseEngineSink::OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_CheckInQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_CheckInQueryInfo)) return false;

		//请求处理
		DBR_GP_CheckInQueryInfo * pCheckInQueryInfo=(DBR_GP_CheckInQueryInfo *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pCheckInQueryInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pCheckInQueryInfo->szPassword);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_CheckInQueryInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_CheckInInfo CheckInInfo;
			ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

			//银行信息
			CheckInInfo.wSeriesDate=m_AccountsDBAide.GetValue_WORD(TEXT("SeriesDate"));
			CheckInInfo.wAwardDate =m_AccountsDBAide.GetValue_WORD(TEXT("AwardDate"));
			CheckInInfo.bTodayChecked = (bool)m_AccountsDBAide.GetValue_BYTE(TEXT("TodayCheckIned"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_INFO,dwContextID,&CheckInInfo,sizeof(CheckInInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GP_CheckInResult CheckInResult;
			CheckInResult.bSuccessed=false;
			lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

			//发送结果
			WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}
//执行签到
bool CDataBaseEngineSink::OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_CheckInDone));
		if (wDataSize!=sizeof(DBR_GP_CheckInDone)) return false;

		//请求处理
		DBR_GP_CheckInDone * pCheckInDone=(DBR_GP_CheckInDone *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCheckInDone->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pCheckInDone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pCheckInDone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pCheckInDone->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_CheckInDone"),true);

		//变量定义
		DBO_GP_CheckInResult CheckInResult;
		ZeroMemory(&CheckInResult,sizeof(CheckInResult));
		CheckInResult.bType = false;

		//读取分数
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			CheckInResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		
		CheckInResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}
bool CDataBaseEngineSink::OnRequestCheckAward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_CheckInDone));
		if (wDataSize!=sizeof(DBR_GP_CheckInDone)) return false;

		//请求处理
		DBR_GP_CheckInDone * pCheckInDone=(DBR_GP_CheckInDone *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCheckInDone->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pCheckInDone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pCheckInDone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pCheckInDone->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_CheckAward"),true);

		//变量定义
		DBO_GP_CheckInResult CheckInResult;
		ZeroMemory(&CheckInResult,sizeof(CheckInResult));
		CheckInResult.bType = true;

		//读取分数
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			CheckInResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


		CheckInResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//低保参数
bool CDataBaseEngineSink::OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadBaseEnsure"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//构造结构
			DBO_GP_BaseEnsureParameter BaseEnsureParameter;
			ZeroMemory(&BaseEnsureParameter,sizeof(BaseEnsureParameter));

			//变量定义
			if(m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				BaseEnsureParameter.cbTakeTimes = m_AccountsDBAide.GetValue_WORD(TEXT("TakeTimes"));
				BaseEnsureParameter.lScoreAmount = m_AccountsDBAide.GetValue_WORD(TEXT("ScoreAmount"));
				BaseEnsureParameter.lScoreCondition = m_AccountsDBAide.GetValue_WORD(TEXT("ScoreCondition"));				

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_PARAMETER,dwContextID,&BaseEnsureParameter,sizeof(BaseEnsureParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}

//领取低保
bool CDataBaseEngineSink::OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_TakeBaseEnsure));
		if (wDataSize!=sizeof(DBR_GP_TakeBaseEnsure)) return false;

		//提取数据
		DBR_GP_TakeBaseEnsure * pTakeBaseEnsure=(DBR_GP_TakeBaseEnsure *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTakeBaseEnsure->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pTakeBaseEnsure->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pTakeBaseEnsure->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pTakeBaseEnsure->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_TakeBaseEnsure"),true);

		//变量定义
		DBO_GP_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//读取分数
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			BaseEnsureResult.lGameScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);

		//银行信息
		BaseEnsureResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(BaseEnsureResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(BaseEnsureResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GP_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//设置变量
		BaseEnsureResult.bSuccessed=false;
		lstrcpyn(BaseEnsureResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(BaseEnsureResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return false;
	}

	return true;
}

//比赛报名
bool CDataBaseEngineSink::OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_MatchSignup));
		if (wDataSize!=sizeof(DBR_GP_MatchSignup)) return false;

		//请求处理
		DBR_GP_MatchSignup * pMatchSignup=(DBR_GP_MatchSignup *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pMatchSignup->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pMatchSignup->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),pMatchSignup->wServerID);
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchID"),pMatchSignup->dwMatchID);
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchSignup->dwMatchNO);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pMatchSignup->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_DeductMatchFee"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//构造结果
		DBO_GP_MatchSignupResult MatchSignupResult;
		MatchSignupResult.bSignup = true;
		MatchSignupResult.bSuccessed = lResultCode==DB_SUCCESS;
		lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));

		//投递结果
		WORD wDataSize=CountStringBuffer(MatchSignupResult.szDescribeString);
		WORD wHeadSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结果
		DBO_GP_MatchSignupResult MatchSignupResult;
		MatchSignupResult.bSignup = true;
		MatchSignupResult.bSuccessed = false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(MatchSignupResult.szDescribeString));

		//投递结果
		WORD wDataSize=CountStringBuffer(MatchSignupResult.szDescribeString);
		WORD wHeadSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//取消报名
bool CDataBaseEngineSink::OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_MatchUnSignup));
		if (wDataSize!=sizeof(DBR_GP_MatchUnSignup)) return false;

		//请求处理
		DBR_GP_MatchUnSignup * pMatchUnSignup=(DBR_GP_MatchUnSignup *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchUnSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pMatchUnSignup->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pMatchUnSignup->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),pMatchUnSignup->wServerID);
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchID"),pMatchUnSignup->dwMatchID);
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchUnSignup->dwMatchNO);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pMatchUnSignup->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_ReturnMatchFee"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//构造结果
		DBO_GP_MatchSignupResult MatchSignupResult;
		MatchSignupResult.bSignup = false;
		MatchSignupResult.bSuccessed = lResultCode==DB_SUCCESS;
		lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));

		//投递结果
		WORD wDataSize=CountStringBuffer(MatchSignupResult.szDescribeString);
		WORD wHeadSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT(""),false);

		//构造结果
		DBO_GP_MatchSignupResult MatchSignupResult;
		MatchSignupResult.bSignup = false;
		MatchSignupResult.bSuccessed = false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(MatchSignupResult.szDescribeString));

		//投递结果
		WORD wDataSize=CountStringBuffer(MatchSignupResult.szDescribeString);
		WORD wHeadSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestLoadMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LoadMatchReward));
	if (wDataSize!=sizeof(DBR_GR_LoadMatchReward)) return false;

	//变量定义
	DBR_GR_LoadMatchReward * pMatchReward=(DBR_GR_LoadMatchReward *)pData;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchID"),pMatchReward->dwMatchID);
		m_PlatformDBAide.AddParameter(TEXT("@dwMatchNo"),pMatchReward->dwMatchNO);

		//结果处理
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchReward"),true);

		//构造结构
		DBO_GR_MatchAwardList MatchRewardResult;

		//读取财富
		if(lReturnValue==DB_SUCCESS)
		{
			MatchRewardResult.dwMatchID = pMatchReward->dwMatchID;
			MatchRewardResult.dwMatchNO = pMatchReward->dwMatchNO;
			while(m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				tagDBMatchAwardkInfo kAwardInfo;
				kAwardInfo.MatchRank = m_PlatformDBAide.GetValue_WORD("MatchRank");
				kAwardInfo.RewardGold = m_PlatformDBAide.GetValue_LONGLONG("RewardGold");
				kAwardInfo.RewardMedal = m_PlatformDBAide.GetValue_DWORD("RewardMedal");
				kAwardInfo.RewardExperience = m_PlatformDBAide.GetValue_DWORD("RewardExperience");
				MatchRewardResult.kAwards.push_back(kAwardInfo);
				m_PlatformDBModule->MoveToNext();
			}
		}
		datastream kDataStream;
		MatchRewardResult.StreamValue(kDataStream,true);
		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MATCH_AWARD,dwContextID,&kDataStream[0],kDataStream.size());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}
//游戏记录
bool CDataBaseEngineSink::OnRequestGameRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//请求处理
		DBR_GP_GameRecordList * pNetInfo = (DBR_GP_GameRecordList *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pNetInfo->dwUserID);

		//执行命令
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_GameRecord_List_RecordID"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//构造结构
			tagPrivateRandTotalRecordList kGameRecordList;
			kGameRecordList.dwUserID = pNetInfo->dwUserID;

			std::vector<int>	kRecordID;
			//变量定义
			while(m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				kRecordID.push_back(m_TreasureDBAide.GetValue_DWORD("RecordID"));
				m_TreasureDBModule->MoveToNext();
			}
			for (int i = 0;i<(int)kRecordID.size();i++)
			{
				m_TreasureDBAide.ResetParameter();
				m_TreasureDBAide.AddParameter(TEXT("@dwRecordID"),kRecordID[i]);
				LONG lTempResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_PrivateGameRecord"),true);
				if (lTempResultCode == DB_SUCCESS)
				{
					datastream kTempData;
					m_TreasureDBAide.GetParameter(TEXT("UserDate"),kTempData);

					tagPrivateRandTotalRecord kTotalRecord;
					kTotalRecord.StreamValue(kTempData,false);
					kTotalRecord.iRecordID = kRecordID[i];
					kGameRecordList.kList.push_back(kTotalRecord);
				}
			}

			datastream kDataStream;
			kGameRecordList.StreamValue(kDataStream,true);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_RECORD_LIST,dwContextID,&kDataStream[0],kDataStream.size());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}
//游戏回放
bool CDataBaseEngineSink::OnRequestGameTotalRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		//请求处理
		DBR_GP_GetGameTotalRecord * pNetInfo = (DBR_GP_GetGameTotalRecord *)pData;

		tagPrivateRandTotalRecord kTotalRecord;
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwRecordID"),pNetInfo->dwRecordID);
		LONG lTempCode1 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_PrivateGameRecord"),true);
		if (lTempCode1 == DB_SUCCESS)
		{
			datastream kTempData;
			m_TreasureDBAide.GetParameter(TEXT("UserDate"),kTempData);
			kTotalRecord.StreamValue(kTempData,false);
		}

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwRecordID"),pNetInfo->dwRecordID);
		//执行命令
		LONG lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_PrivateGameRecord_Child"),true);
		if(lResultCode2==DB_SUCCESS)
		{
			//变量定义
			while(m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				DWORD RecordChildID = m_TreasureDBAide.GetValue_DWORD(TEXT("RecordChildID"));
				datastream kDataStream;
				m_TreasureDBAide.GetParameter(TEXT("UserDate"),kDataStream);
				tagPrivateRandRecordChild kRecordChild;
				kRecordChild.StreamValue(kDataStream,false);
				kRecordChild.iRecordChildID = RecordChildID;
				kTotalRecord.kRecordChild.push_back(kRecordChild);

				m_TreasureDBModule->MoveToNext();
			}
			datastream kDataStream;
			kTotalRecord.StreamValue(kDataStream,true);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_RECORD_TOTAL,dwContextID,&kDataStream[0],kDataStream.size());

		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}

//登录成功
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (bMobileClient==false)
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LogonSuccess.wFaceID=0;
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			LogonSuccess.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			LogonSuccess.dwSpreaderID=m_AccountsDBAide.GetValue_DWORD(TEXT("SpreaderID"));
			LogonSuccess.cbInsureEnabled=m_AccountsDBAide.GetValue_DWORD(TEXT("InsureEnabled"));
			
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));

			//用户成绩
			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));

			//用户资料
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//会员资料
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));


			try
			{
				//构造参数
				m_PlatformDBAide.ResetParameter();
				m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),LogonSuccess.dwUserID);

				//输出参数
				TCHAR szDescribeString[128]=TEXT("");
				m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

				//执行查询
				LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchSignupList"),true);
				if(lResultCode==DB_SUCCESS)
				{
					//变量定义
					tagSignupMatchInfo * pSignupMatchInfo=NULL;

					while (m_PlatformDBModule->IsRecordsetEnd()==false)
					{
						//溢出判断
						if(LogonSuccess.wSignupCount>=CountArray(LogonSuccess.SignupMatchInfo)) break;

						//获取对象
						pSignupMatchInfo=&LogonSuccess.SignupMatchInfo[LogonSuccess.wSignupCount++];

						//读取记录
						pSignupMatchInfo->wServerID=m_PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
						pSignupMatchInfo->dwMatchID=m_PlatformDBAide.GetValue_DWORD(TEXT("MatchID"));
						pSignupMatchInfo->dwMatchNO=m_PlatformDBAide.GetValue_DWORD(TEXT("MatchNo"));

						//移动记录
						m_PlatformDBModule->MoveToNext();
					}
				}
			}
			catch(...)
			{
				ASSERT(FALSE);
			};

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else if(dwErrorCode == DB_NEEDMB)
		{
			//登录成功
			DBR_GP_ValidateMBCard ValidateMBCard;
			ZeroMemory(&ValidateMBCard,sizeof(ValidateMBCard));

			//读取变量
			ValidateMBCard.uMBCardID=m_AccountsDBAide.GetValue_UINT(TEXT("PasswordID"));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_VALIDATE_MBCARD,dwContextID,&ValidateMBCard,sizeof(ValidateMBCard));
		}
		else
		{
			//变量定义
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}
	else
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			//变量定义
			DBO_MB_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));

			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//变量定义
			DBO_MB_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}

//银行结果
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//构造变量
		UserInsureSuccess.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserInsureSuccess.lSourceScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"));
		UserInsureSuccess.lSourceInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"));
		UserInsureSuccess.lInsureRevenue=m_TreasureDBAide.GetValue_LONGLONG(TEXT("InsureRevenue"));
		UserInsureSuccess.lDestScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("DestScore"));
		UserInsureSuccess.lDestInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("DestInsure"));
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//构造变量
		UserInsureFailure.lResultCode=dwErrorCode;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//操作结果
VOID CDataBaseEngineSink::OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造变量
		OperateSuccess.lResultCode=dwErrorCode;
		lstrcpyn(OperateSuccess.szDescribeString,pszErrorString,CountArray(OperateSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lResultCode=dwErrorCode;
		lstrcpyn(OperateFailure.szDescribeString,pszErrorString,CountArray(OperateFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
	}

	return;
}


bool CDataBaseEngineSink::OnRequestGetExchangeHuaFeiList( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_GetExchangeHuaFei));
		if (wDataSize!=sizeof(DBR_GP_GetExchangeHuaFei)) return false;

		//请求处理
		DBR_GP_GetExchangeHuaFei * pNetInfo=(DBR_GP_GetExchangeHuaFei *)pData;
		//构造参数
		m_AccountsDBAide.ResetParameter();
		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetExchangeHuaFei"),true);

		//构造结构
		DBO_GP_BackExchangeHuaFei kDBInfo;

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				kDBInfo.kExchangeID.push_back(m_AccountsDBAide.GetValue_INT(TEXT("ExchangeID")));
				kDBInfo.kUseType.push_back(m_AccountsDBAide.GetValue_INT(TEXT("UseType"))); 
				kDBInfo.kUseNum.push_back(m_AccountsDBAide.GetValue_INT(TEXT("UseNum"))); 
				kDBInfo.kGetType.push_back(m_AccountsDBAide.GetValue_INT(TEXT("GetType")));  
				kDBInfo.kGetNum.push_back(m_AccountsDBAide.GetValue_INT(TEXT("GetNum")));  

				TCHAR		szGoodsName[LEN_NICKNAME];				// 物品名称
				m_AccountsDBAide.GetValue_String(TEXT("GoodsName"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kGoodsName.push_back(szGoodsName);

				m_AccountsDBAide.GetValue_String(TEXT("ExchangeDesc"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kExchangeDesc.push_back(szGoodsName);

				m_AccountsDBAide.GetValue_String(TEXT("ImgIcon"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kImgIcon.push_back(szGoodsName);

				kDBInfo.kFlag.push_back(m_AccountsDBAide.GetValue_INT(TEXT("Flag")));  

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			datastream kTempDataStream;
			kDBInfo.StreamValue(kTempDataStream,true);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHAGE_HUAFEI_BACK,dwContextID,kTempDataStream.data(),kTempDataStream.size());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestGetShopInfoList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_GetShopInfo));
		if (wDataSize!=sizeof(DBR_GP_GetShopInfo)) return false;

		//请求处理
		DBR_GP_GetShopInfo * pNetInfo=(DBR_GP_GetShopInfo *)pData;
		//构造参数
		m_AccountsDBAide.ResetParameter();
		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetShopInfo"),true);

		//构造结构
		DBO_GP_BackShopInfo kDBInfo;

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			while (m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				kDBInfo.kItemID.push_back(m_AccountsDBAide.GetValue_INT(TEXT("ItemID")));
				kDBInfo.kItemType.push_back(m_AccountsDBAide.GetValue_INT(TEXT("ItemType"))); 
				kDBInfo.kOrderID_IOS.push_back(m_AccountsDBAide.GetValue_INT(TEXT("OrderID_IOS"))); 
				kDBInfo.kOrderID_Android.push_back(m_AccountsDBAide.GetValue_INT(TEXT("OrderID_Android")));  
				kDBInfo.kPrice.push_back(m_AccountsDBAide.GetValue_INT(TEXT("Price")));  
				kDBInfo.kGoodsNum.push_back(m_AccountsDBAide.GetValue_INT(TEXT("GoodsNum")));  

				TCHAR		szGoodsName[LEN_NICKNAME];				// 物品名称
				m_AccountsDBAide.GetValue_String(TEXT("ItemTitle"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kItemTitle.push_back(szGoodsName);

				m_AccountsDBAide.GetValue_String(TEXT("ItemDesc"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kItemDesc.push_back(szGoodsName);

				m_AccountsDBAide.GetValue_String(TEXT("ItemIcon"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kItemIcon.push_back(szGoodsName);

				m_AccountsDBAide.GetValue_String(TEXT("ItemName"),szGoodsName,CountArray(szGoodsName));
				kDBInfo.kItemName.push_back(szGoodsName);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			datastream kTempDataStream;
			kDBInfo.StreamValue(kTempDataStream,true);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SHOPINFO_BACK,dwContextID,kTempDataStream.data(),kTempDataStream.size());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//查询推荐人信息
bool CDataBaseEngineSink::OnRequestQuerySpreaterNickName( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	try
	{
		CMD_GP_QuerySpreader * pUser = (CMD_GP_QuerySpreader*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pUser->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"),pUser->dwSpreaderID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySpreader"),true);

		CMD_GP_QuerySpreader_Reslut stResult;
		ZeroMemory(&stResult,sizeof(stResult));

		if(lResultCode == DB_SUCCESS)
		{
			stResult.byType = 0;
			stResult.dwSpreaderID = pUser->dwSpreaderID;
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),stResult.szNickName,sizeof(stResult.szNickName));
		}
		else
		{
			stResult.byType = 1;
			stResult.dwSpreaderID = pUser->dwSpreaderID;

			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(stResult.szReslut,CW2CT(DBVarValue.bstrVal),CountArray(stResult.szReslut));

		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_QUERY_SPREADER,dwContextID,&stResult,sizeof(stResult));
	}

	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

	return true;
}

//填写推荐人返回
bool CDataBaseEngineSink::OnRequestAddSpreater( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	try
	{
		CMD_GP_QuerySpreader * pUser = (CMD_GP_QuerySpreader*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pUser->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"),pUser->dwSpreaderID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行命令
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AddSpreader"),true);

		CMD_GP_AddSpreader_Result stResult;
		ZeroMemory(&stResult,sizeof(stResult));

		if(lResultCode == DB_SUCCESS)
		{
			stResult.byType = 0;
			stResult.dwSpreaderID = pUser->dwSpreaderID;
		}
		else
		{
			stResult.byType = 1;
			stResult.dwSpreaderID = 0;

			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(stResult.szReslut,CW2CT(DBVarValue.bstrVal),CountArray(stResult.szReslut));

		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_ADD_SPREADER,dwContextID,&stResult,sizeof(stResult));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
