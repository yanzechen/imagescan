/********************************************************************
	created:	2011/06/22	
	filename: 	ImageRotateFlipRecord	
	author:		haifeng_liu
	
	purpose:	记录图片旋转(向左旋转, 向右旋转)及翻转(水平翻转, 垂直
				翻转)状态, 并且在图片反复进行多次旋转及翻转操作后可以
				计算出图片最终状态
*********************************************************************/
#pragma once

class CImageRotateFlipRecord
	{ 	
	public:
		CImageRotateFlipRecord(void);
		~CImageRotateFlipRecord(void);

		typedef enum {
			RotateNone = 0,
			Rotate90,
			Rotate180,
			Rotate270
		} ROTATETYPE;  

		typedef enum {
			FlipNone = 0,
			FlipX		
		} FLIPTYPE;

	private:
		RotateFlipType m_rft;	 

		void ParseRotateFlipType(RotateFlipType rft, ROTATETYPE& rt, FLIPTYPE& ft);
		void SetRotateFlipType(ROTATETYPE rt, FLIPTYPE ft); 

	public:
		void SetRecord(RotateFlipType rft);	  
		void GetRecord(RotateFlipType& rft);
		RotateFlipType GetRecord();	

	public:
		void ResetRecord();
		void RotateFilp(RotateFlipType rft);
		
};
