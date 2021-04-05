#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

/**
 * This enum represents state the game is in
 */
enum class GameState
{
    mainMenu, // Game is in the main menu
    mazeMenu, // User is selecting the maze
    inGame,   // User is playing game
    finished  // Game has finished and the user is entering their name
};

/**
 * Gets a line from stdin and returns false if the eof bit is set
 */
bool getInput(string &input)
{
    getline(cin, input);

    if (cin.eof())
    {
        return false;
    }

    return true;
}

/**
 * Prints the rules of the game
 */
void printRules()
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
    cout << "   ->The player can exit the game at any moment by typing CTRL-Z, in Windows, or CTRL-D, in Linux.\n\n";
}

/**
 * The start of the game. Asks the user what he wants to do
 * Returns false if the user wants to exit the game
 */
bool mainMenu(GameState &gameState)
{
    string input;

    // Print menu
    cout << "Main menu: \n\n"
            "1) Rules \n"
            "2) Play \n"
            "0) Exit \n\n"
            "Please insert option (number): ";

    // Get input
    if (!getInput(input))
        return false; // EOF, exit game

    if (input == "1")
        printRules(); // Show the rules

    else if (input == "2")
        gameState = GameState::mazeMenu; // Pick the maze

    else if (input == "0")
        return false; // Leave the game

    else
        cout << "\nInvalid input!\n\n";

    return true;
}

int main()
{
    bool running = true;
    GameState gameState = GameState::mainMenu;

    while (running)
    {
        switch (gameState)
        {
        case GameState::mainMenu:
            running = mainMenu(gameState);
            break;
        case GameState::mazeMenu:
            cout << "MAZE MENU\n";
            break;
        case GameState::inGame:
            break;
        case GameState::finished:
            break;
        }
    }
}
