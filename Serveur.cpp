/*
* Serveur.cpp
*
*  Created on: 27 oct. 2017
*      Author: infi
*/


#include "stdafx.h"

#include "Serveur.h"
#include <iostream>
#include <vector>
#include <boost/regex.hpp>

using namespace std;

Serveur::Serveur() {
	this->host = "";
	this->port = 10000;
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
		cout << "Veuillez entrer le port du serveur (entre 10000 et 10050):" << endl;
		int portGiven;
		cin >> portGiven;
		if (10000 <= portGiven  && portGiven <= 10050) {
			continu = false;
			this->port = portGiven;
		}
		else {
			cout << "Le port doit etre valide ! (Entre 10000 et 10050)" << endl;
		}
	}
}

string Serveur::getIP() {
	return this->host;
}

int Serveur::getPort() {
	return this->port;
}


int main(void) {
	Serveur serv;
	serv.enterInformation();
	int quit=1;
	while (quit != 0) {
		cout << "Port : " << serv.getPort() << endl;
		cout << "IP : " << serv.getIP() << endl;
		cin >> quit;
	}
	return 0;
}

