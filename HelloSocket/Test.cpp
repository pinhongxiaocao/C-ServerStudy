#define WIN32_LEAN_AND_MEAN 
//����������ֹwin��ĺ��ض���
//��Ҳ���Ե������ǵ�����˳�� ���ǲ��Ƽ�
#include<Windows.h>
#include<WinSock2.h>


//#pragma comment (lib,"ws2_32.lib") 
//����ͨ������Ŀ�ﶯ̬���� ��Ϊ������ģʽ�²�֧������д��
int main()
{
	//����Windows 2.x���绷��
	//�����汾��
	WORD ver = MAKEWORD(2, 2);
	//��������ָ��
	WSADATA dat;
	//���� 
	WSAStartup(ver,&dat);



	WSACleanup();//�������Ǹ�ƥ��
	
	return 0;
}