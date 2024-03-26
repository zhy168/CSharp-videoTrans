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

//this is server for accept!    ��Ӧ�ʼǱ�   VTrans4

bool init_Socket()
{
	WSADATA wsadata;
	//�ɹ�����0��ʧ�ܷ��ش������
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
	int err = -1;        /*  ����ֵ */
	int s = -1;            /*  socket������ */
	int snd_size = 0;   /*  ���ͻ�������С */
	int rcv_size = 0;    /*  ���ջ�������С */
	socklen_t optlen;    /* ѡ��ֵ���� */

	/*
	 * ����һ��TCP�׽���
	 */
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		printf("�����׽��ִ���\n");
		return -1;
	}

	/*
	 * �ȶ�ȡ���������õ����
	 * ��� ԭʼ���ͻ�������С
	 */
	optlen = sizeof(snd_size);
	err = getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &optlen);
	if (err < 0)
	{
		printf("��ȡ���ͻ�������С����\n");
	}
	/*
	 * ��ӡԭʼ�������������
	 */

	 /*
	  * ��� ԭʼ���ջ�������С
	  */
	optlen = sizeof(rcv_size);
	err = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, &optlen);
	if (err < 0)
	{
		printf("��ȡ���ջ�������С����\n");
	}

	printf(" ���ͻ�����ԭʼ��СΪ: %d �ֽ�\n", snd_size);
	printf(" ���ջ�����ԭʼ��СΪ: %d �ֽ�\n", rcv_size);

	/*
	 *  ���÷��ͻ�������С
	 */
	snd_size = 65536;    /* ���ͻ�������СΪ8K */
	optlen = sizeof(snd_size);
	err = setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, optlen);
	if (err < 0)
	{
		printf("���÷��ͻ�������С����\n");
	}

	/*
	 *  ���ý��ջ�������С
	 */
	rcv_size = 65536;    /* ���ջ�������СΪ8K */
	optlen = sizeof(rcv_size);
	err = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, optlen);
	if (err < 0)
	{
		printf("���ý��ջ�������С����\n");
	}

	/*
	 * ����������������õ����
	 *  ����޸ĺ��ͻ�������С
	 */
	optlen = sizeof(snd_size);
	err = getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &optlen);
	if (err < 0)
	{
		printf("��ȡ���ͻ�������С����\n");
	}

	/*
	 *  ����޸ĺ���ջ�������С
	 */
	optlen = sizeof(rcv_size);
	err = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_size, &optlen);
	if (err < 0)
	{
		printf("��ȡ���ջ�������С����\n");
	}

	/*
	 * ��ӡ���
	 */
	printf(" ���ͻ�������СΪ: %d �ֽ�\n", snd_size);
	printf(" ���ջ�������СΪ: %d �ֽ�\n", rcv_size);

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

	//�˿ڸ��ã��������Ӻ������ӻ���ʱ�䡣
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
	memset(&server_addr, 0, sizeof(server_addr)); //����ַ��������
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.117");
	inet_pton(AF_INET, "192.168.1.18", (void*)&server_addr.sin_addr.S_un.S_addr);

	//�󶨱���IP
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

	//3.listen ����
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
		//4.accept ��ѭ���еȴ�����
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		SOCKET connfd = accept(serverfd, (struct sockaddr*)&client_addr, &length); //����������Ҫѭ��
		if (INVALID_SOCKET == connfd)
		{
			err("4.accept");
			closesocket(connfd);
			//break;
		}
		else
			std::cout << "4.accept succeed" << std::endl;
		Mat image;


		//5.receive����
		while (1)
		{
			//int n = recv(connfd, buf, sizeof(buf), 0);//���ܻ���
			char frame_cnt[10] = { 0, };

			//��һ�����г���
			recv(connfd, frame_cnt, 10, 0);
			int cnt = atoi(frame_cnt);
			printf("\nnSize = %d\n", cnt);
			std::vector<uchar> data_decode;
			data_decode.resize(cnt);
			int index = 0;//��ʾ�������ݳ��ȼ���
			int count = cnt;//��ʾ����Ҫ�ӽ��ջ����������ֽڵ�����
			char* recv_char = new char[cnt];//�½�һ���ֽ����� ���鳤��ΪͼƬ�ֽڳ���
			while (count > 0)//����ֻ��дcount > 0 ���дcount >= 0 ��ôwhileѭ��������һ����ѭ��
			{
				//������ͨ����  recv ����һ���Խ��յ����ֽ�������С�ڵ����趨��SIZE��С����ʱ������Ҫ���recv
				int nSize = recv(connfd, recv_char, count, 0);
				for (int k = 0; k < nSize; k++)
				{
					index++;
					if (index >= cnt) { break; }
				}
				memcpy(&data_decode[index - nSize], recv_char, nSize);
				if (!nSize) { return -1; }
				count -= nSize;//����������Ҫ�ӽ��ջ��������յ��ֽ�����
			}
			delete[]recv_char;

			try
			{
				image = cv::imdecode(data_decode, CV_LOAD_IMAGE_COLOR);
				if (!image.empty())
				{
					namedWindow("ˮ��ͼ��", WINDOW_NORMAL);
					imshow("ˮ��ͼ��", image);
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
















