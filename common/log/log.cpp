#include "log.h"
#include "../basic/platform.h"
#include "../basic/stringFunctions.h"

// static初始化
bool		CLog::m_bInitiate		= false;
SYSTEMTIME	CLog::m_gSysTime[CLog::LT_Max];
char		CLog::m_szDirectory[256]= {0};
char		CLog::m_szBuffer[2048]	= {0};
CMutex		CLog::m_csLock;
CFileStream	CLog::m_clgFile[CLog::LT_Max];
char        CLog::m_logFilePrev[ 256 ] = {0};

//初始化
bool CLog::initialize( const char* root, const char* prev ){
	if(m_bInitiate) return true;
    if ( root == nullptr )  root = ".";
    if ( prev != nullptr )
        dStrcpy( m_logFilePrev, sizeof( m_logFilePrev ), prev );

	CConsoleColor::initialize();
	memset( m_gSysTime, 0, sizeof( m_gSysTime ) );
	dSprintf( m_szDirectory,sizeof(m_szDirectory),"%s/logs", root );
	memset( m_szBuffer,0,sizeof(m_szBuffer));
    if ( !findDirectory( m_szDirectory ) ){
        if (!createDirectory(m_szDirectory)) return false;
    }

	m_bInitiate = true;
	return true;
}
 
void CLog::shutdown(){
	m_bInitiate = false;
	CConsoleColor::shutdown();
	CCritLocker lock(m_csLock);
	for (int i = 0;i < LT_Max;i++)
		m_clgFile[i].close();
}

// 获取日期前缀 
void CLog::getDate(char* pDate,const uint32& uSize){
	SYSTEMTIME systime;
	getLocalTime(systime);
	dSprintf(pDate,uSize,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d:%.3d",systime.wYear, systime.wMonth, systime.wDay,
		systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds);
}

// 设置目录
void CLog::setDirectory(const char* pDir){
	if (!pDir || !*pDir) return;
	dSprintf(m_szDirectory,sizeof(m_szDirectory),pDir);
}

// 创建文件 
void CLog::createAFile(uint8 ucType){
	if (ucType >= LT_Max)
		ucType	= LT_Log;

	char szTime[32]	= {0};
	SYSTEMTIME systime;
	getLocalTime(systime);

	if (m_gSysTime[ucType].wYear == systime.wYear && m_gSysTime[ucType].wMonth == systime.wMonth && m_gSysTime[ucType].wDay == systime.wDay)
		return;

	m_gSysTime[ucType]	= systime;
	/*文件时间*/ 
	dSprintf(szTime,sizeof(szTime),"%.4d%.2d%.2d",m_gSysTime[ucType].wYear,m_gSysTime[ucType].wMonth,m_gSysTime[ucType].wDay);
	/*目录标志*/ 
	bool dirFlag = true;

	if(m_szDirectory[0] && !findDirectory(m_szDirectory)){
		if(!createDirectory(m_szDirectory))
			dirFlag = false;
	}

	/*子目录*/ 
	char szDir[256]	= {0};
	if (dirFlag && m_szDirectory[0])
		dSprintf(szDir,sizeof(szDir),"%s/%s",m_szDirectory,szTime);
	else
		dSprintf(szDir,sizeof(szDir),"%s",szTime);

	if(!findDirectory(szDir)){
		if(!createDirectory(szDir))
			szDir[0]	= 0;
	}

	char szFile[256]	= {0};
	switch(ucType){
	case LT_Warn:
		dSprintf(szFile,sizeof(szFile),"%s_WARN_", m_logFilePrev);
		break;
	case LT_Error:
		dSprintf(szFile,sizeof(szFile),"%s_ERROR_", m_logFilePrev);
		break;
	case LT_Log:
	default:
		dSprintf(szFile,sizeof(szFile),"%s_LOG_", m_logFilePrev);
		break;
	}

	char szFilename[512];
	if(szDir[0])
		dSprintf(szFilename,sizeof(szFilename),"%s/%s%s.log",szDir,szFile,szTime);
	else
		dSprintf(szFilename,sizeof(szFilename),"%s%s.log",szFile,szTime);

	m_clgFile[ucType].close();
	m_clgFile[ucType].open(szFilename,"at+");


	char szDate[32]={0};
	getDate(szDate,sizeof(szDate));
	m_clgFile[ucType].fprintf("\r\n-------------------<%s>-------------------\r\n",szDate);
    m_clgFile[ucType].flush();
}

 // print
void CLog::_print(const char* pFormat,va_list&argptr){
	if(!pFormat) return;

	char szStrint[ 2048 ] = {0};
	dVsprintf( szStrint, sizeof( szStrint ), pFormat, argptr );
    szStrint[ 2047 ] = 0;

	char szDate[32] = {0};
	getDate( szDate, sizeof( szDate ) );
	memset( m_szBuffer, 0, sizeof( m_szBuffer ) );
	dSprintf( m_szBuffer, sizeof( m_szBuffer ), "%s %s", szDate, szStrint );
    m_szBuffer[ 2047 ] = 0;
#ifdef WIN32
	dPrintf("%s\n", m_szBuffer );
#else
	dPrintf("%s\033[0m\n", m_szBuffer );
#endif
}

// print
void CLog::print(const char* pFormat,...){
	if (!pFormat) return;
    CCritLocker lock(m_csLock);
	initialize();
	CConsoleColor clColor(_BIT32(CConsoleColor::color_white) | _BIT32(CConsoleColor::color_intensity));
	va_list	argptr;
	va_start(argptr,pFormat);
	_print(pFormat,argptr);
	va_end(argptr);
}

// info
void CLog::info(const char* pFormat, ...){
	if (!pFormat) return;
	CCritLocker lock(m_csLock);
	initialize();

	/*设置绿色字符前景色*/
	CConsoleColor clColor(_BIT32(CConsoleColor::color_green) | _BIT32(CConsoleColor::color_intensity));
	va_list	argptr;
	va_start(argptr, pFormat);
	_print(pFormat, argptr);
	va_end(argptr);

	createAFile(LT_Log);
	m_clgFile[LT_Log].fprintf("%s\r\n", m_szBuffer);
	m_clgFile[LT_Log].flush();
}

// 警告
void CLog::warn(const char* pFormat,...){
	if (!pFormat) return;

	CCritLocker lock(m_csLock);
	initialize();

	/*设置黄色字符前景色*/ 
	CConsoleColor clColor(_BIT32(CConsoleColor::color_yellow)|_BIT32(CConsoleColor::color_intensity));

	va_list	argptr;
	va_start(argptr,pFormat);
	_print(pFormat,argptr);
	va_end(argptr);

	createAFile(LT_Warn);
	m_clgFile[LT_Warn].fprintf("%s\r\n",m_szBuffer);
    m_clgFile[LT_Warn].flush();
}

// error 
void CLog::error(const char* pFormat,...){
	if (!pFormat) return;

	CCritLocker lock(m_csLock);
	initialize();

	/*设置红色字符前景色*/ 
	CConsoleColor clColor(_BIT32(CConsoleColor::color_read)|_BIT32(CConsoleColor::color_intensity));

	va_list	argptr;
	va_start(argptr,pFormat);
	_print(pFormat,argptr);
	va_end(argptr);

	createAFile(LT_Error);
	m_clgFile[LT_Error].fprintf("%s\r\n",m_szBuffer);
	m_clgFile[LT_Error].flush();
}
 
// lasterror
void CLog::lastError(const char* pFormat,...){
	if (!pFormat || !*pFormat) return;

	CCritLocker lock(m_csLock);
	initialize();

	/*设置红色字符前景色*/ 
	CConsoleColor clColor(_BIT32(CConsoleColor::color_read)|_BIT32(CConsoleColor::color_intensity));

	char szDate[32]={0};
	getDate(szDate,sizeof(szDate));

	char szStrint[2048] = {0};
	va_list	argptr;
	va_start(argptr,pFormat);
	dVsprintf(szStrint,sizeof(szStrint),pFormat,argptr);
	va_end(argptr);

	memset(m_szBuffer,0,sizeof(m_szBuffer));
	dSprintf(m_szBuffer,sizeof(m_szBuffer),"%s %s",szDate,szStrint);

	long _error = get_error();
	dSprintf(m_szBuffer,sizeof(m_szBuffer),"%s [<ID:%ld>%s]",m_szBuffer,_error,get_error_str(_error));
	dPrintf("%s\n",m_szBuffer);

	createAFile(LT_Error);
	m_clgFile[LT_Error].fprintf("%s\r\n",m_szBuffer);
	m_clgFile[LT_Error].flush();
}
