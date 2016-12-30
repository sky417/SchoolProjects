// File:     Card.cpp
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

#include "Card.hpp"

string Card::toString()
{
	string s = "";

	switch (this->_number) {
	case Ace:
		s += "A";
		break;
	case Two:
		s += "2";
		break;
	case Three:
		s += "3";
		break;
	case Four:
		s += "4";
		break;
	case Five:
		s += "5";
		break;
	case Six:
		s += "6";
		break;
	case Seven:
		s += "7";
		break;
	case Eight:
		s += "8";
		break;
	case Nine:
		s += "9";
		break;
	case Ten:
		s += "10";
		break;
	case Jack:
		s += "J";
		break;
	case Queen:
		s += "Q";
		break;
	case King:
		s += "K";
		break;
	}

	//s += " of ";

	switch (this->_suit) {
	case Spade:
		//s += "Spade"; 
		s += "S";
		break;
	case Diamond:
		//s += "Diamond"; 
		s += "D";
		break;
	case Heart:
		//s += "Heart"; 
		s += "H";
		break;
	case Club:
		//s += "Club"; 
		s += "C";
		break;

	}

	return s;
}

string Card::intToString(const int& i)
// return string of integer
{
	stringstream ss;
	ss << i;
	return ss.str();
}