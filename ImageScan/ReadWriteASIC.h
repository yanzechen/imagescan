/********************************************************************
created:	2011/05/05	
filename: 	ReadWriteASIC	
author:		haifeng_liu

purpose:	
*********************************************************************/
#pragma once

// �豸��д������
class CReadWriteASIC
{
public:
	CReadWriteASIC(void);
	~CReadWriteASIC(void);

private:
	IBaseFilter *m_pSrcFilter;

private:
	BOOL ReadFromRegister (ULONG id, PLONG pValue);
	BOOL WriteToRegister (ULONG id , PLONG pValue , ULONG index, ULONG valueSize, ULONG common);

public:
	// �����豸Filter, �豸����ΪSonix Device(Vid=0x0C45)
	BOOL AttachSrcFilter(IBaseFilter *pSrcFilter);
	IBaseFilter* DetachSrcFilter();	

	BOOL ReadFromASIC(USHORT Addr, BYTE *pValue);
	BOOL WriteToASIC(USHORT Addr, BYTE Value);
	BOOL ReadSlaveIDFromASIC( BYTE *pSlaveID );
};
