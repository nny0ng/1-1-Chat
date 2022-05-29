#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>

#define BUF_SIZE 100
#define NORMAL_SIZE 20

void* send_msg(void* arg);
void* namecolor(void *arg, int cl);
void* recv_msg(void* arg);
void error_handling(char* msg);

void menu();
void changeName();
void menuOptions();

char name[NORMAL_SIZE] = "[DEFAULT]";     // name
char msg_form[NORMAL_SIZE];            // msg form
char serv_time[NORMAL_SIZE];        // server time
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // server port number
char clnt_ip[NORMAL_SIZE];            // client ip address


int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void* thread_return;

	if (argc != 4)
	{
		printf("%c[1;31m", 27);
		printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
		printf("%c[0m\n", 27);
		exit(1);
	}

	/** local time **/
	time_t t = time(NULL);
	printf("%s\n", ctime(&t));
	sprintf(name, "[%s]", argv[3]);
	sprintf(clnt_ip, "%s", argv[1]);
	sprintf(serv_port, "%s", argv[2]);
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling(" connect() error");

	/** call menu **/
	menu();

	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	close(sock);
	return 0;
}

void* send_msg(void* arg)
{
	int cl;
	int sock = *((int*)arg);
	char name_msg[NORMAL_SIZE + BUF_SIZE];
	char myInfo[BUF_SIZE];
	char* who = NULL;
	char temp[BUF_SIZE];

	/** send join messge **/
	printf(" >> join the chat !! \n");
	sprintf(myInfo, "%s's join. IP_%s", name, clnt_ip);
	write(sock, myInfo, strlen(myInfo));

	printf("------------------------------------------------------------\n");
	printf("%-15.15s", "WHITE:1");
	printf("%-15.15s", "GRAY:2");
	printf("%-15.15s", "RED:3");
	printf("%-15.15s", "YELLOW:4");
	printf("\n%-15.15s", "GREEN:5");
	printf("%-15.15s", "CYAN:6");
	printf("%-15.15s", "BLUE:7");
	printf("%-15.15s", "PURPLE:8");
	printf("\n------------------------------------------------------------\n");

	printf("글자의 색상을 선택하세요 :  ");
	scanf("%d", &cl);

	while (1)
	{
		time_t t = time(NULL);
		fgets(msg, BUF_SIZE, stdin);

		// menu_mode command -> !menu
		if (!strcmp(msg, "!menu\n"))
		{
			menuOptions();
			continue;
		}

		else if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}

		// send message
		namecolor(arg, cl);
	}
	return NULL;
}

void* namecolor(void *arg, int cl) {
	int sock = *((int*)arg);
	time_t t = time(NULL);
	char name_msg[NORMAL_SIZE + BUF_SIZE];
	switch (cl) {
	case 1:
		sprintf(name_msg, "%c[1;37m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 2:
		sprintf(name_msg, "%c[1;30m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 3:
		sprintf(name_msg, "%c[1;31m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 4:
		sprintf(name_msg, "%c[1;33m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 5:
		sprintf(name_msg, "%c[1;32m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 6:
		sprintf(name_msg, "%c[1;36m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 7:
		sprintf(name_msg, "%c[1;34m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	case 8:
		sprintf(name_msg, "%c[1;35m %s %s%s\n", 27, name, msg, ctime(&t));
		write(sock, name_msg, strlen(name_msg));
		break;
	}
}

void* recv_msg(void* arg)
{
	int sock = *((int*)arg);
	char name_msg[NORMAL_SIZE + BUF_SIZE];
	int str_len;

	while (1)
	{
		str_len = read(sock, name_msg, NORMAL_SIZE + BUF_SIZE - 1);
		if (str_len == -1)
			return (void*)-1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

void menuOptions()
{
	int select;
	// print menu
	printf("%c[1;33m", 27);
	printf("\n\t**** menu mode ****\n");
	printf("\t1. change name\n");
	printf("\t2. clear/update\n\n");
	printf("\tthe other key is cancel");
	printf("\n\t*******************");
	printf("\n\t>> ");
	printf("%c[0m\n", 27);
	scanf("%d", &select);
	getchar();
	switch (select)
	{
		// change user name
	case 1:
		changeName();
		break;

		// console update(time, clear chatting log)
	case 2:
		menu();
		break;

		// menu error
	default:
		printf("\tcancel.");
		break;
	}
}

/** change user name **/
void changeName()
{
	char nameTemp[100];
	printf("%c[1;36m", 27);
	printf("\n\tInput new name -> ");
	scanf("%s", nameTemp);
	sprintf(name, "[%s]", nameTemp);
	printf("\n\tComplete.\n\n");
	printf("%c[0m\n", 27);
}

void menu()
{
	system("clear");
	printf("%c[1;35m", 27);
	printf(" **** chatting client ****\n");
	printf(" server port : %s \n", serv_port);
	printf(" client IP   : %s \n", clnt_ip);
	printf(" chat name   : %s \n", name);
	printf(" server time : %s \n", serv_time);
	printf("%c[0m\n", 27);
	printf("%c[1;33m", 27);
	printf(" ************* menu ***************\n");
	printf(" if you want to select menu -> !menu\n");
	printf(" 1. change name\n");
	printf(" 2. clear/update\n");
	printf(" **********************************\n");
	printf(" Exit -> q & Q\n\n");
	printf("%c[0m\n", 27);
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
