// File:     Player.hpp
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

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <iostream>
#include <string>

using namespace std;

class Player {

public:
	//Player() {}; // default constructor
	
	Player(const string& name = "unknown") { this->_name = name; };

	string getPlayerName() { return this->_name; };
	
private:
	string _name;
};

#endif