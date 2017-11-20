

#ifndef SERVEUR_H_
#define SERVEUR_H_

#include <string>
#include <vector>
#include <winsock2.h>
#include <algorithm>
#include <strstream>
#include <iostream>
#pragma comment( lib, "ws2_32.lib" )

using namespace std;

class Serveur {
public:
	Serveur();
	void enterInformation();
	void waitConnexion();
	int connexion(string user, string pass);
	void receiveMessage();
	void sendMessages(string message);
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
	SOCKADDR_IN rsin;
};

#endif /* SERVEUR_H_ */
