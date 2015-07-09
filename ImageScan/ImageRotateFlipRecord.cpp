/********************************************************************
created:	2011/06/22	
filename: 	ImageRotateFlipRecord	
author:		haifeng_liu

purpose:	记录图片旋转(向左旋转, 向右旋转)及翻转(水平翻转, 垂直
翻转)状态, 并且在图片反复进行多次旋转及翻转操作后可以
计算出图片最终状态
*********************************************************************/
#include "StdAfx.h"
#include "ImageRotateFlipRecord.h"

CImageRotateFlipRecord::CImageRotateFlipRecord(void)
{
	m_rft = RotateNoneFlipNone;
}

CImageRotateFlipRecord::~CImageRotateFlipRecord(void)
{
}

void CImageRotateFlipRecord::ResetRecord()
{
	m_rft = RotateNoneFlipNone;
}

void CImageRotateFlipRecord::RotateFilp( RotateFlipType rft )
{
	ROTATETYPE rt;
	FLIPTYPE ft;

	ParseRotateFlipType(rft, rt, ft);
	SetRotateFlipType(rt, ft);
}

void CImageRotateFlipRecord::GetRecord( RotateFlipType& rft )
{
	rft = m_rft;
}

RotateFlipType CImageRotateFlipRecord::GetRecord()
{
	return m_rft;
}

void CImageRotateFlipRecord::ParseRotateFlipType( RotateFlipType rft, ROTATETYPE& rt, FLIPTYPE& ft )
{
	switch (rft)
	{
	case RotateNoneFlipNone:
		//		case Rotate180FlipXY:
		rt = RotateNone;
		ft = FlipNone;
		break;
	case Rotate90FlipNone:
		//		case Rotate270FlipXY:
		rt = Rotate90;
		ft = FlipNone;
		break;
	case Rotate180FlipNone:
		//		case RotateNoneFlipXY:
		rt = Rotate180;
		ft = FlipNone;
		break;
	case Rotate270FlipNone:
		//		case Rotate90FlipXY:
		rt = Rotate270;
		ft = FlipNone;
		break;
	case RotateNoneFlipX:
		//		case Rotate180FlipY:
		rt = RotateNone;
		ft = FlipX;
		break;
	case Rotate90FlipX:
		//		case Rotate270FlipY:
		rt = Rotate90;
		ft = FlipX;
		break;
	case Rotate180FlipX:
		//		case RotateNoneFlipY:
		rt = Rotate180;
		ft = FlipX;
		break;
	case Rotate270FlipX:
		//		case Rotate90FlipY:
		rt = Rotate270;
		ft = FlipX;
		break;
	}	
}

void CImageRotateFlipRecord::SetRotateFlipType( ROTATETYPE rt, FLIPTYPE ft)
{
	RotateFlipType rft = m_rft;

	ROTATETYPE mrt;
	FLIPTYPE mft;
	ParseRotateFlipType(m_rft, mrt, mft);

	int imrt = (int)mrt;
	int imft = (int)mft;
	int irt = (int)rt;
	int ift = (int)ft;

	imrt += irt;
	imft += ift;

	imrt %= 4;		// Enum: 0, 1, 2, 3
	imft %= 2;		// Enum: 0, 1 	

	// 查看"RotateFlipType"的定义发现, 其枚举值为[0, 7], 刚好可以
	// 用3位表示图像所有的旋转翻转状态
	// 并且, FlipType代表: bit2; RotateType代表: bit0bit1
	m_rft = (RotateFlipType)((imft<<2)+imrt);
}

void CImageRotateFlipRecord::SetRecord( RotateFlipType rft )
{
	m_rft = rft;
}


/*
typedef enum {
RotateNoneFlipNone = 0,
Rotate90FlipNone = 1,
Rotate180FlipNone = 2,
Rotate270FlipNone = 3,
RotateNoneFlipX = 4,
Rotate90FlipX = 5,
Rotate180FlipX = 6,
Rotate270FlipX = 7,
RotateNoneFlipY = Rotate180FlipX,
Rotate90FlipY = Rotate270FlipX,
Rotate180FlipY = RotateNoneFlipX,
Rotate270FlipY = Rotate90FlipX,
RotateNoneFlipXY = Rotate180FlipNone,
Rotate90FlipXY = Rotate270FlipNone,
Rotate180FlipXY = RotateNoneFlipNone,
Rotate270FlipXY = Rotate90FlipNone
} RotateFlipType;*/

