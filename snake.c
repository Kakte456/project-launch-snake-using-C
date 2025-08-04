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
} node;

typedef struct tile
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
void move_head(node *head);
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

    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->direction = true;
    head->axis = true;
    head->next = NULL;

    node *tail = head;

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
        move_head(head);

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

void move_head(node *head)
{
    if (head->axis == true)
    {
        if (head->direction == true)
        {
            head->y++;
        }
        else
        {
            head->y--;
        }
    }
    else
    {
        if (head->direction == true)
        {
            head->x++;
        }
        else
        {
            head->x--;
        }
    }
    return;
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
    if (t->axis)
    {
        if (t->direction)
        {
            n->y = t->y--;
        }
        else
        {
            n->y = t->y++;
        }
        n->x = t->x;
    }
    else
    {
        if (t->direction)
        {
            n->x = t->x--;
        }
        else
        {
            n->x = t->x++;
        }
        n->y = t->y;
    }

    n->next = t->next;
    t->next = n;

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