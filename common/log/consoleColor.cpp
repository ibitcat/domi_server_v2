/*----------------- consoleColor.cpp
*windows:
*前景色:FOREGROUND_BLUE（蓝色）、FOREGROUND_GREEN（绿色）、FOREGROUND_RED（红色）、FOREGROUND_INTENSITY（高亮）
*背景色:BACKGROUND_BLUE（蓝色）、BACKGROUND_GREEN（绿色）、BACKGROUND_RED（红色）、BACKGROUND_INTENSITY（高亮）
*
*
*--------------------------------------------------------------
*linux:\033[颜色;颜色;m
*显示方式:0（默认值）、1（高亮）、22（非粗体）、4（下划线）、24（非下划线）、5（闪烁）、25（非闪烁）、7（反显）、27（非反显）
*前景色:30（黑色）、31（红色）、32（绿色）、 33（黄色）、34（蓝色）、35（洋红）、36（青色）、37（白色）
*背景色:40（黑色）、41（红色）、42（绿色）、 43（黄色）、44（蓝色）、45（洋红）、46（青色）、47（白色）
*------------------------------------------------------------*/

#include "consoleColor.h"
#include "../basic/stringFunctions.h"

#ifdef WIN32
HANDLE	CConsoleColor::g_hConsole	= INVALID_HANDLE_VALUE;

// 前景色
WORD CConsoleColor::g_fore_color[color_max] = {
	FOREGROUND_RED										,//红
	FOREGROUND_GREEN									,//绿
	FOREGROUND_RED | FOREGROUND_GREEN					,//黄
	FOREGROUND_BLUE										,//蓝
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE	,//白
	FOREGROUND_INTENSITY								,//高亮
};

// 背景色
WORD CConsoleColor::g_back_color[color_max] = {
	BACKGROUND_RED										,//红
	BACKGROUND_GREEN									,//绿
	BACKGROUND_RED | BACKGROUND_GREEN					,//黄
	BACKGROUND_BLUE										,//蓝
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE	,//白
	BACKGROUND_INTENSITY								,//高亮
};
#else // WIN32
const char*	CConsoleColor::g_fore_color[CConsoleColor::color_max] =
{
	"31"	,//红
	"32"	,//绿
	"33"	,//黄
	"34"	,//蓝
	"37"	,//白
	"1"		,//高亮
};
const char*	CConsoleColor::g_back_color[CConsoleColor::color_max] =
{
	"41"	,//红
	"42"	,//绿
	"43"	,//黄
	"44"	,//蓝
	"47"	,//白
	"1"		,//高亮
};
#endif // WIN32

// 初始化
bool CConsoleColor::initialize(){
#ifdef WIN32
	if(g_hConsole == INVALID_HANDLE_VALUE || g_hConsole == nullptr){
		g_hConsole= ::GetStdHandle(STD_OUTPUT_HANDLE);
		if(g_hConsole == nullptr || g_hConsole == INVALID_HANDLE_VALUE)
			return false;
	}
#endif // WIN32
	return true;
}

// 关闭 
void CConsoleColor::shutdown(){
#ifdef WIN32
	if(g_hConsole != nullptr && g_hConsole != INVALID_HANDLE_VALUE)
		::CloseHandle(g_hConsole);

	g_hConsole = nullptr;
#endif // WIN32
}

CConsoleColor::CConsoleColor(uint8 _fore,uint8 _bank){
	initialize();
#ifdef WIN32
	m_wColorSave	= 0;
	if(g_hConsole != nullptr && g_hConsole != INVALID_HANDLE_VALUE){
		CONSOLE_SCREEN_BUFFER_INFO stInfo;
		::GetConsoleScreenBufferInfo(g_hConsole, &stInfo);
		m_wColorSave = stInfo.wAttributes;
	}
#endif // WIN32

	setColor(_fore,_bank);
}
 
CConsoleColor::~CConsoleColor(){
	//清除
#ifdef WIN32
	if(g_hConsole != nullptr && g_hConsole != INVALID_HANDLE_VALUE)
		::SetConsoleTextAttribute(g_hConsole,m_wColorSave);
#else
	//dPrintf("\033[0m");
#endif // WIN32
}
 
void CConsoleColor::setColor(uint8 _fore,uint8 _bank){
#ifdef WIN32
	if(g_hConsole != nullptr && g_hConsole != INVALID_HANDLE_VALUE){
		WORD wColor = 0;
		for (int i = 0;i < color_max;i++){
			if(_fore && _CHECK_BIT(_fore,_BIT32(i)))
				wColor |= g_fore_color[i];

			if(!_fore && _CHECK_BIT(m_wColorSave,g_fore_color[i]))
				wColor |= g_fore_color[i];

			if(_bank && _CHECK_BIT(_bank,_BIT32(i)))
				wColor |= g_back_color[i];

			if(!_bank && _CHECK_BIT(m_wColorSave,g_back_color[i]))
				wColor |= g_back_color[i];
		}

		::SetConsoleTextAttribute(g_hConsole,wColor);
	}
#else // WIN32
	bool bFirst = true;
	for (int i = 0;i < color_max;i++){
		if(_CHECK_BIT(_fore,_BIT32(i))){
			if(bFirst){
				bFirst	= false;
				dPrintf("\033[%s",g_fore_color[i]);
			} else {
				dPrintf(";%s",g_fore_color[i]);
			}
		}

		if(_CHECK_BIT(_bank,_BIT32(i))){
			if(bFirst){
				bFirst	= false;
				dPrintf("\033[%s",g_fore_color[i]);
			}else{
				dPrintf(";%s",g_fore_color[i]);
			}
		}
	}
	if(!bFirst)
		dPrintf("m");
#endif // WIN32
}
