/*
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/

#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "snakegame.h"
#include <queue>
#include <tuple>
#include <chrono>

using namespace std;

webSocket server;
SnakeGame game;
int current_time = 0;
int next_time = 40;
vector<int> clientIDs;
bool already2connected = false;
long long NTP_X;
long long NTP_Y;
long long NTP_A;
long long NTP_X2;
long long NTP_Y2;
long long NTP_A2;


struct messageBlock{
	string message;
	long long current_time; //the current time; also used to store clientID for the putinqueue (not intuitive, and kinda gross)
	long long lag_time; //a lagged timestamp in the future

	messageBlock(string a, long long b, long long c) : message(a),
										current_time(b), 
										lag_time(c)
										{}
	bool operator<(const struct messageBlock& other) const
	{
		return lag_time > other.lag_time;
	}
};

priority_queue<messageBlock> inMessageQueue;
priority_queue<messageBlock> outMessageQueue;



void openHandler(int clientID);
void closeHandler(int clientID);
void messageHandler(int clientID, string message);
void periodicHandler();

long long currtime()
{
	//gets time in seconds and converts it to milliseconds, since Javascript can only use milliseconds
	chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
	return millis;
}


void putOutQueue(string message) {
	long long current_time = currtime(); //retrieves the current time
	long long lag = rand() % 200 + 200; //randomly generates 500-1000 miliseconds of lag
	long long lag_time = current_time + lag; //adds lag to create a scheduled dequeue time
	outMessageQueue.push(messageBlock(message, current_time, lag_time)); //add message with time-lock to the queue
}

void putInQueue(string message, int clientID) {
	//instead of current time for when the message was sent, we use clientID to determine which client sent the message
	long long current_time = currtime();
	long long lag = rand() % 200 + 200;
	long long lag_time = current_time + lag;
	inMessageQueue.push(messageBlock(message, (long long) clientID, lag_time));
}

void process_message(int clientID, string message){
	//called when checking the queue. Responds to messages according to their labels and contents

	ostringstream os;
	
	if (message.find("Direction") == 0)
	{	
		if (clientID == 0)
		{
			game.setp1direction(message.substr(11));
		}
		else
		{
			game.setp2direction(message.substr(11));
		}
	}
	else if (message.find("Player ID") == 0)
	{
		if (clientID == 0)
		{
			game.setp1id(message.substr(11));
			os << "Player:1:" + game.getp1id();
			putOutQueue(os.str());
		}
		else if(clientID == 1)
		{
			game.setp2id(message.substr(11));
			os << "Player:2:" + game.getp2id();
			putOutQueue(os.str());
		}
	}
	else if (message.find("Player1 Score") == 0)
	{
		game.setp1score(message.substr(15));
		os << "Player" << game.getp1id() << " Score: " << game.getp1score();
		putOutQueue(os.str());
	}
	else if (message.find("Player2 Score") == 0)
	{
		game.setp2score(message.substr(15));
		os << "Player" << game.getp2id() << " Score: " << game.getp2score();
		putOutQueue(os.str());
	}
}

void updateClients(string message)
{
	//goes through all the clients and sends a message to all of them.
	//only called in init, as none of these messages are lagged.

	vector<int> clientIDs = server.getClientIDs();
	for (unsigned int i = 0; i < clientIDs.size(); i++)
	{
		server.wsSend(clientIDs[i], message);
	}

}

void checkQueues()
{
	//called at end of game loop
	// if either queue (in or out) still has a message, check to see if any message is ready to process
	//   if a message is ready, process it. the top message is always handled first

	if (!outMessageQueue.empty())
	{
		if (currtime() >= outMessageQueue.top().lag_time)
		{
			pair < string, long long > result(outMessageQueue.top().message, outMessageQueue.top().current_time);

			string message = result.first + ":" + to_string(result.second);
			updateClients(message);
			outMessageQueue.pop(); //pops message so it is not called again
		}
	}

	if (!inMessageQueue.empty())
	{
		if (currtime() >= inMessageQueue.top().lag_time)
		{
			int clientID = (int) inMessageQueue.top().current_time; //here, we repurpose the "current_time" field to hold clientID's
			string message = inMessageQueue.top().message;
			process_message(clientID, message);
			inMessageQueue.pop();
		}
	}
}


void init() {
	
	if (server.getClientIDs().size() == 2) { //only proceeds if 2 clients have connected
		
		already2connected = true;
		//creates servergame and sends information to each of the clients for the snakes, board, and food

		//synchronize the server times
		//send message to client requesting A
		updateClients("A"); //updateClients bypasses lag

		game.createSnakes(); //initializes the snakes's positions and directions in the game state
		pair<string, string> snakes = game.getSnakeMessages(); //returns a pair containing the snakes converted to strings. This is needed for the clients to read

		updateClients(snakes.first);
		updateClients(snakes.second);

		updateClients(game.getDirectionMessage()); //checks snake directions and updates clients

		game.createFood();
		updateClients(game.getFoodMessage()); //sends food location to clients

		ostringstream os;
		updateClients("Scores:" + game.getp1score() + ":" + game.getp2score()); //initializes scores to zero
		
		updateClients("Initialize"); //tells the clients to call init and start running their drawing loops

		// updateClients();

		//starts game loop
		server.setPeriodicHandler(periodicHandler); //starts the server's game loop
	}
}

/* called when a client connects */
void openHandler(int clientID){
	//called whenever somebody enters the server

	if (!already2connected){ //if we don't already have 2 players, assign an ID to each player that joins
		ostringstream os;

		os << "Player:" << clientID + 1;
		updateClients(os.str());

		os.str("");
		os.clear();
		os << "Identity:" << clientID + 1;
		server.wsSend(clientID, os.str());

		init();
	}
	else
		server.wsClose(clientID);
}

/* called when a client disconnects */
void closeHandler(int clientID){
	//called when someone disconnects.
	//tells the other player that their partner has disconnected, and sets periodic handler to null (which should stop the game loop)

	ostringstream os;
	string playername;
	if (clientID == 0)
		playername = game.getp1id();
	else
		playername = game.getp2id();

	os << "Player:" << clientID + 1 << ": " << playername << " left.";
	vector<int> clientIDs = server.getClientIDs();
	for (unsigned int i = 0; i < clientIDs.size(); i++)
	{
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}
    
    server.setPeriodicHandler(NULL);
    already2connected = false;
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
	
	if (message.find("Time") == 0) //ensures that the time syncing algorithm is not lagged
	{
		if (clientID == 0) //responds accordingly to the correct client
		{
			NTP_X = currtime();
			NTP_A = stoll(message.substr(6));
			NTP_Y = currtime();
			ostringstream os;
			os << "XYA:" << to_string(NTP_X) << ":" << to_string(NTP_Y) << ":" << to_string(NTP_A);
			server.wsSend(clientID, os.str()); //send a message to the client that bypasses queue lag, containing X, Y, and A
		}
		else
		{
			NTP_X2 = currtime();
			NTP_A2 = stoll(message.substr(6));
			NTP_Y2 = currtime();
			ostringstream os;
			os << "XYA:" << to_string(NTP_X2) << ":" << to_string(NTP_Y2) << ":" << to_string(NTP_A2);
			server.wsSend(clientID, os.str());
		}
	}
	else
		putInQueue(message, clientID);
	
}

/* called once per select() loop */
void periodicHandler(){


    if (game.checkgameEnd()) //if the game has ended, stops the periodic handler by setting it to null
    {
		updateClients("Player Dead");

    	server.setPeriodicHandler(NULL);
    }

    current_time++;
    if (current_time >= next_time) //ensures that these updates occur only about every 40 milliseconds
    {
    	//move snakes here
    	game.advanceGame();

    	pair<string, string> snakes = game.getSnakeMessages(); //updates the clients with the current snake states

    	putOutQueue(snakes.first);
    	putOutQueue(snakes.second);
    	putOutQueue("Scores:" + game.getp1score() + ":" + game.getp2score());
    	putOutQueue(game.getFoodMessage());

    	current_time = 0;
    }

    checkQueues(); //checks the queues fr any messages that are ready to handle
}

int main(int argc, char *argv[]){
    int port;


    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}

