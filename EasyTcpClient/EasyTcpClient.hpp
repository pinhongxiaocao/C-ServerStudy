#ifndef  _EasyTcpClient_hpp
#define _EasyTcpClient_hpp

#define WIN32_LEAN_AND_MEAN 
//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<thread>

#include"MessageHeader.hpp"
using namespace std;

class EasyTcpClient
{
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	//虚析构函数防止内存泄漏
	virtual ~EasyTcpClient()
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
	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		//如果它是一个无效的连接帮它初始化一下
		if (_sock == INVALID_SOCKET)
		{
			InitSocket();
		}

		//2 连接服务器 Connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("错误,连接服务器失败\n");
		}
		else
		{
			printf("连接服务器成功\n");
		}
		return ret;
	}
	//关闭socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
			//7关闭socket
			closesocket(_sock);
			WSACleanup();//与上面那个匹配

			_sock = INVALID_SOCKET;
		}

	}

	//运行中
	bool OnRun()
	{
		if(!isRun())
		{
			return false;
		}

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval t = { 1,0 };
		int ret = select(_sock, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "select任务结束 代号是"<<_sock << endl;
			return false;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == RecvData(_sock))
			{
				std::cout << "select任务结束" << endl;
				return false;
			}
		}
		return true;
	}
	//判断是否在运行
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//收数据
	int RecvData(SOCKET _cSock)
	{
		DataHeader header = {};
		//5 接收服务端
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			std::cout << "与服务器断开连接,任务结束" << endl;
			return -1;
		}
		OnNetMsg(_cSock,&header);
		return 0;
	}

	//处理数据
	virtual void OnNetMsg(SOCKET _cSock,DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult loginResult = {};
			recv(_cSock, (char*)&loginResult + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
			std::cout << "收到服务端消息:CMD_LOGIN_RESULT,数据长度是" << loginResult.dataLength << endl;
		}
		break;
		case CMD_LOGINOUT_RESULT:
		{
			LoginOutResult loginOutResult = {};
			recv(_cSock, (char*)&loginOutResult + sizeof(DataHeader), sizeof(LoginOutResult) - sizeof(DataHeader), 0);
			std::cout << "收到服务端消息:CMD_LOGINOUT_RESULT,数据长度是" << loginOutResult.dataLength << endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin newUserJoin = {};
			recv(_cSock, (char*)&newUserJoin + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
			std::cout << "收到服务端消息:CMD_NEW_USER_JOIN,数据长度是" << newUserJoin.dataLength << endl;
		}
		break;
		}
	}

	//发送数据 DataHeader是所有消息的基类
	int SendData(DataHeader*header)
	{
		if(isRun()&&header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:

};
#endif // ! _EasyTcpClient_hpp

