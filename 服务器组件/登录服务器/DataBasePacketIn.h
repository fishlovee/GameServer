#ifndef DATA_BASE_PACKETIN_HEAD_FILE
#define DATA_BASE_PACKETIN_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//登录命令
#define	DBR_GP_LOGON_GAMEID			1									//I D 登录
#define	DBR_GP_LOGON_ACCOUNTS		2									//帐号登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号

//账号服务
#define DBR_GP_MODIFY_MACHINE		10									//修改机器
#define DBR_GP_MODIFY_LOGON_PASS	11									//修改密码
#define DBR_GP_MODIFY_INSURE_PASS	12									//修改密码
#define DBR_GP_MODIFY_UNDER_WRITE	13									//修改签名
#define DBR_GP_MODIFY_INDIVIDUAL	14									//修改资料
#define DBR_GP_MODIFY_ACCOUNTS		15									//修改帐号
#define DBR_GP_MODIFY_SPREADER		16									//修改推荐人
#define DBR_GP_QUERY_SPREADER		17									//查询推荐人昵称
#define DBR_GP_ADD_SPREADER			18									//添加推荐人
//头像命令
#define DBR_GP_MODIFY_SYSTEM_FACE	20									//修改头像
#define DBR_GP_MODIFY_CUSTOM_FACE	21									//修改头像

//银行命令
#define DBR_GP_USER_SAVE_SCORE		30									//存入游戏币
#define DBR_GP_USER_TAKE_SCORE		31									//提取游戏币
#define DBR_GP_USER_TRANSFER_SCORE	32									//转账游戏币

//查询命令
#define DBR_GP_QUERY_INDIVIDUAL		40									//查询资料
#define DBR_GP_QUERY_INSURE_INFO	41									//查询银行
#define DBR_GP_QUERY_USER_INFO	    42									//查询用户
#define DBR_GP_QUERY_ACCOUNTINFO	44									//查询资料
#define DBR_GP_USER_INGAME_SERVERID	45									//游戏状态


//系统命令
#define DBR_GP_LOAD_GAME_LIST		50									//加载列表
#define DBR_GP_ONLINE_COUNT_INFO	51									//在线信息

//比赛命令
#define DBR_GP_MATCH_SIGNUP			55									//比赛报名
#define DBR_GP_MATCH_UNSIGNUP		56									//取消报名
#define DBR_GP_MATCH_AWARD			57									//比赛奖励

//签到命令
#define DBR_GP_LOAD_CHECKIN_REWARD	60									//签到奖励
#define DBR_GP_CHECKIN_DONE			61									//执行签到
#define DBR_GP_CHECKIN_QUERY_INFO	62									//查询信息
#define DBR_GP_CHECKIN_AWARD		64									//查询信息

#define DBR_GP_LOAD_BASEENSURE		70									//加载低保

#define DBR_GP_LOAD_BEGINNER		80									//加载新手引导
#define DBR_GP_BEGINNER_QUERY_INFO	81									//查询新手引导
#define DBR_GP_BEGINNER_DONE		82									//领取新手引导

//低保命令
#define DBR_GP_BASEENSURE_TAKE		100									//领取低保

//赚金排行
#define DBR_GP_LOAD_ADDRANK_CONFIG	110									//更新
#define DBO_GP_ADDRANK_UP_AWARD		111									//是否领取排行奖励		
#define DBR_GP_GET_ADDRANK			112									//获得排行

//游戏记录
#define DBR_GP_GAME_RECORD_LIST		120									//游戏记录
#define DBR_GP_GAME_TOTAL_RECORD	121									//游戏回放

//兑换话费	
#define DBR_GP_GET_EXCHANGE_HUAFEI			130									//获得兑换话费列表

//商城	
#define DBR_GP_GET_SHOPINFO			140									//获得商城列表

//自定义字段查询
#define DBR_GP_PUBLIC_NOTIC			200									//自定义字段查询

//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GP_LogonGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_GP_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//游客登录
struct DBR_GP_LogonVisitor
{
	DWORD							dwClientIP;							//连接地址
	TCHAR							szComputerID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szNickName[LEN_ACCOUNTS];			//人名
	TCHAR							szPassWord[LEN_MD5];				//登录密码
	TCHAR							szPassWordBank[LEN_MD5];			//登录密码
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号
	WORD							wFaceID;							//注册头像
	BYTE							cbGender;							//用户性别
	LPVOID							pBindParameter;						//绑定参数
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//修改机器
struct DBR_GP_ModifyMachine
{
	BYTE							cbBind;								//绑定标志
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改密码
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct DBR_GP_ModifyAccounts
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szDesAccount[LEN_ACCOUNTS];			//用户帐号
};

//修改推荐人
struct DBR_GP_ModifySpreader
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szSpreader[LEN_ACCOUNTS];			//用户帐号
};

//修改签名
struct DBR_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//修改头像
struct DBR_GP_ModifySystemFace
{
	//用户信息
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct DBR_GP_ModifyCustomFace
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改资料
struct DBR_GP_ModifyIndividual
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//帐号资料
	BYTE							cbGender;							//用户性别
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//用户信息
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	
	TCHAR							szHeadHttp[LEN_USER_NOTE];			//头像HTTP
	TCHAR							szUserChannel[LEN_NICKNAME];		//渠道号
};

//查询用户游戏状态
struct DBR_GP_QueryUserInGameServerID
{
	DWORD							dwUserID;							//用户 I D
};

//查询资料
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

//查询个人信息
struct DBR_GP_QueryAccountInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

//存入游戏币
struct DBR_GP_UserSaveScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出游戏币
struct DBR_GP_UserTakeScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转账游戏币
struct DBR_GP_UserTransferScore
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	BYTE                            cbByNickName;                       //昵称赠送
	SCORE							lTransferScore;						//转账游戏币
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询银行
struct DBR_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

//查询用户
struct DBR_GP_QueryInsureUserInfo
{
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};

//用户资料
struct DBO_GP_UserTransferUserInfo
{
	DWORD							dwGameID;							//用户 I D
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
};

//在线信息
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//类型数目
	DWORD							dwOnLineCountSum;					//总在线数
	tagOnLineInfoKind				OnLineCountKind[MAX_KIND];			//类型在线
};

//查询签到
struct DBR_GP_CheckInQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//执行签到
struct DBR_GP_CheckInDone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

////////////////////////////////////////////////////////////////////////////////////////////
//新手引导查询
struct DBR_GP_BeginnerQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};
struct DBR_GP_BeginnerDone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

////////////////////////////////////////////////////////////////////////////////////////////

//低保参数
struct DBO_GP_BaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//低保结果
struct DBO_GP_BaseEnsureResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//领取低保
struct DBR_GP_TakeBaseEnsure
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//报名结果
struct DBO_GP_MatchSignupResult
{
	bool							bSignup;							//报名标识
	bool							bSuccessed;							//成功标识
	TCHAR							szDescribeString[128];				//描述信息
};


//加载奖励
struct DBR_GR_LoadMatchReward
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
};

//修改机器
struct DBR_GP_PublicNotic
{
	TCHAR							szKeyName[LEN_NICKNAME];			//关键字
};

/////////////////////////////////////
//赚金榜
struct DBR_GP_GetAddBankConfig
{
	INT								iIdex;
};
//获得排行榜
struct DBR_GP_GetAddBank
{
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	INT								iIdex;
};


//比赛报名
struct DBR_GP_MatchSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛报名
struct DBR_GP_MatchUnSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//游戏记录
struct DBR_GP_GameRecordList
{
	DWORD							dwUserID;							
};


//游戏回放
struct DBR_GP_GetGameTotalRecord
{
	DWORD							dwUserID;		
	DWORD							dwRecordID;							
};

//话费兑换
struct DBR_GP_GetExchangeHuaFei
{
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
};

//话费兑换
struct DBR_GP_GetShopInfo
{
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
};

#endif