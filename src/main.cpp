#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <algorithm>

using namespace std;

const string GENERIC_ERROR = "Invalid input!"s;
const string CELL_OCCUPIED = "That cell is occupied!"s;
const string OUT_OF_BOUNDS = "Cannot move out of bounds!"s;
const string INVALID_MAZE_NUMBER = "Must be a number from 1 to 99!"s;
const string MAZE_NOT_FOUND = "That maze could not be found!"s;
const string INVALID_NAME = "Must have 15 characters or fewer!"s;
const string ANOTHER_NAME = "";


/**
 * This struct represents a robot or the player
 */
struct Entity
{
    /** Position on the x-axis */
    size_t column;
    /** Position on the y-axis */
    size_t line;
    /** Whether the entity is alive or dead */
    bool alive = true;

    /**
     * Default constructor, creates an entity that is alive at specified position
     */
    Entity(size_t c, size_t l)
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
    size_t nLines;
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

    size_t index(const size_t column, const size_t line) const
    {
        return line * nCols + column;
    }
};

/**
 * This struct holds all the information nabout the leaderboard
 */
struct LeaderBoard
{
    string name;
    unsigned points;
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

int sign(int x)
{
    return (x > 0) - (x < 0);
}

/**
 * Prints the game's rules
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
bool mainMenu(GameState &gameState, bool &validInput, string &errorMessage)
{
    string input;

    // Print menu
    if (validInput)
        cout << "Main menu: \n\n"
                "1) Rules \n"
                "2) Play \n"
                "0) Exit \n\n";

    cout << "Please insert option: ";

    // Get input
    if (!getInput(input))
        return false; // EOF, exit game

    validInput = true;

    if (input == "1")
        printRules(); // Show the rules

    else if (input == "2")
        gameState = GameState::mazeMenu; // Pick the maze

    else if (input == "0")
        return false; // Leave the game

    else
    {
        validInput = false;
        errorMessage = GENERIC_ERROR;
    }

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
bool mazeMenu(GameState &gameState, Maze &maze, bool &validInput, string &errorMessage)
{
    // Reset maze variable
    maze = Maze();

    validInput = true;

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
        return true;
    }

    // Maze number is invalid
    if (!validMazeNumber(maze.mazeNumber))
    {
        validInput = false;
        errorMessage = INVALID_MAZE_NUMBER;
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
        validInput = false;
        errorMessage = MAZE_NOT_FOUND;
        return true;
    }

    // Get number of rows and columns from top of file
    char x;
    infile >> maze.nLines >> x >> maze.nCols;

    // Load maze
    for (size_t i = 0; i < maze.nLines; i++)
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

bool doPlayerMove(Maze &maze, string &errorMessage, int columnDelta, int lineDelta)
{
    int newCol = maze.player.column + columnDelta;
    int newLine = maze.player.line + lineDelta;

    if (newCol < 0 || newCol >= maze.nCols || newLine < 0 || newLine >= maze.nLines)
    {
        errorMessage = OUT_OF_BOUNDS;
        return false;
    }
    else if (maze.visualMap.at(maze.index(newCol, newLine)) == 'r')
    {
        errorMessage = CELL_OCCUPIED;
        return false;
    }

    maze.player.column = newCol;
    maze.player.line = newLine;
    return true;
}

/**
 * Receives the input from the player and moves them accordingly.
 * Ends the game if the user pressed crt-Z by returning false
 * @param player The player
 */
bool movePlayer(Maze &maze, bool &validInput, string &errorMessage)
{
    string input;

    cout << "Insert movement: ";

    if (!getInput(input))
        return false;

    if (input.length() != 1)
    {
        validInput = false;
        errorMessage = GENERIC_ERROR;
        return true;
    }

    char move = tolower(input.at(0));

    switch (move)
    {
    case 'q':
        validInput = doPlayerMove(maze, errorMessage, -1, -1);
        return true;
    case 'w':
        validInput = doPlayerMove(maze, errorMessage, 0, -1);
        return true;
    case 'e':
        validInput = doPlayerMove(maze, errorMessage, 1, -1);
        return true;
    case 'a':
        validInput = doPlayerMove(maze, errorMessage, -1, 0);
        return true;
    case 's':
        validInput = doPlayerMove(maze, errorMessage, 0, 0);
        return true;
    case 'd':
        validInput = doPlayerMove(maze, errorMessage, 1, 0);
        return true;
    case 'z':
        validInput = doPlayerMove(maze, errorMessage, -1, 1);
        return true;
    case 'x':
        validInput = doPlayerMove(maze, errorMessage, 0, 1);
        return true;
    case 'c':
        validInput = doPlayerMove(maze, errorMessage, 1, 1);
        return true;
    default:
        validInput = false;
        errorMessage = GENERIC_ERROR;
        return true;
    }
}

bool entityEntityCollision(const Entity &e1, const Entity &e2)
{
    return e1.line == e2.line && e1.column == e2.column;
}

/**
 * checks if a player or a robots collides with the fence
 *
 * @param entity The entity
 * @param maze The maze
 */
bool entityFenceCollision(const Entity &entity, const Maze &maze)
{
    return maze.fenceMap.at(maze.index(entity.column, entity.line)) == '*';
}

/**
 * moves the robots in the direction of the player
 *
 * @param1  vector with the positions of the robots
 * @param2  position of the player
 */
void moveRobots(Maze &maze)
{
    for (Entity &robot : maze.robots)
    {
        if (!robot.alive)
            continue;

        robot.line += sign(maze.player.line - robot.line);
        robot.column += sign(maze.player.column - robot.column);

        robot.alive = !entityFenceCollision(robot, maze);

        for (Entity &other : maze.robots)
        {
            if (&robot == &other)
                continue;

            if (entityEntityCollision(robot, other))
            {
                robot.alive = false;
                other.alive = false;
            }
        }

        if (entityEntityCollision(robot, maze.player))
        {
            maze.player.alive = false;
        }
    }
}

bool isGameOver(const Maze &maze)
{
    bool allDead = true;
    for (const Entity &robot : maze.robots)
    {
        if (robot.alive)
        {
            allDead = false;
            break;
        }
    }

    return allDead || !maze.player.alive;
}

/**
 * Updates the maze's visualMap with the robots and the player
 * 
 * @param maze The maze
 */
void updateVisualMap(Maze &maze)
{
    maze.visualMap = maze.fenceMap;

    for (const Entity &r : maze.robots)
    {
        maze.visualMap.at(r.line * maze.nCols + r.column) = r.alive ? 'R' : 'r';
    }

    maze.visualMap.at(maze.player.line * maze.nCols + maze.player.column) = maze.player.alive ? 'H' : 'h';
}

/**
 * Prints the maze's visualMap
 * 
 * @param maze The maze
 */
void displayMaze(const Maze &maze)
{
    for (size_t i = 0; i < maze.visualMap.size(); i++)
    {
        if (i % maze.nCols == 0)
            cout << '\n';

        cout << maze.visualMap.at(i);
    }
    cout << '\n';
}

bool inGame(GameState &gameState, Maze &maze, bool &validInput, string &errorMessage)
{
    // Show maze
    if (validInput)
    {
        updateVisualMap(maze);
        displayMaze(maze);
    }

    // Check if game is over
    if (isGameOver(maze))
    {
        gameState = GameState::finished;
        return true;
    }

    if (!movePlayer(maze, validInput, errorMessage))
        return false;
    if (!validInput)
        return true;

    if (entityFenceCollision(maze.player, maze) || maze.visualMap.at(maze.index(maze.player.column, maze.player.line)) == 'R')
    {
        maze.player.alive = false;
        return true;
    }

    moveRobots(maze);

    return true;
}

void readLeaderboard(const string &mazeNumber, vector<LeaderBoard> &leaderBoard)
{
    string fileName = "MAZE_"s + mazeNumber + "_WINNERS.txt"s;

    ifstream file;
    file.open(fileName);

    // File doesn't exist
    if (!file.is_open())
        return;

    // Ignore header
    file.ignore(100, '\n');
    file.ignore(100, '\n');

    do 
    {
        LeaderBoard person;
        char c;
        do
        {
            c = file.get();
            person.name += c;
        } while (!(c == ' ' && file.peek() == ' ') && person.name.length() < 15);
        person.name.pop_back();

        // Ignore dash
        file >> c >> person.points;
        leaderBoard.push_back(person);

        // Ignore \n
        file.ignore();
    } while (!file.eof());

    file.close();
}

bool compareLeaderboard(LeaderBoard person1, LeaderBoard person2)
{
    return (person1.points < person2.points);
}

void printLeaderboard(ostream &out, vector<LeaderBoard> leaderBoard)
{
    out << "Player          - Time\n----------------------\n";
    sort(leaderBoard.begin(), leaderBoard.end(), compareLeaderboard);
    for (auto person : leaderBoard)
    {
        out << setw(16) << left << person.name << '-' << setw(5) << right << person.points << '\n';
    }
}

void saveLeaderboard(const string &mazeNumber, vector<LeaderBoard> leaderBoard)
{
    string fileName = "MAZE_"s + mazeNumber + "_WINNERS.txt"s;

    ofstream file;
    file.open(fileName);

    // Add header
    printLeaderboard(file, leaderBoard);
}

bool  searchName(vector<LeaderBoard>& leaderBoard, LeaderBoard person)
{
    bool foundName = false;
    for (auto& gamer : leaderBoard)
    {
        if (gamer.name == person.name)
        {
            foundName = true;
            string decision;
            cout << "The name already exits in the leader board do you wish to continue with it? (y/n)";
            getInput(decision);
            if (decision=="y"|| decision=="Y") gamer.points = person.points;
            else
            {
                return true;
            }
        }
    }
    if (!foundName) leaderBoard.push_back(person);
    return false;
}


bool finished(GameState &gameState, const Maze &maze, bool &validInput, string &errorMessage)
{
    if (maze.player.alive)
    {
        if (validInput)
            cout << "You win!\n";

        cout << "Please insert your name: ";

        string name;
        if (!getInput(name))
            return false;

        if (name.length() > 15)
        {
            validInput = false;
            errorMessage = INVALID_NAME;
            return true;
        }
        else if (name.length() == 0)
        {
            validInput = false;
            errorMessage = GENERIC_ERROR;
            return true;
        }

 
        validInput = true;

        LeaderBoard person;
        vector<LeaderBoard> leaderBoard;
        readLeaderboard(maze.mazeNumber, leaderBoard);
        person.name = name;
        person.points = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - maze.startTime).count();
        
        if (searchName(leaderBoard, person))
        {
            validInput = false;
            errorMessage = ANOTHER_NAME;
            return true;
        }

        cout << '\n';
        printLeaderboard(cout, leaderBoard);
        cout << '\n';

        saveLeaderboard(maze.mazeNumber, leaderBoard);
    }
    else
        cout << "You lose :(\n";

    cout << "Press enter to continue\n";
    gameState = GameState::mainMenu;

    string i;
    return getInput(i);
}

int main()
{
    /** Whether the program is running */
    bool running = true;
    /** Whether the last input was valid */
    bool validInput = true;
    /** The message to show if the input was invalid */
    string errorMessage;
    /** The game state */
    GameState gameState = GameState::mainMenu;
    /** Information about the maze */
    Maze maze;

    while (running)
    {
        if (!validInput)
            cout << errorMessage << "\n\n";

        switch (gameState)
        {
        case GameState::mainMenu:
            running = mainMenu(gameState, validInput, errorMessage);
            break;
        case GameState::mazeMenu:
            running = mazeMenu(gameState, maze, validInput, errorMessage);
            break;
        case GameState::inGame:
            running = inGame(gameState, maze, validInput, errorMessage);
            break;
        case GameState::finished:
            running = finished(gameState, maze, validInput, errorMessage);
            break;
        }
    }
}
