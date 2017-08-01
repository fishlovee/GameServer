#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//用户事件
#define	DBR_GR_LOGON_USERID			100									//I D 登录
#define	DBR_GR_LOGON_MOBILE			101									//手机登录
#define	DBR_GR_LOGON_ACCOUNTS		102									//帐号登录

//系统事件
#define DBR_GR_WRITE_GAME_SCORE		200									//游戏积分
#define DBR_GR_LEAVE_GAME_SERVER	201									//离开房间
#define DBR_GR_GAME_SCORE_RECORD	202									//积分记录
#define DBR_GR_MANAGE_USER_RIGHT	203									//权限管理
#define DBR_GR_LOAD_SYSTEM_MESSAGE	204									//系统消息
#define DBR_GR_LOAD_SENSITIVE_WORDS	205									//敏感词
#define DBR_GR_PRIVATE_GAME_RECORD	206									//敏感词

//配置事件
#define DBR_GR_LOAD_PARAMETER		300									//加载参数
#define DBR_GR_LOAD_GAME_COLUMN		301									//加载列表
#define DBR_GR_LOAD_ANDROID_USER	302									//加载机器
#define DBR_GR_LOAD_GAME_PROPERTY	303									//加载道具

//银行事件
#define DBR_GR_USER_SAVE_SCORE		400									//存入游戏币
#define DBR_GR_USER_TAKE_SCORE		401									//提取游戏币
#define DBR_GR_USER_TRANSFER_SCORE	402									//转账游戏币
#define DBR_GR_QUERY_INSURE_INFO	403									//查询银行
#define DBR_GR_QUERY_TRANSFER_USER_INFO	    404							//查询用户

//游戏事件
#define DBR_GR_PROPERTY_REQUEST		500									//购买礼物
#define DBR_GR_GAME_FRAME_REQUEST	502									//游戏请求

//比赛事件
#define DBR_GR_MATCH_SIGNUP			600									//比赛费用
#define DBR_GR_MATCH_UNSIGNUP		601									//退出比赛
#define DBR_GR_MATCH_START			602									//比赛开始
#define DBR_GR_MATCH_OVER			603									//比赛结束	
#define DBR_GR_MATCH_REWARD			604									//比赛奖励
#define DBR_GR_MATCH_ENTER_FOR		605									//报名进入
#define DBR_GR_MATCH_ELIMINATE      606									//比赛淘汰

//游戏事件
#define DBR_GR_BEGINNER_GAME_TIME	700		

//私人场事件
#define DBR_GR_PRIVATE_INFO					800				//私人场信息
#define DBR_GR_CREATE_PRIVAT				801				//创建私人场
#define DBR_GR_CREATE_PRIVAT_COST		802				//返还私人场

//////////////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_LOGON_SUCCESS		100									//登录成功
#define DBO_GR_LOGON_FAILURE		101									//登录失败

//配置事件
#define DBO_GR_GAME_PARAMETER		200									//配置信息
#define DBO_GR_GAME_COLUMN_INFO		201									//列表信息
#define DBR_GR_GAME_ANDROID_INFO	202									//机器信息
#define DBO_GR_GAME_PROPERTY_INFO	203									//道具信息

//银行命令
#define DBO_GR_USER_INSURE_INFO		300									//银行资料
#define DBO_GR_USER_INSURE_SUCCESS	301									//银行成功
#define DBO_GR_USER_INSURE_FAILURE	302									//银行失败
#define DBO_GR_USER_INSURE_USER_INFO   303								//用户资料

//游戏事件
#define DBO_GR_PROPERTY_SUCCESS		400									//道具成功
#define DBO_GR_PROPERTY_FAILURE		401									//道具失败
#define DBO_GR_GAME_FRAME_RESULT	402									//游戏结果

//比赛事件
#define DBO_GR_MATCH_EVENT_START	500									//比赛标识
#define DBO_GR_MATCH_SIGNUP_RESULT	500									//报名结果
#define DBO_GR_MATCH_UNSIGNUP_RESULT 501								//退赛结果
#define DBO_GR_MATCH_RANK_LIST		502									//比赛排行
#define DBO_GR_MATCH_REWARD_RESULT  503									//奖励结果
#define DBO_GR_MATCH_EVENT_END		599									//比赛标识

//系统事件
#define DBO_GR_SYSTEM_MESSAGE_RESULT	600								//消息结果
#define DBO_GR_SENSITIVE_WORDS		    601								//敏感词

//私人场事件
#define DBO_GR_PRIVATE_EVENT_START	700									//私人场标识
#define DBO_GR_PRIVATE_INFO			701									//私人场信息	
#define DBO_GR_CREATE_PRIVATE			702									//私人场信息	
#define DBO_GR_PRIVATE_EVENT_END	799									//私人场标识

//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GR_LogonUserID
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛编号
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//ID 登录
struct DBR_GR_LogonMobile
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//附加信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛编号
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//帐号登录
struct DBR_GR_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//游戏积分
struct DBR_GR_WriteGameScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//进出索引

	//成绩变量
	tagVariationInfo				VariationInfo;						//提取信息

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//新手引导记录
struct DBR_GR_Beginner_InGame
{
	//用户信息
	DWORD							dwUserID;							//用户标识
};

//离开房间
struct DBR_GR_LeaveGameServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwInoutIndex;						//记录索引
	DWORD							dwLeaveReason;						//离开原因
	DWORD							dwOnLineTimeCount;					//在线时长

	//成绩变量
	tagVariationInfo				RecordInfo;							//记录信息
	tagVariationInfo				VariationInfo;						//提取信息

	//系统信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

struct DBR_GR_GameScoreRecord
{
	DBR_GR_GameScoreRecord()
	{
		wTableID = 0;
		wUserCount = 0;
		wAndroidCount = 0;
		lWasteCount = 0;
		lRevenueCount = 0;
		dwUserMemal = 0;
		dwPlayTimeCount = 0;
		wRecordCount = 0;
	}
	//桌子信息
	WORD							wTableID;							//桌子号码
	WORD							wUserCount;							//用户数目
	WORD							wAndroidCount;						//机器数目

	DWORD							dwPrivateID;						//私人房标识
	
	//损耗税收
	SCORE							lWasteCount;						//损耗数目
	SCORE							lRevenueCount;						//税收数目

	//统计信息
	DWORD							dwUserMemal;						//奖牌数目
	DWORD							dwPlayTimeCount;					//游戏时间

	//时间信息
	SYSTEMTIME						SystemTimeStart;					//开始时间
	SYSTEMTIME						SystemTimeConclude;					//结束时间

	//积分记录
	WORD							wRecordCount;						//记录数目
	datastream						dataGameDefine;
	std::vector<tagGameScoreRecord>	GameScoreRecord;			//游戏记录
	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(wTableID);
		Stream_VALUE(wUserCount);
		Stream_VALUE(wAndroidCount);
		Stream_VALUE(dwPrivateID);
		Stream_VALUE(lWasteCount);
		Stream_VALUE(lRevenueCount);
		Stream_VALUE(dwUserMemal);
		Stream_VALUE(dataGameDefine);
		Stream_VALUE_SYSTEMTIME(SystemTimeStart);
		Stream_VALUE_SYSTEMTIME(SystemTimeConclude);
		StructVecotrMember(tagGameScoreRecord,GameScoreRecord);
	}
};

//私人场记录
struct BR_GR_PrivateRandTotalRecord
{
	DWORD						dwPrivateID;	//私人房标识
	int							iRoomNum;
	std::vector<int>			kUserID;
	std::vector<std::string>	kNickName;
	std::vector<int>			kToatlScore;
	SYSTEMTIME					kPlayTime;

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(dwPrivateID);
		Stream_VALUE(iRoomNum);
		Stream_VALUE(kUserID);
		Stream_VALUE(kNickName);
		Stream_VALUE(kToatlScore);
		Stream_VALUE_SYSTEMTIME(kPlayTime);
	}
};
//存入游戏币
struct DBR_GR_UserSaveScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出游戏币
struct DBR_GR_UserTakeScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转账游戏币
struct DBR_GR_UserTransferScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	BYTE                            cbByNickName;                       //昵称赠送
	SCORE							lTransferScore;						//转账游戏币
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询银行
struct DBR_GR_QueryInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

//查询用户
struct DBR_GR_QueryTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
	DWORD							dwUserID;							//用户 I D
};

//道具请求
struct DBR_GR_PropertyRequest
{
	//购买信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	DWORD							dwSourceUserID;						//购买对象
	DWORD							dwTargetUserID;						//使用对象

	//消费模式
	BYTE                            cbRequestArea;			            //使用范围 
	BYTE							cbConsumeScore;						//积分消费
	SCORE							lFrozenedScore;						//冻结积分

	//用户权限
	DWORD                           dwUserRight;						//会员权限

	//系统信息
	WORD							wTableID;							//桌子号码
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GR_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团 I D
	DWORD							dwCustomID;							//自定头像
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	
	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力

	//用户权限
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限

	//索引变量
	DWORD							dwInoutIndex;						//记录索引
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识

	//手机定义
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//辅助信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szDescribeString[128];				//错误消息
};

//登录失败
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//配置信息
struct DBO_GR_GameParameter
{
	//汇率信息
	WORD							wMedalRate;							//奖牌汇率
	WORD							wRevenueRate;						//银行税收

	//版本信息
	DWORD							dwClientVersion;					//客户版本
	DWORD							dwServerVersion;					//服务版本
};

//列表信息
struct DBO_GR_GameColumnInfo
{
	LONG							lResultCode;						//结果代码
	BYTE							cbColumnCount;						//列表数目
	tagColumnItem					ColumnItemInfo[MAX_COLUMN];			//列表信息
};

//机器信息
struct DBO_GR_GameAndroidInfo
{
	LONG							lResultCode;						//结果代码
	WORD							wAndroidCount;						//用户数目
	tagAndroidParameter				AndroidParameter[MAX_ANDROID];		//机器信息
};

//道具信息
struct DBO_GR_GamePropertyInfo
{
	LONG							lResultCode;						//结果代码
	BYTE							cbPropertyCount;					//道具数目
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//道具信息
};

//银行资料
struct DBO_GR_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转账条件
};

//银行成功
struct DBO_GR_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lFrozenedScore;						//冻结积分
	SCORE							lVariationScore;					//游戏币变化
	SCORE							lVariationInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct DBO_GR_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //游戏动作
	LONG							lResultCode;						//操作代码
	SCORE							lFrozenedScore;						//冻结积分
	TCHAR							szDescribeString[128];				//描述消息
};

//用户资料
struct DBO_GR_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwGameID;							//用户 I D
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
};

//道具成功
struct DBO_GR_S_PropertySuccess
{
	//道具信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	DWORD							dwSourceUserID;						//购买对象
	DWORD							dwTargetUserID;						//使用对象

	//消费模式
	BYTE                            cbRequestArea;						//请求范围
	BYTE							cbConsumeScore;						//积分消费
	SCORE							lFrozenedScore;						//冻结积分

	//会员权限
	DWORD                           dwUserRight;						//会员权限

	//结果信息
	SCORE							lConsumeGold;						//消费游戏币
	LONG							lSendLoveLiness;					//赠送魅力
	LONG							lRecvLoveLiness;					//接受魅力

	//会员信息
	BYTE							cbMemberOrder;						//会员等级
};

//道具失败
struct DBO_GR_PropertyFailure
{
	BYTE                            cbRequestArea;						//请求范围
	LONG							lResultCode;						//操作代码
	SCORE							lFrozenedScore;						//冻结积分
	TCHAR							szDescribeString[128];				//描述消息
};

//报名结果
struct DBO_GR_MatchSingupResult
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrGold;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szDescribeString[128];				//描述消息
};

//排行信息
struct tagMatchRankInfo
{
	WORD							wRankID;							//比赛名次
	DWORD							dwUserID;							//用户标识
	SCORE							lMatchScore;						//比赛分数
	SCORE							lRewardGold;						//奖励金币
	DWORD							dwRewardIngot;						//奖励元宝
	DWORD							dwRewardExperience;					//奖励经验
};

//比赛排行
struct DBO_GR_MatchRankList
{
	WORD							wUserCount;							//用户数目
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
	tagMatchRankInfo				MatchRankInfo[128];					//奖励信息
};

//权限管理
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限
	bool							bGameRight;							//游戏权限
};

//权限管理
struct DBR_GR_ManageMatchRight
{	
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限	
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//比赛报名
struct DBR_GR_MatchSignup
{
	//报名费用
	DWORD							dwUserID;							//用户ID
	SCORE							lMatchFee;							//报名费用

	//系统信息
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwMatchID;							//比赛ID
	DWORD							dwMatchNO;							//比赛场次
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛开始
struct DBR_GR_MatchStart
{
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
};

//比赛结束
struct DBR_GR_MatchOver
{
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchEndTime;						//结束时间
};

//比赛淘汰
struct DBR_GR_MatchEliminate
{
	DWORD							dwUserID;							//用户标识
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
};


//比赛奖励
struct DBR_GR_MatchReward
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lRewardGold;						//奖励金币
	DWORD							dwRewardIngot;						//奖励元宝
	DWORD							dwRewardExperience;					//用户经验	
	DWORD							dwClientAddr;						//连接地址
};

//奖励结果
struct DBR_GR_MatchRewardResult
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrGold;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	DWORD							dwCurrExperience;					//当前经验
};



//系统消息
struct DBR_GR_SystemMessage
{
	BYTE                            cbMessageID;                        //消息ID
	BYTE							cbMessageType;						//消息类型
	BYTE                            cbAllRoom;                          //全体房间
	DWORD							dwTimeRate;						    //时间频率
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};
//////////////////////////////////////////////////////////////////////////////////

//私人场信息
struct DBR_GR_Private_Info
{
	WORD							wKindID;
};


//创建私人场
struct DBR_GR_Create_Private
{
	WORD							wKindID;
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
	BYTE							cbRoomType;
	DWORD							dwAgaginTable;
	BYTE							bPlayCoutIdex;							//游戏局数
	BYTE							bGameTypeIdex;							//游戏类型
	DWORD							bGameRuleIdex;							//游戏规则
};

//创建私人场
struct DBR_GR_Create_Private_Cost
{
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
};


//私人场信息
struct DBO_GR_Private_Info
{
	WORD							wKindID;
	SCORE							lCostGold;
	BYTE							bPlayCout[4];							//玩家局数
	SCORE							lPlayCost[4];							//消耗点数
};

//比赛排行
struct DBO_GR_CreatePrivateResoult
{
	BOOL							bSucess;
	BYTE							cbRoomType;
	TCHAR							szDescribeString[128];					//错误信息
	SCORE							lCurSocre;								//当前剩余
	DWORD							dwAgaginTable;							//重新加入桌子
	BYTE							bPlayCoutIdex;							//玩家局数
	BYTE							bGameTypeIdex;							//游戏类型
	DWORD							bGameRuleIdex;							//游戏规则
};

#endif