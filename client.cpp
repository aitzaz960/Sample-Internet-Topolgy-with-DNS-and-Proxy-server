#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<vector>
using namespace std;

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	int choice=0;
	int ports[3]={6000, 5000, 4000};
	char buff[128] = {0};	
	assert(sockfd != -1 );

	do
	{
		printf("Enter the server you want to connect to:\n1. Server#1\n2. Server#2\n3. Server#3\nEnter your choice:\t");
		scanf("%d", &choice);
	} while (choice < 1 || choice >3);
	
	printf("Functional Port of your entered choice is= %d\n", ports[choice-1]);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(ports[choice-1]);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Link to server
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);

	while(1)
	{
		printf("Please Input:");
		fgets(buff,128,stdin);
		if(strncmp(buff,"end",3) ==0 )
		{
			break;
		}
		send(sockfd,buff,strlen(buff),0);
		memset(buff,0,128);
//		recv(sockfd,buff,127,0);
//		printf("RecvBuff:%s\n",buff);
 	       	printf("\n");
	}
	close(sockfd);
}







