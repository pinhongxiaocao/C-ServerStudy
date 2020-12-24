#ifndef _EasyTcpServer_hpp
#define _EasyTcpServer_hpp

#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //防止转换函数因为过老报错 

//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<vector>

#include"MessageHeader.hpp"

using namespace std;
class EasyTcpServer
{
	SOCKET _sock;
	std::vector<SOCKET> g_clients;
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;

	}

	virtual~EasyTcpServer()
	{
		Close();
	}

	//初始化Socket
	void InitSocket()
	{
		//启动Windows 2.x网络环境
		//创建版本号
		WORD ver = MAKEWORD(2, 2);
		//传递数据指针
		WSADATA dat;
		//启动 
		WSAStartup(ver, &dat);
		//如果之前已经被初始化了 就关掉它
		if (_sock != INVALID_SOCKET)
		{
			cout << "关闭了之前的连接" << endl;
			Close();
		}
		//建立一个socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("错误,建立socket失败\n");
		}
		else
		{
			printf("建立socket成功\n");
		}
	}

	//绑定端口号
	int Bind(const char* ip, unsigned short port)
	{
		//2 bind 绑定一个用户客户端连接的网络端口
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;//流
		_sin.sin_port = htons(port);//端口 为了在网络流里面类型不被改
							 //是 host to net unsigned short的缩写 
		if (ip)
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//ip 地址 随意的ip地址公网主机都可以访问
		}
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("绑定一个用户客户端连接的网络端口失败...\n");
		}
		else
		{
			printf("绑定成功...");
		}
		return ret;
	}
	//关闭socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			//把socket们关闭掉
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);
			}
			//6.关闭套接字
			closesocket(_sock);
			//*****************//
			WSACleanup();//与上面那个匹配
		}

	}

	//监听端口号
	int Listen(int n)
	{
		//3 listen 监听网络端口 (绑定可能不成功 所以要分为两步)
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)//后面是最大多少人连接
		{
			printf("监听网络端口失败\n");
		}
		else
		{
			printf("监听网络端口成功\n");
		}
		return ret;
	}

	//接受客户端连接 默认返回一下我们创建的socket
	SOCKET Accept()
	{
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
		NewUserJoin userJoin;
		SendDataToAll(&userJoin);
		g_clients.push_back(_cSock);
		printf("新的客户端加入:IP= %s", inet_ntoa(clientAddr.sin_addr));
		return _sock;
	}

	//处理网络消息
	bool OnRun()
	{
		if (!isRun())
		{
			return false;
		}

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

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		//nfds 是一个整数值 是指 set集合中所有描述符(socket)的范围 而不是数量
		//是所有文件描述符最大值+1 在windows中这个参数可以写0
		timeval t = { 0,0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			cout << "select,任务结束" << endl;
			Close();
			return false;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
		//循环处理进程
		for (size_t n = 0; n < fdRead.fd_count; ++n)
		{
			if (-1 == ReceiveData(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
		return true;
	}

	//判断是否在运行
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//接受数据
	int ReceiveData(SOCKET _cSock)
	{
		DataHeader header = {};
		//5 接收客户端数据
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("客户端已经退出,任务结束");
			return -1;
		}

		OnNetMsg(_cSock, &header);
	}

	//响应网络消息
	virtual void OnNetMsg(SOCKET _cSock, DataHeader* header)
	{
		switch (header->cmd)
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
			header->cmd = CMD_ERROR;
			header->dataLength = 0;
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
		break;
		}
	}


	//单发送数据 DataHeader是所有消息的基类
	int SendData(SOCKET _cSock, DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	//群发送数据
	void SendDataToAll(DataHeader* header)
	{
		//有新客户端加入就群发给其他所有客户端
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			SendData(g_clients[n], header);
		}
	}
private:

};


#endif // !_EasyTcpServer_hpp

