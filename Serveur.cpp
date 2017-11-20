


#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Serveur.h"
#include <iostream>
#include <vector>
#include <boost/regex.hpp>
#pragma comment( lib, "ws2_32.lib" )

using namespace std;

DWORD WINAPI EchoHandler(void* sd_)
{
	SOCKET sd = (SOCKET)sd_;

	// Read Data from client
	char readBuffer[10];
	int readBytes;

	readBytes = recv(sd, readBuffer, 7, 0);
	if (readBytes > 0) {
		cout << "Received " << readBytes << " bytes from client." << endl;
		cout << "Received " << readBuffer << " from client." << endl;
		send(sd, readBuffer, 7, 0);
	}
	else if (readBytes == SOCKET_ERROR) {
		cout << "Echec de la reception !" << endl;
	}
	closesocket(sd);

	return 0;
}

Serveur::Serveur() {
	this->host = "";
	this->port = 5000;
	this->messages = vector<string>();
}

Serveur::~Serveur() {
	// TODO Auto-generated destructor stub
}

void Serveur::enterInformation() {
	this->validIP();
	this->validPort();	
	this->waitConnexion();
}

void Serveur::validIP() {
	bool continu = true;
	boost::regex ipTest("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
	while (continu) {
		cout << "Veuillez entrer l'adresse IP du serveur:" << endl;
		string ip;
		cin >> ip;
		if (boost::regex_match(ip, ipTest)) {
			continu = false;
			this->host = ip;
		}
		else {
			cout << "L'adresse doit etre valide!" << endl;
		}
	}
}

void Serveur::validPort() {
	bool continu = true;
	while (continu) {
		cout << "Veuillez entrer le port du serveur (entre 5000 et 5050):" << endl;
		int portGiven;
		cin >> portGiven;
		if (5000 <= portGiven  && portGiven <= 5050) {
			continu = false;
			this->port = portGiven;
		}
		else {
			cout << "Le port doit etre valide ! (Entre 5000 et 5050)" << endl;
		}
	}
}

void Serveur::waitConnexion() {
	this->rsock = socket(AF_INET, SOCK_STREAM, 0);
	const char *cstr = this->host.c_str();
	this->rsin.sin_addr.s_addr = inet_addr(cstr);
	this->rsin.sin_family = AF_INET;
	this->rsin.sin_port = htons(this->port);
	bind(this->rsock, (SOCKADDR *)&rsin, sizeof(this->rsin));
	listen(this->rsock, 0);
	while (true) {
		sockaddr_in sinRemote;
		int nAddrSize = sizeof(sinRemote);
		// Create a SOCKET for accepting incoming requests.
		// Accept the connection.
		SOCKET sd = accept(this->rsock, (sockaddr*)&sinRemote, &nAddrSize);
		if (sd != INVALID_SOCKET) {
			cout << "Connection acceptee De : " <<
				inet_ntoa(sinRemote.sin_addr) << ":" <<
				ntohs(sinRemote.sin_port) << "." <<
				endl;

			DWORD nThreadID;
			CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
		}
		else {
			cerr << "Echec d'une connection." << endl;
			// return 1;
		}
	}
}

string Serveur::getHost() {
	return this->host;
}

int Serveur::getPort() {
	return this->port;
}


int main(void) {
	Serveur serv;
	serv.enterInformation();
	return 0;
}

