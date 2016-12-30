// File:     CardDeck.hpp
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

#ifndef _CARDDECK_HPP_
#define _CARDDECK_HPP_

#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include "Card.hpp"
using namespace std;

class CardDeck {

public:
	CardDeck() { 
		this->deckID = "cd" + Card::intToString(deck_count++); 
		this->reset(); 
	};

	// draw a random card from the deck
	Card getRandomCard();

	// accessor
	int getCount() const;
	
	// mutator
	void reset();

	string toString();

private:
	bool cards[4][13]; 
	// double array of 4 suits and 13 numbers
	// if any card is used, that card returns false
	int cardCount;
	string deckID;

	static const int CARD_COUNT = 52;
	static int deck_count;

};

#endif