// ECONOMIC SNAKE
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

// Data struct: Node that belong to snake linked list
struct node
{
    // 2 integers to represent position on grid
    int x;
    int y;
    // 2 bits to represent Direction
    bool axis;
    bool direction;
    // 1 pointer towards previous node / segment
    node *prev;
};

// Data type: Tile which marks the snake / apple / trap positions & ages
struct tile
{
    bool snake;
    bool apple;
    bool trap;
    int trap_age;
    int apple_age; 
};

// Constant: Game speed in milliseconds
const int SPEED = 500;

// Constant: Grid dimensions
const int COLUMNS = 25;
const int ROWS = 15;

// Constant: Trap life span
const int TRAP_LIFE = 125;

// Global variable: 2D Grid of tiles
tile GRID[ROWS][COLUMNS];

// Prototypes
void spawn_apple(void);
void spawn_trap(void);
void default_grid(void);
void update_grid(node *tail);
void print_grid(int score, int moves);
void layout(void);
char backwards(char cursor);
void point_head(char arrow, node *head);
void move_snake(node *tail);
void move_node(node *n);
void lead_node(node *n);
void crash(void);
bool intersect(node *head, node *tail);
bool eat(node *head);
bool hit(node *head);
void sizeup(node **tail);
void age(void);
int recharge(int age);
int reward(int age);
void lfree(node *tail);
void enable_live(void);
void disable_live(void);
char read_key(void);

int main(void)
{
    // Seed for random coordinate GENERATION
    srand(time(NULL));

    // Default head node setup
    node *head = new node;
    // Initially Point tail towards head node
    node *tail = head;

    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->direction = true;
    head->axis = false;
    head->prev = NULL;

    // Preconditions 

    // Prepare to spawn first apple
    bool ate = true;
    // Snake initial size
    int size = 1;
    // Initial score
    int score = 0;
    // Initial moves
    int moves = 50;
    char inertia = 'X'; // Initial inertia to the invariant direction

    spawn_apple();

    // Enable live mode for terminal input
    enable_live(); 

    // Loop game
    while (moves > 0)
    {
        // Default grid setup for snake positions
        default_grid();
        // Updete grid with snake positions
        update_grid(tail);
        // Spawn an apple and new trap if no apple left on grid
        if (ate)
        {
            spawn_trap();
            spawn_apple();
        }

        // Print grid with snake, trap and apple positions
        print_grid(score, moves);

        // Adjust Game speed
        this_thread::sleep_for(chrono::milliseconds(SPEED)); // Sleep for SPEED milliseconds to reduce flickering

        // Prompt user for valid key input for cursor
        char cursor, key;
        key = read_key(); // Read key input without blocking
        if (key != 0)
        {
            // Convert key to uppercase
            key = static_cast <char> (toupper(key));

            // Update cursor if key is valid and not opposite to inertia
            if ((key == 'W' || key == 'S' || key == 'D' || key == 'A') && key != inertia)
            {
                cursor = key;
            }
        }

        inertia = backwards(cursor);
        // Change head direction using cursor input
        point_head(cursor, head);
        // Move snake nodes towards their directions and updating directions
        move_snake(tail);

        // Crash if head hits boundary
        if (head->x < 0 || head->x >= COLUMNS || head->y < 0 || head->y >= ROWS)
        {
            crash();
            break;
        }
        // Crash if head hits snake body
        else if (intersect(head, tail))
        {
            crash();
            break;
        }
        // Crash if head hits trap
        else if (hit(head))
        {
            crash();
            break;
        }
        // Upgrade snake if head eats apple
        else if (eat(head))
        {
            moves += recharge(GRID[head->y][head->x].apple_age);
            score += reward(GRID[head->y][head->x].apple_age);
            GRID[head->y][head->x].apple = false;
            GRID[head->y][head->x].apple_age = 0;
            ate = true;

            sizeup(&tail);
            size++;
        }
        // Next turn
        else
        {
            ate = false;
            moves--;
        }

        // Age apples and traps on grid
        age();
    }

    // Recursively free the snake list
    lfree(tail);

    // Disable live mode and restore terminal settings
    disable_live();
    
    return 0;
}

// Spawn apple in random position on grid
void spawn_apple(void)
{
    int x, y;
    do
    {
        y = (int) rand() % ROWS;
        x = (int) rand() % COLUMNS;
    }
    while (GRID[y][x].snake || GRID[y][x].apple || GRID[y][x].trap); // Avoid snake, apple and trap positions

    GRID[y][x].apple = true;
    GRID[y][x].apple_age = -1; // Set apple age to -1
    return;
}

// Spawn trap in random position on grid
void spawn_trap(void)
{
    int x, y;
    do
    {
        y = (int) rand() % ROWS;
        x = (int) rand() % COLUMNS;
    }
    while (GRID[y][x].snake || GRID[y][x].apple || GRID[y][x].trap); // Avoid snake, apple and trap positions

    GRID[y][x].trap = true;
    GRID[y][x].trap_age = -1; // Set trap age to -1
    return;
}

// Reset snake positions on grid
void default_grid(void)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            GRID[i][j].snake = false;
        }
    }
    return;
}

// Update grid with new snake positions
void update_grid(node *tail)
{
    // Iterate through each node on snake link list
    for (node *ptr = tail; ptr != NULL; ptr = ptr->prev)
    {
        GRID[ptr->y][ptr->x].snake = true;
    }
    return;
}

// Print the grid and layout on screen
void print_grid(int score, int moves)
{
    // Clear screen and move cursor to top left
    cout << "\033[2J\033[1;1H";

    // Title and spacing
    cout << "\n";
    cout << "\033[1;34m      SEECS & SNAKES\033[0m\n\n"; // Title in blue color

    // Top layout
    layout();
    // Print grid row by row
    for (int i = 0; i < ROWS; i++)
    {
        cout << "#";
        for (int j = 0; j < COLUMNS; j++)
        {
            if (GRID[i][j].snake)
            {
                cout << "\033[32mO\033[0m"; // Snake: O
            }
            else if (GRID[i][j].apple)
            {
                cout << "\033[31mA\033[0m"; // Apple: A
            }
            else if (GRID[i][j].trap)
            {
                cout << "X"; // Trap: X
            }
            else
            {
                cout << " ";
            }
        }
        cout << "#\n";
    }
    // Bottom layout
    layout();

    // Print the score & moves to screen in yellow color
    cout << "\nSCORE : \033[1;33m" << score << "\033[0m\n";
    cout << "MOVES LEFT : \033[1;33m" << moves << "\033[0m\n\n";

    // Print the keys
    cout << "(Up: W | Down: S | <-: A | ->: D):\n\n";

    cout.flush(); // Flush the output buffer to ensure all output is printed
    return;
}

// Brick (#) Boundaries
void layout(void)
{
    for (int i = 0; i < COLUMNS + 2; i++)
    {
        cout << "#";
    }
    cout << "\n";
    return;
}

// Get opposite direction of cursor input
char backwards(char cursor)
{
    if (cursor == 'W')
    {
        return 'S';
    }
    else if (cursor == 'S')
    {
        return 'W';
    }
    else if (cursor == 'D')
    {
        return 'A';
    }
    else // cursor == 'A'
    {
        return 'D';
    }
}

// Use cursor input to change direction of head
// Vertical axis: true | Horizontal axis: false
// Right/Down: true | Left/Up: false
void point_head(char arrow, node *head)
{
    if (arrow == 'D' || arrow == 'S')
    {
        head->direction = true;
    }
    else
    {
        head->direction = false;
    }

    if (arrow == 'W' || arrow == 'S')
    {
        head->axis = true;
    }
    else
    {
        head->axis = false;
    }
    return;
}

// Move snake body by changing coordinates of each snake node
void move_snake(node *tail)
{
    // Iterate node by node
    for (node *ptr = tail; ptr != NULL; ptr = ptr->prev)
    {
        // Move node in its direction
        move_node(ptr);
        // Inherit direction form previous node
        if (ptr->prev != NULL)
        {
            lead_node(ptr);
        }
    }
    return;
}

// Move node in its given direction by translating the direction bits
void move_node(node *n)
{
    if (n->axis == true)
    {
        if (n->direction == true)
        {
            n->y++;
        }
        else
        {
            n->y--;
        }
    }
    else
    {
        if (n->direction == true)
        {
            n->x++;
        }
        else
        {
            n->x--;
        }
    }
    return;
}

// Inherit directions from prev node
void lead_node(node *n)
{
    n->axis = n->prev->axis;
    n->direction = n->prev->direction;
    return;
}

// Crash statement protocols
void crash(void)
{
    cout << "\n\033[1;31mGAME OVER!!\033[0m\n";
    return;
}

// Check if head hits snake body
bool intersect(node *head, node *tail)
{
    for (node *ptr = tail; ptr != head; ptr = ptr->prev)
    {
        if (head->x == ptr->x && head->y == ptr->y)
        {
            return true;
        }
    }

    return false;
}

// Check if head hits apple
bool eat(node *head)
{
    if (GRID[head->y][head->x].apple)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if head hits trap
bool hit(node *head)
{
    if (GRID[head->y][head->x].trap)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Upgarde snake list by appending node to the tail
void sizeup(node **tail)
{
    node *t = *tail;

    // Allocate a new node
    node *n = new node;

    // On grid, Place new node behind tail node
    n->prev = NULL;
    if (t->axis)
    {
        if (t->direction)
        {
            n->y = t->y - 1;
        }
        else
        {
            n->y = t->y + 1;
        }
        n->x = t->x;
    }
    else
    {
        if (t->direction)
        {
            n->x = t->x - 1;
        }
        else
        {
            n->x = t->x + 1;
        }
        n->y = t->y;
    }
    // Inherit directions form tail node
    n->axis = t->axis;
    n->direction = t->direction;

    // Append new node to tail without orphaning
    n->prev = t;
    *tail = n;

    return;
}

// Age apples and traps on grid
void age(void)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (GRID[i][j].apple)
            {
                GRID[i][j].apple_age++; // Increase apple age by 1
            }
            if (GRID[i][j].trap)
            {
                GRID[i][j].trap_age++; // Increase trap age by 1
                // Remove trap if it exceeds life span
                if (GRID[i][j].trap_age >= TRAP_LIFE)
                {
                    GRID[i][j].trap = false;
                    GRID[i][j].trap_age = 0;
                }
            }
        }
    }
    return;
}

// Recursively free snake link list
void lfree(node *tail)
{
    if (tail == NULL)
    {
        return;
    }

    lfree(tail->prev);
    delete tail;
    return;
}

// Enable live mode for terminal input
void enable_live(void)
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    return;
}

// Disable live mode and restore terminal settings
void disable_live(void)
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    return;
}

// Read key input without blocking
char read_key(void)
{
    char c;
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    if (read(STDIN_FILENO, &c, 1) > 0)
    {
        return c;
    }

    return 0;
}

// Economic Algorithms

// Recharge moves based on apple age
int recharge(int age)
{
    int moves = 21 - (age * 0.70);
    if (moves < 8)
    {
        return 8;
    }
    else
    {
        return moves;
    }
}

// Reward score based on apple age
int reward(int age)
{
    int points = age * 0.5;
    if (points > 10)
    {
         return 10;
    }
    else
    {
         return points;
    }
}
