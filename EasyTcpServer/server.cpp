#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //��ֹת��������Ϊ���ϱ��� 

//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<vector>

#include"EasyTcpServer.hpp"
using namespace std;
//����ͨ������Ŀ�ﶯ̬���� ��Ϊ������ģʽ�²�֧������д��
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
		//cout << "����ʱ�䴦������ҵ��" << endl;
	}

	server.Close();
	cout << "���˳� �������" << endl;
	getchar();
	return 0;
}

