#include "StdAfx.h"
#include "PrivateTableInfo.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
PrivateTableInfo::PrivateTableInfo()
	:pITableFrame(NULL)
{
	restValue();
}

PrivateTableInfo::~PrivateTableInfo(void)
{
	//关闭定时器
}
void PrivateTableInfo::restAgainValue()
{
	bStart = false;
	bInEnd = false;
	kDismissChairID.clear();
	kNotAgreeChairID.clear();
	fDismissPastTime = 0;
	dwFinishPlayCout = 0;
	dwStartPlayCout = 0;
	fAgainPastTime = 0.0f;
	ZeroMemory(lPlayerWinLose,sizeof(lPlayerWinLose));
	ZeroMemory(lPlayerAction,sizeof(lPlayerAction));
	kTotalRecord = tagPrivateRandTotalRecord();

	if (pITableFrame)
	{
		kTotalRecord.kScore.resize(pITableFrame->GetChairCount());
		kTotalRecord.kUserID.resize(pITableFrame->GetChairCount());
		kTotalRecord.kNickName.resize(pITableFrame->GetChairCount());
		kTotalRecord.dwKindID = pITableFrame->GetGameServiceAttrib()->wKindID;
		kTotalRecord.dwVersion = pITableFrame->GetGameServiceAttrib()->dwClientVersion;
		for (int i = 0;i<(int)getChairCout();i++)
		{
			kTotalRecord.kScore[i] = 0;
		}
	}
}
void PrivateTableInfo::restValue()
{
	bStart = false;
	bInEnd = false;
	bPlayCoutIdex = 0;
	bGameTypeIdex = 0;
	bGameRuleIdex = 0;
	cbRoomType = Type_Private;
	dwPlayCout = 0;
	dwRoomNum = 0;
	dwCreaterUserID = 0;
	fDismissPastTime = 0;
	dwFinishPlayCout = 0;
	dwStartPlayCout = 0;
	dwPlayCost = 0;
	fAgainPastTime = 0.0f;
	kHttpChannel = "";

	restAgainValue();
}
void PrivateTableInfo::newRandChild()
{
	tagPrivateRandRecordChild kRecordChild;
	kRecordChild.dwKindID = pITableFrame->GetGameServiceAttrib()->wKindID;
	kRecordChild.dwVersion = pITableFrame->GetGameServiceAttrib()->dwClientVersion;
	kRecordChild.kScore.resize(getChairCout());
	for (int i = 0;i<(int)getChairCout();i++)
	{
		IServerUserItem * pServerItem = pITableFrame->GetTableUserItem(i);
		kTotalRecord.kNickName[i] = pServerItem->GetNickName();
		kTotalRecord.kUserID[i] = pServerItem->GetUserID();

		kRecordChild.kScore[i] = 0;
	}
	GetLocalTime(&kRecordChild.kPlayTime);
	kTotalRecord.kRecordChild.push_back(kRecordChild);
}
WORD PrivateTableInfo::getChairCout()
{
	return pITableFrame->GetChairCount();
}
void PrivateTableInfo::setRoomNum(DWORD RoomNum)
{
	kTotalRecord.iRoomNum = (int)RoomNum;
	dwRoomNum = RoomNum;
}
void PrivateTableInfo::writeSocre(tagScoreInfo ScoreInfoArray[], WORD wScoreCount,datastream& daUserDefine)
{
	if (kTotalRecord.kRecordChild.size() == 0)
	{
		ASSERT(false);
		return;
	}
	tagPrivateRandRecordChild& kRecord = kTotalRecord.kRecordChild.back();
	if (kRecord.kScore.size() < wScoreCount)
	{
		ASSERT(false);
		return;
	}
	for(WORD i = 0;i < wScoreCount;i++)
	{

		kRecord.kScore[i] += (int)ScoreInfoArray[i].lScore;
		kTotalRecord.kScore[i] += (int)ScoreInfoArray[i].lScore;
		lPlayerWinLose[i] += ScoreInfoArray[i].lScore;
	}

	GetLocalTime(&kRecord.kPlayTime);
	kRecord.kRecordGame = daUserDefine;
}