#ifndef _EasyTcpServer_hpp
#define _EasyTcpServer_hpp

#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //��ֹת��������Ϊ���ϱ��� 

//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
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

	//�󶨶˿ں�
	int Bind(const char* ip, unsigned short port)
	{
		//2 bind ��һ���û��ͻ������ӵ�����˿�
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;//��
		_sin.sin_port = htons(port);//�˿� Ϊ�����������������Ͳ�����
							 //�� host to net unsigned short����д 
		if (ip)
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//ip ��ַ �����ip��ַ�������������Է���
		}
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("��һ���û��ͻ������ӵ�����˿�ʧ��...\n");
		}
		else
		{
			printf("�󶨳ɹ�...");
		}
		return ret;
	}
	//�ر�socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			//��socket�ǹرյ�
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);
			}
			//6.�ر��׽���
			closesocket(_sock);
			//*****************//
			WSACleanup();//�������Ǹ�ƥ��
		}

	}

	//�����˿ں�
	int Listen(int n)
	{
		//3 listen ��������˿� (�󶨿��ܲ��ɹ� ����Ҫ��Ϊ����)
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)//������������������
		{
			printf("��������˿�ʧ��\n");
		}
		else
		{
			printf("��������˿ڳɹ�\n");
		}
		return ret;
	}

	//���ܿͻ������� Ĭ�Ϸ���һ�����Ǵ�����socket
	SOCKET Accept()
	{
		//4.accept �ȴ����ܿͻ�������
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _cSock = INVALID_SOCKET;//��ʼ������Ч�ĵ�ַ
		_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cSock)
		{
			printf("����,���յ�����Ч�ͻ���SOCKET...\n");
		}
		//���¿ͻ��˼����Ⱥ�����������пͻ���
		NewUserJoin userJoin;
		SendDataToAll(&userJoin);
		g_clients.push_back(_cSock);
		printf("�µĿͻ��˼���:IP= %s", inet_ntoa(clientAddr.sin_addr));
		return _sock;
	}

	//����������Ϣ
	bool OnRun()
	{
		if (!isRun())
		{
			return false;
		}

		//������ socket fd_set socket����
		fd_set fdRead;
		fd_set fdWrite;//�����select������Ŀǰ�ò��� �����п��Դ���
		fd_set fdExp;//�����select������Ŀǰ�ò��� �����п��Դ���

		//���fd_set
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		//����
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		//nfds ��һ������ֵ ��ָ set����������������(socket)�ķ�Χ ����������
		//�������ļ����������ֵ+1 ��windows�������������д0
		timeval t = { 0,0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			cout << "select,�������" << endl;
			Close();
			return false;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
		//ѭ���������
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

	//�ж��Ƿ�������
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//��������
	int ReceiveData(SOCKET _cSock)
	{
		DataHeader header = {};
		//5 ���տͻ�������
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ����Ѿ��˳�,�������");
			return -1;
		}

		OnNetMsg(_cSock, &header);
	}

	//��Ӧ������Ϣ
	virtual void OnNetMsg(SOCKET _cSock, DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
			cout << "�յ������� " << login.cmd << "���ݳ���" << login.dataLength << endl;
			cout << "�˺���" << login.userName << "������" << login.passWord << endl;
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//.......
			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			LoginOut loginOut = {};
			recv(_cSock, (char*)&loginOut + sizeof(DataHeader), sizeof(LoginOut) - sizeof(DataHeader), 0);
			cout << "�յ������� " << loginOut.cmd << "���ݳ���" << loginOut.dataLength << endl;
			cout << "�˺���" << loginOut.userName << endl;
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//.......
			LoginOutResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginOutResult), 0);
		}
		break;
		default://Ĭ���Ǵ��������
		{
			header->cmd = CMD_ERROR;
			header->dataLength = 0;
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
		break;
		}
	}


	//���������� DataHeader��������Ϣ�Ļ���
	int SendData(SOCKET _cSock, DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	//Ⱥ��������
	void SendDataToAll(DataHeader* header)
	{
		//���¿ͻ��˼����Ⱥ�����������пͻ���
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			SendData(g_clients[n], header);
		}
	}
private:

};


#endif // !_EasyTcpServer_hpp

