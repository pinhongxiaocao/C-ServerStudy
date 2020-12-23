#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //��ֹת��������Ϊ���ϱ��� 

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
	//*****************//
	//1����һ��socket �׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2 bind ��һ���û��ͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;//��
	_sin.sin_port = htons(4567);//�˿� Ϊ�����������������Ͳ�����
						 //�� host to net unsigned short����д 
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip ��ַ �����ip��ַ�������������Է���
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)))
	{
		printf("��һ���û��ͻ������ӵ�����˿�ʧ��...\n");
	}
	else
	{
		printf("�󶨳ɹ�...");
	}
	//3 listen ��������˿� (�󶨿��ܲ��ɹ� ����Ҫ��Ϊ����)
	if (SOCKET_ERROR == listen(_sock, 5))//������������������
	{
		printf("��������˿�ʧ��\n");
	}
	else
	{
		printf("��������˿ڳɹ�\n");
	}
	//4.accept �ȴ����ܿͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;//��ʼ������Ч�ĵ�ַ
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("����,���յ�����Ч�ͻ���SOCKET...\n");
	}
	printf("�µĿͻ��˼���:IP= %s", inet_ntoa(clientAddr.sin_addr));


	while (true)
	{
		DataHeader header = {};
		//5 ���տͻ�������
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ����Ѿ��˳�,�������");
			break;
		}
		
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
			cout << "�յ������� " << login.cmd << "���ݳ���" << login.dataLength << endl;
			cout << "�˺���" << login.userName << "������" << login.passWord<<endl;
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//.......
			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			LoginOut loginOut = {};
			recv(_cSock, (char*)&loginOut + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
			cout << "�յ������� " << loginOut.cmd << "���ݳ���" << loginOut.dataLength << endl;
			cout << "�˺���" << loginOut.userName <<endl;
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//.......
			LoginOutResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginOutResult), 0);
		}
		break;
		default://Ĭ���Ǵ��������
		{
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
		break;
		}
	}
	//6.�ر��׽���
	closesocket(_sock);
	//*****************//
	WSACleanup();//�������Ǹ�ƥ��
	cout << "���˳� �������" << endl;
	getchar();
	return 0;
}

