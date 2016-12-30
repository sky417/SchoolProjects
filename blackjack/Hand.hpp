// File:     Hand.hpp
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

#ifndef _HAND_HPP_
#define _HAND_HPP_

#include <vector>
#include "Card.hpp"

typedef vector<Card> vCard;

class Hand {

public:

	Hand() { _cards = new vCard(); _counts = 0; };
	virtual ~Hand() { cleanup(); };

	int cardCount() const { return this->_counts; };

	// add the card c to the deck
	virtual void addCard(const Card& c) { 
		this->_cards->push_back(c);
		(this->_counts)++; };

	// clear all the cards in the hand
	virtual void clearCard() { 
		delete this->_cards;
		this->_cards = new vCard();
		this->_counts = 0; };

	virtual string toString() {
		string s = "";
		for (int i=0; i < _counts; i++) {
			
			s += _cards->at(i).toString() + 
				((i != _counts-1)? ", ":"");
		}
		
		return s;
	};

protected:

	vCard* _cards;
	int _counts;

private:

	void cleanup() { delete _cards; _cards = NULL; };
};


#endif
