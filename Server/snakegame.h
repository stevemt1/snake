/*
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class SnakeGame
{

private:
    string p1_id;
    string p2_id;
    int p1_score;
    int p2_score;
    int canvasWidth = 800;
    int canvasHeight = 600;
    int pixelSize = 40;
    int gameHeight = canvasHeight / pixelSize;
    int gameWidth = canvasWidth / pixelSize;
    vector<vector<int>> food;
    vector<vector<int>> snake1;
    vector<vector<int>> snake2;
    bool gameEnd = false;
    vector<vector<int>> boundaries;

public:

    SnakeGame();
    // void reset();
    string getp1id();
    string getp2id();
    void setp1id(string id);
    void setp2id(string id);
    string getp1score();
    string getp2score();
    void setp1score(string score);
    void setp2score(string score);
    bool checkgameEnd();
    string getp1direction();
    string getp2direction();
    void setp1direction(string direction);
    void setp2direction(string direction);
    string getDirectionMessage();
    void gameBoundaries();
    void createFood();
    string getFoodMessage();
    void createSnakes();
    vector<int> moveSegment(vector<int> segment);
    vector<int> segmentFurtherForwardThan(int index, vector<vector<int>> snake);
    bool detectCollisionBetween(vector<int> pixelA, vector<int> pixelB);
    vector<vector<int>> moveSnake(vector<vector<int>> snake);
    bool ate(vector<vector<int> > snake, vector<vector<int> > otherThing);
    void advanceGame();
    pair<string, string> getSnakeMessages();

};