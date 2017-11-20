

#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>

using namespace std;

class Client {

public:
	Client();
	void enterInformation();
	void receiveMessage();
	void sendMessage(string message);
	int getPort();
	string getHost();
	void validIP();
	void validPort();
	void connexion();
	virtual ~Client();

private:
	string user;
	string pass;
	vector<string> messages;
	string host;
	int port;
};

#endif /* CLIENT_H_ */
