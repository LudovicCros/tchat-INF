#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>
const int NMAX = 200;
const int USERMAX = 30;
const int MAXMESSAGE = NMAX + USERMAX;

class Client;

typedef struct {
	SOCKET* sock;
	Client* clt;
}ThreadParam;

using namespace std;

class Client {

public:
	Client();
	void enterInformation();
	string receiveMessage();
	int listenMessage();
	int processSend();
	int sendMessage(string message, int maxSize);
	int getPort();
	string getHost();
	void validIP();
	void validPort();
	void connexion();
	virtual ~Client();

private:
	string user;
	string pass;
	string host;
	int port;
	SOCKET sock;
};

#endif /* CLIENT_H_ */