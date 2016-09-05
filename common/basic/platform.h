#pragma once
#include "basicTypes.h"

// 错误码
extern long get_error();//获得错误码
extern const char*	get_error_str(long _error);//获得错误字符串
extern const char*	get_herror_str();//获得错误字符串(linux下调用时无错误会多余打印输出)
extern const char*	get_herror_str(long _error);//获得错误字符串

// 文件目录相关
extern bool	findDirectory(pc_str _dir);//查询目录是否存在
extern bool	createDirectory(pc_str _dir,char _break = '/');//创建目录