#pragma once
#include "../basic/basicTypes.h"
#ifdef WIN32	// WIN32
#include <windows.h>
#endif 

class CConsoleColor
{
public:
	enum _emColor{
		color_read		,//红
		color_green		,//绿
		color_yellow	,//黄
		color_blue		,//蓝
		color_white		,//白
		color_intensity	,//高亮
		color_max
	};
private:
#ifdef WIN32
	static HANDLE		g_hConsole;
	static WORD			g_fore_color[color_max];
	static WORD			g_back_color[color_max];
#else // WIN32
	static const char*	g_fore_color[color_max];
	static const char*	g_back_color[color_max];
#endif // WIN32

private:
#ifdef WIN32
	WORD	m_wColorSave;
#endif // WIN32

public:
	CConsoleColor(uint8 _fore,uint8 _bank = 0);
	virtual~CConsoleColor();

public:
	static bool	initialize();
	static void	shutdown();

	void setColor(uint8 _fore,uint8 _bank = 0);
};
