/******************************************************************** 
创建时间:        2015/06/22 14:07
文件名称:        main.cpp
文件作者:        Domi
功能说明:        main   
其他说明:         
*********************************************************************/

#include <iostream>
#include "logic/domiServer.h"
#include "common/basic/timeFunction.h"

int main(int argc, char* argv[])
{
	if (!CDomiServer::GetSingleton().initialize(7)){
		printf("CGameServer::Initialize()失败!\n");
		return 0;
	}
	
	if (!CDomiServer::GetSingleton().startServices()) {
		printf(":StartServices()启动服务失败!\n");
	}
	CDomiServer::GetSingleton().stopServices();
	dSleep(1000);
	return 0;
}