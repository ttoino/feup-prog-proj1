#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;
/**
Gives the output of the rules of the game
*/
void rules()
{
	cout << "\nSymbols meaning: \n   ->* = electrical fence or post\n   ->H = player (alive); h = player (dead); the player dies when he/she collides with a fence or a post, or is captured by a robot;\n";
	cout << "   ->R = robot(alive); r = robot(destroyed = dead / stuck); a dead robot is one that collided with a fence or a post;\na stuck robot is one that collided with another robot(alive or destroyed)\n";
	cout << "\nHow to play: \n   ->The player can only move to one of the 8 neighbour cells of his/her current cell. The movement is indicated by \ntyping one of the letters indicated below (the position of each letter relatively to the player's position \nindicates the movement that the player wants to do):\n";
	cout << setw(8) << 'Q' << setw(10) << 'W' << setw(13) << 'E' << '\n';
	cout << setw(8) << 'A' << setw(20) << "player's position" << setw(3) << 'D' << '\n';
	cout << setw(8) << 'Z' << setw(10) << 'X' << setw(13) << 'C' << '\n';
	cout << "   ->The player has the option to stay in his/her current position by typing 'S'.\n";
	cout << "   ->The above mentioned letters may be typed in uppercase or lowercase. If the user inputs an invalid letter/symbol, \nthe input must be repeated.\n";
	cout << "   ->The player should not be allowed to move to cells occupied by destroyed robots; if he/she tries to do so, he/she \nmust be informed that the movement is invalid and asked for a new movement.\n";
	cout << "   ->The player can exit the game at any moment by typing CTRL-Z, in Windows, or CTRL-D, in Linux.\n";

}

/**
The start of the game. Asks the user what he wants to do
*/
bool start()
{
	int number;
	bool validOperation = false;
	do {
		cout << "menu: \n" << "1) Rules \n" << "2) Play \n" << "0) Exit \n" << "Please insert option (number): ";
		cin >> number;
		if (cin.fail()) // in case the input doesn't match the parameters
		{
			cout << "Invalid input, please try again." << endl;
			cin.clear();
			cin.ignore(100000, '\n');
		}
		else validOperation = true;
	} while (!validOperation);
	if (number == 1) rules(); //show the rules
	else if (number == 2) return true; //start the game
	else return false; //leave the game
}

int main()
{
	bool playGame = start();
	while (playGame)
	{

	}
	cout << "GAME OVER :)";
}
