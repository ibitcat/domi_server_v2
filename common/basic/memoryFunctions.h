// 内存操作相关函数

#pragma once
#include <iostream>
#include <stdio.h>
#include "basicTypes.h"

#ifdef WIN32	//WIN32
extern int32 dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);  //内存拷贝
extern int32 dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount); //内存移动
#else	// linux
extern void* dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);  //内存拷贝
extern void* dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount); //内存移动
#endif
