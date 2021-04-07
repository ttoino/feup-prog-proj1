#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <chrono>

using namespace std;

/**
 * This struct represents a robot or the player
 */
struct Entity
{
    /** Position on the x-axis */
    int column;
    /** Position on the y-axis */
    int line;
    /** Whether the entity is alive or dead */
    bool alive = true;

    /**
     * Default constructor, creates an entity that is alive at specified position
     */
    Entity(int c, int l)
    {
        column = c;
        line = l;
    }
};

/**
 * This struct holds all the information needed for a game to be played
 */
struct Maze
{
    /** Size of the maze on the x-axis */
    size_t nCols;
    /** Size of the maze on the y-axis */
    size_t nRows;

    /** The maze number, "01" to "99", used to save high scores at the end of the game */
    string mazeNumber;
    /** When the player started playing */
    chrono::steady_clock::time_point startTime;

    /** Maze map containing only the fences/posts */
    vector<char> fenceMap;
    /** The full maze map, created from the fenceMap */
    vector<char> visualMap;
    /** A vector that holds all the robots */
    vector<Entity> robots;
    /** The player */
    Entity player = Entity(0, 0);
};

/**
 * This enum represents state the game is in
 */
enum class GameState
{
    /** Game is in the main menu */
    mainMenu,
    /** User is selecting the maze */
    mazeMenu,
    /** User is playing game */
    inGame,
    /** Game has finished and the user is entering their name */
    finished
};

/**
 * Gets a line from stdin and returns false if the eof bit is set
 * 
 * @param input Where to store the input
 * @returns false if the eof bit is set, true otherwise
 */
bool getInput(string &input)
{
    getline(cin, input);

    if (cin.eof())
        return false;

    return true;
}

/**
 * Prints the rules of the game
 */
void printRules()
{
    cout << "\nSymbols meaning: \n"
            "   ->* = electrical fence or post\n"
            "   ->H = player (alive); h = player (dead); the player dies when he/she collides with a fence or a post, or is captured by a robot;\n"
            "   ->R = robot (alive); r = robot (destroyed = dead / stuck); a dead robot is one that collided with a fence or a post;\n"
            "a stuck robot is one that collided with another robot(alive or destroyed)\n\n"

            "How to play: \n"
            "   ->The player can only move to one of the 8 neighbour cells of his/her current cell. The movement is indicated by \n"
            "typing one of the letters indicated below (the position of each letter relatively to the player's position \n"
            "indicates the movement that the player wants to do):\n"
         << setw(8) << 'Q' << setw(10) << 'W' << setw(13) << 'E' << '\n'
         << setw(8) << 'A' << setw(20) << "player's position" << setw(3) << 'D' << '\n'
         << setw(8) << 'Z' << setw(10) << 'X' << setw(13) << 'C' << '\n'
         << "   ->The player has the option to stay in his/her current position by typing 'S'.\n"
            "   ->The above mentioned letters may be typed in uppercase or lowercase. If the user inputs an invalid letter/symbol, \n"
            "the input must be repeated.\n"
            "   ->The player should not be allowed to move to cells occupied by destroyed robots; if he/she tries to do so, he/she \n"
            "must be informed that the movement is invalid and asked for a new movement.\n"
            "   ->The player can exit the game at any moment by typing CTRL-Z, in Windows, or CTRL-D, in Linux.\n\n";
}

/**
 * The start of the game. Asks the user what he wants to do
 * 
 * @param gameState The game state
 * @returns false if the user wants to exit the game
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

/**
 * Checks if a maze number is valid.
 * A number is considered valid if it falls in the range "00" to "99" (needs two characters).
 * 
 * @param number The maze number
 * @returns true if the number is valid
 */
bool validMazeNumber(const string &number)
{
    return number.length() == 2 && isdigit(number.at(0)) && isdigit(number.at(1));
}

/**
 * Receives input from the player and loads the respective maze
 * 
 * @param gameState The game state
 * @param maze Where the maze is stored
 * 
 * @returns false if the player wants to exit the game
 */
bool mazeMenu(GameState &gameState, Maze &maze)
{
    // Reset maze variable
    maze = Maze();

    // Ask user for input
    cout << "Input number of the maze: ";

    // Get input
    if (!getInput(maze.mazeNumber))
        return false;

    // Pad out maze number
    // "" -> "0" -> "00"
    // "5" -> "05"
    while (maze.mazeNumber.length() < 2)
    {
        maze.mazeNumber = "0"s + maze.mazeNumber;
    }

    // User wants to return to main menu
    if (maze.mazeNumber == "00")
    {
        gameState = GameState::mainMenu;
    }

    // Maze number is invalid
    if (!validMazeNumber(maze.mazeNumber))
    {
        cout << "\nInvalid input!\n\n";
        return true;
    }

    // Construct file name
    string filename = "MAZE_"s + maze.mazeNumber + ".txt"s;

    // Open file
    ifstream infile;
    infile.open(filename);
    // File doesn't exist
    if (!infile.is_open())
    {
        cout << "\nFile " << filename << " not found!\n\n";
        return true;
    }

    // Get number of rows and columns from top of file
    char x;
    infile >> maze.nRows >> x >> maze.nCols;

    // Load maze
    for (size_t i = 0; i < maze.nRows; i++)
    {
        // Ignore \n
        infile.ignore();

        for (size_t j = 0; j < maze.nCols; j++)
        {
            char c;
            infile.get(c);

            if (c == '*')
                // Tile is a post/fence
                maze.fenceMap.push_back('*');
            else
                // Tile is not a post/fence
                maze.fenceMap.push_back(' ');

            if (c == 'R')
                // Tile is a robot
                maze.robots.push_back(Entity(j, i));
            else if (c == 'H')
                // Tile is the player
                maze.player = Entity(j, i);
        }
    }

    infile.close();

    // Start the game
    gameState = GameState::inGame;
    maze.startTime = chrono::steady_clock::now();
    return true;
}

/**
 * Receives the input from the player and moves them accordingly.
 * 
 * @param player The player
 */
void movePlayer(Entity &player)
{
    char move;
    cout << "insert movement: ";
    cin >> move;
    if (cin.fail())
    {
        cout << "\nInvalid input!\n\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    if (move == 'Q' || move == 'q')
    {
        player.line -= 1;
        player.column -= 1;
    }
    else if (move == 'W' || move == 'w')
        player.line -= 1;
    else if (move == 'E' || move == 'e')
    {
        player.line -= 1;
        player.column += 1;
    }
    else if (move == 'A' || move == 'a')
        player.column -= 1;
    else if (move == 'S' || move == 's')
        ;
    else if (move == 'D' || move == 'd')
        player.column += 1;
    else if (move == 'Z' || move == 'z')
    {
        player.line += 1;
        player.column -= 1;
    }
    else if (move == 'X' || move == 'x')
        player.line += 1;
    else if (move == 'C' || move == 'c')
    {
        player.line += 1;
        player.column += 1;
    }
    else
        cout << "\nInvalid input!\n\n";
}

/**
 * checks if the player position is the same as the Robots (if they collied)
 *
 * @param1 player The player
 * @param2 vector with the positions of the robots
 * @param3 position of the player before the movement
 */
void checkPlayerandRobot(Entity &player, vector<Entity> &robots, Entity positionBefore)
{
    for (int i = 0; i < robots.size(); i++)
    {
        if (robots[i].line == player.line && robots[i].column == player.line && robots[i].alive)
            player.alive = false;
        else if (robots[i].line == player.line && robots[i].column == player.line && !robots[i].alive)
        {
            cout << "Cell ocupied by a dead robot, please try again.\n";
            player = positionBefore;
        }
    }
}

/**
 * checks if two robots collied
 *
 * @param vector with the positions of the robots
 */
void checkRobotandRobot(vector<Entity>& robots)
{
    for (int i = 0; i < robots.size(); i++)
    {
        for (int j = i + 1; j < robots.size() - 1; j++)
        {
            if (robots[i].line == robots[j].line && robots[i].column == robots[j].line)
            {
                robots[i].alive = false;
                robots[j].alive = false;
            }
        }
    }
}

/**
 * checks if a player or a robots collides with the fence
 *
 * @param1  the positions of the being
 * @param2  the maze
 */
void checkBeingandFence(Entity& being, Maze maze)
{
    size_t i = being.line * maze.nCols + being.column; 
    if (maze.fenceMap[i] == true) being.alive = false;
}

void moveRobot

int main()
{
    /** Whether the program is running */
    bool running = true;
    /** The game state */
    GameState gameState = GameState::mainMenu;
    /** Information about the maze */
    Maze maze;

    while (running)
    {
        switch (gameState)
        {
        case GameState::mainMenu:
            running = mainMenu(gameState);
            break;
        case GameState::mazeMenu:
            running = mazeMenu(gameState, maze);
            break;
        case GameState::inGame:
            break;
        case GameState::finished:
            break;
        }
    }
}
