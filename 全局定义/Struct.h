#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#include "datastream.h"

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//游戏列表

//游戏类型
struct tagGameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct tagGameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct tagGameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct tagGamePage
{
	WORD							wPageID;							//页面索引
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//游戏房间
struct tagGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	//WORD                            wServerKind;                        //房间类型
	WORD							wServerType;						//房间类型
	WORD							wServerPort;						//房间端口
	SCORE							lCellScore;							//单元积分
	SCORE							lEnterScore;						//进入积分
	DWORD							dwServerRule;						//房间规则
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szServerAddr[32];					//房间名称
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//比赛报名
struct tagSignupMatchInfo
{
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//比赛信息
struct tagGameMatch
{
	//基本信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称

	//比赛信息
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMatchFeeType;						//扣费类型
	SCORE							lMatchFee;							//比赛费用	
	SCORE							lMatchEnterScore;					//准入金币
	
	//比赛信息
	WORD							wStartUserCount;					//开赛人数
	WORD							wMatchPlayCount;					//比赛局数

	//比赛奖励
	WORD							wRewardCount;						//奖励人数

	//比赛时间
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchEndTime;						//结束时间	
};

//视频配置
struct tagAVServerOption
{
	WORD							wAVServerPort;						//视频端口
	DWORD							dwAVServerAddr;						//视频地址
};

//在线信息
struct tagOnLineInfoKind
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
};

//在线信息
struct tagOnLineInfoServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息

//桌子状态
struct tagTableStatus
{
	BYTE							cbTableLock;						//锁定标志
	BYTE							cbPlayStatus;						//游戏标志
};

//用户状态
struct tagUserStatus
{
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态
};

//用户属性
struct tagUserAttrib
{
	BYTE							cbCompanion;						//用户关系
};

//用户积分
struct tagUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数

	//全局信息
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
};

//用户积分
struct tagMobileUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数

	//全局信息
	DWORD							dwExperience;						//用户经验
};


//道具使用
struct tagUsePropertyInfo
{
	WORD                            wPropertyCount;                     //道具数目
	WORD                            dwValidNum;						    //有效数字
	DWORD                           dwEffectTime;                       //生效时间
};


//用户道具
struct tagUserProperty
{
	WORD                            wPropertyUseMark;                   //道具标示
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//使用信息   
};

//道具包裹
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //小喇叭数
	WORD                            wTyphonCount;                      //大喇叭数
};

//时间信息
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//进出桌子时间
	DWORD						dwLeaveTableTimer;						//离开桌子时间
	DWORD						dwStartGameTimer;						//开始游戏时间
	DWORD						dwEndGameTimer;							//离开游戏时间
};

//用户信息
struct tagUserInfo
{
	//基本属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团 I D
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

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

	//时间信息
	tagTimeInfo						TimerInfo;							//时间信息	
};

//用户信息
struct tagUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGroupID;							//社团 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

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
};

//头像信息
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};

//用户信息
struct tagUserRemoteInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//等级信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//位置信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	TCHAR							szGameServer[LEN_SERVER];			//房间位置
};

//////////////////////////////////////////////////////////////////////////////////

//广场子项
struct tagGamePlaza
{
	WORD							wPlazaID;							//广场标识
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[32];					//服务器名
};

//级别子项
struct tagLevelItem
{
	LONG							lLevelScore;						//级别积分
	TCHAR							szLevelName[16];					//级别描述
};

//会员子项
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//等级标识
	TCHAR							szMemberName[16];					//等级名字
};

//管理子项
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//等级标识
	TCHAR							szMasterName[16];					//等级名字
};

//列表子项
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//列表宽度
	BYTE							cbDataDescribe;						//字段类型
	TCHAR							szColumnName[16];					//列表名字
};

//地址信息
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//服务地址
};

//数据信息
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//数据库端口
	TCHAR							szDataBaseAddr[32];					//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//房间配置
struct tagServerOptionInfo
{
	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间属性
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户信息
struct tagMobileUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
};
//////////////////////////////////////////////////////////////////////////////////
//用户一盘输赢
struct tagPrivateRandRecordChild
{
	DWORD						dwKindID;
	DWORD						dwVersion;
	int							iRecordID;
	int							iRecordChildID;
	std::vector<int>			kScore;
	SYSTEMTIME					kPlayTime;
	datastream					kRecordGame;
	std::string					kUserDefine;
	tagPrivateRandRecordChild()
	{
		dwKindID = 0;
		dwVersion = 0;
		iRecordID = 0;
		iRecordChildID = 0;
	}
	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(iRecordID);
		Stream_VALUE(iRecordChildID);
		Stream_VALUE(kScore);
		Stream_VALUE(kRecordGame);
		Stream_VALUE_SYSTEMTIME(kPlayTime);
		Stream_VALUE(kUserDefine);
		Stream_VALUE(dwKindID);
		Stream_VALUE(dwVersion);
	}
};

//用户一轮总输赢
struct tagPrivateRandTotalRecord
{
	tagPrivateRandTotalRecord()
	{
		dwKindID = 0;
		dwVersion = 0;
		iRoomNum = 0;
		iRecordID = 0;
	}
	DWORD						dwKindID;
	DWORD						dwVersion;
	int							iRoomNum;
	int							iRecordID;
	std::vector<int>			kScore;
	std::vector<int>			kUserID;
	std::vector<std::string>	kNickName;
	SYSTEMTIME					kPlayTime;
	std::string					kUserDefine;

	std::vector<tagPrivateRandRecordChild>	kRecordChild;
	
	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(iRoomNum);
		Stream_VALUE(kUserID);
		Stream_VALUE(kNickName);
		Stream_VALUE(kScore);
		Stream_VALUE_SYSTEMTIME(kPlayTime);
		StructVecotrMember(tagPrivateRandRecordChild,kRecordChild);
		Stream_VALUE(iRecordID);
		Stream_VALUE(kUserDefine);
		Stream_VALUE(dwKindID);
		Stream_VALUE(dwVersion);
	}
};
struct tagPrivateRandTotalRecordList
{
	DWORD		dwUserID;
	std::vector<tagPrivateRandTotalRecord> kList;

	void StreamValue(datastream& kData,bool bSend)
	{
		Stream_VALUE(dwUserID);
		StructVecotrMember(tagPrivateRandTotalRecord,kList);
	}
};
#pragma pack()

#endif