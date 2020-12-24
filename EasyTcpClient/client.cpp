#define WIN32_LEAN_AND_MEAN 
//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<thread>
#include"EasyTcpClient.hpp"
using namespace std;
//建议通过在项目里动态加入 因为在其他模式下不支持这种写法
#pragma comment (lib,"ws2_32.lib") 
//这里要进行前后端 字节对齐 考虑平台和系统


void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		//3 输入请求命令
		char cmdBuf[128] = {};
		cin >> cmdBuf;
		//4 处理请求命令
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			std::cout << "收到退出命令,退出输入线程" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lala");
			strcpy(login.passWord, "lala123");
			//5向服务器发送请求命令
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "loginout"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "lala");
			//5 向服务器发送请求命令
			client->SendData(&loginOut);
		}
		else
		{
			std::cout << "不支持的命令,请重新输入" << endl;
		}
	}

	
}
int main()
{
	EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1", 4567);
	//启动线程
	std::thread t1(cmdThread,&client);
	t1.detach();

	while (client.isRun())
	{
		client.OnRun();
	}

	client.Close();
	std::cout << "退出客户端程序" << endl;
	getchar();
	return 0;
}