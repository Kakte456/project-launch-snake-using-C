#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Data struct: Node that belong to snake linked list
typedef struct node
{
    // 2 integers to represent position on grid
    int x;
    int y;
    // 2 bits to represent Direction
    bool axis;
    bool direction;
    // 1 pointer towards previous node / segment
    struct node *prev;
} node;

// Data type: Tile which marks the snake / apple / trap positions
typedef struct
{
    bool snake;
    bool apple;
    bool trap;
} tile;

// Constant: Grid dimensions
#define COLUMNS 25
#define ROWS 15

// Global variable: 2D Grid of tiles
tile GRID[ROWS][COLUMNS];

// Prototypes
void spawn_apple(void);
void spawn_trap(void);
void default_grid(void);
void update_grid(node *tail);
bool print_grid(int score);
void layout(FILE *screen);
char backwards(char cursor);
void point_head(char arrow, node *head);
void move_snake(node *tail);
void move_node(node *n);
void lead_node(node *n);
void crash(void);
bool intersect(node *head, node *tail);
bool eat(node *head);
bool hit(node *head);
bool sizeup(node **tail);
void lfree(node *tail);

int main(void)
{
    // Seed for random coordinate GENERATION
    srandom(time(NULL));

    // Default head node setup
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        return 1;
    }
    // Initially Point tail towards head node
    node *tail = head;

    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->direction = true;
    head->axis = false;
    head->prev = NULL;

    // Prepare to spawn first apple
    bool ate = true;
    // Snake initial size
    int size = 1;
    char inertia = 'X'; // Initial inertia to the invariant direction

    // Loop game
    while (true)
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
        if (!print_grid(size - 1))
        {
            lfree(tail);
            return 1;
        }

        // Prompt user for valid key input for cursor
        char cursor;
        do 
        {
            printf("(Up: R | Down: C | <-: D | ->: F): ");
            scanf(" %c", &cursor);
            cursor = toupper(cursor);
        }
        while ((cursor != 'R' && cursor != 'C' && cursor != 'F' && cursor != 'D') || cursor == inertia);

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
        else if (hit(head))
        {
            crash();
            break;
        }
        // Upgrade snake if head eats apple
        else if (eat(head))
        {
            GRID[head->y][head->x].apple = false;
            ate = true;

            if (!sizeup(&tail))
            {
                lfree(head);
                return 1;
            }
            size++;
        }
        // Next turn
        else
        {
            ate = false;
        }
    }

    // Recursively free the snake list
    lfree(tail);
    return 0;
}

// Spawn apple in random position on grid
void spawn_apple(void)
{
    int x, y;
    do
    {
        y = (int) random() % ROWS;
        x = (int) random() % COLUMNS;
    }
    while (GRID[y][x].snake || GRID[y][x].trap); // Avoid snake and trap positions

    GRID[y][x].apple = true;
    return;
}

void spawn_trap(void)
{
    int x, y;
    do
    {
        y = (int) random() % ROWS;
        x = (int) random() % COLUMNS;
    }
    while (GRID[y][x].snake || GRID[y][x].apple || GRID[y][x].trap); // Avoid snake, apple and trap positions

    GRID[y][x].trap = true;
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
bool print_grid(int score)
{
    FILE *screen = fopen("screen.txt", "w");
    if (screen == NULL)
    {
        // Error protocols
        return false;
    }

    // Top layout
    layout(screen);
    // Print grid row by row
    for (int i = 0; i < ROWS; i++)
    {
        fprintf(screen, "#");
        for (int j = 0; j < COLUMNS; j++)
        {
            if (GRID[i][j].snake)
            {
                fprintf(screen, "O"); // Snake: O
            }
            else if (GRID[i][j].apple)
            {
                fprintf(screen, "A"); // Apple: A
            }
            else if (GRID[i][j].trap)
            {
                fprintf(screen, "X"); // Trap: X
            }
            else
            {
                fprintf(screen, " ");
            }
        }
        fprintf(screen, "#\n");
    }
    // Bottom layout
    layout(screen);

    // Print the score to screen
    fprintf(screen, "\nSCORE : %i\n", score);

    fclose(screen);
    return true;
}

// Brick (#) Boundaries
void layout(FILE *screen)
{
    for (int i = 0; i < COLUMNS + 2; i++)
    {
        fprintf(screen, "#");
    }
    fprintf(screen, "\n");
}

char backwards(char cursor)
{
    if (cursor == 'R')
    {
        return 'C';
    }
    else if (cursor == 'C')
    {
        return 'R';
    }
    else if (cursor == 'D')
    {
        return 'F';
    }
    else // cursor == 'F'
    {
        return 'D';
    }
}

// Use cursor input to change direction of head
// Vertical axis: true | Horizontal axis: false
// Right/Down: true | Left/Up: false
void point_head(char arrow, node *head)
{
    if (arrow == 'C' || arrow == 'F')
    {
        head->direction = true;
    }
    else
    {
        head->direction = false;
    }

    if (arrow == 'R' || arrow == 'C')
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
}

// Crash statement protocols
void crash(void)
{
    printf("Game over!!\n");
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
bool sizeup(node **tail)
{
    node *t = *tail;

    // Allocate a new node
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }

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

    return true;
}

// Recursively free snake link list
void lfree(node *tail)
{
    if (tail == NULL)
    {
        return;
    }

    lfree(tail->prev);
    free(tail);
    return;
}