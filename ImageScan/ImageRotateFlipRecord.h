/********************************************************************
	created:	2011/06/22	
	filename: 	ImageRotateFlipRecord	
	author:		haifeng_liu
	
	purpose:	��¼ͼƬ��ת(������ת, ������ת)����ת(ˮƽ��ת, ��ֱ
				��ת)״̬, ������ͼƬ�������ж����ת����ת���������
				�����ͼƬ����״̬
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
