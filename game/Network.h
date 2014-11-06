#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <math.h>
#include <list>
#include "MyString.h"
#include "OneMoreFile.h"

//----------Constants---------
#define M 400
#define QUEUE_LIMIT 5

#define DATA_SIZE_KILO 1024

#define MSG_BROADCAST "b:"
#define MSG_MOVE "m:"
#define MSG_ACK "a:"

#define SERVER_BUSY 'x'

//----------Globals---------
char GLOBAL_ARR[M][DATA_SIZE_KILO];

char server_send_data[DATA_SIZE_KILO], server_recv_data[DATA_SIZE_KILO];
char client_send_data[DATA_SIZE_KILO], client_recv_data[DATA_SIZE_KILO];
char broad_client_send_data[DATA_SIZE_KILO],
		broad_client_recv_data[DATA_SIZE_KILO];

unsigned int server_port = 0;
unsigned int remote_port = 0; // port with which to connect to server
unsigned int broadRemote_port = 0;

char ip2Join[IP_SIZE]; //used by client to join the server
char broadIp2Join[IP_SIZE];

char primaryNodeIp[IP_SIZE];
int primaryNodePort;

pthread_t serverThreadId;
pthread_t clientThreadId; //TODO: check if not required
pthread_t broadcastThreadId;
pthread_t sendDataServerThreadId;
pthread_t updateServerThreadId;

int serverSock;

int isCreated = false;
int isJoined = false;
int retry_count = 5;

nodeHelper* selfNode = new nodeHelper;

list<string> queuePrimary;
pthread_mutex_t mutexQueuePrimary;

//****************Function Declarations*******************
//-----Helper Functions----
void takeUpdateAction(string msg);

void sendDataAndWaitForResult();
void sendDataDontWaitForResult();
void sendDataToServer();
void* threadSendDataToServer(void* arg);

void enqueMy(list<string> *l, string msg);
string dequeMy(list<string> *l);
void printQueue(list<string> *l);
void emptyQueue(list<string> *queue);

void helperSendServerMove(Coordinate_grid targetCell);

void createServerThread();
void createClientBroadcastThread();
void createSendServerDataThread();

void populateClientSendDataForBroadcast();
nodeHelper* convertToNodeHelper(char *ipWithPort);

void setRemoteNode(char* ip, unsigned int port);
int createThread(pthread_t* threadId, void* threadFn(void*));
void joinIpWithPort(char* ip, unsigned int port, char* ipWithPort);
void getMyIp(char* ip);
int getMyPort(int mySock);
void fillNodeEntries(struct sockaddr_in server_addr);

void processBroadcast(char *data);
void processMove(char *completeData);

//-----TCP Functions-------
bool connectToServer(int & sock, char ip2Join[IP_SIZE], int port);
void* threadClientBroadcast(void* arg);
void* server(void* arg);

//****************Function Definitions*******************
//-----Helper Functions----
void takeUpdateAction(const char* msg) {
	char* type = substring(msg, 0, 2);
	char* data = substring(msg, 3, strlen(msg));

	char dataValArr[2][DATA_SIZE_KILO];
	split(data, '?', dataValArr);

	char* reqData = dataValArr[0];
	int requestingPlayerId = atoi(dataValArr[1]);

	if (strcmp(type, MSG_MOVE) == 0) {
		char coordinates[2][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);
		aStarMove(requestingPlayerId, true); //TODO: abhi ke liye through
	}

}

void sendDataAndWaitForResult() {
	createSendServerDataThread();
	while (client_recv_data[0] == '\0')
		; //wait until data is received
}

void sendDataDontWaitForResult() {
	createSendServerDataThread();
}

void sendDataToServer() {
	int sock, bytes_recieved;

	//Appending myId in the request
	strcat(client_send_data, "?");
	strcat(client_send_data, numToStr(currPlayerId).c_str());

	client_recv_data[0] = '\0';

	if (!connectToServer(sock, ip2Join, remote_port)) {
		client_recv_data[0] = SERVER_BUSY;
		return;
	}

	//cout << "Client socket ID:" << sock << endl;
	send(sock, client_send_data, strlen(client_send_data), 0);

	bytes_recieved = recv(sock, client_recv_data, DATA_SIZE_KILO, 0);
	client_recv_data[bytes_recieved] = '\0';

	//cout << "Data successfully received" << client_recv_data << endl;

	close(sock);
}

void* threadSendDataToServer(void* arg) {
	sendDataToServer();
	return NULL;
}

void* threadUpdateServer(void* arg) {
	while (1) {
		if (!queuePrimary.empty()) {
			pthread_mutex_lock(&mutexQueuePrimary);
			string msg = dequeMy(&queuePrimary);
			pthread_mutex_unlock(&mutexQueuePrimary);
			takeUpdateAction(msg.c_str());
		}
	}
	return NULL;
}

void connectServerBroadcast() {
	memset(broad_client_recv_data, 0, sizeof broad_client_recv_data); //TODO: make generic for all nodes with for loop
	int sock, bytes_recieved;
	broad_client_recv_data[0] = '\0';
	if (!connectToServer(sock, broadIp2Join, broadRemote_port)) {
		broad_client_recv_data[0] = SERVER_BUSY;
		return;
	}
	//cout << "Client socket ID:" << sock << endl;
	send(sock, broad_client_send_data, strlen(broad_client_send_data), 0);
	bytes_recieved = recv(sock, broad_client_recv_data, DATA_SIZE_KILO, 0);
	broad_client_recv_data[bytes_recieved] = '\0';
	//cout << "Data successfully received" << client_recv_data << endl;
	close(sock);
}

void* threadClientBroadcast(void* arg) {
	cout << "Client started" << endl;

	while (1) {
		populateClientSendDataForBroadcast();

		strcpy(broadIp2Join, "127.0.0.1");
		broadRemote_port = 5001;
		connectServerBroadcast();

		/*strcpy(broadIp2Join, "127.0.0.1");
		broadRemote_port = 5000;
		connectServerBroadcast();*/
	}
	return NULL;
}

void enqueMy(list<string> *l, string msg) {
	(*l).push_back(msg);
	cout << "enqued: " << endl;
}

string dequeMy(list<string> *l) {
	string msg = (*l).front();
	(*l).pop_front();
	return msg;
}

void printQueue(list<string> *l) {
	for (list<string>::iterator it = (*l).begin(); it != (*l).end(); it++) {
		cout << (*it) << ", ";
	}
	cout << endl;
}

void emptyQueue(list<string> *queue) {
	while (!(*queue).empty()) {
		cout << dequeMy(queue);
	}
}

void helperSendServerMove() {
	Coordinate_grid targetCell;
	targetCell.row = players[currPlayerId].targetCell.row;
	targetCell.col = players[currPlayerId].targetCell.col;

	//Setting client_send_data
	strcpy(client_send_data, MSG_MOVE);

	char rowChar[4];
	intToChar(targetCell.row, rowChar);
	char colChar[4];
	intToChar(targetCell.col, colChar);

	strcat(client_send_data, rowChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, colChar);

	//setting the remoteNode ip & port
	setRemoteNode(primaryNodeIp, primaryNodePort);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void processBroadcast(char *data) {
	//cout << "received: " << data << endl;

	for (int i = 0; i < M; i++) {
		memset(GLOBAL_ARR[i], 0, sizeof GLOBAL_ARR[i]);
	}

	split(data, '|', GLOBAL_ARR);

	int k = 0;
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			putCharToGrid(i, j,
					static_cast<charCellType> (atoi(GLOBAL_ARR[k++])), true);
		}
	}

	strcpy(server_send_data, MSG_ACK);
}

void processMove(char *completeData) {
	cout << "data received for move: " << completeData << endl;
	pthread_mutex_lock(&mutexQueuePrimary);
	enqueMy(&queuePrimary, completeData);
	pthread_mutex_unlock(&mutexQueuePrimary);
}

void createServerThread() {
	createThread(&serverThreadId, server);
}

void createClientBroadcastThread() {
	createThread(&broadcastThreadId, threadClientBroadcast);
}

void createSendServerDataThread() {
	createThread(&sendDataServerThreadId, threadSendDataToServer);
}

void createUpdateServerThread() {
	createThread(&updateServerThreadId, threadUpdateServer);
}

void populateClientSendDataForBroadcast() {
	strcpy(broad_client_send_data, MSG_BROADCAST);

	//populating 'broad_client_send_data' with grid
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			strcat(broad_client_send_data,
					numToStr(getInnerGridChar(i, j, true)).c_str());
			strcat(broad_client_send_data, "|");
		}
	}
}

nodeHelper* convertToNodeHelper(char *ipWithPort) {
	nodeHelper* toReturn = new nodeHelper;

	strcpy(toReturn->ipWithPort, ipWithPort);
	char* ipAddr = substring(ipWithPort, 0, indexOf(ipWithPort, ':'));
	char* portString = substring(ipWithPort, indexOf(ipWithPort, ':') + 2,
			strlen(ipWithPort));

	unsigned int portNum = atoi(portString);

	strcpy(toReturn->ip, ipAddr);
	toReturn->port = portNum;

	return toReturn;
}

void setRemoteNode(char* ip, unsigned int port) {
	memset(client_recv_data, 0, sizeof client_recv_data);
	strcpy(ip2Join, ip);
	remote_port = port;
}

int createThread(pthread_t* threadId, void* threadFn(void*)) {
	if (pthread_create(threadId, NULL, threadFn, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	return 0;
}

void joinIpWithPort(char* ip, unsigned int port, char* ipWithPort) {
	char portChar[10];
	intToChar(port, portChar);
	strcpy(ipWithPort, ip);
	strcat(ipWithPort, ":");
	strcat(ipWithPort, portChar);
}

void getMyIp(char* ip) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			if (strcmp(ifa->ifa_name, "eth0") == 0 || strcmp(ifa->ifa_name,
					"wlan0") == 0) {
				strcpy(ip, addressBuffer);
			}
		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}
}

int getMyPort(int mySock) {
	struct sockaddr_in sin;
	socklen_t addrlen = sizeof(sin);
	if (getsockname(mySock, (struct sockaddr *) &sin, &addrlen) == 0
			&& sin.sin_family == AF_INET && addrlen == sizeof(sin)) {
		int local_port = ntohs(sin.sin_port);
		return local_port;
	} else {
		; // handle error
	}
	return 0;
}

void fillNodeEntries(struct sockaddr_in server_addr) {
	char ip[IP_SIZE];
	memset(ip, 0, sizeof ip);
	getMyIp(ip);

	if (strlen(ip) == 0) {
		strcpy(ip, "127.0.0.1");
	}

	strcpy(selfNode->ip, ip);

	selfNode->port = getMyPort(serverSock);

	char ipWithPort[IP_SIZE];
	joinIpWithPort(selfNode->ip, selfNode->port, ipWithPort);
	strcpy(selfNode->ipWithPort, ipWithPort);

}

//-----TCP Functions-------
bool connectToServer(int & sock, char ip2Join[IP_SIZE], int port) {
	struct hostent *host;
	struct sockaddr_in server_addr;
	//cout << "Inside connect to server: " << ip2Join << ":" << remote_port
	//<< endl;
	host = gethostbyname(ip2Join);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	//cout << "Client socket created" << endl;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr *) host->h_addr);
	server_addr.sin_port = htons(port);
	bzero(&(server_addr.sin_zero), 8);

	int retriedCount = 0;
	while (connect(sock, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {

		//trying again assuming the server is busy
		retriedCount++;
		//cout << "Server busy --- retrying(" << retriedCount << "/"
		//	<< retry_count << ")" << endl;
		sleep(1);
		if (retriedCount == retry_count) {
			cout
					<< "Server is not up or not responding, terminating client...please try again"
					<< endl;
			close(sock);
			return false;
		}
	}
	//cout << "Client successfully connected to server" << endl;
	return true;
}

void* server(void* arg) {
	int sock, connected, trueint = 1;

	struct sockaddr_in server_addr, client_addr;
	unsigned int sin_size;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	serverSock = sock;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &trueint, sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;

	if (server_port != 0) { //Let the server choose the port itself if not supplied externally
		server_addr.sin_port = htons(server_port);
	}
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(server_addr.sin_zero), 8);

	if (bind(sock, (struct sockaddr *) ((&server_addr)),
			sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(sock, QUEUE_LIMIT) == -1) {
		perror("Listen");
		exit(1);
	}

	fillNodeEntries(server_addr);

	cout << "Starting to listen on: " << selfNode->ipWithPort << endl;
	cout << ">>>: ";
	fflush(stdout);
	while (1) {
		int bytes_received;
		sin_size = sizeof(struct sockaddr_in);
		connected
				= accept(sock, (struct sockaddr*) ((&client_addr)), &sin_size);

		bytes_received = recv(connected, server_recv_data, DATA_SIZE_KILO, 0);
		server_recv_data[bytes_received] = '\0';

		char* type = substring(server_recv_data, 0, 2);
		char* data = substring(server_recv_data, 3, strlen(server_recv_data));

		char dataValArr[2][DATA_SIZE_KILO];
		split(data, '?', dataValArr);

		char* reqData = dataValArr[0];

		if (strcmp(type, MSG_BROADCAST) == 0) {
			processBroadcast(reqData);
		}

		else if (strcmp(type, MSG_MOVE) == 0) {
			processMove(server_recv_data);
		}

		send(connected, server_send_data, strlen(server_send_data), 0);
		fflush(stdout);//may be fatal, adding for UI

		close(connected);
	}
	cout << "right now, doesn't reach here" << endl;
	close(sock);
}

#endif
