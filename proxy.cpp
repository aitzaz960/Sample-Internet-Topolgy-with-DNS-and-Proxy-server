//------------------------Aitzaz Ahmad 18I-0589
//------------------------Muhammad Athar 18I-0692


// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
#define MAXLINE 1024 


struct file_info
{
	char ip[30];
	char name[30];
};
	
// Driver code 
int main() 
{ 
	int PORT = 5050;
	int sockfd; 
	char sending_buffer[200], receiving_buffer[200];
	char file_reader[100];
	bool data_found = false;
	int client_port;
	file_info obj;
	vector<file_info> info(3);
	fstream my_file;	
										
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{ 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	int n;
	socklen_t len; 

	len = sizeof(cliaddr); //len is value/resuslt 

	while (1)
	{
		n = recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, ( struct sockaddr *) &cliaddr, &len); 
		receiving_buffer[n] = '\0'; 
		printf("Client requested: %s\n", receiving_buffer); 
		client_port = ntohs(cliaddr.sin_port);
		
		my_file.open("history.txt", ios::out | ios::in | ios::app);
		while (1)
		{
			if ( !(my_file.getline(obj.name, 30, '\t') ))
			{
				break;
			}
			if (strcmp(receiving_buffer, obj.name)==0)
				data_found = true;
			else
				data_found = false;		
			my_file.getline(obj.ip, 30);
		}			
		if (data_found)
		{
			sendto(sockfd, obj.ip, sizeof(obj.ip), 0, (const struct sockaddr *) &cliaddr, len); 	
			sendto(sockfd, "P", sizeof("P"), 0, (const struct sockaddr *) &cliaddr, len);
			data_found = false;
			my_file << obj.name << "\t" << obj.ip << endl;
			my_file.close();
			continue;			
		}	
		else
			cout << "there was no data found so going to DNS now\n";
		my_file.close();
		
		my_file.open("history.txt", ios::out | ios_base::app);
		my_file << receiving_buffer << "\t";
		
		bzero(&cliaddr, sizeof(cliaddr)); 
		cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
		cliaddr.sin_port = htons( 5000 ); 
		cliaddr.sin_family = AF_INET; 
		
		sendto(sockfd, receiving_buffer, sizeof( receiving_buffer ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		cout << "Sent to DNS server ----> " << receiving_buffer << endl;
		recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr *) &servaddr, &len); 
		cout << "Received from DNS server ----> " << receiving_buffer << endl;
		
		my_file << receiving_buffer << endl;
		my_file.close();
		
		bzero(&cliaddr, sizeof(cliaddr)); 
		cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
		cliaddr.sin_port = htons( client_port ); 
		cliaddr.sin_family = AF_INET; 
		sendto(sockfd, receiving_buffer, sizeof( receiving_buffer ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		sendto(sockfd, "D", sizeof( "D" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 	
		cout << "Sent the ip to the client\n";
	}
	close (sockfd);
	return 0; 
} 

