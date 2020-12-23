#define WIN32_LEAN_AND_MEAN 
//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>

using namespace std;
//����ͨ������Ŀ�ﶯ̬���� ��Ϊ������ģʽ�²�֧������д��
#pragma comment (lib,"ws2_32.lib") 
//����Ҫ����ǰ��� �ֽڶ��� ����ƽ̨��ϵͳ
#pragma region ��Ϣ�ṹ��
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_ERROR
};

struct DataHeader //��Ϣͷ
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
	//����Windows 2.x���绷��
	//�����汾��
	WORD ver = MAKEWORD(2, 2);
	//��������ָ��
	WSADATA dat;
	//���� 
	WSAStartup(ver, &dat);
	//**********************//
	//1 ����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET==_sock)
	{
		printf("����,����socketʧ��\n");
	}
	else
	{
		printf("����socket�ɹ�\n");
	}
	//2 ���ӷ����� Connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR==ret)
	{
		printf("����,���ӷ�����ʧ��\n");
	}
	else
	{
		printf("���ӷ������ɹ�\n");
	}

	while (true)
	{
		//3 ������������
		char cmdBuf[128] = {};
		cin >> cmdBuf;
		//4 ������������
		if(0==strcmp(cmdBuf,"exit"))
		{
			cout << "�յ��˳�����" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lala");
			strcpy(login.passWord, "lala123");
			//5�������������������
			send(_sock, (const char*)&login,sizeof(Login),0);
			//���շ��������ص�����
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			cout << "����Ľ����" << loginRet.result;
		}
		else if(0 == strcmp(cmdBuf, "loginout"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "lala");
			//5 �������������������
			send(_sock, (const char*)&loginOut, sizeof(LoginOut), 0);
			//���շ��������ص�����
			LoginOutResult loginOutRet = {};
			recv(_sock, (char*)&loginOutRet, sizeof(LoginOutResult), 0);
			cout << "�ǳ��Ľ����" << loginOutRet.result<<endl;
		}
		else
		{
			cout << "��֧�ֵ�����,����������" << endl;
		}
	}
	//7�ر�socket
	closesocket(_sock);
	//**********************//
	WSACleanup();//�������Ǹ�ƥ��
	getchar();
	return 0;
}