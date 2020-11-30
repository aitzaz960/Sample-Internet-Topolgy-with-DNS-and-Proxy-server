#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<vector>
using namespace std;
#define MAXFD 10	//Size of fds array
int PORT = 4000;

struct broadcasting_table
{
	int my_port;
	int conn_port;
	int hop;
	vector<int>path;
};

void fds_add(int fds[],int fd)	//Add a file descriptor to the fds array
{
	int i=0;
	for(;i<MAXFD;++i)
	{
		if(fds[i]==-1)
		{
	      		fds[i]=fd;
			break;
		}
	}
}

void ports_add(int ports[],int port)	//Add a port to the ports array
{
	int i=0;
	for(;i<MAXFD;++i)
	{
		if(ports[i]==-1)
		{
	      		ports[i]=port;
			break;
		}
	}
}

int main()
{
	printf("-------------------------------------------SERVER#3----------------------------------------------\n\n");
	vector<broadcasting_table> table;
	struct broadcasting_table temp;
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	char connected_ip[16];
	int connected_port;
	assert(sockfd!=-1);
	
    	printf("sockfd=%d\n",sockfd);
    
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(PORT);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res!=-1);
	
	//Create listening queue
	listen(sockfd,5);
	
   	//Define fdset collection
    	fd_set fdset;
	
	//Define fds array
    	int fds[MAXFD];
    	int ports[MAXFD];
    	int i=0;
    	for(;i<MAXFD;++i)
    	{
		fds[i]=-1;
		ports[i]=-1;
 	}
	
	//Add a file descriptor to the fds array
    	fds_add(fds,sockfd);
	ports_add(ports, PORT);
	while(1)
    	{
		FD_ZERO(&fdset);//Clear the fdset array to 0

		int maxfd=-1;

		int i=0;

		//For loop finds the maximum subscript for the ready event in the fds array
		for(;i<MAXFD;i++)
		{
			if(fds[i]==-1)
			{
				continue;
			}

			FD_SET(fds[i],&fdset);

			if(fds[i]>maxfd)
			{
				maxfd=fds[i];
			}
		}

		struct timeval tv={5,0};	//Set timeout of 5 seconds

		int n=select(maxfd+1,&fdset,NULL,NULL,&tv);//Selectect system call, where we only focus on read events
		if(n==-1)	//fail
		{
			perror("select error");
		}
		else if(n==0)//Timeout, meaning no file descriptor returned
		{
			printf("time out\n");
		}
		else//Ready event generation
		{
		//Because we only know the number of ready events by the return value of select, we don't know which events are ready.
		//Therefore, each file descriptor needs to be traversed for judgment
			for(i=0;i<MAXFD;++i)
			{
				if(fds[i]==-1)	//If fds[i]==-1, the event is not ready
				{
					continue;
				}
				if(FD_ISSET(fds[i],&fdset))	//Determine if the event corresponding to the file descriptor is ready
				{
			   
				//There are two kinds of cases for judging file descriptors
			   
					if(fds[i]==sockfd)	//A file descriptor is a socket, meaning accept if a new client requests a connection
					{
						//accept
						struct sockaddr_in caddr;
						socklen_t len=sizeof(caddr);

						int c=accept(sockfd,(struct sockaddr *)&caddr,&len);	//Accept new client connections
						if(c<0)
						{
							continue;
						}
						printf("accept client=%s:%d\n",inet_ntoa(caddr.sin_addr), (int) ntohs(caddr.sin_port));
						fds_add(fds,c);//Add the connection socket to the array where the file descriptor is stored
						ports_add(ports, (int) ntohs(caddr.sin_port) );
					}
					else   //Receive data recv when an existing client sends data
					{
						char buff[128]={0};
						int res=recv(fds[i],buff,127,0);
						if(res<=0)
						{
							close(fds[i]);
							fds[i]=-1;
							printf("one client over\n");
						}
						else
						{
							printf("recv(127.0.0.1:%d)=%s\n",ports[i],buff);	//Output Client Sent Information
							send(fds[i],"OK",2,0);	//Reply message to client
						}
					}
				}
			}
		}
	}
}
