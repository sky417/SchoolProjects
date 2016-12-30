// File:     WordJumble.cpp
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
// Modified: 2010 Oct 11
//			 2010 Sep 27   *** Add a new date if you modify the file!
//
#include "WordJumble.hpp"
#include "time.h"

WordJumble::WordJumble() 
// Creates a DummyGame object and sets its name string
// @intput: none
// @output: A DummyGame object
{
	WordJumble::gameName_ = "WordJumble";
}

void WordJumble::runGame( Player* p ) 
// Runs the DummyGame game for player p
// @input: The current Player object
// @output: none
{
	
	srand((unsigned int)time(NULL));

	vString wordlist = getWords();

	if (wordlist[0] == "does not exist") 
	// if words.txt does not exist, exits the game
	{
		cout << endl << "!! words.txt does not exist." << endl;
		return;
	}

	int pick; // used as an index to pick a word from the list
	string pickedWord;
	int wordsize;
	char* c; // points a shuffled word
	string strAnswer; // user typed in answer
	int score; // score of current game
	int totalScore = 0; // total score user has collected
	
	cout << endl;

	while(true)
	{
		pick = rand() % wordlist.size();
		pickedWord = wordlist[pick];
		wordsize = pickedWord.size();

		c = wordShuffle(pickedWord);
	
		//cout << endl;			
		//cout << pickedWord << endl;   // un-comment this line to see the original word

		displayChars(c, wordsize);		
	
		//get the answer, and check if correct
	
		for (score = 3; score > 0 ; score--)		// allows 3 tries
		{
			cout << "Enter your answer (-1 to exit WordJumble): "; 
			cin >> strAnswer;
			
			if (strAnswer == "-1")   // -1 to exit
			{
				cout << endl << p->getPlayerName() << ", your total score is: " << totalScore << endl;
				cout << "Thank you for playing WordJumble!" << endl;
				delete[] c; // clear memory on exit
				return;
			}

			else if (strAnswer == pickedWord) // if answer input is correct,
			{
				totalScore += score;
				break;
			}

			else
			{
				cout << "Your answer is WRONG!!! Tries remaining : " << score-1 << endl << endl;
			}
		}
		cout << endl << p->getPlayerName() << ", you have achieved " << score << " points this game." << endl;
		cout << "Your total score is : " << totalScore << endl << endl;
	}
}

string WordJumble::getGameName() 
// Returns the name of the game as a string
// @input: none
// @output: The name of the game as a string
{
	return WordJumble::gameName_;
}

vString WordJumble::getWords()
// Reads a file which contains a list of words and stores the words into a vString
// @input: none
// @output: list of all words as a vString
{
	vString str;
	string s;

	fstream myFile("words.txt", ios::in);
	
	if (myFile.is_open())
	{
		while (!myFile.eof())
		{
			getline(myFile, s); // read each line of the file and store it into a string s
			if (s != "") 
				str.push_back(s);
		}

		myFile.close();
	}

	else str.push_back("does not exist"); // if words.txt does not exist

	return str;

}

char* WordJumble::wordShuffle(string word)
// shuffles the characters of a given string
// @input: string
// @output: pointer to the shuffled word
{
	char* letters = new char[word.size()];
	
	memcpy(letters, word.c_str(), word.size()); // converts string word into an array letters
												// we used a pointer as an array
	
	return charArrayShuffle(letters, word.size()); 
}

char* WordJumble::charArrayShuffle(char* letters, int size)
// shuffles the contents of an array
// @input: array of characters, size of the array
// @output: array of characters shuffled
{
	int charleft = size; // char left in the original array
	int counter = 0;
	int randnum;

	char* newString = new char[size];

	while (charleft > 0)
	{
		randnum = rand() % charleft; // randomly pick a letter from the word

		newString[counter] = letters[randnum]; // put the letter into the new string
		charleft--;
		counter++;
		
		// remove the picked character from the original array
		for (int i = randnum; i < charleft; i++) 
		{
			letters[i] = letters[i+1];
		}

	}

	return newString;

}

void WordJumble::displayChars(char* letters, int size)
// displays the contents of letters up to the specified size
// @input: char pointer, size(length) to be displayed
// @output: none
{
	for (int i=0; i < size; i++)
	{
		cout << *(letters+i);
	}
	
	cout << endl;
}
