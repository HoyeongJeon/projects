/*
SERVER!

1. create socket
2. bind socket
3. listen socket
4. accept client
5. read , send all
	5.1 read using thread
6. close

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>

#define CLNT_MAX 10

int g_clnt_socks[CLNT_MAX];
int g_clnt_count = 0;

pthread_mutex_t g_mutex =  PTHREAD_MUTEX_INITIALIZER;


void send_msg_to_all_clnt(char * msg, int my_sock) {

	int sock = my_sock;
	// write msg to all users except sender

	pthread_mutex_lock(&g_mutex);
	for(int i = 0; i < g_clnt_count; i++) {
		if(g_clnt_socks[i] != sock) {
			write(g_clnt_socks[i], msg, strlen(msg) +1);
		}
	}
	pthread_mutex_unlock(&g_mutex);

}

void * clnt_connection(void *arg) {
	printf("You are in the clnt connection!!\n");
	int clnt_sock = (int)arg;
	int msg_len = 0;
	char user_chat[200];
	char * user_exit = "Chat member left\n";
	while(1) {
		// read message from clinet
		msg_len = read(clnt_sock, user_chat, sizeof(user_chat));

		if( msg_len == -1) {
			printf("Unable to read User chat...:( \n");
			return NULL;
		} else if (msg_len != 0) {
			printf("%s\n", user_chat);
			send_msg_to_all_clnt(user_chat, clnt_sock);

		} else {
			// if a user exit, reduce one user in sockets list
			printf("%s", user_exit);
				send_msg_to_all_clnt(user_exit, clnt_sock);

			pthread_mutex_lock(&g_mutex);
			for(int i = 0; i < g_clnt_count; i++) {
				if(clnt_sock == g_clnt_socks[i]) {
					for(i = 0; i < g_clnt_count - 1; i++) {
						g_clnt_socks[i] = g_clnt_socks[i + 1];
					}
				}
			}
			pthread_mutex_unlock(&g_mutex);

			break;

		}
	}

}

int main(int argc, char **argv) {
	int serv_sock;
	int clnt_sock;


	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	pthread_t * thread_t;
	// socket create
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	// socket setting
	memset(&serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(7000);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	// socket binding
	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)  {
		printf("Bind Error :(\n");
	}


	//socket listen
	if(listen(serv_sock, 5) == -1) {
		printf("Listen Error :(\n");
	}

	socklen_t * clnt_addr_size = sizeof(clnt_addr);
	char welcome_msg[] = "Hello! Welcome to the chat room!\n";
	int user_chat_len = 0;

	while(1) {
		// accept request from client
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, (socklen_t *)&clnt_addr_size);
		if(clnt_sock  == -1) {
			printf("Accept Error :(\n");
		}
		printf("Someone enter the server!:) \n");
		write(clnt_sock , welcome_msg, strlen(welcome_msg) + 1);

		pthread_mutex_lock(&g_mutex);
		g_clnt_socks[g_clnt_count++] = clnt_sock;
		pthread_mutex_unlock(&g_mutex);

		// create thread for maintaing connection with clients
		pthread_create(&thread_t, NULL, clnt_connection, (void *)clnt_sock);

	}



	close(serv_sock);
	close(clnt_sock);

	return 0;
}
