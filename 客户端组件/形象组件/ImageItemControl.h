#ifndef IMAGE_ITEM_CONTROL_HEAD_FILE
#define IMAGE_ITEM_CONTROL_HEAD_FILE

#pragma once

#include "AvatarControlHead.h"

//用户3D形象控件
class AVATAR_CONTROL_CLASS  CImageItemControl : public IImageItemControl
{
	//变量定义
protected:
	WORD							m_wLineCount;						//头像数目
	WORD							m_wFullCount;						//头像数目
	CPngImage						m_ImageUserImage;					//用户形象


	//对象接口
protected:
	static CImageItemControl *		m_pImageItemControl;					//头像接口

	//函数定义
public:
	//构造函数
	CImageItemControl();
	//析构函数
	virtual ~CImageItemControl();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	
	//信息接口
public:
	//头像数目
	virtual WORD GetImageCount();
	//获取尺寸
	virtual VOID GetImageItemSize(CSize & SizeFace);

	//资源接口
public:
	//获取资源
	virtual VOID GetAvatarResource(tagAvatarResource & AvatarResource);

	//绘画接口
public:
	//绘画头像
	virtual VOID DrawImage(CDC * pDC, INT nXPos, INT nYPos,INT nSrcWidth, INT nSrcHeight);

	//绘画接口
//public:
	//绘画头像
	//virtual VOID DrawImageFree(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, WORD wFaceID);
	//绘画头像
	//virtual VOID DrawImagePlaying(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, WORD wFaceID);


	//静态函数
public:
	//获取对象
	static CImageItemControl * GetInstance() { return m_pImageItemControl; }
};

#endif