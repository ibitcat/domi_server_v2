#include "basicFunctions.h"
#include "stringFunctions.h"
#include "memoryFunctions.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <locale.h>
#ifdef WIN32
#include <windows.h>
#include <mbctype.h>
#else
#endif // WIN32
#include <string.h>
#include <time.h>

#pragma warning(disable:4996)
std::string g_strLocale = "zh_CN.utf8";

void set_locale(pc_str _locale){
	if(_locale)
		g_strLocale = _locale;
	else
		g_strLocale.clear();
}

pc_str get_locale(){
	return g_strLocale.c_str();
}

uint32 dStrlen(const char *s){
	return ::strlen(s);
}
#ifdef WIN32
int32 dStrcat(char *dst, uint32 _size, const char *src) {
	return ::strcat_s(dst, _size, src);
}
#else // WIN32
char* dStrcat(char *dst, uint32 _size, const char *src) {
	return ::strcat(dst, src);
}
#endif // WIN32

#ifdef WIN32
int32 dStrncat(char *dst,uint32 _size, const char *src,uint32 _count){
	return ::strncat_s(dst,_size,src,_count);
}
#else // WIN32
char* dStrncat(char *dst,uint32 _size, const char *src,uint32 _count){
	return ::strncat(dst,src,_count);
}
#endif // WIN32

#ifdef WIN32
int32 dStrcpy(char *dst,uint32 _size, const char *src){
	return ::strcpy_s(dst,_size,src);
}   
#else // WIN32
char* dStrcpy(char *dst,uint32 _size, const char *src){
	return ::strcpy(dst,src);
}   
#endif // WIN32

#ifdef WIN32
int32 dStrncpy(char *dst,uint32 _size, const char *src, uint32 len){
	return ::strncpy_s(dst,_size,src,len);
}
#else // WIN323
char* dStrncpy(char *dst,uint32 _size, const char *src, uint32 len)
{
	return ::strncpy(dst,src,len);
}
#endif // WIN32

extern int32 dStrcpyMax(char *dst,uint32 _size, const char *src, uint32 len){
	if(len >= _size) len = _size - 1;
	dMemcpy(dst,_size,src,len);
	return len;
}

void* dMemchr(void *buf,char ch,uint32 count){
	return ::memchr(buf,ch,count);
}

char* dStrchr(char *s,char c){
	return ::strchr(s,c);
}

const char*	dStrchr(const char *s,char c){
	return ::strchr(s,c);
}

char* dStrtok(char *s, const char *delim){
	return ::strtok(s,delim);
}

char* dStrlwr(char *s){
#ifdef WIN32
	return ::_strlwr(s);
#else // WIN32
	if(s){
		char * cp;
		for (cp=s; *cp; ++cp){
			if ('A' <= *cp && *cp <= 'Z')
				*cp += 'a' - 'A';
		}
	}
	return(s);
#endif // WIN32
}

char* dStrupr(char *s){
#ifdef WIN32
	return ::_strupr(s);
#else // WIN32
	if(s){
		char *cp;
		for ( cp = s ; *cp ; ++cp ){
			if ( ('a' <= *cp) && (*cp <= 'z') )
				*cp -= 'a' - 'A';
		}
	}
	return(s);
#endif // WIN32
}

int32 ismbblead(char c){
	/*是否宽字节*/ 
#ifdef WIN32
	return ::_ismbblead(c);
#else // WIN32
	return ((unsigned char)(c) & 0x04);
	/*if(( (unsigned char )test_array[k]>=0xA1 && (unsigned char )test_array[k]<=0xF7)
		&&
		((unsigned char )test_array[k+1]>=0xA1&&(unsigned char )test_array[k+1]<=0xFE))
	{	
		if(begin==0)
			begin=k;
		tmp_array[i]=test_array[k];
		tmp_array[i+1]=test_array[k+1];
		k=k+2;
		i=i+2;
		have_chinese=1;
		if(flag==1)
			have_chinese=2;
	}*/
#endif // WIN32
}

// 字符串比较(区分大小写)
int	dStrncmp(const char *s1,const char * s2,int n){
	return ::strncmp(s1,s2,n);
}

// 字符串比较(不区分大小写)
int	dStrnicmp(const char *s1,const char * s2,int n){
#ifdef WIN32
	return ::_strnicmp(s1,s2,n);
#else // WIN32
	//return strcasecmp(s1,s2);
	if(s1 && s2 && n > 0){
		int f=0;
		int l=0;
		do
		{
			if ( ((f = (unsigned char)(*(s1++))) >= 'A') &&
				(f <= 'Z') )
				f -= 'A' - 'a';

			if ( ((l = (unsigned char)(*(s2++))) >= 'A') &&
				(l <= 'Z') )
				l -= 'A' - 'a';

		}
		while ( --n && f && (f == l) );
		return ( f - l );
	}
	return 0;
#endif // WIN32
}

//字符串查找第一次出现的位置(区分大小写)
char* dStrstr(char *haystack,char *needle){
	return ::strstr(haystack,needle);
}

//字符串查找第一次出现的位置(不区分大小写)
char* dStristr(char *haystack,char *needle){
	//strcasestr(haystack, needle);
	size_t len = ::dStrlen(needle);
	if(len == 0) return haystack;	/* 这里我并未照strstr一样返回str，而是返回NULL*/

	while(*haystack){
		/* 这里使用了可限定比较长度的strnicmp*/
		if(dStrnicmp(haystack, needle, len) == 0)
			return haystack;
		haystack++;
	}
	return nullptr;
}

int	dSprintf(char *string,size_t sizeInBytes,const char *format, ... ){
	if(!string || !sizeInBytes || !format) return 0;

	int iRes = 0;
	va_list	argptr;
	va_start(argptr,format);
#ifdef WIN32
	iRes = ::vsprintf_s(string,sizeInBytes,format,argptr);
#else // WIN32
	iRes = ::vsprintf(string,format,argptr);
#endif // WIN32
	va_end(argptr);
	return iRes;
}

int	dVsprintf(char *string,size_t sizeInBytes,const char *format,va_list _Args){
	if(!string || !sizeInBytes || !format) return 0;
	int iRes = 0;
#ifdef WIN32
	iRes = ::vsprintf_s(string,sizeInBytes,format,_Args);
#else // WIN32
	iRes = ::vsprintf(string,format,_Args);
#endif // WIN32
	return iRes;
}

int	dPrintf(const char *format, ... ){
	if(!format)	return 0;
	int iRes = 0;
	va_list	argptr;
	va_start(argptr,format);
#ifdef WIN32
	iRes = ::vprintf_s(format,argptr);
#else // WIN32
	iRes = ::vprintf(format,argptr);
#endif // WIN32
	va_end(argptr);
	return iRes;
}

int	dVprintf(const char *format,va_list _Args){
	if(!format) return 0;
	int iRes = 0;
#ifdef WIN32
	iRes = ::vprintf_s(format,_Args);
#else // WIN32
	iRes = ::vprintf(format,_Args);
#endif // WIN32
	return iRes;
}

bool testMultibyte(char* pStr){
	if (!pStr || !*pStr) return false;
	uint32 uLen			= (uint32)::strlen(pStr);
	pStr[uLen + 1]		= 0;
	uint32 uLastIndex	= 0;
	uint32 uMultibyte	= 0;

	for (uint32 i = uLen;i > 0;i--) {
		if (!pStr[i - 1]) continue;
		/*是否宽字节*/ 
		if (!ismbblead(pStr[i - 1]))break;
		uMultibyte++;
		if (!uLastIndex) uLastIndex	= i - 1;
	}
	if (!uMultibyte || uMultibyte % 2 == 0) return true;
	pStr[uLastIndex]	= 0;
	return false;
}

void wipeOffChar(char* pStr,uint32 uLen,char cChar){
	if (!pStr || !uLen) return;

	pStr[uLen] = 0;
	uint32	uCount = 0;
	/*1、清除所有字符*/ 
	for (uint32 i = 0;i < uLen;i++){
		if (pStr[i] == 0) break;
		if (pStr[i] != cChar) continue;
		uCount++;
		pStr[i] = 0;
	}
	if (!uCount) return;

	/*2、移动*/ 
	uint32 uMove = 0;
	for (uint32 i = 0;i < uLen;i++){
		if (uMove >= uCount) break;
		if (pStr[i] != 0)continue;
		uMove++;
		if (i != uLen - 1)
			dMemmove(pStr + i,uLen - i,pStr + i + 1,uLen - i - 1);
	}
	pStr[uLen - uCount] = 0;
}

void filtrationBlank(char* pStr,uint32 uLen){
	if (!pStr || !uLen) return;
	pStr[uLen] = 0;

	/*1.将所有头部空格全部变为0*/ 
	for (uint32 i = 0;i < uLen;i++){
		/*遇到0跳出*/ 
		if (pStr[i] == 0) break;
		if (pStr[i] == ' '){
			pStr[i] = 0;
			continue;
		}

		if (pStr[i] == -95){
			if (i < uLen && pStr[i+1] == -95){
				pStr[i] = 0;
				pStr[++i] = 0;
				continue;
			}
		}

		/*1.将所有尾部空格全部变为0*/ 
		for (uint32 j = uLen - 1;j > i;j--){
			if (pStr[j] == 0) continue;
			if(pStr[j] == ' '){
				pStr[j] = 0;
				continue;
			}
			if(pStr[j] == -95){
				if (j > i && pStr[j-1] == -95){
					pStr[j] = 0;
					pStr[--j] = 0;
					continue;
				}
			}
			break;
		}
		break;
	}

	/*3.将0之后的数据移动到前面来*/ 
	for (uint32 i = 0;i < uLen;i++){
		if (pStr[i] == 0) continue;
		dMemmove(pStr,uLen,pStr + i,uLen - i);
		break;
	}
}

bool numeralString(const char* pStr,int32 nLen){
	if (!pStr || nLen <= 0) return  false;
	for (int32 i = 0;i < nLen;i++){
		if (pStr[i] < '0' || pStr[i] > '9') return false;
	}
	return true;
}

bool haveFromString(std::string&strString,char c){
	int32 iFind = strString.find(c);
	return (iFind != std::string::npos);
}

void getSubString(std::string&strString,std::string&strSubString,char c){
	int32 iFind = strString.find(c);
	if(iFind == std::string::npos) iFind = strString.length();

	//内容段begin
	strSubString = strString.substr(0,iFind);
	strString.erase(0,iFind+1);
}

void truncateString(std::string&strString,char c){
	int32 iFind = strString.find(c);
	if(iFind == std::string::npos) iFind = strString.length();
	strString.erase(0,iFind+1);
}

int32 readInt32FromString(std::string&strString,char c){
	int32 iRelsult = dAtoi(strString.c_str());
	truncateString(strString,c);
	return iRelsult;
}

int64 readInt64FromString(std::string&strString,char c){
	int64 iRelsult = dAtoll(strString.c_str());
	truncateString(strString,c);
	return iRelsult;
}

uint64 readDateTimeFromString(std::string&strString){//[年-月-日 时:分:秒]
	if(strString.empty())return 0;

	tm tmTime;
	memset(&tmTime,0,sizeof(tmTime));

	int32 iTemp;
	std::string strTemp;
	//年
	getSubString(strString,strTemp,'-');
	iTemp = readInt32FromString(strTemp);
	if(iTemp > 1900)
		tmTime.tm_year = iTemp - 1900;
	else
		return 0;

	//月
	getSubString(strString,strTemp,'-');
	iTemp = readInt32FromString(strTemp);
	if(iTemp > 0)
		tmTime.tm_mon = iTemp - 1;
	else
		return 0;
	//日
	getSubString(strString,strTemp,' ');
	iTemp = readInt32FromString(strTemp);
	if(iTemp > 0 && iTemp <= 31)
		tmTime.tm_mday = iTemp;
	else
		return 0;

	//时
	getSubString(strString,strTemp,':');
	iTemp = readInt32FromString(strTemp);
	if(iTemp >= 0 && iTemp <= 23)
		tmTime.tm_hour = iTemp;
	else
		return 0;

	//分
	getSubString(strString,strTemp,':');
	iTemp = readInt32FromString(strTemp);
	if(iTemp >= 0 && iTemp <= 59)
		tmTime.tm_min = iTemp;
	else
		return 0;

	//秒
	getSubString(strString,strTemp);
	iTemp = readInt32FromString(strTemp);
	if(iTemp >= 0 && iTemp <= 59)
		tmTime.tm_sec = iTemp;
	else
		return 0;

	return ::mktime(&tmTime);
}

bool getSubString(std::string&strString,std::string&strSubString,const char*pStrtok){
	if(strString.empty() ||!pStrtok) return false;
	int32 iFind = strString.find(pStrtok);
	if(iFind == std::string::npos) return false;

	uint32 uLen = dStrlen(pStrtok);
	//内容段begin
	strSubString = strString.substr(iFind+uLen);
	strString.erase(iFind);

	return true;
}

strarray splitStr ( const char* src, char lexpr, char rexpr ){
    strarray strs;
    if ( src == nullptr || src[ 0 ] == 0 )  return std::move( strs );

    int lpos = 0;
    int rpos = 0;
    int tpos = 0;
    if ( lexpr != 0 && rexpr != 0 ){
        do{
            if ( src[ lpos ] == lexpr ){
                rpos = lpos + 1;
                tpos = rpos;
                while ( src[ rpos ] != 0 ){
                    if ( src[ rpos ] == rexpr ) {
                        strs.resize( strs.size() + 1 );
                        std::string& tmp = strs.back();
                        tmp.resize( rpos - tpos );
                        memcpy( &tmp[ 0 ], &src[ tpos ], rpos - tpos );
                        break;
                    }
                    ++rpos;
                }

                lpos = rpos;
                if ( src[ lpos ] == 0 ) break;
            }
            ++lpos;

        } while ( src[ lpos ] != 0 );
    }else if ( lexpr != 0 ) {
        do{
            if ( src[ lpos ] == lexpr || src[ lpos + 1 ] == 0 ){
                strs.resize( strs.size() + 1 );
                std::string& tmp = strs.back();
                tmp.resize( lpos - tpos );
                memcpy( &tmp[ 0 ], &src[ tpos ], lpos - tpos );

                tpos = lpos + 1;
                if ( src[ tpos ] == 0 ) {
                    tmp += src[ lpos ];
                    break;
                }
            }
            ++lpos;
        } while ( src[ lpos ] != 0 );
    } else {
        // error
    }
    return std::move( strs );
}

extern strarray divideStr( const char* src, char token ){
	strarray vecStr;
	if ( src == nullptr || src[ 0 ] == 0 ) return std::move( vecStr );

	int lpos = 0;
	int tpos = 0;
	do{
		if ( src[ lpos ] == token || src[ lpos + 1 ] == 0 ){
			if (src[tpos] == token){
				tpos = lpos + 1;
			}else{
				vecStr.resize( vecStr.size() + 1 );
				std::string& tmp = vecStr.back();
				tmp.resize( lpos - tpos );
				memcpy( &tmp[ 0 ], &src[ tpos ], lpos - tpos );

				tpos = lpos + 1;
				if ( src[ tpos ] == 0 ){
					tmp += src[ lpos ];
					break;
				}
			}
		}
		++lpos;
	} while ( src[ lpos ] != 0 );
	return std::move( vecStr );
}

bool convertANSItoUTF8(char* pAnsiString,int32 nLen){
	if(!pAnsiString || nLen <= 0) return false;

#ifdef WIN32
	//------------------------------ ANSI转UNICODE
	int32 wcsLen = ::MultiByteToWideChar(CP_ACP, 0, pAnsiString, strlen(pAnsiString), nullptr, 0);

	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间 
	wchar_t* wszString = new wchar_t[wcsLen + 1]; 
	::MultiByteToWideChar(CP_ACP, 0, pAnsiString, strlen(pAnsiString), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	//------------------------------ UNICODE转UTF8
	int32 u8Len = WideCharToMultiByte(CP_UTF8, 0, wszString, wcslen(wszString), nullptr, 0, nullptr, nullptr);
	if(nLen < u8Len) u8Len = nLen - 1;

	::WideCharToMultiByte(CP_UTF8, 0, wszString, wcslen(wszString), pAnsiString, u8Len, nullptr, nullptr);
	pAnsiString[u8Len] = '\0';
	delete[] wszString;
	return (nLen < u8Len);
#else
	return true;
#endif // WIN32
}

bool convertUTF8toANSI(char* pUtf8String,int32 nLen){
	if(!pUtf8String || nLen <= 0) return false;

#ifdef WIN32
	//------------------------------ UTF8转UNICODE
	int32 wcsLen = ::MultiByteToWideChar(CP_UTF8, 0, pUtf8String, strlen(pUtf8String), nullptr, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1]; 
	::MultiByteToWideChar(CP_UTF8, 0, pUtf8String, strlen(pUtf8String), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	//------------------------------ UNICODE转ANSI
	int32 ansiLen = ::WideCharToMultiByte(CP_ACP, 0, wszString, wcslen(wszString), nullptr, 0, nullptr, nullptr);
	if(nLen < ansiLen) ansiLen = nLen - 1;

	//unicode版对应的strlen是wcslen 
	::WideCharToMultiByte(CP_ACP, 0, wszString, wcslen(wszString), pUtf8String, ansiLen, nullptr, nullptr);
	pUtf8String[ansiLen] = '\0';

	delete[] wszString;
	return (nLen < ansiLen);
#else
	return true;
#endif // WIN32
}

char* _convertANSItoUTF8(const char* pAnsiString,int32 nLen){
	static char	szText[1024 * 4] = {0};
	memset(szText,0,sizeof(szText));
	if(pAnsiString){
		dStrcpy(szText,sizeof(szText),pAnsiString);
		convertANSItoUTF8(szText,nLen);
	}
	return szText;
}

char* _convertUTF8toANSI(const char* pUtf8String,int32 nLen){
	static char	szText[1024 * 4] = {0};
	memset(szText,0,sizeof(szText));
	if(pUtf8String){
		dStrcpy(szText,sizeof(szText),pUtf8String);
		convertUTF8toANSI(szText,nLen);
	}

	return szText;
}

std::wstring&convertToWString(const char *str){
	static std::wstring wString;

	wString.clear();
	if(!str || !*str) return wString;

#ifdef WIN32
	setlocale(LC_ALL, "");
#else // WIN32
	setlocale(LC_ALL, g_strLocale.c_str());
#endif // WIN32

	size_t _len = dStrlen(str);
	size_t _Len = mbstowcs(nullptr,str,_len);
	if(_Len > 0 && (_len * 3) > _Len){
		wString.resize(_Len);
		size_t total = mbstowcs(&wString[0],str,_Len);
	}
	return wString;

	/*
	mbtowc 和 wctomb 是单个字符相互转换
	int len;
	setlocale (LC_ALL, "chs");  //设置为简体中文环境
	wchar_t wc = L''''中''
	wprintf(L"1个宽中文字符：%c \n",wc);
	char* p = "中";
	len = mbtowc (&wc, p, MB_LEN_MAX);
	wprintf(L"单字符串转换为1个宽字符：%c 长度： %d\n",wc,len);
	char pcmb[MB_LEN_MAX];
	len = wctomb (pcmb, wc);
	pcmb[len] = 0;
	printf("宽字符转换为单字符串：%s 长度:%d\n",pcmb,len);

	BYTE utf8[1024];        //utf8 字符串
	wchar_t wstr[1024];
	char mstr[1024];
	//UTF-8 转换为宽字符
	MultiByteToWideChar( CP_UTF8, 0, utf8,1024, wstr, sizeof(wstr)/sizeof(wstr[0]) );
	WideCharToMultiByte( CP_ACP,0,wstr,-1,mstr,1024,NULL,NULL );
	注：mbstowcs()是C库函数，要正确的设置Locale才能进行转换，MultiByteToWideChar()是win32函数，别搞混了！
	*/
}

std::string&convertToCString(std::wstring&str){
#ifdef WIN32
	setlocale(LC_ALL, "");
#else // WIN32
	setlocale(LC_ALL, g_strLocale.c_str());
#endif // WIN32
	static std::string cString;
	cString.clear();

	size_t _Len = wcstombs(nullptr,str.c_str(),str.length());
	if(_Len > 0){
		cString.resize(_Len);
		size_t total = wcstombs(&cString[0],str.c_str(),_Len);
	}

	//cString.resize(str.length() * 2 + 1);
	//size_t total = wcstombs(&cString[0],str.c_str(),str.length() * 2);
	return cString;
}

void replaceString(std::string& strString, const char* szSrc, const char* szDst){
	std::string::size_type pos		= 0;
	std::string::size_type srclen	= strlen(szSrc);
	std::string::size_type dstlen	= strlen(szDst);

	while((pos = strString.find(szSrc, pos)) != std::string::npos){
		strString.replace(pos, srclen, szDst);
		pos += dstlen;
	}
}