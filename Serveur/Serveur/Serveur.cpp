

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


DWORD WINAPI EchoHandler(void* param)
{
	ThreadParam* p = (ThreadParam*) param;
	SOCKET* sd = p->sock;
	Serveur* serv = p->serv;

	serv->identification(*sd);
	closesocket(*sd);

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
			ThreadParam* param = new ThreadParam();
			param->sock = &sd;
			param->serv = this;
			CreateThread(0, 0, EchoHandler, (void*) param, 0, &nThreadID);
		}
		else {
			cerr << "Echec d'une connection." << endl;
			closesocket(this->rsock);
			// return 1;
		}
	}
}

//renvoie un vector contenant [utilisateur,password], il est vide si il y a une erreur
vector<string> Serveur::identification(SOCKET sock) {
	char motRecu[USERMAX];
	vector<string> user;
	int iResult = recv(sock, motRecu, USERMAX, 0);
	if (iResult > 0) {
		user.push_back(string(motRecu));
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
		user.clear();
		return user;
	}
	iResult = recv(sock, motRecu, USERMAX, 0);
	if (iResult > 0) {
		user.push_back(string(motRecu));
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
		user.clear();
		return user;
	}
	return user;
}

string Serveur::receiveMessage(int index) {
	char motRecu[MAXMESSAGE];
	string resultat = "";
	int iResult = recv(*(users[index].sock), motRecu, MAXMESSAGE, 0);
	if (iResult > 0) {
		resultat = string(motRecu);
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
	}
	return resultat;
}

int Serveur::sendMessage(int index, string message) {

	if (message.size() > NMAX) {
		cout << "Erreur : Le mot saisie depasse les 200 caracteres" << endl;
		return 1;
	}
	string envoi = "";
	envoi += users[index].name + message;
	int iResult = send(*(users[index].sock), envoi.c_str(), envoi.size(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("Erreur du send: %d\n", WSAGetLastError());
		closesocket(*(users[index].sock));
		return 2;
	}
	return 0;
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


