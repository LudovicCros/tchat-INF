

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Serveur.h"
#include <iostream>
#include <vector>
#include <boost/regex.hpp>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace std;


DWORD WINAPI EchoHandler(void* sd_)
{
	SOCKET sd = (SOCKET)sd_;

	// Read Data from client
	char readBuffer[201];
	int readBytes;

	readBytes = recv(sd, readBuffer, 200, 0);
	if (readBytes > 0) {
		cout << "Received " << readBytes << " bytes from client." << endl;
		cout << "Received " << readBuffer << " from client." << endl;
		send(sd, readBuffer, 200, 0);
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
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		cerr << "Error at WSAStartup()\n" << endl;
		return;
	}
	this->rsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->rsock == INVALID_SOCKET) {
		cerr << "Error at socket()" << endl;
		WSACleanup();
		return;
	}
	char option[] = "1";
	setsockopt(this->rsock, SOL_SOCKET, SO_REUSEADDR, option, sizeof(option));
	hostent *thisHost;

	thisHost = gethostbyname(this->host.c_str());
	char* ip;
	ip = inet_ntoa(*(struct in_addr*) *thisHost->h_addr_list);
	this->rsin.sin_addr.s_addr = inet_addr(ip);
	this->rsin.sin_family = AF_INET;
	this->rsin.sin_port = htons(this->port);
	if (::bind(this->rsock, (SOCKADDR *) &(this->rsin), sizeof(this->rsin)) == SOCKET_ERROR) {
		cerr << "bind() failed." << endl;
		closesocket(this->rsock);
		WSACleanup();
		return ;
	}
	if (listen(this->rsock, 30) == SOCKET_ERROR) {
		cerr << "Error listening on socket." << endl;
		closesocket(this->rsock);
		WSACleanup();
		return ;
	}
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
			closesocket(this->rsock);
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
	serv.waitConnexion();
	return 0;
}


