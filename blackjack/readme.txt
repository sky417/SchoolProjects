Name: Andrew Kisoo Yoon
Student Number: 10653038
CS login: i5l4

Name: Another Ryan Youngik Song
Student Number: 20909032
CS login: e8d5

By submitting this assignment, I/we certify that the work submitted is
entirely my/our own. Any assistance has been acknowledged below and any web
sites used have been cited below. If I/we discussed aspects of this
assignment with anyone, I/we have listed their names below If this is a
partner-based assignment, then we have shared the workload and have done our
best to ensure that we each completely understand this assignment submitted.

Notes to marker:

Brief description about each classes is in the .txt files which has corresponding file name.

How to Play!!:	
Before game starts:
	1. Enter the total amount of money you want to play
	   This sets total bankroll (how much you bought in for)
    	2. Enter the number of decks
	   BlackJack game uses multiple deck, making harder for players to count cards.
           You can enter the number of decks from which cards are drawn.
	3. Enter the number of hands you would like to play
           You can play multiple hands at a time.
	   Each hand gets different cards, and different amount of bet, but evaluated against the same dealer's hand
	4. Enter a bet for each of your hands
	   Place your bet!!

During a game:
	1. Dealer gets one card, and each hand gets two cards
	2. If dealer has an ace, players can buy insurance up to half of their bet.
	   If insurance is bought, and dealer gets BlackJack, the hand gets paid 2 times of the insurance. 
	3. For each hand, player has several options: Hit, Stand, Double Down
	   Hit: Draw one more card
	   Stand: No more card
	   Double Down: Double the bet, but draws only one more card
	4. Ace can be counted as either 1 or 11; Ten, Jack, Queen and King are counted as 10.
	5. If a sum of cards in a hand goes over 21, player loses his bet.
	6. When every hand has finished action, dealer's hand gets completed.
	7. When dealer's hand is completed, player's hands get evaluated.
	   Win: won the game, the amount of bet will be added to the bankroll; if blackjack, 1.5 times of the bet will be added.
	   Lose: loses the game, the amount of bet player made will be subtracted from the bankroll.
	8. Push: Tied the game, no change in bankroll.

End of game:
	1. The game quits if player put "-1" for the bet amount, or player has no more money to play the next game.

Limitation on Our Program:
We could not cover all the situations/actions a player can have/take:
1. When first two cards are a pair, our program doesn't allow to split the cards and make them two separate hands.
2. Our program does not offer an option to surrender.
We have some technical difficulties we couldn't figure out ourselves:
1. When BlackJack gets paid 1.5 times of the bet, we ignore cents.
2. We can not change the number of decks/ hands once the game runs.
3. Despite our best effort to eliminate memory leak, it seems we still have some.
Also note,
1. We did not implement copy constructors, since we never needed them.
2. No destructor unless we need to delete pointers.

We tried to implement the BlackJack game played at casinos in BC (minor rules vary in different regions).
Detailed BlackJack Rules and available Player's actions can be found at:
`http://en.wikipedia.org/wiki/Blackjack
 
	

Acknowledgments:
Referenced string, vector manipulation from cplusplus.com 
Game, GameSuite, DummyGame, WordJumble, Player classes are taken from the assignment 1 of CPSC 260


I/we certify that the above is true to the best of my/our knowledge:
Digital signature: Andrew Kisoo Yoon
		   Ryan Youngik Song