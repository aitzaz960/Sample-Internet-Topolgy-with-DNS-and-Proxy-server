//------------------------Aitzaz Ahmad 18I-0589
//------------------------Muhammad Athar 18I-0692

// server program for udp connection 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <unistd.h>
#include "header.h"
using namespace std;

#define MAXLINE 1000 

// Driver code 


		
int main() 
{ 
	char sending_buffer[100], receiving_buffer[100], current_ip[20]; 
	int current_port;										// for holding ports from where clients connected
	int listenfd; 		
	bool connection_found = false, client_exists = false;
	int my_port = 2000;	
	int char_to_int; 
	char int_to_char[6];
	int server_ports[4] = {8080, 2000, 3000, 4000};							//ports of all 4 servers. Mainte=aine in array to ease broadcasting
	int temp_index=0;
	bool closed = false;
	vector<int>closed_nodes;
	vector<int>allowed_nodes;
	bool allowed=false;
		
	routing_table_entry obj, recv_obj;								//for holding information about 1 client in routing table
	pending_msg msg;										//for holding information about incoming and outgoing message
	vector<routing_table_entry> routing_table;							//The routing table of this server
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
		 
		 
	bzero(&servaddr, sizeof(servaddr)); 
	// Create a UDP Socket 
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);		 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(my_port); 
	servaddr.sin_family = AF_INET; 

	// bind server address to socket descriptor 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	
	//receive the datagram 
	len = sizeof(cliaddr); 
	
	
	
//--------------------------------------request means = client is requesting to make a connection
//--------------------------------------GET means = client is asking information about connected clients
//--------------------------------------RT means = some router is sharing its routing table
//--------------------------------------AN means = some one is sending an announcement of a new established connection
//--------------------------------------exit or close means = client has ended the connection
	
	while (1)								//infinite loop because servers will remain on
	{
		recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive message from server
		char_to_int = ntohs(cliaddr.sin_port);
		if (strcmp(receiving_buffer, "request")==0)
		{			
			cout << "Request received from client\n";
			sendto(listenfd, "OK", sizeof( "OK" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
	
			obj.other_port = ntohs(cliaddr.sin_port);						//this port will act as name of the connected client
			obj.my_port = my_port;									//writing down its own port
			obj.path.push_back(obj.my_port);						
			obj.path.push_back(obj.other_port);
			
			for(int i=0; i<routing_table.size(); i++)
			{
				if (obj.other_port == routing_table[i].other_port)
				{
					cout << "This client already exists\n";
					connection_found = true;
				}	
			}
			if (connection_found == false)
			{
				cout << "This client connected for the first time\n";
				routing_table.push_back(obj);
				sprintf(int_to_char, "%d", obj.other_port);					//sending the identifier of the connected client
				sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
				
//----------------------------------------broadcasting the connection announcement to all servers				
				for(int i=0; i<4; i++)
				{
					if (server_ports[i] == my_port)
					{
						cout << obj.other_port << " got connected with " << my_port << endl;
						continue;	
					}	
					bzero(&cliaddr, sizeof(cliaddr)); 
					cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
					cliaddr.sin_port = htons( server_ports[i] ); 
					cliaddr.sin_family = AF_INET; 					
					
					sendto(listenfd, "AN", sizeof( "AN" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 				
					sprintf(int_to_char, "%d", obj.other_port);
					sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 	
					sprintf(int_to_char, "%d", my_port);
					sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 	
				}	
				
				obj.my_port = obj.other_port = -1;							//clearing contenets for furthur use
				obj.path.clear();

//--------------------------------------------------Sharing the routing table-------------------------------------------------			
				for(int i=0; i<4; i++)
				{
					if (server_ports[i] == my_port)
						continue;	
					bzero(&cliaddr, sizeof(cliaddr)); 
					cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
					cliaddr.sin_port = htons( server_ports[i] ); 
					cliaddr.sin_family = AF_INET; 
												
					sendto(listenfd, "RT", sizeof( "RT" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					for(int i=0; i<routing_table.size(); i++)
					{
						sprintf(int_to_char, "%d", routing_table[i].my_port);
						sendto(listenfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));	
						sprintf(int_to_char, "%d", routing_table[i].other_port);
						sendto(listenfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
						for(int j=0; j<routing_table[i].path.size(); j++)
						{
							sprintf(int_to_char, "%d", routing_table[i].path[j]);
							sendto(listenfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
						}
						sprintf(int_to_char, "%d", -1);
						sendto(listenfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));	
					}
					sprintf(int_to_char, "%d", -2);
					sendto(listenfd, int_to_char, sizeof(int_to_char), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}			
			}	
			else
				connection_found = false;			
			
			
//----------------------------------Displaying the routing table-----------------------------------------------				
			if (routing_table.size() > 0)	
			{
				cout << "...............Routing Table----------------------\n";
				cout << "Source\t\t\tDestination\t\t\tConnecting Path\n";
				for(int i=0; i< routing_table.size(); i++)
				{
					cout<< routing_table[i].my_port << "\t\t\t" << routing_table[i].other_port << "\t\t\t";
					for(int j=0; j<routing_table[i].path.size(); j++)
						cout << routing_table[i].path[j] << "-->";
					cout << endl;	
				}
				cout << endl;
			}
			else
				cout << "...................Routing Table is empty---------------------------\n";
					
			cout<<"\n\n--------------------GOOGLE.COM---------------------\n";		
		}
		
		
		
		
		else if (strcmp(receiving_buffer, "GET")==0)
		{		
			for(int i=0; i<routing_table.size(); i++)
			{
				if (routing_table[i].other_port == ntohs(cliaddr.sin_port))
					continue;
				sprintf(int_to_char, "%d", routing_table[i].other_port);
				sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
			}	
			sprintf(int_to_char, "%d", -1);
			sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		}
		
		
		else if (strcmp(receiving_buffer, "MSG")==0)
		{
			msg.source = msg.destination = -1;				
			msg.path.clear();
			
			recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive sender of message	
			msg.source = atoi(receiving_buffer);
			recvfrom(listenfd, msg.content, sizeof(msg.content), 0, (struct sockaddr*)&cliaddr,&len); //receive message from server	
			recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive receipient of message message	
			msg.destination = atoi(receiving_buffer);
			for(int i=0; ;i++)										//receiving path of the message
			{
				recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); 	
				char_to_int = atoi(receiving_buffer);
				if (char_to_int == -1)
					break;
				msg.path.push_back(char_to_int);	
			}
			cout << "We received this from " << ntohs(cliaddr.sin_port) << endl;
			cout << "Source=" << msg.source << " \tDestination=" << msg.destination << endl;
			cout << "Message=" << msg.content << endl << "Path:\t";
			for(int i=0; i<msg.path.size(); i++)
				cout<<msg.path[i] << "-->";
			cout << endl;
						
			for(int i=0; i<routing_table.size(); i++)			//checking whether we are connected with that receipient
			{
				if (routing_table[i].other_port == msg.destination)
				{
					for(temp_index=0; ;temp_index++)				//checking the next stop for the message
					{
						if (routing_table[i].path[temp_index] == my_port)
						{
							msg.path.push_back( routing_table[i].path[++temp_index] );
							break;
						}	
					}
					if (strcmp(msg.content, "close")==0)
					{
						if (closed == true)
						{
							closed = false;
							cout << "The circuit has now been opened\n";
							closed_nodes.clear();
							allowed_nodes.clear();
						}
						else
						{
							cout << "The circuit is already opened\n";						
						}
					}
					else
					{
						if (closed == true)
						{
							for(int j=0; j<allowed_nodes.size(); j++)
							{
								if (allowed_nodes[j] == msg.source)
								{
									cout << "source of the message is one of the allowed nodes\n";
									allowed= true;
								}					
							}
							if (allowed == false)
							{
								cout << "This message cannot be sent due to closure of circuit\n";
								msg.path.clear();
								break;
							}
							else
							{
								cout << "This circuit was closed but for these clients it is open\n";
								allowed = false;
							}
						}
						else
						{
							closed = true;
							cout << "Now we have closed the ciruit \n";
							closed_nodes.push_back( my_port );
							closed_nodes.push_back( msg.path.back() );
							cout << "The nodes of " << closed_nodes.back() << " and " << my_port << " are closed now\n";
							allowed_nodes.push_back(msg.source);
							allowed_nodes.push_back(msg.destination);
							cout << "except for " << msg.source << " and " << msg.destination << endl;
						}
					}
					cout << "Going to forward the packet to " << routing_table[i].path[temp_index] << "\n";
					bzero(&cliaddr, sizeof(cliaddr)); 
					cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
					cliaddr.sin_port = htons( routing_table[i].path[temp_index] ); 
					cliaddr.sin_family = AF_INET; 
					
					sendto(listenfd, "MSG", sizeof( "MSG" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					sprintf(int_to_char, "%d", msg.source);
					sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					sendto(listenfd, msg.content, sizeof( msg.content ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					sprintf(int_to_char, "%d", msg.destination);
					sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					for(int i=0; i<msg.path.size(); i++)
					{
						sprintf(int_to_char, "%d", msg.path[i]);
						sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					}
					
					sprintf(int_to_char, "%d", -1);
					sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					msg.source = msg.destination = -1;				
					msg.path.clear();
				}	
			}
		}
		
		else if (strcmp(receiving_buffer, "AN")==0)
		{
			recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive client port
			cout << atoi(receiving_buffer) << " Got connected with ";
			recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive server port			
			cout << atoi(receiving_buffer) << endl;
		}
		
		else if (strcmp(receiving_buffer, "DN")==0)
		{
			recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive client port
			for(int counter =0; counter<routing_table.size(); counter++)
			{
				if (routing_table[counter].other_port == atoi(receiving_buffer))
				{
					routing_table.erase( routing_table.begin() + counter );
					break;
				}						
			}
			cout << atoi(receiving_buffer) << " disconnected\n";
			if (routing_table.size() > 0)	
			{
				cout << "...............Routing Table----------------------\n";
				cout << "Source\t\t\tDestination\t\t\tConnecting Path\n";
				for(int i=0; i< routing_table.size(); i++)
				{
					cout<< routing_table[i].my_port << "\t\t\t" << routing_table[i].other_port << "\t\t\t";
					for(int j=0; j<routing_table[i].path.size(); j++)
						cout << routing_table[i].path[j] << "-->";
					cout << endl;	
				}
				cout << endl;
			}
			else
				cout << "...........................Routing table is empty------------------------------\n";			
		}
		
		else if (strcmp(receiving_buffer, "RT")==0)
		{
			cout << "Routing table incoming....\n";
			while (1)
			{
				client_exists = false;
				recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive server port
				if ( atoi(receiving_buffer) == -2)								//-2 means routing table finished
				{
					break;
				}
				recv_obj.my_port = my_port;	
				recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive client port
				recv_obj.other_port = atoi(receiving_buffer);
				while (1)
				{
					recvfrom(listenfd, receiving_buffer, sizeof(receiving_buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive path ports
					if ( atoi(receiving_buffer) == -1)							//-1 means one entry of routing table finished
					{
						break;
					}
					recv_obj.path.push_back( atoi(receiving_buffer) );	
				} 
				
//-----------------------------Checking if this one entry should be added to the routing table or it exists already				
				for(int i=0; i<routing_table.size(); i++)
				{
					if (routing_table[i].other_port == recv_obj.other_port)
					{
						client_exists = true;
					}	
				}
				if (client_exists == false)
				{
					obj.my_port = recv_obj.my_port;
					obj.other_port = recv_obj.other_port;
					obj.path.push_back(my_port);
					for(int i=0; i<recv_obj.path.size(); i++)
						obj.path.push_back( recv_obj.path[i] );
					routing_table.push_back(obj);
					obj.my_port = obj.other_port = -1;
					obj.path.clear();
				}	
				recv_obj.my_port = recv_obj.other_port = -1;			//clearing contents for furthur use
				recv_obj.path.clear();
			}
//-----------------------------------------------------Displaying updated version of routing table----------------------------			
			if (routing_table.size() > 0)	
			{
				cout << "...............Routing Table----------------------\n";
				cout << "Source\t\t\tDestination\t\t\tConnecting Path\n";
				for(int i=0; i< routing_table.size(); i++)
				{
					cout<< routing_table[i].my_port << "\t\t\t" << routing_table[i].other_port << "\t\t\t";
					for(int j=0; j<routing_table[i].path.size(); j++)
						cout << routing_table[i].path[j] << "-->";
					cout << endl;	
				}
				cout << endl;
			}
			else
				cout << "...........................Routing table is empty------------------------------\n";
		}
//-----------------------------------------------------------------------------------------------------------------		
		else if (strcmp(receiving_buffer, "exit")==0)
		{
			inet_ntop(AF_INET, &(cliaddr.sin_addr), current_ip, 20);
			cout << "[+][+][+]..." << current_ip << "::" << ntohs(cliaddr.sin_port) << " disconnected\n";
			char_to_int = ntohs(cliaddr.sin_port);
			for(int i=0; i<4; i++)
			{
				if (server_ports[i] == my_port)
					continue;
				bzero(&cliaddr, sizeof(cliaddr)); 
				cliaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
				cliaddr.sin_port = htons( server_ports[i] ); 
				cliaddr.sin_family = AF_INET; 
												
				sendto(listenfd, "DN", sizeof( "DN" ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 	
				sprintf(int_to_char, "%d", char_to_int);
				sendto(listenfd, int_to_char, sizeof( int_to_char ), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 	
			}	
							
			for(int i=0; i<routing_table.size(); i++)
			{
				if (ntohs(cliaddr.sin_port) == routing_table[i].other_port)
				{
					routing_table.erase( routing_table.begin() + i );
					break;
				}	
			}
			
			
			if (routing_table.size() > 0)	
			{
				cout << "...............New Routing Table----------------------\n";
				cout << "Source\t\t\tDestination\t\t\tConnecting Path\n";
				for(int i=0; i< routing_table.size(); i++)
				{
					cout<< routing_table[i].my_port << "\t\t\t" << routing_table[i].other_port << "\t";
					for(int j=0; j<routing_table[i].path.size(); j++)
						cout << routing_table[i].path[j] << "-->";
					cout << endl;	
				}
				cout << endl;
			}	
			else
				cout << "...............New Routing Table is empty----------------------\n";	
			
		}
		else
			cout << "we received ----->> " << receiving_buffer << endl;
	}		
} 

