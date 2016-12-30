// File:     GameSuite.hpp
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

#ifndef _GAMESUITE_HPP_
#define _GAMESUITE_HPP_

#include <iostream>
#include <vector>

#include "Game.hpp"
#include "WordJumble.hpp"
#include "DummyGame.hpp"
#include "BlackJack.hpp"


class GameSuite {
	
public:
	GameSuite();
	~GameSuite();
	
	void runGame( int gameNum, Player* p );
	// Runs the game stored at index gameNum-1
	// @input: gameNum and current player
	// @output: none
	
	void printGameListing();
	// Prints out a listing of all games in the GS vector
	// @input: none
	// @output: none

private:

	void init() {
		GameItem *gi;

		// Add WordJumble below:
		WordJumble* wjg = new WordJumble();
		gi = new GameItem();
		gi->gameName = wjg->getGameName();
		gi->game = wjg;
		games.push_back(gi);
		
		// Add DummyGame 
		DummyGame* dg = new DummyGame();
		gi = new GameItem();
		gi->gameName = dg->getGameName();
		gi->game = dg;
		games.push_back(gi);

		// Add BlackJack
		BlackJack* blj = new BlackJack();
		gi = new GameItem();
		gi->gameName = blj->getGameName();
		gi->game = blj;
		games.push_back(gi);

	}
		
	struct GameItem {
		string gameName;
		Game* game;
	};
	
	vector<GameItem*> games;

};

#endif