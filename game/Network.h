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
#include "Globals.h"

//----------Constants---------
#define M 400
#define QUEUE_LIMIT 5
#define RETRY_COUNT 5

#define DATA_SIZE_KILO 1024

#define MSG_BROADCAST "b:"
#define MSG_MOVE "m:"

#define MSG_SERVER_ACK "a:"
#define MSG_SERVER_REJECT "r:"
#define SERVER_REQ_IGNORED "i:"
#define MSG_SERVER_WELCOME_BACK "w:"

#define MSG_POWER_BASIC "B:"
#define MSG_POWER_MAGIC "M:"

#define MSG_ATTACK_TEMPLE "t:"
#define MSG_ATTACK_HERO "h:"

#define MSG_CONNECT "c:"
#define MSG_VALIDATE_TEAM "y:"
#define MSG_VALIDATE_HERO "z:"
#define MSG_REQ_PLAYER_DETAILS "l:"

#define MSG_GAME_OVER "o:"
#define MSG_JOINING "g:"

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

enum broadcastType {
	BROADCAST_ALL, BROADCAST_GAME_OVER, BROADCAST_JOINING
};

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

bool isServerOk();

connectStatus helperSendConnect();
bool helperValidateTeam(teamName team);
bool helperValidateHero(heroes hero);
void helperRequestPlayersDetails();
void helperSendServerMove(Coordinate_grid targetCell);

void createServerThread();
void createClientBroadcastThread();
void createSendServerDataThread();

void populateClientSendDataForGameOver();
void populateClientSendDataForJoining();
void populateClientSendDataForBroadcast();

nodeHelper* convertToNodeHelper(char *ipWithPort);

void setRemoteNode(char* ip, unsigned int port);
int createThread(pthread_t* threadId, void* threadFn(void*));
void joinIpWithPort(char* ip, unsigned int port, char* ipWithPort);
void getMyIp(char* ip);
int getMyPort(int mySock);
void fillNodeEntries(struct sockaddr_in server_addr);

int getIpOfPlayer(playerStatus status);
bool isTeamFull(teamName team);
bool isHeroTaken(heroes hero);

void processGameOver(char* data);
void processJoining(char* data);
void processBroadcast(char *data);
void processConnect(char *data);
void processValidateTeam(char *data, int id);
void processValidateHero(char *data, int id);
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
		if (players[i].status == STATUS_NOT_REACHABLE) {
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

	//since master maintains if player's reborn timer, handle at master
	//player reborn timer is not being sent to the client nodes.
	if (players[requestingPlayerId].isHeroRebornTimer) {
		cout << "At master node: " << "ignoring message as just reborn hero : "
				<< requestingPlayerId << endl;
		return;
	}

	if (strcmp(type, MSG_MOVE) == 0) {
		char coordinates[2][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);

		players[requestingPlayerId].atleastOnceAstar = true;

		aStarMove(requestingPlayerId, true); //TODO: AStar through
	}

	else if (strcmp(type, MSG_POWER_BASIC) == 0) {
		selectBasicPower(requestingPlayerId);
	}

	else if (strcmp(type, MSG_POWER_MAGIC) == 0) {
		selectMagicPower(requestingPlayerId);
	}

	else if (strcmp(type, MSG_ATTACK_TEMPLE) == 0) {
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
	}

	else if (strcmp(type, MSG_ATTACK_HERO) == 0) {
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
	memset(client_recv_data, 0, sizeof client_recv_data);
	client_recv_data[0] = '\0';
	createSendServerDataThread();
	while (client_recv_data[0] == '\0')
		; //wait until data is received
}

void sendDataDontWaitForResult() {
	memset(client_recv_data, 0, sizeof client_recv_data);
	client_recv_data[0] = '\0';
	createSendServerDataThread();
}

void sendDataToServer() {
	int sock, bytes_recieved;

	//Appending myId in the request
	strcat(client_send_data, "?");
	strcat(client_send_data, numToStr(currPlayerId).c_str());

	if (!connectToServer(sock)) {
		client_recv_data[0] = SERVER_BUSY;
		return;
	}

	//cout << "Client socket ID:" << sock << endl;
	send(sock, client_send_data, strlen(client_send_data), 0);

	bytes_recieved = recv(sock, client_recv_data, DATA_SIZE_KILO, 0);
	client_recv_data[bytes_recieved] = '\0';

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
	memset(broad_client_recv_data, 0, sizeof broad_client_recv_data);
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

void supportBroadCast(broadcastType type) {
	switch (type) {
	case BROADCAST_ALL:
		populateClientSendDataForBroadcast();
		break;
	case BROADCAST_GAME_OVER:
		populateClientSendDataForGameOver();
		break;
	case BROADCAST_JOINING:
		populateClientSendDataForJoining();
		break;
	}

	for (int i = NUM_OF_PLAYERS - 1; i >= 0; i--) { //loop is backwards, since we want to send info to server at last (Don't ask why)
		if (players[i].status == STATUS_PRESENT) {
			strcpy(broadIp2Join, players[i].networkDetails->ip);
			broadRemote_port = players[i].networkDetails->port;
			connectServerBroadcast(i);
		}
	}
}

void* threadClientBroadcast(void* arg) {
	cout << "Broadcast Client Thread started" << endl;

	while (!isGameOver) {
		supportBroadCast(BROADCAST_ALL);
	}

	cout << "Master node: Game over!!!" << endl;
	cout << "Winning Team: " << winningTeam << endl;
	cout << "Stopping broadcast!!" << endl;
	supportBroadCast(BROADCAST_GAME_OVER);

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

connectStatus helperSendConnect() { //returns true
	cout << "trying to connect to host" << endl;

	strcpy(client_send_data, MSG_CONNECT);
	strcat(client_send_data, selfNode->ipWithPort);
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataAndWaitForResult();

	cout << "helperSendConnect rcvd- " << client_recv_data << endl; //TODO: remove

	char* type = substring(client_recv_data, 0, 2);
	char* data = substring(client_recv_data, 3, strlen(client_recv_data));

	if (type[0] == 'x' || strcmp(type, SERVER_REQ_IGNORED) == 0) {
		//gameDetails.isConnectedToServer = false; //TODO: remove
		cout << "reached" << endl;
		gameDetails.isIssueConnectingToServer = true;
		return CONNECTED_NOT;
	}

	if (strcmp(type, MSG_SERVER_ACK) == 0) {
		//gameDetails.isConnectedToServer = true;//TODO: remove
		gameDetails.isIssueConnectingToServer = false;
		//gameDetails.myId = atoi(data); //TODO: remove
		currPlayerId = atoi(data);
		return CONNECTED_NEW;
	}

	if (strcmp(type, MSG_SERVER_WELCOME_BACK) == 0) {
		//gameDetails.isConnectedToServer = true;//TODO: remove
		gameDetails.isIssueConnectingToServer = false;
		//TODO: copy details
		return CONNECTED_ALREADY;
	}

	return CONNECTED_NOT;
}

bool isServerOk() {
	char *type = substring(client_recv_data, 0, 2);
	if (strcmp(type, MSG_SERVER_ACK) == 0) {
		return true;
	}

	if (strcmp(type, MSG_SERVER_REJECT) == 0) {
		return false;
	}

	return false;
}

bool helperValidateTeam(teamName team) {
	strcpy(client_send_data, MSG_VALIDATE_TEAM);
	strcat(client_send_data, numToChar(team));
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();
	cout << "helperValidateTeam rcvd- " << client_recv_data << endl; //TODO: remove
	return isServerOk();
}

bool helperValidateHero(heroes hero) {
	strcpy(client_send_data, MSG_VALIDATE_HERO);
	strcat(client_send_data, numToChar(hero));
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();
	cout << "helperValidateHero rcvd- " << client_recv_data << endl; //TODO: remove
	return isServerOk();
}

void helperRequestPlayersDetails() {
	strcpy(client_send_data, MSG_REQ_PLAYER_DETAILS);
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();
	cout << "helperRequestPlayersDetails rcvd- " << client_recv_data << endl; //TODO: remove

	split(client_recv_data, ',', DATA_RCVD);
	int k = 0, i = 0;
	for (i = 0; i < NUM_OF_PLAYERS; i++) {
		players[i].status = STATUS_PRESENT;
		players[i].heroType = static_cast<heroes> (atoi(DATA_RCVD[k++]));

		teamName team = static_cast<teamName> (atoi(DATA_RCVD[k++]));
		if (team == TEAM_ANGELS) {
			players[i].team = &angelsTeam;
		} else if (team == TEAM_DEMONS) {
			players[i].team = &demonsTeam;
		}

		players[i].isFirstPlayerInTeam = atoi(DATA_RCVD[k++]);

		if (DATA_RCVD[k][0] == 'x') {
			break; //breaking since all subsequent players must not be joined
		}
	}

	i++;
	while (i < NUM_OF_PLAYERS) {
		players[i++].status = STATUS_NOT_JOINED;
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
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void helperSendPowerMode(int type) {

	if (type == 0)
		strcpy(client_send_data, MSG_POWER_BASIC);
	else if (type == 1)
		strcpy(client_send_data, MSG_POWER_MAGIC);
	else
		return;//something is wrong

	//setting the remoteNode ip & port
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

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
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

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
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

//for non-primary nodes
void processGameOver(char* data) {
	cout << "At non-primary node: gameover received" << endl;
	cout << "At non-primary node: winningTeam " << data << endl;
	gameDetails.winningTeam = static_cast<teamName> (atoi(data));
	gameDetails.isGameOver = true;
}

void processJoining(char* data) {
	gameDetails.isStartJoiningTimer = true;
}

//used by non-primary nodes
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
		cout << GLOBAL_ARR[1] << endl; //TODO: remove
		split(GLOBAL_ARR[1], ',', DATA_RCVD);
		k = 0;
		for (int i = 0; i < NUM_OF_PLAYERS; i++) {
			players[i].isFirstPlayerInTeam = atoi(DATA_RCVD[k++]);
			angelsTeam->templeHealth = atoi(DATA_RCVD[k++]);
			demonsTeam->templeHealth = atoi(DATA_RCVD[k++]);
			players[i].currPowerMode = static_cast<powerMode> (atoi(
					DATA_RCVD[k++]));
			players[i].heroHealth = atoi(DATA_RCVD[k++]);
			players[i].strength = atoi(DATA_RCVD[k++]);
			players[i].speedMove = atoi(DATA_RCVD[k++]);
			players[i].curseType = static_cast<curse> (atoi(DATA_RCVD[k++]));
			players[i].isTimerItemGlobalRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerMagicSpellRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerCurseRunning = atoi(DATA_RCVD[k++]);

			if (DATA_RCVD[k][0] == 'x') {
				break;
			}
		}
	}

	strcpy(server_send_data, MSG_SERVER_ACK);
}

int getIpOfPlayer(playerStatus status) {

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == status) {
			return i;
		}
	}

	return -1;
}

void processConnect(char *data) {

	cout << "received connect message: " << data << endl;

	if (!gameDetails.isTimerNotHostWaiting) { //host waiting
		int notJoinedIp = getIpOfPlayer(STATUS_NOT_JOINED);

		if (notJoinedIp == -1) { //all players have joined
			strcpy(server_send_data, SERVER_REQ_IGNORED);
		}

		else { //some player has not joined, joining now
			players[notJoinedIp].status = STATUS_JOINED;
			players[notJoinedIp].networkDetails = convertToNodeHelper(data);
			strcpy(server_send_data, MSG_SERVER_ACK);
			strcat(server_send_data, numToChar(notJoinedIp));
		}

	}

	else { //host is not waiting (most probably while in the game)
		int leftIp = getIpOfPlayer(STATUS_LEFT);

		if (leftIp == -1) { //all players are in the game
			strcpy(server_send_data, SERVER_REQ_IGNORED);
		}

		else { //some player has left
			players[leftIp].status = STATUS_PRESENT;
			players[leftIp].networkDetails = convertToNodeHelper(data);
			strcpy(server_send_data, MSG_SERVER_WELCOME_BACK);
		}
	}
}

int getMembersInTeam(teamName team) {
	int memberCount = 0;
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT && team
				== players[i].team->name) {
			memberCount++;
		}
	}

	return memberCount;
}

void processValidateTeam(char *data, int id) {
	cout << "received validateTeam msg: '" << data << "' from: " << id << endl;

	teamName team = static_cast<teamName> (atoi(data));

	int memberCount = getMembersInTeam(team);

	if (memberCount == 2) {
		strcpy(server_send_data, MSG_SERVER_REJECT);
		return;
	}

	if (team == TEAM_ANGELS) {
		players[id].team = &angelsTeam;
	} else {
		players[id].team = &demonsTeam;
	}

	players[id].status = STATUS_PRESENT;

	if (memberCount == 0) {
		players[id].isFirstPlayerInTeam = true;
	}

	strcpy(server_send_data, MSG_SERVER_ACK);

}

bool isHeroTaken(heroes hero) {
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT && hero == players[i].heroType) {
			return true;
		}
	}

	return false;
}

void processValidateHero(char *data, int id) {
	cout << "received validateHero msg: " << data << "' from: " << id << endl;

	heroes hero = static_cast<heroes> (atoi(data));
	if (!isHeroTaken(hero)) {
		players[id].heroType = hero;
		strcpy(server_send_data, MSG_SERVER_ACK);
	}

	else {
		strcpy(server_send_data, MSG_SERVER_REJECT);
	}
}

void processReqPlayerDetails(char *data) {
	cout << "received reqPlayerDetails msg: " << data << endl;

	strcpy(server_send_data, MSG_SERVER_ACK);

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT) {
			strcat(server_send_data, numToChar(players[i].heroType));
			strcat(server_send_data, ",");
			strcat(server_send_data, numToChar(players[i].team->name));
			strcat(server_send_data, ",");
			strcat(server_send_data, numToChar(players[i].isFirstPlayerInTeam));
			strcat(server_send_data, ",");
		} else {
			strcat(server_send_data, "x");
			break; //breaking since if a client is found not joined, all subsequent shall also not be joined
		}
	}
}

//used by master/primary node and no one else
void processGeneral(char *completeData) {
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

void populateClientSendDataForGameOver() {
	cout << "At master node: called me!! game over!!" << endl;
	strcpy(broad_send_data, MSG_GAME_OVER);
	strcat(broad_send_data, numToStr(winningTeam).c_str());
}

void populateClientSendDataForJoining() {
	cout << "Broadcast Joining" << endl;
	strcpy(broad_send_data, MSG_JOINING);
}

void populateClientSendDataForBroadcast() {
	strcpy(broad_send_data, MSG_BROADCAST);

	//populating 'broad_client_send_data' with grid
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			strcat(broad_send_data,
					numToStr(getGridChar(i, j, true, true)).c_str());
			strcat(broad_send_data, "|");
		}
	}

	strcat(broad_send_data, "+");

	//populating 'broad_client_send_data' with attributes
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status != STATUS_NOT_JOINED) {

			Player player = players[i];

			strcat(broad_send_data,
					numToStr(player.isFirstPlayerInTeam).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(angelsTeam->templeHealth).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(demonsTeam->templeHealth).c_str());
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

			strcat(broad_send_data,
					numToStr(player.isTimerCurseRunning).c_str());
			strcat(broad_send_data, ",");
		}

		else {
			strcat(broad_send_data, "x");
			break;
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

		players[playerId].status = STATUS_NOT_REACHABLE;

		//trying again assuming the server is busy
		retriedCount++;
		cout << "BroadCast-playerId: " << playerId << " busy --- retrying("
				<< retriedCount << "/" << RETRY_COUNT << ")" << endl;
		sleep(1);
		if (retriedCount == RETRY_COUNT) {

			//client is DEAD!!!
			cout << "playerId: " << playerId
					<< " is no more alive, let's continue" << endl;
			players[playerId].status = STATUS_LEFT;

			close(sock);
			return false;
		}
	}

	if (players[playerId].status == STATUS_NOT_REACHABLE) {
		players[playerId].status = STATUS_PRESENT;
		cout << "playerId: " << playerId << " back now" << endl;
	}

	//cout << "Client successfully connected to server" << endl;
	return true;
}

bool isValidRequest(int requestingPlayerId) { //TODO: function not required....check whether shall be removed
	if (!isAllClientsAlive()) {
		return false;
	}

	if (players[requestingPlayerId].status == STATUS_PRESENT) { //this is to ignore requests from dead client
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

		/*	if (!isValidRequest(requestingPlayerId)) { //TODO: check if required, else remove
		 strcpy(server_send_data, SERVER_REQ_IGNORED);
		 }*/

		if (strcmp(type, MSG_BROADCAST) == 0) {
			processBroadcast(reqData);
		}

		else if (strcmp(type, MSG_CONNECT) == 0) {
			processConnect(reqData);
		}

		else if (strcmp(type, MSG_VALIDATE_TEAM) == 0) {
			processValidateTeam(reqData, requestingPlayerId);
		}

		else if (strcmp(type, MSG_VALIDATE_HERO) == 0) {
			processValidateHero(reqData, requestingPlayerId);
		}

		else if (strcmp(type, MSG_REQ_PLAYER_DETAILS) == 0) {
			processReqPlayerDetails(reqData);
		}

		//for non-primary nodes
		else if (strcmp(type, MSG_GAME_OVER) == 0) {
			processGameOver(reqData);
		}

		else if (strcmp(type, MSG_JOINING) == 0) {
			processJoining(reqData);
		}

		//for primary node
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
