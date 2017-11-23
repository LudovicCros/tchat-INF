

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
#include <fstream>
#include <boost/algorithm/string.hpp>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace std;


DWORD WINAPI EchoHandler(void* param)
{
	ThreadParam* p = (ThreadParam*) param;
	SOCKET* sd = p->sock;
	Serveur* serv = p->serv;

	vector<string> userInfos = serv->identification(*sd);
	cout << userInfos[0] << endl;
	cout << userInfos[1] << endl;
	string envoi = "ok";
	bool continuer = true;
	if (userInfos.empty()) {
		envoi = "connexion refusee";
		continuer = false;
	}
	if(continuer) {
		int res = serv->isLoginOk(userInfos);
		if (res == 2) {
			envoi = "connexion refusee";
			continuer = false;
		}
		else if (res == 1) {
			serv->addUser(userInfos, sd);
		}
	}
	int iResult = send(*sd, envoi.c_str(), envoi.size(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("Erreur du send: %d\n", WSAGetLastError());
		closesocket(*sd);
		return 0;
	}
	if (!continuer) {
		closesocket(*sd);
	}
	return 0;
}


int Serveur::addUser(vector<string> userInfos, SOCKET* sd) {
	Client* client = new Client();
	client->sock = sd;
	client->name = userInfos[0];	
	this->verrou.lock();
	int index = this->users.size();
	this->users.push_back(*client);
	

	// TODO : Ecrire dans fichier users
	ofstream fichier("../Serveur/bd/test.txt", ios::out | ios::app); 

	if (fichier)
	{
		fichier << endl << userInfos.at(0) << '$' << userInfos.at(1);
		fichier.close();
	}
	else {
		cerr << "Impossible d'ouvrir le fichier !" << endl;
	}
	this->verrou.unlock();
	return index;
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
		cout << "user : " << motRecu << endl;
		user.push_back(string(motRecu));
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
		user.clear();
		return user;
	}
	iResult = recv(sock, motRecu, USERMAX, 0);
	if (iResult > 0) {
		cout << "pass : " << motRecu << endl;
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

int Serveur::isLoginOk(vector<string> userInfos) {
	string userName = userInfos.at(0);
	string password = userInfos.at(1);

	// On interdit le caractère $
	std::size_t foundCaracUser = userName.find("$");
	std::size_t foundCaracPass = password.find("$");
	if (foundCaracUser != std::string::npos || foundCaracPass != std::string::npos) {
		return 2;
	}

	ifstream fichier("../Serveur/bd/test.txt", ios::in);  // on ouvre le fichier en lecture

	if (fichier)  
	{
		string ligne;
		while (getline(fichier, ligne)) 
		{
			vector<string> userInfos;
			boost::split(userInfos, ligne, boost::is_any_of("$"));
						
			if (userInfos.size() != 2) {
				continue;
			}
			
			if (userName == userInfos.at(0) && password == userInfos.at(1)) {
				return 0;				
			}
			else if (userName == userInfos.at(0) && password != userInfos.at(1)) {
				return 2;
			}
		}
		fichier.close();
		return 1;
	}
	return 2;
	
}


int main(void) {
	Serveur serv;
	serv.enterInformation();
	serv.waitConnexion();
	return 0;
}


