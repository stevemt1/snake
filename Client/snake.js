/* 
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/

var player1;
var player2;
var score1 = 0;
var score2 = 0;
var snake1 = [];
var snake2 = [];
var apple;
var playerNumber;
var gameEnd = false;
var current_time;
var timefromserver;
var latency;
var offset = 0;
var bucket1 = [];
var bucket2 = [];
var previoustime = 0;
var activebucket1 = true;
var isinitializing = true;

var initializing = function(){
  return isinitializing;
}

var setp1id = function(playername){
  player1 = playername;
}

var setp2id = function(playername){
  player2 = playername;
}

var getp1id = function(){
  return player1;
}

var getp2id = function(){
  return player2;
}

var setidentity = function(playerid){
  	playerNumber = playerid;
}

var setPlayerDead = function(){
	gameEnd = true;
}

var setScore1 = function(score){
	score1 = Number(score);
}

var setScore2 = function(score){
	score2 = Number(score);
}

var savetimestamp = function(time){
	timefromserver = Number(time);
}

var gettime = function(){
	var d = new Date();
	var n = d.getTime();
	current_time = n;
}

var sendtime = function(){
	//sends time A to the server
	gettime();
	send("Time: " + current_time.toString());
}

var syncTime = function(x, y, a){
	gettime();
	var B = current_time;
	var time_on_network = (B - Number(a)) - (Number(y) - Number(x));
	var truetime = Number(y) + (time_on_network/2);
	offset = truetime - B;
}

var calclatency = function(){
	gettime();
	latency = Math.abs(timefromserver - (current_time + offset));
}

var addtobucket = function(bucket, message){
	bucket.push(message);
}

var buckethandler = function(bucket){
	if(previoustime == 0){
		gettime();
		previoustime = current_time;
	}
	var interval = 500; //processes a bucket every 500 milliseconds
	
	gettime();
	if (current_time - previoustime >= interval){
		while (bucket.length != 0){
			//look at first item in queue
			//read it and react accordingly
			var payload = bucket.shift();
			var message = payload.split(":");
			
			
			    if (message[0] == 'Food'){ //draws the food on the canvas
          		setFoodLocation(message[1], message[2]);
          		savetimestamp(message[3]);
        	}
        	else if (message[0] == 'Snake1'){
          		updatesnake1(payload);
        	}
        	else if (message[0] == 'Snake2'){
          		updatesnake2(payload);
        	}
        	else if (message[0] == 'Scores'){
          		setScore1(message[1]);
          		setScore2(message[2]);
          		savetimestamp(message[3]);
        	}
        	else if (message[0] == "Directions"){
          		setdirection1(message[1]);
          		setdirection2(message[2]);
          		savetimestamp(message[3]);
        	}

			calclatency();
      
		}

      gettime();
      previoustime = current_time;
      activebucket1 = !activebucket1;
	}
}


var updatesnake1 = function(snakemessage){
  snake1 = [];
  var message = snakemessage.split(':');
  var messagelength = message.length;
  timefromserver = message[messagelength - 1];

  for (var i = 1; i < messagelength - 1; i++){
      var coorlist = message[i].split(',');
      var coordinate = {top: Number(coorlist[0]), left: Number(coorlist[1])};
      snake1.push(coordinate);
  }
}

var updatesnake2 = function(snakemessage){
  snake2 = [];
  var message = snakemessage.split(':');
  var messagelength = message.length;
  timesfromserver = message[messagelength - 1];

  for (var i = 1; i < messagelength - 1; i++){
      var coorlist = message[i].split(',');
      var coordinate = {top: Number(coorlist[0]), left: Number(coorlist[1])};
      snake2.push(coordinate);
  }
}

var setdirection1 = function (direction){
  var dir;
  if (direction == 12)
    dir = "up";
  else if(direction == 3)
    dir = "right";
  else if (direction == 6)
    dir = "down";
  else if (direction == 9)
    dir = "left";
  snake1[0].direction = dir;
}

var setdirection2 = function (direction){
  var dir;
  if (direction == 12)
    dir = "up";
  else if(direction == 3)
    dir = "right";
  else if (direction == 6)
    dir = "down";
  else if (direction == 9)
    dir = "left";
  snake2[0].direction = dir;
}

var setFoodLocation = function(foodLocationx, foodLocationy){
  apple = { top: foodLocationx, left: foodLocationy };
}

var draw = function(snakeToDraw, snake2toDraw, apple) {
  var drawableSnake = { color: "green", pixels: snakeToDraw };
  var drawableSnake2 = {color: "blue", pixels: snake2toDraw};
  var drawableApple = { color: "red", pixels: [apple] };
  var drawableObjects = [drawableSnake, drawableSnake2, drawableApple];
  CHUNK.draw(drawableObjects);
}

var showScore = function() {
    var canvas = document.getElementById("chunk-game");
    var context = canvas.getContext('2d');
    context.font = '20pt Calibri';
    context.fillStyle = 'green';
    context.fillText("P1: "+ score1, 20, 50);
    context.fillStyle = 'blue';
    context.fillText("P2: "+ score2, 720, 50);
}

var showLatency = function(){
	var canvas = document.getElementById("chunk-game");
	var context = canvas.getContext('2d');
	context.font = '20pt Calibri';
	context.fillStyle = 'white';
	context.fillText("Ping: " + latency, 340, 50);
}


var advanceGame = function() {
  if (gameEnd){
  	draw(snake1, snake2, apple);
  	showScore();
  	showLatency();
  	CHUNK.flashMessage("Game Over!");
  	CHUNK.endGame();
  }
  else if (gameEnd == false){
  draw(snake1, snake2, apple);
  showScore();
  showLatency();
  }
}

var changeDirection = function(direction) { 
  var clockdirection;
  if (direction == "up")
    clockdirection = 12;
  else if (direction == "down")
    clockdirection = 6;
  else if (direction == "right")
    clockdirection = 3;
  else if (direction == "left")
    clockdirection = 9;


  send("Direction: " + clockdirection.toString());
}

var init = function()
{
  isinitializing = false;
  CHUNK.executeNTimesPerSecond(advanceGame, 5);

  if (!gameEnd)
  {
  	CHUNK.onArrowKey(changeDirection);
  }
}

