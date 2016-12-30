// File:     Blackjack.hpp
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

#ifndef _BLACKJACK_HPP_
#define _BLACKJACK_HPP_

#include "CardDeck.hpp"
#include "BlackJackHand.hpp"
#include "Game.hpp"
#include <Windows.h> // to use Sleep()

typedef vector<BlackJackHand*> vpBHand;
typedef vector<CardDeck*> vpDeck;

class BlackJack : public Game {

public:
	// Constructor
	BlackJack();
	~BlackJack();
	
	virtual void runGame(Player* p);
	virtual string getGameName() { return this->_gameName; };
	
	// Accessor
	int getBankRoll() const { return this->_bankroll; };
	int getDeckCount() const { return this->_deck->size(); };
	int getHandsCount() const { return this->_playerHands->size(); };
	Hand* getDealerHand() const { return this->_dealerHand; };
	Hand* getPlayerHand(const int& i) const { return this->_playerHands->at(i); };
	vpBHand* getPlayerHands() const { return this->_playerHands; };
	
	// Mutator
	void setDeckCount(const int& i);
	void setHandsCount(const int& i);

	// Other Function
	void deal(BlackJackHand*& h); // deals one card
	void deal(BlackJackHand*& h, const int& i); // deals multiple cards
	void evaluate(BlackJackHand*& h); // determines win/lose/push
	void reset(); // clears all hands
	void resetDeck(); // reclaims all cards of the decks
	void resetGame(); // resets the BlackJack game object
	

private:

	string _gameName;
	BlackJackHand* _dealerHand;
	vpBHand* _playerHands;
	int _bankroll;
	vpDeck* _deck;
	int _cardleft;
	
	void cleanup();
};

#endif