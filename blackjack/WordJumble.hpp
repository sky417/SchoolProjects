// File:     WordJumble.hpp
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
// Modified: 2010 Oct 11
//			 2010 Sep 27   *** Add a new date if you modify the file!
//
// Defines the WordJumble Class
//

#ifndef _WordJumble_HPP_
#define _WordJumble_HPP_

#include <iostream>
#include <vector>
#include <fstream>

#include "Game.hpp"

typedef vector<string> vString; // used to store a list of words

class WordJumble: public Game {
	
public:
	
	WordJumble();
	// Creates a DummyGame object and sets its name string
	// @intput: none
	// @output: A DummyGame object
	
	void runGame( Player* p );
	// Runs the DummyGame game for player p
	// @input: The current Player object
	// @output: none
	
	string getGameName();
	// Returns the name of the game as a string
	// @input: none
	// @output: The name of the game as a string

	
private:
	string gameName_;
	vString getWords();
	char* wordShuffle(string word); 
	char* charArrayShuffle(char* letters, int size);
	void displayChars(char* letters, int size);
	
	
};

#endif
