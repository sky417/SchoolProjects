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


#include "BlackJack.hpp"

BlackJack::BlackJack() : Game()
// Default Constructor
{
	this->_dealerHand = new BlackJackHand();
	this->_playerHands = new vpBHand();
	this->_deck = new vpDeck();
	this->_gameName = "BlackJack -------- Project!!";
}

BlackJack::~BlackJack()
// Destructor
{
	this->cleanup();
}

void BlackJack::runGame(Player* p)
// Let's play BlackJack!!!
{
	int decks, hands, bankroll;
	int bet, tempbankroll;
	int insurance;
	string action;
	string evenMoney;

	cout << endl << "How much money do you want to play? (please, know your limit) ";
	cin >> bankroll;
	this->_bankroll = bankroll;

	cout << "How many decks? ";
	cin >> decks;
	this->_cardleft = decks * 52;

	cout << "How many hands? ";
	cin >> hands;

	this->setDeckCount(decks);
	this->setHandsCount(hands);

	cout << endl;
		
	while(this->_bankroll > 0)
	// runs as long as you have some money left
	{
		tempbankroll = this->_bankroll; // for displaying bankroll

		for (int i=0; i < this->getHandsCount(); i++)
		{
			bet = 0;

			while (bet == 0)
			{			
				cout << "Enter the bet amount for hand " << i+1;
				cout << " (bankroll: $" << tempbankroll << ", -1 to quit) : ";
				cin >> bet;

				if (bet > tempbankroll || bet < -1) 
				// check if the bet is valid
				{
					cout << "Invalid amount. Please enter a valid amount.\n";
					bet = 0;
				}
			}
			
			if (bet == -1) break; // end the game

			// Set bet amount
			this->_playerHands->at(i)->setBet(bet);
			tempbankroll -= bet;
		}

		if (bet == -1) // end the game
		{
			cout << endl;
			break;
		}

		cout << endl;

		// Deal a dealer's hand
		this->deal(_dealerHand);
		
		for (int i=0; i < this->getHandsCount(); i++)
		// Deal player's hand(s)
		{
			this->deal(this->_playerHands->at(i),2);
			cout << "Hand " << i+1 << " : " << 
				this->_playerHands->at(i)->toString() << endl;
		}	
	
		cout << "\nDealer : " << this->_dealerHand->toString() << endl << endl << endl;
		
		for (int i=0; i < this->getHandsCount(); i++)
		// actions for each hand(s)
		{
			cout << endl;
			cout << "Dealer : " << this->_dealerHand->toString() << endl;
			cout << "-------------------------\n";
			cout << "Hand " << i+1 << " : " << 
				this->_playerHands->at(i)->toString() << endl << endl;

			if (this->_playerHands->at(i)->isBlackJack())
			// if player has a blackjack
			{
				if (this->_dealerHand->isSoft())
				// if dealer has an ace
				{
					cout << "Even money? (Y/N)";
					cin >> evenMoney;

					if (evenMoney == "Y" || evenMoney == "y")
					// get 1 to 1 pay
					{
						this->_bankroll += this->_playerHands->at(i)->getBet();
						this->_playerHands->at(i)->setPaid(true);
					}
				}

				else
				{
					this->_playerHands->at(i)->setWin(Win);
				}
				continue;
			}

			if (this->_dealerHand->isSoft())
			// if dealer has an ace
			{
				insurance = 0;

				while (insurance == 0)
				// insurance can be bought up to bet/2
				{
					cout << "How much insurance for hand " << i+1 << 
					"? (enter 0 if you don't want insurance) ";
					cin >> insurance;
					if (insurance > (int)(this->_playerHands->at(i)->getBet() / 2) ||
						insurance < 0)
					// check if it is a valid amount
					{
						cout << "Invalid insurance amount. Max insurance : $" <<
							(int)(this->_playerHands->at(i)->getBet() / 2) << endl;
						insurance = 0;
					}

					else this->_playerHands->at(i)->setInsurance(insurance);
				}
			}

			cout << "What would you like to do for hand " << i+1 << "?\n";
			cout << "(S)tand, (H)it, (D)ouble down (bet : $" << 
				this->_playerHands->at(i)->getBet() << ", bankroll : $" <<
				tempbankroll << ")\n";
			cin >> action;

			if ((action == "D" || action == "d") 
				&& this->_playerHands->at(i)->getBet() > tempbankroll)
			// if the bankroll is not enough for the double down
			// change to regular hit
			{
				cout << "Not enough money for double down. Hitting instead..\n";
				action = "h";
			}

			else if (action == "D" || action == "d")
			{
				tempbankroll -= this->_playerHands->at(i)->getBet();
				this->_playerHands->at(i)->setDoubleDown(true);
			}

			if (action == "S" || action == "s") continue; // stand - next hand

			while (action != "S" && action != "s")
			// hit
			{
				Sleep(1000);
				deal(this->_playerHands->at(i));
				
				cout << "Hand " << i+1 << " : " << this->_playerHands->at(i)->toString() << endl;
				
				if (this->_playerHands->at(i)->isBust() || 
					this->_playerHands->at(i)->isDoubleDown())
				// if player doubled down, only 1 card is dealt
				// if bust, then skip to the next hand
					break;

				else
				{
					cout << "What would you like to do next?\n";
					cout << "(S)tand, (H)it\n";
					cin >> action;
				}
			}
		}

		cout << endl;

		// Dealer completes his hand
		// Dealer draws a card on soft 17, or less than 17
		while ( this->_dealerHand->getSum() < 17 || 
			(this->_dealerHand->isSoft() && 
			this->_dealerHand->getSum() == 17))
		{
			Sleep(1500); // to give a bit of tension.....
			this->deal(_dealerHand);
			cout << "Dealer : " << this->_dealerHand->toString() << endl;
		}

		cout << endl;

		// When all hands are dealt evaluate 
		for (int i=0; i < this->getHandsCount(); i++)
		{
			// check if the hand is already paid
			if (this->_playerHands->at(i)->isPaid()) continue;

			this->evaluate(this->_playerHands->at(i));

			switch (this->_playerHands->at(i)->isWin())
			{
			case Win: // winning hand!
				cout << "Hand " << i+1 << " won!! ";
				cout << "You won $";
				if (this->_playerHands->at(i)->isBlackJack())
				// Blackjack pays 1.5 times player's bet
				{
					cout << (int)(this->_playerHands->at(i)->getBet() * 1.5) << endl;
					this->_bankroll += (int)(this->_playerHands->at(i)->getBet() * 1.5);
				}

				else 
				{
					cout << this->_playerHands->at(i)->getBet() << endl;
					this->_bankroll += this->_playerHands->at(i)->getBet();					
				}
				break;

			case Push: // tie, nothing changes
				cout << "Hand " << i+1 << " push.\n";
				break;

			case Lose: // losing hand
				cout << "Hand " << i+1 << " lost.";
				
				if (this->_playerHands->at(i)->getInsurance() > 0 && 
					this->_dealerHand->isBlackJack())
				// if you bought the insurance, and dealer gets a blackjack,
				// you get 2 times of insurance bet
				{
					cout << "Luckily, you got $" << this->_playerHands->at(i)->getInsurance() * 2
						<< " back for the insurance you bought.\n";
					this->_bankroll += 3 * this->_playerHands->at(i)->getInsurance();
				}

				cout << " You lost $" << this->_playerHands->at(i)->getBet() << endl;
				this->_bankroll -= this->_playerHands->at(i)->getBet();
				this->_bankroll -= this->_playerHands->at(i)->getInsurance();
				
				break;
			}
		}

		cout << endl;
		
		this->reset(); // reset all the hands
		cout << "==============================================================\n";
		cout << "==============================================================\n\n";
		bet = insurance = 0; // reset all the variables
		action = evenMoney = ""; 


		if ((decks*52*0.2) > this->_cardleft) 
		// if there are below 20% cards left in the deck,
		// reset the deck
		{
			cout << "\nTime to shuffle!!" << endl;
			resetDeck();
		}
	}

	// exit messages
	cout << "Your initial cash : $" << bankroll << endl;
	cout << "You ended up with : $" << this->_bankroll << endl << endl;
	
	if (bankroll > this->_bankroll) 
		cout << "You lost $" << bankroll - this->_bankroll << ". What a loser." << endl;

	else if (bankroll == this->_bankroll) 
		cout << "You just broke even... Had some fun? :)" << endl;

	else cout << "You won $" << this->_bankroll - bankroll << ". Winner!!!!!" << endl;

	this->resetGame(); // re-initialize pointers
}

void BlackJack::setDeckCount(const int& i)
{
	for (int j=0; j < i; j++)
	{
		this->_deck->push_back(new CardDeck());
	}
}

void BlackJack::setHandsCount(const int& i)
{
	for (int j=0; j < i; j++)
	{
		this->_playerHands->push_back(new BlackJackHand());
	}
}

void BlackJack::deal(BlackJackHand*& h)
{
	// deals a card from a randomly chosen deck
	h->addCard(this->_deck->at(rand() % this->_deck->size())->getRandomCard());

	(this->_cardleft)--;
}

void BlackJack::deal(BlackJackHand*& h, const int& i)
{
	for (int j=0; j < i; j++)
		this->deal(h);
	this->_cardleft -= i;
}

void BlackJack::evaluate(BlackJackHand*& h)
{
	if (h->isBust()) h->setWin(Lose);
	// you bust = lose

	else if (this->_dealerHand->isBust()) h->setWin(Win);
	// dealer bust = you win

	else if (h->getSum() > this->_dealerHand->getSum())
		h->setWin(Win);
	// bigger hand wins

	else if (h->getSum() == this->_dealerHand->getSum())
	// if the sum is same
	{
		if (h->isBlackJack() && !this->_dealerHand->isBlackJack())
		// player has blackjack, dealer doesn't -> win
			h->setWin(Win);
		
		else if (!h->isBlackJack() && this->_dealerHand->isBlackJack())
		// player doesn't have blackjack, dealer does -> lose
			h->setWin(Lose);

		else h->setWin(Push);
	}

	else h->setWin(Lose);
}

void BlackJack::reset()
// Clears all cards of the hand(s) 
{
	this->_dealerHand->clearCard();
	
	for (unsigned int i=0; i < this->_playerHands->size(); i++)
	{
		this->_playerHands->at(i)->clearCard();
	}
} 

void BlackJack::resetDeck()
// reclaims all the cards of the deck
{
	for (unsigned int j=0; j < this->_deck->size(); j++)
	{
		this->_deck->at(j)->reset();
	}
	this->_cardleft = 52 * this->_deck->size();
}

void BlackJack::resetGame()
// resets players hands, dealers hands, and deck size
{
	this->reset();

	delete this->_dealerHand;
	for (unsigned int i=0; i < this->_playerHands->size(); i++)
	{
		delete this->_playerHands->at(i);
		this->_playerHands->at(i) = NULL;
	}

	delete this->_playerHands;

	for (unsigned int j=0; j < this->_deck->size(); j++)
	{
		delete this->_deck->at(j);
		this->_deck->at(j) = NULL;
	}

	delete this->_deck;

	this->_dealerHand = new BlackJackHand();
	this->_playerHands = new vpBHand();
	this->_deck = new vpDeck();

}


void BlackJack::cleanup()
// deletes all the pointers
{
	this->reset();
	delete this->_dealerHand;
	this->_dealerHand = NULL;

	for (unsigned int i=0; i < this->_playerHands->size(); i++)
	{
		delete this->_playerHands->at(i);
		this->_playerHands->at(i) = NULL;
	}
	
	this->_playerHands = NULL;
	for (unsigned int j=0; j < this->_deck->size(); j++)
	{
		delete this->_deck->at(j);
		this->_deck->at(j) = NULL;
	}

	delete _deck;
	_deck = NULL;
}