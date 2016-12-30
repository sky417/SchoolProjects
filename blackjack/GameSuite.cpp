// File:     GameSuite.cpp
//
// Author:   Andrew Kisoo Yoon     *** Your name ALWAYS goes here
//           10653038          *** Your ID number ALWAYS goes here
//           i5l4             *** Your userID ALWAYS goes here
//           L1b               *** Your lab section ALWAYS goes here
//
//			 Ryan Youngik Song
//			 20909032
//			 e8d5
//			 L1b
//
// Modified: 2010 Nov 29
//
#include "GameSuite.hpp"

GameSuite::GameSuite()
// Constructs a GameSuite object and initializes the game vector
// @input: none
// @output: GameSuite object
{
	GameSuite::init();
}

GameSuite::~GameSuite()
// clean up all objects contained in games* vector in gamesuite 
{

	for (unsigned int i=0; i < this->games.size(); i++)
	{
		delete this->games.at(i)->game;
		this->games.at(i)->game = NULL;
	}
}

void GameSuite::runGame( int gameNum, Player* p ) 
// Runs the game stored at index gameNum-1
// @input: gameNum and current player
// @output: none
{
	int gameIndex = gameNum-1; // to account for indexing at zero
	GameSuite::GameItem* gi = games[gameIndex];
	
	cout << "Welcome to " << gi->gameName << "!\n";
	
	gi->game->runGame(p);
		
}
	
void GameSuite::printGameListing() 
// Prints out a listing of all games in the GS vector
// @input: none
// @output: none
{
	int gameCount = GameSuite::games.size();

	cout << "\n************************" << endl;
	cout << "Games in the Game Suite:" << endl;
	cout << "************************\n" << endl;

	for( int i=0; i<gameCount; i++ ) {		
		cout << (i+1) << ". " << games[i]->gameName << endl;			
	}	
	
	cout << "Enter 'q' to quit." << endl;
	
}




