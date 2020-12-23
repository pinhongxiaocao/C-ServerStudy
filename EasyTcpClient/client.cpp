#define WIN32_LEAN_AND_MEAN 
//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>

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
#pragma endregion

int main()
{
	//启动Windows 2.x网络环境
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	//传递数据指针
	WSADATA dat;
	//启动 
	WSAStartup(ver, &dat);
	//**********************//
	//1 建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET==_sock)
	{
		printf("错误,建立socket失败\n");
	}
	else
	{
		printf("建立socket成功\n");
	}
	//2 连接服务器 Connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR==ret)
	{
		printf("错误,连接服务器失败\n");
	}
	else
	{
		printf("连接服务器成功\n");
	}

	while (true)
	{
		//3 输入请求命令
		char cmdBuf[128] = {};
		cin >> cmdBuf;
		//4 处理请求命令
		if(0==strcmp(cmdBuf,"exit"))
		{
			cout << "收到退出命令" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lala");
			strcpy(login.passWord, "lala123");
			//5向服务器发送请求命令
			send(_sock, (const char*)&login,sizeof(Login),0);
			//接收服务器返回的数据
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			cout << "登入的结果是" << loginRet.result;
		}
		else if(0 == strcmp(cmdBuf, "loginout"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "lala");
			//5 向服务器发送请求命令
			send(_sock, (const char*)&loginOut, sizeof(LoginOut), 0);
			//接收服务器返回的数据
			LoginOutResult loginOutRet = {};
			recv(_sock, (char*)&loginOutRet, sizeof(LoginOutResult), 0);
			cout << "登出的结果是" << loginOutRet.result<<endl;
		}
		else
		{
			cout << "不支持的命令,请重新输入" << endl;
		}
	}
	//7关闭socket
	closesocket(_sock);
	//**********************//
	WSACleanup();//与上面那个匹配
	getchar();
	return 0;
}