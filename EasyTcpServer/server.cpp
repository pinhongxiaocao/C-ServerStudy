#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //防止转换函数因为过老报错 

//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<vector>

#include"EasyTcpServer.hpp"
using namespace std;
//建议通过在项目里动态加入 因为在其他模式下不支持这种写法
#pragma comment (lib,"ws2_32.lib") 




int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);

	while (server.isRun())
	{
		server.OnRun();
		//cout << "空闲时间处理其他业务" << endl;
	}

	server.Close();
	cout << "已退出 任务结束" << endl;
	getchar();
	return 0;
}

