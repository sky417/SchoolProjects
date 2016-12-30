// File:     GameDriver.cpp
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
#include <iostream>

#include "Player.hpp"
#include "Game.hpp"
#include "GameSuite.hpp"

// memory leak check
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


using namespace std;

string getPlayerName() 
// Queries user for name
// @output: player name as string
{
	
	string player;
	
	cout << "\nWelcome, player!\n\n";
	cout << "Please enter your name: ";

	cin >> player;
	return player;
}


void displayMenu( GameSuite* gs, Player* p ) 
// Prints menu for player to select from
// @input: current game suite, current player
// @output: none
{
	string pInput = "_";

	while( pInput != "q" ) {

		gs->printGameListing();		
		string name = p->getPlayerName();
		cout << "\n\n" << name << ", please enter your selection.\n";
		cin >> pInput;
		if( pInput != "q") gs->runGame(atoi(pInput.c_str()), p); 
		// Converts to a C-style string
		// then applies atoi to convert 
		// to integer
		}
		
	cout << "Thanks for playing! Now leaving the game suite...\n";
}



int main( void ) {

	// memory leak check
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// "Load" games:
	GameSuite* gs = new GameSuite();
	string name = getPlayerName();
	Player* p = new Player(name);
	cout << p->getPlayerName() << endl;
	displayMenu( gs, p ); 

	delete gs;
	delete p;
	gs = NULL;
	p = NULL;

	// memory leak check
	//_CrtDumpMemoryLeaks();

	return 0;
}