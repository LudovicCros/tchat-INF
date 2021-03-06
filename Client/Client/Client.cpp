

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <boost/regex.hpp>
#include "Client.h"
// Link avec ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

using namespace std;


DWORD WINAPI gestionThread(void* param)
{
	ThreadParam* p = (ThreadParam*)param;
	SOCKET* sd = p->sock;
	Client* clt = p->clt;
	//reception des messages
	while (true) {
		string mess = clt->receiveMessage();
		if (mess != "") {
			cout << mess << endl;
		}
	}
	return 0;
}

Client::Client() {
	// TODO Auto-generated constructor stub
	this->host = "";
	this->port = 5000;
	this->user = "";
	this->pass = "";
}

void Client::enterInformation() {
	this->validIP();
	this->validPort();
}

void Client::validIP() {
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

void Client::validPort() {
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

void Client::connexion() {
	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	//--------------------------------------------
	// InitialisATION de Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("Erreur de WSAStartup: %d\n", iResult);
		return ;
	}
	// On va creer le socket pour communiquer avec le serveur
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock == INVALID_SOCKET) {
		printf("Erreur de socket(): %ld\n\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return ;
	}
	//--------------------------------------------
	// On va chercher l'adresse du serveur en utilisant la fonction getaddrinfo.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;        // Famille d'adresses
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  // Protocole utilisé par le serveur

									  // On indique le nom et le port du serveur auquel on veut se connecter
									  //char *host = "L4708-XX";
									  //char *host = "L4708-XX.lerb.polymtl.ca";
									  //char *host = "add_IP locale";

	// getaddrinfo obtient l'adresse IP du host donné
	iResult = getaddrinfo(this->host.c_str(), to_string(this->port).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("Erreur de getaddrinfo: %d\n", iResult);
		WSACleanup();
		return;
	}
	//---------------------------------------------------------------------		
	//On parcours les adresses retournees jusqu'a trouver la premiere adresse IPV4
	while ((result != NULL) && (result->ai_family != AF_INET))
		result = result->ai_next;

	//	if ((result != NULL) &&(result->ai_family==AF_INET)) result = result->ai_next;  

	//-----------------------------------------
	if (((result == NULL) || (result->ai_family != AF_INET))) {
		freeaddrinfo(result);
		printf("Impossible de recuperer la bonne adresse\n\n");
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return ;
	}

	sockaddr_in *adresse;
	adresse = (struct sockaddr_in *) result->ai_addr;
	//----------------------------------------------------
	printf("Adresse trouvee pour le serveur %s : %s\n\n", this->host.c_str(), inet_ntoa(adresse->sin_addr));
	printf("Tentative de connexion au serveur %s avec le port %d\n\n", inet_ntoa(adresse->sin_addr), this->port);

	// On va se connecter au serveur en utilisant l'adresse qui se trouve dans
	// la variable result.
	iResult = connect(this->sock, result->ai_addr, (int)(result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		printf("Impossible de se connecter au serveur %s sur le port %d\n\n", inet_ntoa(adresse->sin_addr), this->port);
		freeaddrinfo(result);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		return ;
	}

	printf("Connecte au serveur %s:%d\n\n", this->host.c_str(), this->port);
	freeaddrinfo(result);

	//----------------------------
	// Demander à l'usager le user et password
	printf("Saisir un username: ");
	std::getline(std::cin, this->user);
	std::getline(std::cin, this->user);
	if(this->sendMessage(this->user, USERMAX)) {
		printf("Erreur de connexion");
		WSACleanup();
		exit(1);
	}
	printf("Saisir un password: ");
	std::getline(std::cin, this->pass);
	if(this->sendMessage(this->pass, USERMAX)) {
		printf("Erreur de connexion");
		WSACleanup();
		exit(1);
	}
	// récupérer la réponse du serveur
	string reponse = this->receiveMessage();
	if (reponse != "ok") {
		cout << "Erreur dans la saisie du mot de passe" << endl;
		exit(0);
	}
	return;
}

string Client::receiveMessage() {
	char motRecu[MAXMESSAGE+1];
	string resultat = "";
	int iResult = recv(this->sock, motRecu, MAXMESSAGE+1, 0);
	if (iResult > 0) {
		resultat = string(motRecu);
	}
	else {
		printf("Erreur de reception : %d\n", WSAGetLastError());
	}
	return resultat;
}

int Client::sendMessage(string message, int maxSize) {

	if (message.size() > maxSize) {
		cout << "Erreur : Le mot saisie depasse les "<< maxSize << " caracteres" << endl;
		return 1;
	}
	int iResult = send(this->sock, message.c_str(), message.size()+1, 0);
	if (iResult == SOCKET_ERROR) {
		printf("Erreur du send: %d\n", WSAGetLastError());
		closesocket(this->sock);
		WSACleanup();
		printf("Appuyez une touche pour finir\n");
		getchar();
		exit(1);
	}
	return 0;
}

int Client::listenMessage() {
	DWORD nThreadID;
	ThreadParam* param = new ThreadParam();
	param->sock = &(this->sock);
	param->clt = this;
	CreateThread(0, 0, gestionThread, (void*)param, 0, &nThreadID);
	return nThreadID;
}

int Client::processSend() {
	string message = "";
	cout << "Vous pouvez maintenant envoyer des messages" << endl;
	while(true) {
		std::getline(std::cin, message);
		if (this->sendMessage(message, NMAX)) {
			cout << "Erreur : l'envoie du message a echoue" << endl;
		}
	}
}

void Client::receptionHistorique() {
	string mess = "nok";
	string ok = "ok";
	string nok = "nok";
	int nb;
	cout << "Bienvenue " << this->user << endl;
	cout << "Historique :"  << endl;
	bool continuer = true;
	while (continuer) {
		mess = this->receiveMessage();
		if (mess == "") {
			return;
		}
		try {
			nb = std::stoi(mess,nullptr,0);
			this->sendMessage(ok, MAXMESSAGE);
			continuer = false;
		}
		catch (invalid_argument ev) {
			this->sendMessage(nok, MAXMESSAGE);
		}
	}
	for (int i = 0;i < nb;i++) {
		mess = this->receiveMessage();
		if (mess == "") {
			return;
		}
		cout << mess << endl;
		this->sendMessage(ok, MAXMESSAGE);
	}
}

string Client::getHost() {
	return this->host;
}

int Client::getPort() {
	return this->port;
}

Client::~Client() {
	// TODO Auto-generated destructor stub
}

int main(void) {
	Client clt;
	clt.enterInformation();
	clt.connexion();
	clt.receptionHistorique();
	clt.listenMessage();
	clt.processSend();
	return 0;
}

