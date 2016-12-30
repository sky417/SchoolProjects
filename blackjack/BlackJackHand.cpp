// File:     Blackjack.cpp
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
#include "BlackjackHand.hpp"

void BlackJackHand::setDoubleDown(const bool& b)
// doubledown allows to double player's bet
{
	this->_doubledown = b;
	if (b) this->_bet = 2 * this->_bet;
}

void BlackJackHand::addCard(const Card& c)
{
	Hand::addCard(c);
	if (c.getNumber() == Jack || c.getNumber() == Queen || 
		c.getNumber() == King)
	// J, Q, K is counted as 10
	{
		this->_sum += 10;
	}

	else if (c.getNumber() == Ace) 
	// ace will be first evaluated as 11
	{
		this->_sum += 11;
		(this->_acecount)++;
	}

	else { this->_sum += c.getNumber() + 1; }

	if (this->_counts == 2 && this->_sum == 21)
	// if only 2 cards makes 21 == blackjack
		this->_blackjack = true;

	if (this->_sum > 21 && this->_acecount > 0)
	// if sum goes over 21 with ace(s)
	// ace(s) will be counted as 1
	{
		while (this->_sum > 21)
		{
			(this->_acecount)--;
			this->_sum -= 10;
		}
	}

	// sum is greater than 21 and no ace in hand = bust
	else if (this->_sum > 21 && this->_acecount == 0) this->_bust = true;
	
}

void BlackJackHand::clearCard()
{
	Hand::clearCard();

	// resetting all the parameters
	this->_sum = 0;
	this->_blackjack = false;
	this->_bust = false;
	this->_insurance = 0;
	this->_acecount = 0;
	this->_paid = false;
	this->_win = Lose;
	this->_bet = 0;
	this->_doubledown = false;
}


string BlackJackHand::toString()
{
	string s = Hand::toString();

	if (this->isBlackJack()) s = s + " (BlackJack!!)";

	else s = s + " ( " + (this->isSoft()? "Soft ":"Hard ") +
		Card::intToString(this->_sum) + (this->isBust()? " BUST!!":"") + " )";

	return s;
}