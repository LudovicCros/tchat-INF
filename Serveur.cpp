/*
 * Serveur.cpp
 *
 *  Created on: 27 oct. 2017
 *      Author: infi
 */

#include "Serveur.h"
#include <iostream>
#include <vector>
#include <boost/regex.hpp>

using namespace std;

Serveur::Serveur() {
	// TODO Auto-generated constructor stub
	this->host = "";
	this->port = 10000;
}

Serveur::~Serveur() {
	// TODO Auto-generated destructor stub
}

void Serveur::enterInformation() {
	bool continu = true;
	boost::regex ipTest ("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
	while(continu) {
		cout << "enter a valide ip" << endl;
		string ip;
		cin >> ip;
		if(boost::regex_match(ip, ipTest)) {
			continu = false;
		}
	}
}

int main(void) {
	Serveur serv;
	serv.enterInformation();
	return 0;
}

