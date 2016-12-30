// File:     Game.hpp
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

#ifndef _GAME_HPP_
#define	_GAME_HPP_

#include <iostream>

#include "Player.hpp"

// abstract class Game
class Game {
	
protected: 
	string pName;
	
public:
	void setName( Player* p ) 
	// Sets the player name variable to the name stored within p
	// @input: The current player object
	// @output: none
	{
		pName = std::string(p->getPlayerName());
	};
	
	virtual string getGameName() =0;
	// Pure virtual function.  Should be overridden by deriving classes
	// to return the name of the game
	// @input: none
	// @output: The name of the game as a string

	virtual void runGame( Player* p ) =0;
	// Pure virtual function.  Should be overridden by deriving classes
	// to run the game as defined by the subclass
	// @input: The current Player object
	// @output: none
	

	
};

#endif