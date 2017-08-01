#include "Stdafx.h"
#include "D3DSound.h"
#include "TraceDebug.h"

CD3DSound::CD3DSound()
{
}

CD3DSound::~CD3DSound()
{
}

//创建环境
bool CD3DSound::CreateD3DSound(HWND hWnd)
{
	return true;
}

//加载声音
HSTREAM CD3DSound::LoadSound(LPCTSTR pszFileName, bool bRepetition)
{
	HSTREAM hStream;
	return hStream;
}

//加载声音
HSTREAM CD3DSound::LoadSound(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, bool bRepetition)
{	
	HSTREAM hStream;

	return hStream;
}

//获取对象
CD3DSound * CD3DSound::GetInstance()
{
	static CD3DSound _Instance;

	return &_Instance;
}

//设置音量
void CD3DSound::SetMusVolume( DWORD dwVolume )
{
}

//获取音量
DWORD CD3DSound::GetMusVolume()
{
	return 0;
}

//播放声音
HCHANNEL CD3DSound::PlaySound( HSTREAM hStream )
{
	HCHANNEL hChannel;
	return hChannel;
}

//播放声音
HCHANNEL CD3DSound::StopSound( HSTREAM hStream )
{
	HCHANNEL hChannel;
	return hChannel;
}
//停止声音
bool CD3DSound::StopSound()
{
	return true;
	
}
//播放声音
bool CD3DSound::PlaySound(LPCTSTR pszFileName, bool bRepetition)
{
	return true;
}
//播放声音
bool CD3DSound::PlaySound(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, bool bRepetition)
{
	return true;
}

//删除声音
void CD3DSound::FreeSound( HSTREAM hStream )
{
}

//暂停声音
void CD3DSound::Channel_Pause( HCHANNEL hChannel )
{
}

//苏醒声音
void CD3DSound::Channel_Resume( HCHANNEL hChannel )
{
}	

//停止声音
void CD3DSound::Channel_Stop( HCHANNEL hChannel )
{
}

//暂停所有
void CD3DSound::Channel_PauseAll()
{
}

//苏醒所有
void CD3DSound::Channel_ResumeAll()
{
}

//停止所有
void CD3DSound::Channel_StopAll()
{
}

//正在播放
bool CD3DSound::Channel_IsPlaying( HCHANNEL hChannel )
{
	return false;
}

//获取大小
double CD3DSound::Channel_GetLength( HCHANNEL hChannel )
{
	return 0.0f;
}

//获取位置
double CD3DSound::Channel_GetPos( HCHANNEL hChannel )
{
	return 0.0f;
}

//设置位置
void CD3DSound::Channel_SetPos( HCHANNEL hChannel, double dSeconds )
{
}



