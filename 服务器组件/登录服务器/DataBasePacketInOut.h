#ifndef DATA_BASE_PACKETOUT_HEAD_FILE
#define DATA_BASE_PACKETOUT_HEAD_FILE

#include "..\..\全局定义\datastream.h"

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////////////
//输出信息

//登录结果
#define DBO_GP_LOGON_SUCCESS		100									//登录成功
#define DBO_GP_LOGON_FAILURE		101									//登录失败
#define DBR_GP_VALIDATE_MBCARD		102									//登录失败

//操作结果
#define DBO_GP_USER_FACE_INFO		110									//用户头像
#define DBO_GP_USER_INDIVIDUAL		111									//用户资料
#define DBO_GP_USER_ACCOUNTINFO		112									//用户信息

//银行命令
#define DBO_GP_USER_INSURE_INFO		120									//银行资料
#define DBO_GP_USER_INSURE_SUCCESS	121									//银行成功
#define DBO_GP_USER_INSURE_FAILURE	122									//银行失败
#define DBO_GP_USER_INSURE_USER_INFO   123								//用户资料
#define DBO_GP_USER_INGAME_SERVER_ID   124								//用户游戏状态

//列表结果
#define DBO_GP_GAME_TYPE_ITEM		130									//种类信息
#define DBO_GP_GAME_KIND_ITEM		131									//类型信息
#define DBO_GP_GAME_NODE_ITEM		132									//节点信息
#define DBO_GP_GAME_PAGE_ITEM		133									//定制信息
#define DBO_GP_GAME_LIST_RESULT		134									//加载结果

//签到命令
#define DBO_GP_CHECKIN_REWARD		160									//签到奖励
#define DBO_GP_CHECKIN_INFO			161									//签到信息
#define DBO_GP_CHECKIN_RESULT		162									//签到结果


#define DBO_GP_BASEENSURE_PARAMETER	180									//低保参数
#define DBO_GP_BASEENSURE_RESULT	181									//领取结果
//报名结果
#define DBO_GP_MATCH_SIGNUP_RESULT	190									//报名结果	
#define DBO_GP_MATCH_AWARD			191									//比赛奖励

//服务结果
#define DBO_GP_PUBLIC_NOTICE		200									//操作成功

//新手活动命令
#define DBO_GP_BEGINNER_CONFIG		210									//新手活动奖励
#define DBO_GP_BEGINNER_INFO		211									//新手活动信息
#define DBO_GP_BEGINNER_RESULT		212									//新手活动结果

//设置推荐人结果
#define DBO_GP_SPREADER_RESOULT		220									//设置推荐人结果

//赚金榜
#define DBO_GP_ADDRANK_AWARD_CONFIG	240									//配置							
#define DBO_GP_ADDRANK_RANK_BACK	241									//排行结果						

//游戏记录
#define DBO_GP_GAME_RECORD_LIST		250									//记录列表							
#define DBO_GP_GAME_RECORD_TOTAL	251									//排行结果	

//兑换话费
#define DBO_GP_EXCHAGE_HUAFEI_BACK	260									//兑换话费列表

//商城信息
#define DBO_GP_SHOPINFO_BACK	270									//商城列表		

//服务结果
#define DBO_GP_OPERATE_SUCCESS		500									//操作成功
#define DBO_GP_OPERATE_FAILURE		501									//操作失败



//登录成功
struct DBO_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定索引
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	DWORD							dwSpreaderID;						//推广ID
	BYTE							cbInsureEnabled;					//银行使能
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//用户成绩
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//用户银行

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间

	//报名信息
	WORD							wSignupCount;						//报名数量
	tagSignupMatchInfo				SignupMatchInfo[32];				//报名列表

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBO_GP_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBR_GP_ValidateMBCard
{
	UINT							uMBCardID;						//机器序列
};

//头像信息
struct DBO_GP_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定索引
};

//个人资料
struct DBO_GP_UserIndividual
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szHeadHttp[LEN_USER_NOTE];			//头像
	TCHAR							szLogonIP[LEN_NICKNAME];			//IP
	TCHAR							szUserChannel[LEN_NICKNAME];		//渠道号
};


//个人信息
struct DBO_GP_UserAccountInfo
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定索引
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	DWORD							dwSpreaderID;						//推广ID
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
	TCHAR							szLogonIp[LEN_ACCOUNTS];			//登录IP

	//用户成绩
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//用户银行

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间
};
//银行资料
struct DBO_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转账条件
};

//银行成功
struct DBO_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lDestScore;					//游戏币变化
	SCORE							lDestInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct  DBO_GP_UserInsureFailure
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//设置推荐人结果
struct DBO_GP_SpreaderResoult
{
	LONG							lResultCode;						//操作代码
	SCORE							lScore;							//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//操作失败
struct DBO_GP_OperateFailure
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct DBO_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////

//游戏类型
struct DBO_GP_GameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct DBO_GP_GameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct DBO_GP_GameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct DBO_GP_GamePage
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wPageID;							//定制索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//加载结果
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//成功标志
};

//签到奖励
struct DBO_GP_CheckInReward
{
	SCORE							lRewardGold[LEN_SIGIN];				//奖励金额
	BYTE							lRewardType[LEN_SIGIN];				//奖励类型 1金币 2道具
	BYTE							lRewardDay[LEN_SIGIN];				//奖励天数 
};

//签到信息
struct DBO_GP_CheckInInfo
{
	WORD							wSeriesDate;						//连续日期
	WORD							wAwardDate;							//物品日期
	bool							bTodayChecked;						//签到标识
};

//签到结果
struct DBO_GP_CheckInResult
{
	bool							bType;								//是否是达到天数领取物品
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前分数
	TCHAR							szNotifyContent[128];				//提示内容
};

//新手引导信息
struct DBO_GP_BeginnerInfo
{
	WORD							wSeriesDate;						//连续日期
	bool							bTodayChecked;						//签到标识
	bool							bLastCheckIned;						//签到标识
};

//查询结果
struct DBO_GP_UserInGameServerID
{
	DWORD							LockKindID;			
	DWORD							LockServerID;							
};

//查询结果
struct DBO_GP_PublicNotice
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[512];				//成功消息
};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//新手活动配置
struct DBO_GP_BeginnerCofig
{
	SCORE							lRewardGold[LEN_BEGINNER];				//奖励金额
	BYTE							lRewardType[LEN_BEGINNER];				//奖励类型 1金币 2道具
};
//新手活动结果
struct DBO_GP_BeginnerResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lAwardCout;							//奖励数量
	SCORE							lAwardType;							//奖励类型
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//赚金榜
struct DBO_GP_AddRankAwardConfig
{
	INT								iIdex;
	INT								kRewardGold[LEN_ADDRANK];
	INT								kRewardType[LEN_ADDRANK];
};

struct DBO_GP_BackAddBank
{
	int iRankIdex;
	std::vector<std::string>		kNickName;
	std::vector<int>				kUserID;
	std::vector<int>				kFaceID;
	std::vector<int>				kCustomID;
	std::vector<int>				kUserPoint;

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(iRankIdex);
		Stream_VALUE(kNickName);
		Stream_VALUE(kUserID);
		Stream_VALUE(kFaceID);
		Stream_VALUE(kCustomID);
		Stream_VALUE(kUserPoint);
	}
};



//兑换话费
struct DBO_GP_BackExchangeHuaFei
{
	std::vector<int>				kExchangeID;            //兑换id
	std::vector<int>				kUseType;			   //兑换道具类型
	std::vector<int>				kUseNum;				   //兑换道具个数
	std::vector<int>				kGetType;			   //兑换商品类型
	std::vector<int>				kGetNum;				   //兑换商品个数
	std::vector<std::string>		kGoodsName;			   //兑换商品名称
	std::vector<std::string>		kExchangeDesc;		   //兑换描述
	std::vector<std::string>		kImgIcon;			   //图标名称
	std::vector<int>				kFlag;				   //标记

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(kExchangeID);
		Stream_VALUE(kUseType);
		Stream_VALUE(kUseNum);
		Stream_VALUE(kGetType);
		Stream_VALUE(kGetNum);
		Stream_VALUE(kGoodsName);
		Stream_VALUE(kExchangeDesc);
		Stream_VALUE(kImgIcon);
		Stream_VALUE(kFlag);
	}
};


//商城列表
struct DBO_GP_BackShopInfo
{
	std::vector<int>				kItemID;					 //商品id
	std::vector<int>				kItemType;				 //商品类型
	std::vector<int>				kOrderID_IOS;				 //商品订单号 苹果
	std::vector<int>				kOrderID_Android;			 //商品订单号 安卓
	std::vector<int>				kPrice;					 //商品价格
	std::vector<int>				kGoodsNum;				 //商品数量
	std::vector<std::string>		kItemTitle;				 //标题
	std::vector<std::string>		kItemDesc;				 //描述
	std::vector<std::string>		kItemIcon;				 //图标名称
	std::vector<std::string>		kItemName;				 //商品名称

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(kItemID);
		Stream_VALUE(kItemType);
		Stream_VALUE(kOrderID_IOS);
		Stream_VALUE(kOrderID_Android);
		Stream_VALUE(kPrice);
		Stream_VALUE(kGoodsNum);
		Stream_VALUE(kItemTitle);
		Stream_VALUE(kItemDesc);
		Stream_VALUE(kItemIcon);
		Stream_VALUE(kItemName);
	}
};

//排行信息
struct tagDBMatchAwardkInfo
{
	WORD							MatchRank;							//比赛名次
	SCORE						RewardGold;					//奖励金币
	DWORD							RewardMedal;						//奖励元宝
	DWORD							RewardExperience;					//奖励经验
	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(MatchRank);
		Stream_VALUE(RewardGold);
		Stream_VALUE(RewardMedal);
		Stream_VALUE(RewardExperience);
	}
};

//比赛奖励
struct DBO_GR_MatchAwardList
{
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
	std::vector<tagDBMatchAwardkInfo> kAwards;

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(dwMatchID);
		Stream_VALUE(dwMatchNO);
		StructVecotrMember(tagDBMatchAwardkInfo,kAwards);
	}
};
#pragma pack()
#endif