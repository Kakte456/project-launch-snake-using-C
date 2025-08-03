#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int x;
    int y;
    bool axis;
    bool direction;
    struct node *next;
} node;

#define COLUMNS 25
#define ROWS 15

bool GRID[ROWS][COLUMNS];

// Prototypes
void default_grid(void);
void update_grid(node *head);
void print_grid(void);
void point_head(char arrow, node *head);
void move_head(node *head);

int main(void)
{
    // Default head setup
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        return 1;
    }
    head->x = COLUMNS / 2;
    head->y = ROWS / 2;
    head->axis = true;
    head->axis = true;
    head->next = NULL;

    int i = 0;
    while (i < 10)
    {
        // Default grid setup
        default_grid();
        // Updete grip with snake positions
        update_grid(head);
        // Print grid and snake
        print_grid();

        char cursor;
        do
        {
            printf("(Up: R | Down: C | ->: F | <-: D) : ");
            scanf("%c", &cursor);
            cursor = toupper(cursor);
        }
        while (cursor != 'R' && cursor != 'C' && cursor != 'F' && cursor != 'D');

        point_head(cursor, head);
        move_head(head);
        i++;
    }

    return 0;
}
void default_grid(void)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            GRID[i][j] = false;
        }
    }
}
void update_grid(node *head)
{
    for (node *ptr = head; ptr != NULL; ptr = ptr->next)
    {
        GRID[ptr->y][ptr->x] = true;
    }
    return;
}

void print_grid(void)
{
    for (int i = 0; i < ROWS; i++)
    {
        printf("|");
        for (int j = 0; j < COLUMNS; j++)
        {
            if (GRID[i][j] == true)
            {
                printf("o");
            }
            else
            {
                printf(" ");
            }
        }
        printf("|\n");
    }
}

void point_head(char arrow, node *head)
{
    if (arrow == 'R' || arrow == 'F')
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
}