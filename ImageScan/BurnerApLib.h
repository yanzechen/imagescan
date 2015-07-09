
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BURNERAPLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
//  functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.


enum{
	UNKNOWN_BACKEND = -2,	// shawn 2008/11/20 add
	NO_DEVICE,				// shawn 2008/11/20 add
	MODE_ERROR,
	MODE_ISP,
	MODE_DEF
	};

// manufacturer ID
#define SF_TYPE_UNKNOWN				0x00
#define SF_TYPE_MXIC				0x01
#define SF_TYPE_ST					0x02
#define SF_TYPE_SST					0x03
#define SF_TYPE_ATMEL_AT25F			0x04
#define SF_TYPE_ATMEL_AT25FS		0x05
#define SF_TYPE_ATMEL_AT45DB		0x06
#define SF_TYPE_WINBOND				0x07
#define SF_TYPE_PMC					0x08
#define SF_TYPE_AMIC				0x0A	// shawn 2008/08/11 add
#define SF_TYPE_EON					0x0B	// shawn 2009/02/17 add
#define SF_TYPE_MXIC_LIKE			0xFF

// control ID
#define SF_WRITE		6
#define SF_READ			7
#define EEPROM_WRITE   12		// shawn 2008/06/16 add
#define EEPROM_READ    13		// shawn 2008/06/16 add

// initialize/ uninitialize driver interface
// driver interface must be initialized before other requests
// init return 1 with ISP mode, and return 2 with default driver mode
int Initialize_DriverInterface(void);
bool UnInitialize_DriverInterface(void);
bool GetDeviceList(unsigned short **wsList, int iDevNum, int iStrLen, int *RetNum);	// james 2008/08/22 modify
int Initialize_DriverInterface(unsigned short *wsDevicePath, int iStrLen);		// james 2008/07/15 add

// request to driver
// ex: ReqToDriver(SF_WRITE, 0x0, pRomBuffer, RomBuffer_length)
bool ReqToDriver(unsigned char control, unsigned int address, unsigned char *pData, unsigned long total_len);
// erase serial flash
bool EraseSF(void);
bool SectorEraseSF(BYTE SectorNum);	// shawn 2008/11/19 add
// shawn 2008/07/02 add for 232A to get memory type
//                  0: none, 1: SF, 2: EEPROM
bool ProbeMem(BYTE *pMemType);

// asic r/w function
bool ProcWriteToASIC(USHORT Addr, BYTE Value);
bool ProcReadFromASIC(USHORT Addr, BYTE *pValue);
void SF_WaitReady(void);
void SF_CMDread_Status(void);

bool GetRomVersion(char *pRomVersion);
bool GetBypassLength(unsigned long &ulBypass);
bool SetFlashType(int iFlashType);
bool GetCodeVersion(BYTE *FlashCodeVer);
bool IsNewSensorTable(void);
bool SetWriteProtect(bool bControl, USHORT &WPAddr, BYTE &bWriteProtect);
bool PullCpuRate(bool bControl);
int GetFlashType();	// james 2008/01/04 modify

bool GetDevLocation(LPTSTR szLocation);	// shawn 2009/01/13 add
int ResetDevice();	// shawn 2009/04/10 add
