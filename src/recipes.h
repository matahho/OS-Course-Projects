#ifndef RECIPES_H
#define RECIPES_H

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>


#define MAX_FOOD_ITEMS 20
#define MAX_INGREDIENTS 10

#define MAX_LEN_NAME 50
#define MAX_INGREDIENT_NUMBER 20

#define MAX_MENU_ITEM 50

typedef struct ingredient {
    char name[MAX_LEN_NAME];
    int quantity;
} Ingredient;

typedef struct food {
    char name[MAX_LEN_NAME];
    Ingredient ingredients[MAX_MENU_ITEM];
    int ingredientCount;
} Food;

char *fileContent();
Food *getKeys(cJSON *object);
Food *parseJson();
int numberOfFoods ();

#endif // RECIPES_H
