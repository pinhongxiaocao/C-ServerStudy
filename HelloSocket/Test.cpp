#define WIN32_LEAN_AND_MEAN 
//加入这个宏防止win库的宏重定义
//你也可以调换他们的引用顺序 但是不推荐
#include<Windows.h>
#include<WinSock2.h>


//#pragma comment (lib,"ws2_32.lib") 
//建议通过在项目里动态加入 因为在其他模式下不支持这种写法
int main()
{
	//启动Windows 2.x网络环境
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	//传递数据指针
	WSADATA dat;
	//启动 
	WSAStartup(ver,&dat);



	WSACleanup();//与上面那个匹配
	
	return 0;
}