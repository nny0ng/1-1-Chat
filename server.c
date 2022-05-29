#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>

#define BUF_SIZE 100
#define MAX_CLNT 100
#define MAX_IP 30



void *handle_clnt(void *arg,char clnt_adr,char sin_addr);
void send_msg(char *msg, int len);
void error_handling(char *msg);
char *serverState(int count);
void menu(char port[]);
void ban();



/****************************/



int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{

   int serv_sock, clnt_sock;
   struct sockaddr_in serv_adr, clnt_adr;
   int clnt_adr_sz;
   pthread_t t_id;

   /** time log **/
 //  struct tm *t;

   if (argc != 2)
   {
      printf(" Usage : %s <port>\n", argv[0]);
      exit(1);
   }
   

   menu(argv[1]);

   pthread_mutex_init(&mutx, NULL);
   serv_sock = socket(PF_INET, SOCK_STREAM, 0);

   memset(&serv_adr, 0, sizeof(serv_adr));
   serv_adr.sin_family = AF_INET;
   serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_adr.sin_port = htons(atoi(argv[1]));

   if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
      error_handling("bind() error");
   if (listen(serv_sock, 10000) == -1)
      error_handling("listen() error");

   while (1)
   {
      time_t t=time(NULL);
      clnt_adr_sz = sizeof(clnt_adr);
      clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
      
      printf("이번에 들어온 클라이언트 : %d\n",clnt_sock);
      pthread_mutex_lock(&mutx);
      clnt_socks[clnt_cnt++] = clnt_sock;
      pthread_mutex_unlock(&mutx);
 pthread_create(&t_id, NULL, (void*)handle_clnt, (void*)&clnt_sock);

      pthread_detach(t_id);
      printf(" Connceted client IP : %s ", inet_ntoa(clnt_adr.sin_addr));
      printf("%s",ctime(&t));
      printf(" chatter (%d/100)\n", clnt_cnt);
    
	printf("===== Menu =====\n");
	printf("1.강퇴\n");
	printf("2.입장\n");

	int ab;
	
	scanf("%d",&ab);

	switch(ab)
	{
		case 1:
			ban();
			break;
		case 2:
			printf("======넘어가요=====\n");
			break;
	}


   }	
   close(serv_sock);
   return 0;

}

void *handle_clnt(void *arg,char clnt_adr,char sin_addr)
{
   int clnt_sock = *((int*)arg);
   int str_len = 0, i;
   char msg[BUF_SIZE];

   while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
      send_msg(msg, str_len);

   // remove disconnected client
   pthread_mutex_lock(&mutx);
   for (i = 0; i<clnt_cnt; i++)
   {
      if (clnt_sock == clnt_socks[i])
      {
         while (i++<clnt_cnt - 1)
            clnt_socks[i] = clnt_socks[i + 1];
         break;
      }
   }
   clnt_cnt--;
   printf("%c[1;31m",27);
   printf("Client has Disconnect from Server\n");
   printf("%c[0m\n",27);
   pthread_mutex_unlock(&mutx);
   close(clnt_sock);
   return NULL;
}

void send_msg(char* msg, int len)
{
   int i;
   pthread_mutex_lock(&mutx);
   for (i = 0; i<clnt_cnt; i++)
      write(clnt_socks[i], msg, len);
   pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
   fputs(msg, stderr);
   fputc('\n', stderr);
   exit(1);
}

char* serverState(int count)
{
   char* stateMsg = malloc(sizeof(char) * 20);
   strcpy(stateMsg, "None");

   if (count < 5)
      strcpy(stateMsg, "Good");
   else
      strcpy(stateMsg, "Bad");

   return stateMsg;
}

void menu(char port[])
{

   system("clear");
   printf(" **** chat server ****\n");
   printf(" server port    : %s\n", port);
   printf(" server state   : %s\n", serverState(clnt_cnt));
   printf(" max connection : %d\n", MAX_CLNT);
   printf(" ****          Log         ****\n\n");
}


void ban()
{
   
   int ban_ds;
   printf("강퇴할 사람의 클라이언트 디스크립터를 입력하여 주세요.\n");
   scanf("%d",&ban_ds);
   while(getchar()!='\n');
   close(ban_ds);
   printf("%c[1;31m",27);
   printf("Client has Disconnected from Server\n");
   printf("%c[0m\n",27);
}
