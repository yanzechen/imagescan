#pragma once


class CFormatConvertArithmetic
{
public:
	CFormatConvertArithmetic(void);
	~CFormatConvertArithmetic(void);  

public:
	static void YUY2ToRGB24(BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight);
	static void I420ToRGB24(BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight);
};
