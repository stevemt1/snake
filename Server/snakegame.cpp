/*
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/

#include "snakegame.h"
#include <iostream>
#include <sstream>
#include <vector>


SnakeGame::SnakeGame()
{
    snake1 = {{2, 1, 6}, {1, 1, 6}};
    snake2 = {{2, 18, 6}, {1, 18, 6}};
    food = {{0, 0}};
    p1_score = 0;
    p2_score = 0;
    int canvasWidth = 800;
    int canvasHeight = 600;
    int pixelSize = 40;
    int gameHeight = canvasHeight / pixelSize;
    int gameWidth = canvasWidth / pixelSize;
    gameEnd = false;
    gameBoundaries();

}

string SnakeGame::getp1id()
{
    return p1_id;
}

string SnakeGame::getp2id()
{
    return p2_id;
}

void SnakeGame::setp1id(string id)
{
    p1_id = id;
}

void SnakeGame::setp2id(string id)
{
    p2_id = id;
}

string SnakeGame::getp1score()
{
    ostringstream os;
    os << p1_score;
    return os.str();
}

string SnakeGame::getp2score()
{
    ostringstream os;
    os << p2_score;
    return os.str();
}

void SnakeGame::setp1score(string score)
{
    p1_score = stoi(score);
}

void SnakeGame::setp2score(string score)
{
    p2_score = stoi(score);
}

bool SnakeGame::checkgameEnd()
{
    return gameEnd;
}

string SnakeGame::getp1direction()
{
	ostringstream os;
	os << snake1[0][2];
	return os.str();
}

string SnakeGame::getp2direction()
{
	ostringstream os;
	os << snake2[0][2];
	return os.str();
}

void SnakeGame::setp1direction(string direction)
{
    snake1[0][2] = stoi(direction);
}

void SnakeGame::setp2direction(string direction)
{
    snake2[0][2] = stoi(direction);
}

string SnakeGame::getDirectionMessage()
{   
	return "Directions: " + getp1direction() + ":" + getp2direction();
}

void SnakeGame::gameBoundaries() {
    if (boundaries.size() > 0) 
        { return; }
        
    for (int top = -1; top <= gameHeight; top++) {
        boundaries.push_back({top, -1});
        boundaries.push_back({top, gameWidth});
    }
    for (int left = -1; left <= gameWidth; left++) {
        boundaries.push_back({-1, left});
        boundaries.push_back({gameHeight, left});
        }
}

void SnakeGame::createFood()
{
    food = { {(rand() % (gameHeight)), (rand() % (gameWidth))} };
}

string SnakeGame::getFoodMessage()
{
	ostringstream os;
	os << "Food: " << food[0][0] << ":"<< food[0][1];
	return os.str();
}

void SnakeGame::createSnakes()
{
    snake1 = {{2, 1, 6}, {1, 1, 6}};
    snake2 = {{2, 18, 6}, {1, 18, 6}};
}

vector<int> SnakeGame::moveSegment(vector<int> segment) { 
        
    vector<int> newSegment = segment;
    if (segment[2] == 12)
        newSegment[0] -= 1;
    else if(segment[2] == 3)
        newSegment[1] += 1;
    else if(segment[2] == 6)
        newSegment[0] += 1;
    else if(segment[2] == 9)
        newSegment[1] -= 1;

    return newSegment;
}
    
vector<int> SnakeGame::segmentFurtherForwardThan(int index, vector<vector<int>> snake) {
    if (index == 0) 
       {return snake[index];}
    else 
       {return snake[index - 1];}
}
    

bool SnakeGame::detectCollisionBetween(vector<int> pixelA, vector<int> pixelB) {
        return (pixelA[0] == pixelB[0]) && (pixelA[1] == pixelB[1]);
    }
    
vector<vector<int>> SnakeGame::moveSnake(vector<vector<int>> snake) {
    vector<vector<int>> newSnake;
    vector<int> firstSegment = moveSegment(snake[0]);
    firstSegment[2] = segmentFurtherForwardThan(0, snake)[2];
    newSnake.push_back(firstSegment);

    for(unsigned int i = 1; i < snake.size(); i++)
    {
        vector<int> newSegment = moveSegment(snake[i]);
        newSegment[2] = segmentFurtherForwardThan(i, snake)[2];
        newSnake.push_back(newSegment);
    }
    return newSnake;
}
    
bool SnakeGame::ate(vector<vector<int> > snake, vector<vector<int> > otherThing) {
    vector<int> head = snake[0];
    for(vector<int> segment: otherThing){
        if (detectCollisionBetween(head, segment)){
           return true;
        }
    }
    return false;
}
    
void SnakeGame::advanceGame() {
    vector<vector<int>> newSnake = moveSnake(snake1);
    vector<vector<int>> newSnake2 = moveSnake(snake2);
    
    if (ate(newSnake, snake1) || ate(newSnake2, snake2)) {
        gameEnd = true;
    }
    
    if (ate(newSnake, snake2) || ate(newSnake2, snake1)) {
        gameEnd = true;

    }
    
    if (ate(newSnake, food)) {
        p1_score++;
        newSnake = {{food[0][0], food[0][1], snake1[0][2]}};
        for(int i = 0; i < snake1.size(); i++)
        {
            newSnake.push_back(snake1[i]);
        }

        createFood();

    }
    
    if (ate(newSnake2, food)) {
        p2_score++;
        newSnake2 = {{food[0][0], food[0][1], snake2[0][2]}};
        for (int i = 0; i < snake2.size(); i++)
        {
            newSnake2.push_back(snake2[i]);
        }
        createFood();

    }
    
    if (ate(newSnake, boundaries) || ate(newSnake2, boundaries)) {
        gameEnd = true;

    }
    if (gameEnd == false) {
        snake1 = newSnake;
        snake2 = newSnake2;
    }
}

pair<string, string> SnakeGame::getSnakeMessages()
{
	ostringstream os1;
    ostringstream os2;

    os1 << "Snake1:";
    os2 << "Snake2:";

    for (unsigned int i = 0; i < snake1.size(); i++)
    {
		os1 << snake1[i][0] << "," << snake1[i][1];
		if (i < snake1.size()-1)
			os1 << ":";
	}

    for (unsigned int i = 0; i < snake2.size(); i++)
    {
        os2 << snake2[i][0] << "," << snake2[i][1];
        if (i < snake2.size()-1)
            os2 << ":";
    }
	pair<string, string> snakes = make_pair(os1.str(), os2.str());
	return snakes;
    
}