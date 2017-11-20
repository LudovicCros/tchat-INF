

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <boost/regex.hpp>
#include "Client.h"

Client::Client() {
	// TODO Auto-generated constructor stub
	this->host = "";
	this->port = 5000;
	this->messages = vector<string>();
	this->user = "";
	this->pass = "";
}

void Client::enterInformation() {
	this->validIP();
	this->validPort();
	this->connexion();
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
	int quit = 1;
	while (quit != 0) {
		cout << "Port : " << clt.getPort() << endl;
		cout << "IP : " << clt.getHost() << endl;
		cin >> quit;
	}
	return 0;
}

