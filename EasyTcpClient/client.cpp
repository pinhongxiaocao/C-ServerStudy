#define WIN32_LEAN_AND_MEAN 
//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<stdio.h>
#include<thread>
#include"EasyTcpClient.hpp"
using namespace std;
//����ͨ������Ŀ�ﶯ̬���� ��Ϊ������ģʽ�²�֧������д��
#pragma comment (lib,"ws2_32.lib") 
//����Ҫ����ǰ��� �ֽڶ��� ����ƽ̨��ϵͳ


void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		//3 ������������
		char cmdBuf[128] = {};
		cin >> cmdBuf;
		//4 ������������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			std::cout << "�յ��˳�����,�˳������߳�" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lala");
			strcpy(login.passWord, "lala123");
			//5�������������������
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "loginout"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "lala");
			//5 �������������������
			client->SendData(&loginOut);
		}
		else
		{
			std::cout << "��֧�ֵ�����,����������" << endl;
		}
	}

	
}
int main()
{
	EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1", 4567);
	//�����߳�
	std::thread t1(cmdThread,&client);
	t1.detach();

	while (client.isRun())
	{
		client.OnRun();
	}

	client.Close();
	std::cout << "�˳��ͻ��˳���" << endl;
	getchar();
	return 0;
}