#include "StdAfx.h"
#include "Resource.h"
#include "ImageItemControl.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量

CImageItemControl *	CImageItemControl::m_pImageItemControl=NULL;			//头像接口

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CImageItemControl::CImageItemControl()
{
	//设置对象
	ASSERT(m_pImageItemControl==NULL);
	if (m_pImageItemControl==NULL) m_pImageItemControl=this;

	//加载资源
	HINSTANCE hResInstance=GetModuleHandle(AVATAR_CONTROL_DLL_NAME);
	m_ImageUserImage.LoadImage(hResInstance,TEXT("USER_TABLE_IMAGE"));

	//设置变量
	m_wLineCount=m_ImageUserImage.GetWidth()/IMAGE_CX/8;
	m_wFullCount=m_wLineCount*m_ImageUserImage.GetHeight()/IMAGE_CY;
	
	return;
}

//析构函数
CImageItemControl::~CImageItemControl()
{
	//设置对象
	ASSERT(m_pImageItemControl==this);
	if (m_pImageItemControl==this) m_pImageItemControl=NULL;

	return;
}

//接口查询
VOID * CImageItemControl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IImageItemControl,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IImageItemControl,Guid,dwQueryVer);
	return NULL;
}

//头像数目
WORD CImageItemControl::GetImageCount()
{
	return m_wFullCount;
}

//获取尺寸
VOID CImageItemControl::GetImageItemSize(CSize & SizeFace)
{
	//设置变量
	SizeFace.SetSize(IMAGE_CX,IMAGE_CY);
	return;
}

//获取资源
VOID CImageItemControl::GetAvatarResource(tagAvatarResource & AvatarResource)
{
	//设置变量
	AvatarResource.pszResource=TEXT("USER_TABLE_IMAGE");
	AvatarResource.hResInstance=GetModuleHandle(AVATAR_CONTROL_DLL_NAME);

	return;
}

//绘画头像
VOID CImageItemControl::DrawImage(CDC * pDC, INT nXPos, INT nYPos, INT nSrcWidth, INT nSrcHeight)
{
	m_ImageUserImage.DrawImage(pDC,nXPos,nYPos,IMAGE_CX,IMAGE_CY,nSrcWidth,nSrcHeight);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(ImageItemControl);

//////////////////////////////////////////////////////////////////////////////////
