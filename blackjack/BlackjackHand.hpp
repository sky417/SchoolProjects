// File:     BlackjackHand.hpp
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

#ifndef _BLACKJACKHAND_HPP_
#define _BLACKJACKHAND_HPP_

#include "Hand.hpp"

// Evaluation enumeration
enum Cond {Lose, Push, Win};

class BlackJackHand : public Hand {

public:
	// Constructor
	BlackJackHand() : Hand(), _sum(0), _blackjack(false), _bust(false), _win(Lose),
		_insurance(0), _acecount(0), _bet(0), _paid(false), _doubledown(false) {};
	
	// Accessor
	bool isBlackJack() const { return this->_blackjack; };
	bool isBust() const { return this->_bust; };
	bool isSoft() const { return ((this->_acecount>0)? true:false); };
	// if any of ace is counted as 11 
	int getBet() const { return this->_bet; };
	int getInsurance() const {return this->_insurance; };
	Cond isWin() const { return this->_win; };
	int getSum() const { return this->_sum; };
	bool isDoubleDown() const { return this->_doubledown; };
	bool isPaid() const { return this->_paid; };
	
	// Mutator
	void setBet(const int& i) { this->_bet = i; };
	void setInsurance(const int& i) {this->_insurance = i; };
	void setWin(const Cond& c) { this->_win = c; };
	void setDoubleDown(const bool& b);
	void setPaid(const bool& b) { this->_paid = b; };

	virtual void addCard(const Card& c);
	virtual void clearCard();

	virtual string toString();

private:
	int _sum; // sum of cards
	bool _blackjack; 
	bool _bust; 
	int _acecount; // counts # of aces
	int _bet;
	int _insurance; 
	Cond _win;
	bool _doubledown; // doubledown allows only one more card to be drawn
	bool _paid; // true if paid

};

#endif