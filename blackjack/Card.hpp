// File:     Card.hpp
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

#ifndef _CARD_HPP_
#define _CARD_HPP_

#include <iostream>
#include <string>
#include <sstream>
using namespace std;


enum Suits {Spade, Diamond, Heart, Club};
enum CardNumber {Ace, Two, Three, Four, Five, Six, Seven,
	Eight, Nine, Ten, Jack, Queen, King};


class Card {

public:
	// Constructor
	Card();
	Card(const Suits& s, const CardNumber& n) : _suit(s), _number(n) {};
	
	// Mutator
	void setSuit(const Suits& s) { this->_suit = s; };
	void setNumber(const CardNumber& n) { this->_number = n; };

	// Accessor
	Suits getSuit() const { return this->_suit; };
	CardNumber getNumber() const { return this->_number; };
	
	string toString();
	static string intToString(const int& i);

private:
	
	Suits _suit;
	CardNumber _number;

};

#endif