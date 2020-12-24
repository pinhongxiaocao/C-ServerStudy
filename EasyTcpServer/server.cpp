#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //防止转换函数因为过老报错 

//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<vector>


using namespace std;
//建议通过在项目里动态加入 因为在其他模式下不支持这种写法
#pragma comment (lib,"ws2_32.lib") 
//这里要进行前后端 字节对齐 考虑平台和系统
#pragma region 消息结构体
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader //消息头
{
	short dataLength;
	short cmd;
};

//DataPackage
struct Login :public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};
struct LoginResult :public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct LoginOut :public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult :public DataHeader
{
	LoginOutResult()
	{
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 1;
	}
	int result;
};
struct NewUserJoin :public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok=0;
	}
	int scok;
};
#pragma endregion

vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
	DataHeader header = {};
	//5 接收客户端数据
	int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		printf("客户端已经退出,任务结束");
		return -1;
	}

	switch (header.cmd)
	{
	case CMD_LOGIN:
	{
		Login login = {};
		recv(_cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		cout << "收到了命令 " << login.cmd << "数据长度" << login.dataLength << endl;
		cout << "账号是" << login.userName << "密码是" << login.passWord << endl;
		//忽略判断用户密码是否正确的过程
		//.......
		LoginResult ret;
		send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{
		LoginOut loginOut = {};
		recv(_cSock, (char*)&loginOut + sizeof(DataHeader), sizeof(LoginOut) - sizeof(DataHeader), 0);
		cout << "收到了命令 " << loginOut.cmd << "数据长度" << loginOut.dataLength << endl;
		cout << "账号是" << loginOut.userName << endl;
		//忽略判断用户密码是否正确的过程
		//.......
		LoginOutResult ret;
		send(_cSock, (char*)&ret, sizeof(LoginOutResult), 0);
	}
	break;
	default://默认是错误的数据
	{
		header.cmd = CMD_ERROR;
		header.dataLength = 0;
		send(_cSock, (char*)&header, sizeof(header), 0);
	}
	break;
	}
}

int main()
{
	//启动Windows 2.x网络环境
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	//传递数据指针
	WSADATA dat;
	//启动 
	WSAStartup(ver, &dat);
	//*****************//
	//1建立一个socket 套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2 bind 绑定一个用户客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;//流
	_sin.sin_port = htons(4567);//端口 为了在网络流里面类型不被改
						 //是 host to net unsigned short的缩写 
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip 地址 随意的ip地址公网主机都可以访问
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)))
	{
		printf("绑定一个用户客户端连接的网络端口失败...\n");
	}
	else
	{
		printf("绑定成功...");
	}
	//3 listen 监听网络端口 (绑定可能不成功 所以要分为两步)
	if (SOCKET_ERROR == listen(_sock, 5))//后面是最大多少人连接
	{
		printf("监听网络端口失败\n");
	}
	else
	{
		printf("监听网络端口成功\n");
	}
	


	while (true)
	{
		//伯克利 socket fd_set socket集合
		fd_set fdRead;
		fd_set fdWrite;//这个在select方法里目前用不到 方法中可以传空
		fd_set fdExp;//这个在select方法里目前用不到 方法中可以传空

		//清空fd_set
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		//放置
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n =(int)g_clients.size()-1; n>=0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		//nfds 是一个整数值 是指 set集合中所有描述符(socket)的范围 而不是数量
		//是所有文件描述符最大值+1 在windows中这个参数可以写0
		timeval t = { 1,0};
		int ret= select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if(ret<0)
		{
			cout << "select,任务结束" << endl;
			break;
		}
		if(FD_ISSET(_sock,&fdRead))
		{
			FD_CLR(_sock, &fdRead);
			//4.accept 等待接受客户端连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;//初始化是无效的地址
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("错误,接收到了无效客户端SOCKET...\n");
			}
			//有新客户端加入就群发给其他所有客户端
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				NewUserJoin userJoin;
				send(g_clients[n], (const char *)&userJoin, sizeof(NewUserJoin),0);
			}
			g_clients.push_back(_cSock);
			printf("新的客户端加入:IP= %s", inet_ntoa(clientAddr.sin_addr));
		}
		//循环处理进程
		for (size_t n = 0; n < fdRead.fd_count; ++n)
		{
			if(-1==processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(),fdRead.fd_array[n]);
				if(iter!=g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
		//cout << "空闲时间处理其他业务" << endl;
	}

	//把socket们关闭掉
	for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	//6.关闭套接字
	closesocket(_sock);
	//*****************//
	WSACleanup();//与上面那个匹配
	cout << "已退出 任务结束" << endl;
	getchar();
	return 0;
}

