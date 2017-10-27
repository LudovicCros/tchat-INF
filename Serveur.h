/*
 * Serveur.h
 *
 *  Created on: 27 oct. 2017
 *      Author: infi
 */

#ifndef SERVEUR_H_
#define SERVEUR_H_

#include <string>
#include <vector>

using namespace std;

class Serveur {
public:
	Serveur();
	void enterInformation();
	void launch();
	int connexion(string user, string pass);
	void receiveMessage();
	void sendMessages(string message);
	virtual ~Serveur();

private:
	vector<string> messages;
	string host;
	int port;

};

#endif /* SERVEUR_H_ */
