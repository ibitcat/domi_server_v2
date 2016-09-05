#include <stdarg.h>
#include <stdio.h>
#include "fileStream.h"

// 文件操作定义
const char	CFileStream::m_gszAccess[Access_Max][5] ={
	"rb",	// 读文件
	"wb",	// 写文件
	"rb+",	// 读写文件
	"ab+",	// 追加
};

CFileStream::CFileStream(){
	m_pFile	= nullptr;
}

CFileStream::~CFileStream(){
	close();
}

// 打开文件
bool CFileStream::open(const char*pszFileName,_enAccess eAccess){
	if(eAccess >= Access_Max) eAccess = _Read;
	return open(pszFileName,m_gszAccess[eAccess]);
}

// 打开文件
bool CFileStream::open(const char*pszFileName,const char* pszMode){
	if(!pszFileName)return false;
	close();
#ifdef WIN32
	::fopen_s(&m_pFile,pszFileName,pszMode ? pszMode : "rb");
#else
	m_pFile = ::fopen(pszFileName,pszMode ? pszMode : "rb");
#endif
	return (m_pFile != nullptr);
}

//关闭
void CFileStream::close(){	
	if(m_pFile) ::fclose(m_pFile);
	m_pFile = nullptr;
}

//更新缓冲区
void CFileStream::flush(){
	//确保写入到文件-不一定是磁盘但是别处可以读取
	if(m_pFile) fflush(m_pFile);
}

// 获得文件长度
uint32	CFileStream::getFileLength(){
	if(!m_pFile) return 0;

	int32 nTell = ftell(m_pFile);
	if(seek(0,SEEK_END) != 0){
		seek(nTell,SEEK_SET);
		return 0;
	}

	uint32 uLength = position();
	seek(nTell,SEEK_SET);
	return uLength;
}

//获得当前文件指针位置
uint32	CFileStream::position(){
	if(!m_pFile) return 0;
	return ::ftell(m_pFile);
}

//清除文件
int	CFileStream::clear(){
	if(!m_pFile) return 0;
	return ::fflush(m_pFile);
}

//是否已到文件尾
bool CFileStream::eof(){
	if(!m_pFile) return true;
	return (::feof(m_pFile) != 0);
}

/*
**偏移
**|SEEK_SET | SEEK_CUR		| SEEK_END	|
**|文件开头 | 当前读写的位置| 文件尾	|
*/
int	CFileStream::seek(int32 _n8Offset, int _Origin){
	if(!m_pFile) return 0;
	return ::fseek(m_pFile,_n8Offset,_Origin);
}

//读取
bool CFileStream::_read(uint32 uBytes,void* outBuffer){
	if(!m_pFile || !uBytes || !outBuffer) return 0;
	return (::fread(outBuffer,1,uBytes,m_pFile) == uBytes);
}

//写入
bool CFileStream::_write(uint32 uBytes,const void*inBuffer){
	if(!m_pFile || !uBytes || !inBuffer) return 0;
	return (::fwrite(inBuffer,1,uBytes,m_pFile) == uBytes);
}

// 读取出uMaxBytes个字节
uint32 CFileStream::fread(uint32 uMaxBytes,void* outBuffer){
	if(!m_pFile || !uMaxBytes || !outBuffer) return 0;
	return ::fread(outBuffer,1,uMaxBytes,m_pFile);
}

uint32 CFileStream::fwrite(uint32 uMaxBytes,const void*inBuffer){
	if(!m_pFile || !uMaxBytes || !inBuffer) return 0;
	return ::fwrite(inBuffer,1,uMaxBytes,m_pFile);
}

uint32 CFileStream::fprintf(const char* pszFormat,...){
	if(!m_pFile || !pszFormat) return 0;

	va_list argptr;
	va_start(argptr, pszFormat);
#ifdef WIN32
	uint32 uLen = ::vfprintf_s(m_pFile,pszFormat,argptr);
#else//WIN32
	uint32 uLen = ::vfprintf(m_pFile,pszFormat,argptr);
#endif//WIN32
	va_end(argptr);
	return uLen;
}
