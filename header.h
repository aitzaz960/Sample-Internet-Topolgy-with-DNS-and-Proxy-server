//------------------------Aitzaz Ahmad 18I-0589
//------------------------Muhammad Athar 18I-0692


#ifndef HEADER_H
#define HEADER_H
#include<vector>
using namespace std;

struct pending_msg
{
	int source;
	char content [200];
	int destination;
	vector<int> path;
};

struct routing_table_entry
{
	int my_port;
	int other_port;
	vector<int> path;
	bool closed = false;
	int open_port;
};


#endif
