#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node
{
    int x;
    int y;
    bool axis;
    bool direction;
    struct node *prev;
} node;

typedef struct
{
    bool snake;
    bool apple;
} tile;

#define COLUMNS 25
#define ROWS 15

tile GRID[ROWS][COLUMNS];

// Prototypes
void spawn_apple(void);
void default_grid(void);
void update_grid(node *tail);
bool print_grid(void);
void point_head(char arrow, node *head);
void move_snake(node *tail);
void move_node(node *n);
void lead_node(node *n);
void crash(void);
bool intersect(node *head, node *tail);
bool eat(node *head);
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
    int size = 1;

    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->direction = true;
    head->axis = false;
    head->prev = NULL;

    // Prepare to spawn first apple
    bool ate = true;

    // Loop game stages
    while (true)
    {
        // Default grid setup for snake positions
        default_grid();
        // Updete grid with snake positions
        update_grid(tail);
        // Spawn an apple if no apple left on grid
        if (ate)
        {
            spawn_apple();
        }
        // Print grid with snake and apple positions
        if (!print_grid())
        {
            lfree(tail);
            return 1;
        }

        // Prompt user for valid key input
        char cursor;
        do 
        {
            printf("(Up: R | Down: C | <-: D | ->: F): ");
            scanf(" %c", &cursor);
            cursor = toupper(cursor);
        }
        while (cursor != 'R' && cursor != 'C' && cursor != 'F' && cursor != 'D');

        point_head(cursor, head);
        move_snake(tail);

        if (head->x < 0 || head->x >= COLUMNS || head->y < 0 || head->y >= ROWS)
        {
            crash();
            break;
        }
        else if (intersect(head, tail))
        {
            crash();
            break;
        }
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
        else
        {
            ate = false;
        }
    }

    lfree(tail);
    return 0;
}

void spawn_apple(void)
{
    int x, y;
    do
    {
        y = (int) random() % ROWS;
        x = (int) random() % COLUMNS;
    }
    while (GRID[y][x].snake);

    GRID[y][x].apple = true;
    return;
}

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

void update_grid(node *tail)
{
    for (node *ptr = tail; ptr != NULL; ptr = ptr->prev)
    {
        GRID[ptr->y][ptr->x].snake = true;
    }
    return;
}

bool print_grid(void)
{
    FILE *screen = fopen("screen.txt", "w");
    if (screen == NULL)
    {
        // Error protocols
        return false;
    }

    for (int i = 0; i < ROWS; i++)
    {
        fprintf(screen, "|");
        for (int j = 0; j < COLUMNS; j++)
        {
            if (GRID[i][j].snake)
            {
                fprintf(screen, "+");
            }
            else if (GRID[i][j].apple)
            {
                fprintf(screen, "O");
            }
            else
            {
                fprintf(screen, " ");
            }
        }
        fprintf(screen, "|\n");
    }

    fclose(screen);
    return true;
}

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

void move_snake(node *tail)
{
    for (node *ptr = tail; ptr != NULL; ptr = ptr->prev)
    {
        move_node(ptr);
        if (ptr->prev != NULL)
        {
            lead_node(ptr);
        }
    }
    return;
}

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

void lead_node(node *n)
{
    n->axis = n->prev->axis;
    n->direction = n->prev->direction;
}

void crash(void)
{
    printf("Game over!!\n");
}

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

bool sizeup(node **tail)
{
    node *t = *tail;

    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }

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
    n->axis = t->axis;
    n->direction = t->direction;

    n->prev = t;
    *tail = n;

    return true;
}

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