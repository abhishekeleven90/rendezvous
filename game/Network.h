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
#define RETRY_COUNT 3

#define DATA_SIZE_KILO 1024

#define MSG_BROADCAST "b:"
#define MSG_MOVE "m:"

#define MSG_SERVER_ACK "a:"
#define MSG_SERVER_REQ_IGNORED "i:"

#define MSG_BASIC_POWER "B:"
#define MSG_MAGIC_POWER "M:"

#define MSG_ATTACK_TEMPLE "t:"
#define MSG_ATTACK_HERO "h:"

#define SERVER_BUSY 'x'

//----------Globals---------
char DATA_RCVD[M][DATA_SIZE_KILO];
char GLOBAL_ARR[2][DATA_SIZE_KILO];

char server_send_data[DATA_SIZE_KILO], server_recv_data[DATA_SIZE_KILO];
char client_send_data[DATA_SIZE_KILO], client_recv_data[DATA_SIZE_KILO];
char broad_send_data[DATA_SIZE_KILO], broad_client_recv_data[DATA_SIZE_KILO];

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

nodeHelper* selfNode = new nodeHelper;

list<string> queuePrimary;

//****************Function Declarations*******************
//-----Helper Functions----
bool isAllClientsAlive();
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
void processGeneral(char *completeData);

//-----TCP Functions-------
bool connectToServer(int & sock);
bool connectToServerBroadcast(int & sock, int playerId);
void* threadClientBroadcast(void* arg);
void* server(void* arg);

//****************Function Definitions*******************
//-----Helper Functions----
bool isAllClientsAlive() {
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == CLIENT_NOT_REACHABLE) {
			return false;
		}
	}
	return true;
}

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
		aStarMove(requestingPlayerId, true);
	} else if (strcmp(type, MSG_BASIC_POWER) == 0) {
		selectBasicPower(requestingPlayerId);
	} else if (strcmp(type, MSG_MAGIC_POWER) == 0) {
		selectMagicPower(requestingPlayerId);
	} else if (strcmp(type, MSG_ATTACK_TEMPLE) == 0) {
		char coordinates[2][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);
		players[requestingPlayerId].atleastOnceAstar = true;
		//TODO: remove attack cout
		cout << "calling attack enemy temple generic for player "
				<< requestingPlayerId << " at :  " << coordinates[0] << ","
				<< coordinates[1] << endl;
		attackEnemyTempleGeneric(requestingPlayerId);
	} else if (strcmp(type, MSG_ATTACK_HERO) == 0) {
		char coordinates[3][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);
		int enemyPlayerId = atoi(coordinates[2]);
		players[requestingPlayerId].atleastOnceAstar = true;
		//TODO: remove attack cout
		cout << "calling attack hero temple generic for player "
				<< requestingPlayerId << " at :  " << coordinates[0] << ","
				<< coordinates[1] << endl;
		attackHeroEnemyGeneric(requestingPlayerId, enemyPlayerId);
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

	if (!connectToServer(sock)) {
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

void connectServerBroadcast(int playerId) {
	memset(broad_client_recv_data, 0, sizeof broad_client_recv_data); //TODO: make generic for all nodes with for loop -- kya likha hai ye?
	int sock, bytes_recieved;
	broad_client_recv_data[0] = '\0';
	if (!connectToServerBroadcast(sock, playerId)) {
		broad_client_recv_data[0] = SERVER_BUSY;
		return;
	}
	//cout << "Client socket ID:" << sock << endl;
	send(sock, broad_send_data, strlen(broad_send_data), 0);
	bytes_recieved = recv(sock, broad_client_recv_data, DATA_SIZE_KILO, 0);
	broad_client_recv_data[bytes_recieved] = '\0';
	//cout << "Data successfully received" << client_recv_data << endl;
	close(sock);
}

void* threadClientBroadcast(void* arg) {
	cout << "Client started" << endl;

	while (1) {
		populateClientSendDataForBroadcast();

		int playerId;

		//TODO: shall be for all clients

		playerId = 2;
		if (players[playerId].status == CLIENT_ALIVE) {
			//strcpy(broadIp2Join, "10.192.11.114");
			//strcpy(broadIp2Join, "10.208.23.158");
			strcpy(broadIp2Join, "127.0.0.1");
			broadRemote_port = 5002;
			connectServerBroadcast(playerId);
		}

		/*	playerId = 1;
		 if (clientStatus[playerId] == CLIENT_ALIVE) {
		 strcpy(broadIp2Join, "10.192.11.114");
		 broadRemote_port = 5001;
		 connectServerBroadcast(playerId);
		 }*/

		playerId = 0;
		if (players[playerId].status == CLIENT_ALIVE) {
			strcpy(broadIp2Join, "127.0.0.1");
			broadRemote_port = 5000;
			connectServerBroadcast(playerId);
		}
	}
	return NULL;
}

void enqueMy(list<string> *l, string msg) {
	(*l).push_back(msg);
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
	//Coordinate_grid targetCell = players[currPlayerId].targetCell;
	Coordinate_grid targetCell = onClickTargetCell;

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
	cout << "sending move data to remote node " << client_send_data << endl;
	sendDataDontWaitForResult();
}

void helperSendPowerMode(int type) {

	if (type == 0)
		strcpy(client_send_data, MSG_BASIC_POWER);
	else if (type == 1)
		strcpy(client_send_data, MSG_MAGIC_POWER);
	else
		return;//something is wrong

	//setting the remoteNode ip & port
	setRemoteNode(primaryNodeIp, primaryNodePort);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void helperSendAttackTemple() {
	Coordinate_grid targetCell = onClickTargetCell;

	strcpy(client_send_data, MSG_ATTACK_TEMPLE);

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

void helperSendAttackHero(int enemyPlayer) {
	Coordinate_grid targetCell = onClickTargetCell;

	strcpy(client_send_data, MSG_ATTACK_HERO);

	char rowChar[4];
	intToChar(targetCell.row, rowChar);
	char colChar[4];
	intToChar(targetCell.col, colChar);
	char enemyIdChar[4];
	intToChar(enemyPlayer, enemyIdChar);

	strcat(client_send_data, rowChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, colChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, enemyIdChar);

	//setting the remoteNode ip & port
	setRemoteNode(primaryNodeIp, primaryNodePort);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void processBroadcast(char *data) {
	//cout << "received: " << data << endl;

	for (int i = 0; i < M; i++) {
		memset(DATA_RCVD[i], 0, sizeof DATA_RCVD[i]);
	}

	for (int i = 0; i < 2; i++) {
		memset(GLOBAL_ARR[i], 0, sizeof GLOBAL_ARR[i]);
	}

	split(data, '+', GLOBAL_ARR);

	//copying GridReceived
	split(GLOBAL_ARR[0], '|', DATA_RCVD);

	int k = 0;
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (!isOponentCellForTeam(Coordinate_grid(i, j), currPlayerId)) {
				putChar2Grid(i, j,
						static_cast<charCellType> (atoi(DATA_RCVD[k++])), true,
						false);
			} else {
				k++;
			}
		}
	}

	for (int i = 0; i < M; i++) {
		memset(DATA_RCVD[i], 0, sizeof DATA_RCVD[i]);
	}

	if (currPlayerId != PLAYER_ID_PRIMARY) { //copying the players information only if I am not the primary Node
		//copying player attributes
		split(GLOBAL_ARR[1], ',', DATA_RCVD);
		k = 0;
		for (int i = 0; i < NUM_OF_PLAYERS; i++) {
			players[i].team->templeHealth = atoi(DATA_RCVD[k++]);
			players[i].currPowerMode = static_cast<powerMode> (atoi(
					DATA_RCVD[k++]));
			players[i].heroHealth = atoi(DATA_RCVD[k++]);
			players[i].strength = atoi(DATA_RCVD[k++]);
			players[i].speedMove = atoi(DATA_RCVD[k++]);
			players[i].curseType = static_cast<curse> (atoi(DATA_RCVD[k++]));
			players[i].isTimerItemGlobalRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerMagicSpellRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerCurseRunning = atoi(DATA_RCVD[k++]);
		}
	}

	strcpy(server_send_data, MSG_SERVER_ACK);
}

void processGeneral(char *completeData) {
	pthread_mutex_lock(&mutexQueuePrimary);
	enqueMy(&queuePrimary, completeData);
	pthread_mutex_unlock(&mutexQueuePrimary);
	cout << "enqueued " << completeData << endl;
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
	strcpy(broad_send_data, MSG_BROADCAST);

	//populating 'broad_client_send_data' with grid
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			strcat(broad_send_data,
					numToStr(getInnerGridChar(i, j, true)).c_str());
			strcat(broad_send_data, "|");
		}
	}

	strcat(broad_send_data, "+");

	//populating 'broad_client_send_data' with attributes
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		Player player = players[i];

		strcat(broad_send_data, numToStr(player.team->templeHealth).c_str());
		strcat(broad_send_data, ",");

		strcat(broad_send_data, numToStr(player.currPowerMode).c_str());
		strcat(broad_send_data, ",");

		//attributes
		strcat(broad_send_data, numToStr(player.heroHealth).c_str());
		strcat(broad_send_data, ",");

		strcat(broad_send_data, numToStr(player.strength).c_str());
		strcat(broad_send_data, ",");

		strcat(broad_send_data, numToStr(player.speedMove).c_str());
		strcat(broad_send_data, ",");

		//related to curse
		strcat(broad_send_data, numToStr(player.curseType).c_str());
		strcat(broad_send_data, ",");

		//timers
		strcat(broad_send_data,
				numToStr(player.isTimerItemGlobalRunning).c_str());

		strcat(broad_send_data, ",");

		strcat(broad_send_data,
				numToStr(player.isTimerMagicSpellRunning).c_str());
		strcat(broad_send_data, ",");

		strcat(broad_send_data, numToStr(player.isTimerCurseRunning).c_str());
		strcat(broad_send_data, ",");
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
bool connectToServer(int & sock) {
	struct hostent *host;
	struct sockaddr_in server_addr;

	host = gethostbyname(ip2Join);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr *) host->h_addr);
	server_addr.sin_port = htons(remote_port);
	bzero(&(server_addr.sin_zero), 8);

	int retriedCount = 0;
	while (connect(sock, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {

		//trying again assuming the server is busy
		retriedCount++;
		cout << "Server busy --- retrying(" << retriedCount << "/"
				<< RETRY_COUNT << ")" << endl;
		sleep(1);
		if (retriedCount == RETRY_COUNT) {
			cout
					<< "Server is not up or not responding, terminating client...please try again"
					<< endl;
			close(sock);
			return false;
		}
	}

	return true;
}

bool connectToServerBroadcast(int & sock, int playerId) { //TODO: a lot redundant with above fxn
	struct hostent *host;
	struct sockaddr_in server_addr;

	host = gethostbyname(broadIp2Join);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr *) host->h_addr);
	server_addr.sin_port = htons(broadRemote_port);
	bzero(&(server_addr.sin_zero), 8);

	int retriedCount = 0;
	while (connect(sock, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {

		players[playerId].status = CLIENT_NOT_REACHABLE;

		//trying again assuming the server is busy
		retriedCount++;
		cout << "BroadCast-playerId: " << playerId << " busy --- retrying("
				<< retriedCount << "/" << RETRY_COUNT << ")" << endl;
		sleep(1);
		if (retriedCount == RETRY_COUNT) {

			//client is DEAD!!!
			cout << "playerId: " << playerId
					<< " is no more alive, let's continue" << endl;
			players[playerId].status = CLIENT_DEAD;

			close(sock);
			return false;
		}
	}

	if (players[playerId].status == CLIENT_NOT_REACHABLE) {
		players[playerId].status = CLIENT_ALIVE;
		cout << "playerId: " << playerId << " back now" << endl;
	}

	//cout << "Client successfully connected to server" << endl;
	return true;
}

bool isValidRequest(int requestingPlayerId) {
	if (!isAllClientsAlive()) {
		return false;
	}

	if (!players[requestingPlayerId].status == CLIENT_ALIVE) { //this is to ignore requests from dead client
		return false;
	}

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
		int requestingPlayerId = atoi(dataValArr[1]);

		if (!isValidRequest(requestingPlayerId)) {
			strcpy(server_send_data, MSG_SERVER_REQ_IGNORED);
		}

		else if (strcmp(type, MSG_BROADCAST) == 0) {
			processBroadcast(reqData);
		}

		else {
			processGeneral(server_recv_data);
		}

		send(connected, server_send_data, strlen(server_send_data), 0);
		fflush(stdout);//may be fatal, adding for UI

		close(connected);
	}
	cout << "right now, doesn't reach here" << endl;
	close(sock);
}

#endif
