/*
CLIENT

1. socket 생성
2. socket 에 server와 통신할 ip, port 설정
3. connect
4. write and read !!
	4.1 read 는 쓰레드로!


*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


void * rcv(void * arg) {
	int sock = (int)arg;
	char msg[200];
	int msg_len = 0;
	while(1) {
		msg_len = read(sock, msg, sizeof(msg));
		if(msg_len == -1 | msg_len == 0) {
			printf("Sock closed :( \n");
			return NULL;
		}
		printf("%s\n", msg);
	}

	pthread_exit(0);
	return NULL;
}

int main(int argc, char **argv) {
	int my_sock;
	int str_len = 0;

	struct sockaddr_in serv_addr;

	pthread_t * thread_t;

	if(argc < 2) {
		printf("Please enter your ID :)\n");
		return 0;
	}


	my_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(7000);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// connect request to server
	if(connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))  == -1   ) {

		printf("Connect error :(");
	}
	char welcome_msg[100];
	str_len = read(my_sock,welcome_msg, sizeof(welcome_msg) - 1);
	if(str_len == -1) {
		printf("Read error :(");
	}
	printf("%s\n", welcome_msg);

	// create thread for receiving messages from server and other users
	pthread_create(&thread_t, NULL, rcv, (void *)my_sock);
	while(1) {
		char my_msg[200];
		char chat[100];
		gets(chat);
		sprintf(my_msg,"[%s] : %s", argv[1], chat);
		printf("%s\n", my_msg);
		write(my_sock, my_msg, strlen(my_msg) + 1);
	}
	close(my_sock);

	return 0;
}
