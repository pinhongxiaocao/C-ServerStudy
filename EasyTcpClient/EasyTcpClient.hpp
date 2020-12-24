#ifndef  _EasyTcpClient_hpp
#define _EasyTcpClient_hpp

#define WIN32_LEAN_AND_MEAN 
//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
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
	//������������ֹ�ڴ�й©
	virtual ~EasyTcpClient()
	{
		Close();
	}
	//��ʼ��Socket
	void InitSocket()
	{
		//����Windows 2.x���绷��
		//�����汾��
		WORD ver = MAKEWORD(2, 2);
		//��������ָ��
		WSADATA dat;
		//���� 
		WSAStartup(ver, &dat);
		//���֮ǰ�Ѿ�����ʼ���� �͹ص���
		if (_sock != INVALID_SOCKET)
		{
			cout << "�ر���֮ǰ������" << endl;
			Close();
		}
		//����һ��socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("����,����socketʧ��\n");
		}
		else
		{
			printf("����socket�ɹ�\n");
		}
	}
	//���ӷ�����
	int Connect(const char* ip, unsigned short port)
	{
		//�������һ����Ч�����Ӱ�����ʼ��һ��
		if (_sock == INVALID_SOCKET)
		{
			InitSocket();
		}

		//2 ���ӷ����� Connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("����,���ӷ�����ʧ��\n");
		}
		else
		{
			printf("���ӷ������ɹ�\n");
		}
		return ret;
	}
	//�ر�socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
			//7�ر�socket
			closesocket(_sock);
			WSACleanup();//�������Ǹ�ƥ��

			_sock = INVALID_SOCKET;
		}

	}

	//������
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
			std::cout << "select������� ������"<<_sock << endl;
			return false;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == RecvData(_sock))
			{
				std::cout << "select�������" << endl;
				return false;
			}
		}
		return true;
	}
	//�ж��Ƿ�������
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//������
	int RecvData(SOCKET _cSock)
	{
		DataHeader header = {};
		//5 ���շ����
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			std::cout << "��������Ͽ�����,�������" << endl;
			return -1;
		}
		OnNetMsg(_cSock,&header);
		return 0;
	}

	//��������
	virtual void OnNetMsg(SOCKET _cSock,DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult loginResult = {};
			recv(_cSock, (char*)&loginResult + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
			std::cout << "�յ��������Ϣ:CMD_LOGIN_RESULT,���ݳ�����" << loginResult.dataLength << endl;
		}
		break;
		case CMD_LOGINOUT_RESULT:
		{
			LoginOutResult loginOutResult = {};
			recv(_cSock, (char*)&loginOutResult + sizeof(DataHeader), sizeof(LoginOutResult) - sizeof(DataHeader), 0);
			std::cout << "�յ��������Ϣ:CMD_LOGINOUT_RESULT,���ݳ�����" << loginOutResult.dataLength << endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin newUserJoin = {};
			recv(_cSock, (char*)&newUserJoin + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
			std::cout << "�յ��������Ϣ:CMD_NEW_USER_JOIN,���ݳ�����" << newUserJoin.dataLength << endl;
		}
		break;
		}
	}

	//�������� DataHeader��������Ϣ�Ļ���
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

