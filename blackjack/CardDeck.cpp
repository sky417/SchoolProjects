// File:     CardDeck.cpp
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

#include "CardDeck.hpp"

int CardDeck::deck_count = 0; // to initialize only once

Card CardDeck::getRandomCard() 
{
	srand((unsigned int)time(NULL));

	// draw a card
	int suit = rand() % 4;
	int num = rand() % 13;

	while (!this->cards[suit][num])
	// check if the card is available
	// if not, try drawing again
	{
		suit = rand() % 4;
		num = rand() % 13;
	}
	// set picked card to false(used)
	this->cards[suit][num] = false;
	(this->cardCount)--;

	return Card(Suits(suit), CardNumber(num));
}

int CardDeck::getCount() const
{
	return this->cardCount;
}

void CardDeck::reset()
// reclaim all the cards of a deck
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 13; j++)
		{
			// turn all card indicators to true
			this->cards[i][j] = true;
		}
	}

	this->cardCount = CARD_COUNT;
	//cout << "Resetting deck " << this->deckID << "..." << endl;
}

string CardDeck::toString()
{

	return "There are " + Card::intToString(this->cardCount) + " cards left in the deck ID " +
		this->deckID;
}
