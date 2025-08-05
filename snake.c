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
    struct node *next;
    struct node *prev;
} node;

typedef struct
{
    bool snake;
    bool apple;
} TILE;

#define COLUMNS 25
#define ROWS 15

TILE GRID[ROWS][COLUMNS];

// Prototypes
void spawn_apple(void);
void default_grid(void);
void update_grid(node *head);
bool print_grid(void);
void point_head(char arrow, node *head);
void move_snake(node *tail);
void move_node(node *n);
void lead_node(node *n);
void crash(void);
bool intersect(node *head);
bool eat(node *head);
bool sizeup(node **tail);
void lfree(node *head);

int main(void)
{
    // Seed for random coordinate GENERATION
    srandom(time(NULL));

    // Default head setup
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        return 1;
    }
    node *tail = head;
    int size = 1;

    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->direction = true;
    head->axis = true;
    head->next = NULL;
    head->prev = NULL;

    bool ate = true;

    while (true)
    {
        // Default grid setup for snake positions
        default_grid();
        // Updete grid with snake positions
        update_grid(head);
        // Spawn an apple if no apple left on grid
        if (ate)
        {
            spawn_apple();
        }
        // Print grid with snake and aapple positions
        if (!print_grid())
        {
            lfree(head);
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
        else if (intersect(head))
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

    lfree(head);
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

void update_grid(node *head)
{
    for (node *ptr = head; ptr != NULL; ptr = ptr->next)
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

bool intersect(node *head)
{
    if (head->next == NULL)
    {
        return false;
    }

    for (node *ptr = head->next; ptr != NULL; ptr = ptr->next)
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

    n->next = NULL;
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

    n->next = t->next;
    t->next = n;
    n->prev = t;

    *tail = n;
    return true;
}

void lfree(node *head)
{
    if (head == NULL)
    {
        return;
    }

    lfree(head->next);
    free(head);
    return;
}