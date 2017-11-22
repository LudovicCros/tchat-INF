#ifndef SERVEUR_H_
#define SERVEUR_H_

#include <string>
#include <vector>
#include <winsock2.h>
#include <algorithm>
#include <strstream>
#include <iostream>
#include <mutex>
#pragma comment( lib, "ws2_32.lib" )

const int NMAX = 200;
const int USERMAX = 30;
const int MAXMESSAGE = NMAX + USERMAX;

using namespace std;

class Serveur;

typedef struct {
	SOCKET* sock;
	string name;
}Client;

typedef struct {
	SOCKET* sock;
	Serveur* serv;
}ThreadParam;

class Serveur {
public:
	Serveur();
	void enterInformation();
	void waitConnexion();
	vector<string> identification(SOCKET sock);
	int connexion(string user, string pass);
	string receiveMessage(int index);
	int sendMessages(string message);
	int sendMessage(int index, string message);
	int getPort();
	string getHost();
	void validIP();
	void validPort();
	virtual ~Serveur();

private:
	vector<string> messages;
	string host;
	int port;
	SOCKET rsock;
	vector<Client> users;
	SOCKADDR_IN rsin;
	mutex verrou;
};

#endif /* SERVEUR_H_ */

