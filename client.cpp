//------------------------Aitzaz Ahmad 18I-0589

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
#include <cmath>
#include <vector>
#include "header.h"
using namespace std;
#define MAXLINE 1024 

//------------------------This function will extract IP of server from the response sent from DNS
char* extract_ip(char* msg, char* ip)
{
	for(int i=0; i<20; i++)
		ip[i] = '\0';
	for(int i=0; i < strlen(msg); i++)
	{
		ip[i] = msg[i];
		if (ip[i] == ':')
		{
			ip[i] = '\0';
			break;
		}
	}
	return ip;
}

//------------------------This function will extract port of server from the response sent from DNS
int extract_port(char* msg)
{
	char port[6];
	int to_ret=0;
	int index=0;
	while( msg[index] != ':' && msg[index+1] != ':')
		index++;	
	index += 3;
	for (int i=0; 1; i++)
	{
		port[i] = msg[index++];
		if (port[i] == '\0')
			break;
	}	
	for(int i=0, exp=strlen(port)-1; exp >= 0; i++, exp--)
		to_ret += ( int (port[i]) - 48 ) * pow(10, exp);
	return to_ret;	 
}


// Driver code 
int main() 
{ 
	int PORT;										//to which the client needs to connect
	int my_port ;										//this will store the port by which server identifies this client
	int sockfd; 										//socket descripter
	char str_ip[20]; 									//to store ip returned by DNS
	int char_to_int;									//it will store the data which has been sent as a char but needs to be stored in int
	char int_to_char[6];				//this will store the data that needs to be converted to char for sending it through the socket				
	int choice;										//This will be used to see choice of user					
	char url[100], sending_buffer[200], receiving_buffer[200], buffer[200];			//for sending receiving data  
	vector<int> connected_clients;			 					//this will store which clients are connected to server other than itself
	pending_msg msg;									//this will be used to store info about the msg which is to be sent or to be received
	int destination_index; 							//it will store index of client to which msg is to be sent among the vector of connected clients
	struct sockaddr_in servaddr; 								//socket descriptor for communication with server

	
	PORT = 5050;										//5050 is the port of proxy server
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
		
	// create datagram socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
					
	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 
		cout << "\n[-]...Error : Connect Failed \n"; 
		exit(0); 
	}		
		
	int n;
	socklen_t len; 
	
	
//-------------------------------------------Asking the user to enter the website which will go to DNS for translation	
	while (1)
	{
		cout << "Welcome to our Network \n\n";
		cout << "Enter the URL:\t";
		cin.getline(url, 100);
		if ( strcmp(url, "exit")==0)
		{
			cout << "[+]...Exit request has been received\n";
			cout << "[+]...You have exit successfully\n\n";
			exit(0);
		}
		
		sendto(sockfd, url, sizeof(url), 0, (const struct sockaddr *) NULL, sizeof(servaddr)); 	
		n = recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr *) &servaddr, &len); 
		receiving_buffer[n] = '\0'; 
		cout << "Information found --> " << receiving_buffer << endl;
		PORT = extract_port(receiving_buffer);
		n = recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr *) &servaddr, &len); 
		receiving_buffer[n] = '\0';
//-----------------------------------D means the IP and port are sent by DNS and P stands for proxy server	
		if (strcmp(receiving_buffer, "D")==0)
		{
			cout << "Path: Client<-->Proxy Server<-->DNS\n";
			cout << "Response was sent by DNS\n\n";
		}
		else if (strcmp(receiving_buffer, "P")==0)
		{
			cout << "Pat: Client<-->Proxy\n";
			cout << "Response was sent by Proxy server\n";
		}
		if ( strcmp(receiving_buffer, "not found")==0)
		{
			cout << "Error 404. Server not found\n";
			continue;
		}
		close(sockfd); 	
		
//--------------------------Now we know IP and Port of server to be connected so lets do that--------------------------------------------------------------------------
	
		bzero(&servaddr, sizeof(servaddr)); 
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
		servaddr.sin_port = htons(PORT); 
		servaddr.sin_family = AF_INET; 
					
		sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
		connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));	
		
			
//------------------------The client connected so now we will start client/serever chatting--------------------------------------------------------------------
	
		sendto(sockfd, "request", sizeof("request"), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		if (strcmp(buffer, "OK")==0)
			cout << "The client connected successfully\n";
//--------------------------Now the server will send the port by which this client is recognised. This port will act as a name for this client		
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		my_port = atoi (buffer) ;
		cout << "-------------------------------------My Port === " << my_port << "--------------------------------------\n";	
		while (1)
		{
//---------------------------Keep on receiving and displaying the messages as long as client is not interested in sending the messages	
			do 
			{
				cout << "Do you want to send message or you want to wait for message from someone else?\n";
				cout << "Press 1 for sending message\nPress 2 for Wait for other clients messages\n";
				cin >> choice;
				if (choice == 2)
				{
					recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
					if (strcmp(buffer, "MSG")==0)
					{
						recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)NULL,NULL); //receive message from server	
						msg.source = atoi(receiving_buffer);
						recvfrom(sockfd, msg.content, sizeof(msg.content), 0, (struct sockaddr*)NULL,NULL); //receive message from server	
						recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)NULL,NULL); //receive message from server	
						msg.destination = atoi(receiving_buffer);
						for(int i=0; ;i++)
						{
							recvfrom(sockfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)NULL,NULL); //receive message from 	server	
							char_to_int = atoi(receiving_buffer);
							if (char_to_int == -1)
								break;
							msg.path.push_back(char_to_int);	
						}
						cout << "\n\n[+][+][+][+][+][+][+][+][+][+][+][+][+][+][+][+]\n";
						cout << "Source=" << msg.source << " \tDestination=" << msg.destination << endl;
						cout << "Message=" << msg.content << endl << "Path:\t";
						for(int i=0; i<msg.path.size(); i++)
							cout<<msg.path[i] << "-->";
						cout << endl << endl << endl;
						msg.source = msg.destination = -1;
						msg.path.clear();
					}
					else
						cout << "We received this from server: " << buffer << endl;
				}
			} while (choice == 2);
				
//--------------------------Gettting the list of connected clients so that we can select the client with which we want to communicate 			
			sendto(sockfd, "GET", sizeof("GET"), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
			connected_clients.clear();
			for(int i=1; ;i++)
			{
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
				char_to_int = atoi(buffer);
				if (char_to_int == -1)
					break;
				connected_clients.push_back(char_to_int);
			}
			
//--------------------Now we know which clients are currently connected to server either directy or undirectly so we will now ask with which client to communicate--------------
			
			if (connected_clients.size() > 0)
			{	
				while (1)
				{
					cout << "My own port = " << my_port << endl;
					cout << "To which client you want to send the message\n";
					for(int i=0; i<connected_clients.size(); i++)
						cout << "Press " << i+1 << " for " << connected_clients[i] << endl;
					cin >> destination_index;
					if (destination_index > 0 && destination_index <= connected_clients.size() )
						break;
				} 	
				cin.ignore();
				msg.source = my_port;
				msg.destination = connected_clients[destination_index-1];
//---------------------------The client can send the message only to connected server so we know the sending path till here			
				msg.path.push_back(my_port);
				msg.path.push_back( PORT );
			
			
//-------------------Now we will tell server the src, dst, msg of the client and the path will be maintained along with our progress-------------------------------
					
				cout << "Now enter the message: ";
				cin.getline(msg.content, 200);	
				if (strcmp(msg.content, "exit")==0)
				{
					sendto(sockfd, "exit", sizeof("exit"), 0, (struct sockaddr*)NULL, sizeof(servaddr));	
					exit(0);
				}		
				cout << "You are going to send " << msg.content << " from " << msg.source << " to " << msg.destination << endl;
				sendto(sockfd, "MSG", sizeof("MSG"), 0, (struct sockaddr*)NULL, sizeof(servaddr));
				sprintf(int_to_char, "%d", msg.source);
				sendto(sockfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)NULL, sizeof(servaddr));
				sendto(sockfd, msg.content, sizeof(msg.content), 0, (struct sockaddr*)NULL, sizeof(servaddr));
				sprintf(int_to_char, "%d", msg.destination);
				sendto(sockfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)NULL, sizeof(servaddr));					
				for(int i=0; i<msg.path.size(); i++)
				{
					sprintf(int_to_char, "%d", msg.path[i]);
					sendto(sockfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)NULL, sizeof(servaddr));
				}
//------------------------------------ "-1" means that all the content related to one message has been sent			
				sprintf(int_to_char, "%d", -1);	
				sendto(sockfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)NULL, sizeof(servaddr));	
				msg.path.clear();
			}
			else
			{
				cin.ignore();
				cout << "Till now there are no active clients other than its own\n";
			}	
		}	
	}			
	return 0; 
} 

