#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#include "..\..\全局定义\datastream.h"


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//登录命令

#define MDM_GP_LOGON				1									//广场登录

//登录模式
#define SUB_GP_LOGON_GAMEID			1									//I D 登录
#define SUB_GP_LOGON_ACCOUNTS		2									//帐号登录
#define SUB_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define SUB_GP_LOGON_VISITOR		5									//游客登录

//登录结果
#define SUB_GP_LOGON_SUCCESS		100									//登录成功
#define SUB_GP_LOGON_FAILURE		101									//登录失败
#define SUB_GP_LOGON_FINISH			102									//登录完成
#define SUB_GP_VALIDATE_MBCARD      103                                 //登录失败
#define SUB_GP_MATCH_SIGNUPINFO		106									//报名信息

//升级提示
#define SUB_GP_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////
//

#define MB_VALIDATE_FLAGS           0x01                                //效验密保
#define LOW_VER_VALIDATE_FLAGS      0x02                                //效验低版本

//I D 登录
struct CMD_GP_LogonGameID
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//登录信息
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							cbValidateFlags;			        //校验标识
};

//帐号登录
struct CMD_GP_LogonAccounts
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	BYTE							cbValidateFlags;			        //校验标识
};

//游客登录
struct CMD_GP_VisitorLogon
{
	WORD								wFaceID;						//头像标识
	BYTE								cbGender;						//用户性别
	DWORD								dwPlazaVersion;					//广场版本
	BYTE								cbValidateFlags;			    //校验标识
	TCHAR								szNickName[LEN_NICKNAME];		//用户昵称
	TCHAR								szSpreader[LEN_NICKNAME];		//推广人名
	TCHAR								szPassWord[LEN_PASSWORD];		//登录密码
	TCHAR								szPassWordBank[LEN_PASSWORD];	//登录密码
};

//注册帐号
struct CMD_GP_RegisterAccounts
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	BYTE							cbValidateFlags;			        //校验标识
};

//登录成功
struct CMD_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定标识
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	DWORD							dwSpreaderID;						//推广ID
	BYTE							cbInsureEnabled;					//银行开通
	
	//用户成绩
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//用户银行

	//用户信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//配置信息
	BYTE                            cbShowServerStatus;                 //显示服务器状态
};

//登录失败
struct CMD_GP_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登陆完成
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//中断时间
	WORD							wOnLineCountTime;					//更新时间
};

//登录失败
struct CMD_GP_ValidateMBCard
{
	UINT								uMBCardID;						//机器序列
};

//升级提示
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};

//////////////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_LogonSuccess

#define DTP_GP_GROUP_INFO			1									//社团信息
#define DTP_GP_MEMBER_INFO			2									//会员信息
#define	DTP_GP_UNDER_WRITE			3									//个性签名
#define DTP_GP_STATION_URL			4									//主页信息

//社团信息
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//社团索引
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
};

//会员信息
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间
};

//////////////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_GP_SERVER_LIST			2									//列表信息

//获取命令
#define SUB_GP_GET_LIST				1									//获取列表
#define SUB_GP_GET_SERVER			2									//获取房间
#define SUB_GP_GET_ONLINE			3									//获取在线
#define SUB_GP_GET_COLLECTION		4									//获取收藏

//列表信息
#define SUB_GP_LIST_TYPE			100									//类型列表
#define SUB_GP_LIST_KIND			101									//种类列表
#define SUB_GP_LIST_NODE			102									//节点列表
#define SUB_GP_LIST_PAGE			103									//定制列表
#define SUB_GP_LIST_SERVER			104									//房间列表
#define SUB_GP_LIST_MATCH			105									//比赛列表
#define SUB_GP_VIDEO_OPTION			106									//视频配置

//完成信息
#define SUB_GP_LIST_FINISH			200									//发送完成
#define SUB_GP_SERVER_FINISH		201									//房间完成

//在线信息
#define SUB_GR_KINE_ONLINE			300									//类型在线
#define SUB_GR_SERVER_ONLINE		301									//房间在线

//////////////////////////////////////////////////////////////////////////////////

//获取在线
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//房间数目
	WORD							wOnLineServerID[MAX_SERVER];		//房间标识
};

//类型在线
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//类型数目
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//类型在线
};

//房间在线
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//房间数目
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//房间在线
};

//////////////////////////////////////////////////////////////////////////////////
//服务命令

#define MDM_GP_USER_SERVICE			3									//用户服务

//账号服务
#define SUB_GP_MODIFY_MACHINE		100									//修改机器
#define SUB_GP_MODIFY_LOGON_PASS	101									//修改密码
#define SUB_GP_MODIFY_INSURE_PASS	102									//修改密码
#define SUB_GP_MODIFY_UNDER_WRITE	103									//修改签名
#define SUB_GP_MODIFY_ACCOUNTS		104									//修改帐号
#define SUB_GP_MODIFY_SPREADER		105									//修改推荐人

#define SUB_GP_QUERY_SPREADER		110									//查询推荐人昵称
#define SUB_GP_ADD_SPREADER			111									//添加推荐人
//查询推荐人昵称 //添加推荐人
struct CMD_GP_QuerySpreader
{
	DWORD							dwUserID;
	DWORD							dwSpreaderID;
};
//推荐人昵称返回
struct CMD_GP_QuerySpreader_Reslut		
{
	BYTE							byType;								//为零表示成功
	DWORD							dwSpreaderID;
	TCHAR							szNickName[LEN_ACCOUNTS];
	TCHAR							szReslut[128];						//失败标示
};
//添加推荐人返回
struct CMD_GP_AddSpreader_Result
{
	BYTE							byType;								//为零表示成功
	DWORD							dwSpreaderID;						//推广人ID
	TCHAR							szReslut[128];						//失败标示
};
//修改头像
#define SUB_GP_USER_FACE_INFO		120									//头像信息
#define SUB_GP_SYSTEM_FACE_INFO		121									//系统头像
#define SUB_GP_CUSTOM_FACE_INFO		122									//自定头像

//比赛服务
#define SUB_GP_MATCH_SIGNUP			200									//比赛报名
#define SUB_GP_MATCH_UNSIGNUP		201									//取消报名
#define SUB_GP_MATCH_SIGNUP_RESULT	202									//报名结果
#define SUB_GP_MATCH_AWARD_LIST		203									//比赛奖励

//签到服务
#define SUB_GP_CHECKIN_QUERY		220									//查询签到
#define SUB_GP_CHECKIN_INFO			221									//签到信息
#define SUB_GP_CHECKIN_DONE			222									//执行签到
#define SUB_GP_CHECKIN_RESULT		223									//签到结果
#define SUB_GP_CHECKIN_AWARD		224									//签到奖励

//新手引导
#define SUB_GP_BEGINNER_QUERY		240									//新手引导签到
#define SUB_GP_BEGINNER_INFO		241									//新手引导信息
#define SUB_GP_BEGINNER_DONE		242									//新手引导执行
#define SUB_GP_BEGINNER_RESULT		243									//新手引导结果

//低保服务
#define SUB_GP_BASEENSURE_LOAD			260								//加载低保
#define SUB_GP_BASEENSURE_TAKE			261								//领取低保
#define SUB_GP_BASEENSURE_PARAMETER		262								//低保参数
#define SUB_GP_BASEENSURE_RESULT		263								//低保结果

//个人资料
#define SUB_GP_USER_INDIVIDUAL		301									//个人资料
#define	SUB_GP_QUERY_INDIVIDUAL		302									//查询信息
#define SUB_GP_MODIFY_INDIVIDUAL	303									//修改资料
#define SUB_GP_QUERY_ACCOUNTINFO	304									//个人信息
#define SUB_GP_QUERY_INGAME_SEVERID	305									//游戏状态

//银行服务
#define SUB_GP_USER_SAVE_SCORE		400									//存款操作
#define SUB_GP_USER_TAKE_SCORE		401									//取款操作
#define SUB_GP_USER_TRANSFER_SCORE	402									//转账操作
#define SUB_GP_USER_INSURE_INFO		403									//银行资料
#define SUB_GP_QUERY_INSURE_INFO	404									//查询银行
#define SUB_GP_USER_INSURE_SUCCESS	405									//银行成功
#define SUB_GP_USER_INSURE_FAILURE	406									//银行失败
#define SUB_GP_QUERY_USER_INFO_REQUEST	407								//查询用户
#define SUB_GP_QUERY_USER_INFO_RESULT	408								//用户信息

//自定义字段查询 公告
#define SUB_GP_QUERY_PUBLIC_NOTICE		500									//自定义字段查询
#define	SUB_GP_PUBLIC_NOTICE			501

//设置推荐人结果
#define SUB_GP_SPREADER_RESOULT			520									//设置推荐人结果

//赚金排行榜
#define SUB_GP_ADDRANK_GET_AWARD_INFO	540									//获得奖励信息
#define SUB_GP_ADDRANK_BACK_AWARD_INFO	541									//返回奖励信息
#define SUB_GP_ADDRANK_GET_RANK			542									//获得排行
#define SUB_GP_ADDRANK_BACK_RANK		543									//返回排行

//游戏记录
#define SUB_GP_GAME_RECORD_LIST			550			
#define SUB_GP_GAME_RECORD_TOTAL		551									


//操作结果
#define SUB_GP_OPERATE_SUCCESS			900									//操作成功
#define SUB_GP_OPERATE_FAILURE			901									//操作失败

//话费兑换
#define  SUB_GP_EXCHANGEHUAFEI_GET_LIST_INFO    1000                    //获取兑换话费列表
#define  SUB_GP_EXCHANGEHUAFEI_BACK				1001					//兑换话费列表返回

//商城数据
#define  SUB_GP_SHOPINFO_GET_LIST_INFO		1100                     //获取商城列表
#define  SUB_GP_SHOPINFO_BACK				1101						//商城列表返回

//游戏记录
struct CMD_GP_GetGameRecord_List
{
	DWORD							dwUserID;							
};

//游戏记录
struct CMD_GP_GetGameTotalRecord
{
	DWORD							dwUserID;			
	DWORD							dwRecordID;						
};



struct CMD_GP_BackAddBankAwardInfo
{
	INT							kRewardGold[3][LEN_ADDRANK];				
	INT							kRewardType[3][LEN_ADDRANK];				
};

struct CMD_GP_GetAddBank
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	INT								iRankIdex;
};


struct CMD_GP_BackAddBank
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

//话费兑换

struct CMD_GP_GetExchangeHuaFei
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

struct CMD_GP_BackExchangeHuaFei
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

// 商城列表
struct CMD_GP_GetShopInfo
{
	DWORD						dwUserID;						//用户 I D
	char							szPassword[LEN_PASSWORD];			//用户密码
};

struct CMD_GP_BackShopInfo
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

//////////////////////////////////////////////////////////////////////////////////
//比赛报名
struct CMD_GP_MatchSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取消报名
struct CMD_GP_MatchUnSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//报名结果
struct CMD_GP_MatchSignupResult
{
	bool							bSignup;							//报名标识
	bool							bSuccessed;							//成功标识
	TCHAR							szDescribeString[128];				//描述信息
};


//报名结果
struct CMD_GP_MatchGetAward
{
	DWORD							dwUserID;	
	DWORD							dwMatchID;		//比赛标识
	DWORD							dwMatchNO;		//比赛场次	
};

//排行信息
struct tagMatchAwardkInfo
{
	WORD							MatchRank;							//比赛名次
	DWORD							RewardGold;					//奖励金币
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
struct CMD_GR_MatchAwardList
{
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
	std::vector<tagMatchAwardkInfo> kAwards;

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(dwMatchID);
		Stream_VALUE(dwMatchNO);
		StructVecotrMember(tagMatchAwardkInfo,kAwards);
	}
};
//////////////////////////////////////////////////////////////////////////////////


//修改密码
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改推荐人
struct CMD_GP_ModifySpreader
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szSpreader[LEN_NICKNAME];		//用户密码
};

//修改帐号
struct CMD_GP_ModifyAccounts
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szDesAccount[LEN_ACCOUNTS];			//用户帐号
};

//修改签名
struct CMD_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//////////////////////////////////////////////////////////////////////////////////

//用户头像
struct CMD_GP_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定标识
};

//修改头像
struct CMD_GP_SystemFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct CMD_GP_CustomFaceInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};

//////////////////////////////////////////////////////////////////////////////////

//绑定机器
struct CMD_GP_ModifyMachine
{
	BYTE							cbBind;								//绑定标志
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//个人资料
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//个人信息
struct CMD_GP_UserAccountInfo
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

//操作成功
struct CMD_GP_InGameSeverID
{
	DWORD							LockKindID;			
	DWORD							LockServerID;			
};

//查询信息
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//游戏状态
struct CMD_GP_UserInGameServerID
{
	DWORD							dwUserID;							//用户 I D
};


//查询信息
struct CMD_GP_QueryAccountInfo
{
	DWORD							dwUserID;							//用户 I D
};


//修改资料
struct CMD_GP_ModifyIndividual
{
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};
//////////////////////////////////////////////////////////////////////////////////
//查询签到
struct CMD_GP_CheckInQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//签到信息
struct CMD_GP_CheckInInfo
{	
	WORD							wSeriesDate;						//连续日期
	WORD							wAwardDate;							//物品日期
	bool							bTodayChecked;						//签到标识
	SCORE							lRewardGold[LEN_SIGIN];				//奖励金额
	BYTE							lRewardType[LEN_SIGIN];				//奖励类型 1金币 2道具
	BYTE							lRewardDay[LEN_SIGIN];				//奖励天数 
};

//执行签到
struct CMD_GP_CheckInDone
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//签到结果
struct CMD_GP_CheckInResult
{
	bool							bType;								//是否是达到天数领取物品
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前金币
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////
//新手活动

struct CMD_GP_BeginnerQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};
struct CMD_GP_BeginnerInfo
{	
	WORD							wSeriesDate;						//连续日期
	bool							bTodayChecked;						//签到标识
	bool							bLastCheckIned;						//签到标识
	SCORE							lRewardGold[LEN_BEGINNER];			//奖励金额
	BYTE							lRewardType[LEN_BEGINNER];			//奖励类型 1金币 2道具
};
struct CMD_GP_BeginnerDone
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

struct CMD_GP_BeginnerResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lAwardCout;							//奖励数量
	SCORE							lAwardType;							//奖励类型
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////
//低保服务

//领取低保
struct CMD_GP_BaseEnsureTake
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//低保参数
struct CMD_GP_BaseEnsureParamter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//低保结果
struct CMD_GP_BaseEnsureResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};


//自定义字段查询 公告
struct CMD_GP_QueryNotice
{
	TCHAR							szKeyName[LEN_NICKNAME];			//关键字
};
struct CMD_GP_PublicNotice
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[512];				//成功消息
};
//////////////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_UserIndividual

#define DTP_GP_UI_NICKNAME			1									//用户昵称
#define DTP_GP_UI_USER_NOTE			2									//用户说明
#define DTP_GP_UI_UNDER_WRITE		3									//个性签名
#define DTP_GP_UI_QQ				4									//Q Q 号码
#define DTP_GP_UI_EMAIL				5									//电子邮件
#define DTP_GP_UI_SEAT_PHONE		6									//固定电话
#define DTP_GP_UI_MOBILE_PHONE		7									//移动电话
#define DTP_GP_UI_COMPELLATION		8									//真实名字
#define DTP_GP_UI_DWELLING_PLACE	9									//联系地址
#define DTP_GP_UI_HEAD_HTTP			10									//头像
#define DTP_GP_UI_IP				11									//IP
#define DTP_GP_UI_CHANNEL			12									//渠道号

//////////////////////////////////////////////////////////////////////////////////

//银行资料
struct CMD_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	SCORE							lTransferPrerequisite;				//转账条件
};

//存入金币
struct CMD_GP_UserSaveScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入金币
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//提取金币
struct CMD_GP_UserTakeScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转账金币
struct CMD_GP_UserTransferScore
{
	DWORD							dwUserID;							//用户 I D
	BYTE                            cbByNickName;                       //昵称赠送
	SCORE							lTransferScore;						//转账金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//银行成功
struct CMD_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct CMD_GP_UserInsureFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//提取结果
struct CMD_GP_UserTakeResult
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
};

//查询银行
struct CMD_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//用户 I D
};

//查询用户
struct CMD_GP_QueryUserInfoRequest
{
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};

//用户信息
struct CMD_GP_UserTransferUserInfo
{
	DWORD							dwTargetGameID;						//目标用户
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};
//////////////////////////////////////////////////////////////////////////////////

//操作失败
struct CMD_GP_OperateFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//操作成功
struct CMD_GP_SpreaderResoult
{
	LONG							lResultCode;						//操作代码
	SCORE							lScore;
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////
//远程服务

#define MDM_GP_REMOTE_SERVICE		4									//远程服务

//查找服务
#define SUB_GP_C_SEARCH_DATABASE	100									//数据查找
#define SUB_GP_C_SEARCH_CORRESPOND	101									//协调查找

//查找服务
#define SUB_GP_S_SEARCH_DATABASE	200									//数据查找
#define SUB_GP_S_SEARCH_CORRESPOND	201									//协调查找

//////////////////////////////////////////////////////////////////////////////////

//协调查找
struct CMD_GP_C_SearchCorrespond
{
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

//协调查找
struct CMD_GP_S_SearchCorrespond
{
	WORD							wUserCount;							//用户数目
	tagUserRemoteInfo				UserRemoteInfo[16];					//用户信息
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//登录命令
#define MDM_MB_LOGON				100									//广场登录

//登录模式
#define SUB_MB_LOGON_GAMEID			1									//I D 登录
#define SUB_MB_LOGON_ACCOUNTS		2									//帐号登录
#define SUB_MB_REGISTER_ACCOUNTS	3									//注册帐号

//登录结果
#define SUB_MB_LOGON_SUCCESS		100									//登录成功
#define SUB_MB_LOGON_FAILURE		101									//登录失败

//升级提示
#define SUB_MB_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////

//I D 登录
struct CMD_MB_LogonGameID
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//登录信息
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//帐号登录
struct CMD_MB_LogonAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//注册帐号
struct CMD_MB_RegisterAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//登录成功
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

//登录失败
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//升级提示
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};


//////////////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_MB_SERVER_LIST			101									//列表信息

//列表信息
#define SUB_MB_LIST_KIND			100									//种类列表
#define SUB_MB_LIST_SERVER			101									//房间列表
#define SUB_MB_LIST_FINISH			200									//列表完成

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif