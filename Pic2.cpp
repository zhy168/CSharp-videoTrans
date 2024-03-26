#include<WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include<stdbool.h>
#include<iostream>
#include<string>
#include<opencv2/core.hpp>
#include<opencv.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#define WDSIZE 10000
#define PORT 8899
#define err(errMsg) printf("[line:%d] %s failed code %d \n",__LINE__,errMsg,WSAGetLastError) 

using namespace cv;
//using namespace std;

//this is server for accept!    对应笔记本   VTrans4

bool init_Socket()
{
	WSADATA wsadata;
	//成功返回0；失败返回错误代码
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("WSAStartup failed code %d", WSAGetLastError());
		return false;
	}
	return true;
}

bool  close_Socket()
{
	if (0 != WSACleanup())
	{
		printf("WSAStartup failed code %d", WSAGetLastError());
		return false;
	}
	return true;
}

int f1()
{
	int err = -1;        /*  返回值 */
	int s = -1;            /*  socket描述符 */
	int snd_size = 0;   /*  发送缓冲区大小 */
	int rcv_size = 0;    /*  接收缓冲区大小 */
	socklen_t optlen;    /* 选项值长度 */

	/*
	 * 建立一个TCP套接字
	 */
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		printf("建立套接字错误\n");
		return -1;
	}

	/*
	 * 先读取缓冲区设置的情况
	 * 获得 原始发送缓冲区大小
	 */
	optlen = sizeof(snd_size);
	err = getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &optlen);
	if (err < 0)
	{
		printf("获取发送缓冲区大小错误\n");
	}
	/*
	 * 打印原始缓冲区设置情况
	 */

	 /*
	  * 获得 原始接收缓冲区大小
	  */
	optlen = sizeof(rcv_size);
	err = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, &optlen);
	if (err < 0)
	{
		printf("获取接收缓冲区大小错误\n");
	}

	printf(" 发送缓冲区原始大小为: %d 字节\n", snd_size);
	printf(" 接收缓冲区原始大小为: %d 字节\n", rcv_size);

	/*
	 *  设置发送缓冲区大小
	 */
	snd_size = 65536;    /* 发送缓冲区大小为8K */
	optlen = sizeof(snd_size);
	err = setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, optlen);
	if (err < 0)
	{
		printf("设置发送缓冲区大小错误\n");
	}

	/*
	 *  设置接收缓冲区大小
	 */
	rcv_size = 65536;    /* 接收缓冲区大小为8K */
	optlen = sizeof(rcv_size);
	err = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, optlen);
	if (err < 0)
	{
		printf("设置接收缓冲区大小错误\n");
	}

	/*
	 * 检查上述缓冲区设置的情况
	 *  获得修改后发送缓冲区大小
	 */
	optlen = sizeof(snd_size);
	err = getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &optlen);
	if (err < 0)
	{
		printf("获取发送缓冲区大小错误\n");
	}

	/*
	 *  获得修改后接收缓冲区大小
	 */
	optlen = sizeof(rcv_size);
	err = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, &optlen);
	if (err < 0)
	{
		printf("获取接收缓冲区大小错误\n");
	}

	/*
	 * 打印结果
	 */
	printf(" 发送缓冲区大小为: %d 字节\n", snd_size);
	printf(" 接收缓冲区大小为: %d 字节\n", rcv_size);

	closesocket(s);
	return 0;
}


int main()
{
	init_Socket();

	//f1();

	//1.socket
	SOCKET serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == serverfd)
	{
		err("1.socket");
		return INVALID_SOCKET;
	}
	else
		std::cout << "1.socket succeed" << std::endl;

	//端口复用，避免连接后两分钟缓冲时间。
	int opt = 1;
	setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

	//2.bind
	/*_In_ SOCKET s,
		_In_reads_bytes_(namelen) const struct sockaddr FAR* name,
		_In_ int namelen*/
		/*SOCKET_in
		USHORT sin_port;
		IN_ADDR sin_addr;
		CHAR sin_zero[8];*/

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr)); //将地址内容清零
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.117");
	inet_pton(AF_INET, "192.168.1.18", (void*)&server_addr.sin_addr.S_un.S_addr);

	//绑定本机IP
	/*if (SOCKET_ERROR == bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		err("bind");
		return false;
	}*/

	if (0 != bind(serverfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("2.Bind failed code %d\n", WSAGetLastError());
		closesocket(serverfd);
		return -1;
	}
	else
		printf("2.bind succeed\n");

	//3.listen 监听
	if (0 != listen(serverfd, 5))
	{
		err("3.listen");
		closesocket(serverfd);
		return -1;
	}
	else
	{
		printf("3.listen succeed\n");
		printf("---------- on listening -------------\n");
	}

	//while (1)
	{
		//4.accept 死循环中等待连接
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		SOCKET connfd = accept(serverfd, (struct sockaddr*)&client_addr, &length); //阻塞，不需要循环
		if (INVALID_SOCKET == connfd)
		{
			err("4.accept");
			closesocket(connfd);
			//break;
		}
		else
			std::cout << "4.accept succeed" << std::endl;
		Mat image;


		//5.receive接收
		while (1)
		{
			//int n = recv(connfd, buf, sizeof(buf), 0);//接受缓存
			char frame_cnt[10] = { 0, };

			//收一个队列长度
			recv(connfd, frame_cnt, 10, 0);
			int cnt = atoi(frame_cnt);
			printf("\nnSize = %d\n", cnt);
			std::vector<uchar> data_decode;
			data_decode.resize(cnt);
			int index = 0;//表示接收数据长度计量
			int count = cnt;//表示的是要从接收缓冲区接收字节的数量
			char* recv_char = new char[cnt];//新建一个字节数组 数组长度为图片字节长度
			while (count > 0)//这里只能写count > 0 如果写count >= 0 那么while循环会陷入一个死循环
			{
				//在网络通信中  recv 函数一次性接收到的字节数可能小于等于设定的SIZE大小，这时可能需要多次recv
				int nSize = recv(connfd, recv_char, count, 0);
				for (int k = 0; k < nSize; k++)
				{
					index++;
					if (index >= cnt) { break; }
				}
				memcpy(&data_decode[index - nSize], recv_char, nSize);
				if (!nSize) { return -1; }
				count -= nSize;//更新余下需要从接收缓冲区接收的字节数量
			}
			delete[]recv_char;

			try
			{
				image = cv::imdecode(data_decode, CV_LOAD_IMAGE_COLOR);
				if (!image.empty())
				{
					namedWindow("水下图像", WINDOW_NORMAL);
					imshow("水下图像", image);
					waitKey(1);
					data_decode.clear();
				}
				else
				{
					std::cout << "image is empty" << std::endl;
					data_decode.clear();
					continue;
				}
			}
			catch (Exception & e)
			{
				Sleep(1000);
				std::cout << e.what() << std::endl;
				data_decode.clear();
				continue;
			}
			//Sleep(500);
			//closesocket(connfd);
		}
		//closesocket(serverfd);
	}
	return 0;
}
















