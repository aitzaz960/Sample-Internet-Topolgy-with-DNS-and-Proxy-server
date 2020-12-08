




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
using namespace std;
#define MAXLINE 1024 

// Driver code 
int main() 
{ 
	int PORT = 5000;
	int sockfd; 
	char sending_buffer[200], receiving_buffer[200];
	char file_reader[100];
	bool data_found = false;		
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

		my_file.open("IPS.txt", ios::in);
		while ( !my_file.eof() )
		{
			my_file >> file_reader;
			if (strcmp(file_reader, receiving_buffer)==0)
			{
				cout << "Matched with file\n";
				data_found = true;
				my_file >> file_reader;
				sendto(sockfd, file_reader, sizeof(file_reader), 0, (const struct sockaddr *) &cliaddr, len); 
	//			cout << "[+][+][+]...." << file_reader << "...[+][+][+]sent to client\n"; 
				my_file.close();
				break;
			}
			else
				my_file >> file_reader;
		}		
		if ( !data_found )
		{
	//		cout << "not found\n";
			sendto(sockfd, "not found", sizeof("not found"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
			my_file.close();
		}
	}
	return 0; 
} 

