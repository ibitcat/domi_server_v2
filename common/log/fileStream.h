#pragma once
#include <iostream>
#include <memory.h>
#include "../basic/basicTypes.h"

class CFileStream
{
public:
	enum _enAccess{
		_Read		,/*读*/ 
		_Write		,/*写*/ 
		_ReadWrite	,/*读写*/ 
		_Append		,/*追加*/ 
		Access_Max
	};

protected:
	static const char m_gszAccess[Access_Max][5];

protected:
	FILE*	m_pFile;

public:
	inline bool	isOpen() {return (m_pFile != nullptr);}
	inline FILE* getFile() {return m_pFile;}

public:
	CFileStream();
	virtual~CFileStream();

public:
	bool open(const char*pszFileName,_enAccess eAccess);			// 打开文件
	bool open(const char*pszFileName, const char* pszMode = "rb");	// 打开文件
	void close();													// 关闭文件
	void flush();													// 更新缓冲区

public:
	uint32	getFileLength();							// 获得文件长度
	int	clear();										// 清除文件
	int	seek(int32 _nOffset, int _Origin = SEEK_SET);	// 偏移(成功返回0)

public:
	inline int seekBegin(int32 _nOffset)	{return seek(_nOffset,SEEK_SET);}	// 从文件头偏移
	inline int seekEnd(int32 _nOffset)		{return seek(_nOffset,SEEK_END);}	// 从文件尾偏移
	inline int seekCurrent(int32 _nOffset)	{return seek(_nOffset,SEEK_CUR);}	// 从文件当前位置偏移 

public:
	virtual uint32	size()	{return getFileLength();}
	virtual bool	skip(int32 _Bytes) { return (seekCurrent(_Bytes) == 0); }
	virtual bool	setpos(uint32 newPosition) { return (seekBegin(newPosition) == 0); }
	virtual uint32	position();
	virtual bool	eof();

protected:
	virtual bool	_read(uint32 uBytes,void* outBuffer);
	virtual bool	_write(uint32 uBytes,const void*inBuffer);

public:
	virtual uint32	fread(uint32 uMaxBytes,void* outBuffer);
	virtual uint32	fwrite(uint32 uMaxBytes,const void*inBuffer);
	virtual uint32	fprintf(const char* pszFormat,...);
};
