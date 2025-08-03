#include <stdbool.h>
#include <stdio.h>

typedef struct node
{
    int x;
    int y;
    bool axis;
    bool direction;
    struct node *next;
} node;

const int GRID_SIZE = 25;

bool GRID[GRID_SIZE][GRID_SIZE];

// Prototypes
void update_grid(node *head);
void print_grid(void);

int main(void)
{
    // Default grid setup
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            GRID[i][j] = false;
        }
    }

    // Default head setup
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        return 1;
    }
    head->x = GRID_SIZE / 2;
    head->y = GRID_SIZE / 2;
    head->axis = true;
    head->axis = true;
    head->next = NULL;

    update_grid(head);
    print_grid();

    return 0;
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
    for (int i = 0; i < GRID_SIZE; i++)
    {
        printf("|");
        for (int j = 0; j < GRID_SIZE; j++)
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