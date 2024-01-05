#include "recipes.h"
#define MAX_LEN_NAME 50
#define MAX_INGREDIENT_NUMBER 20
#define MAX_MENU_ITEM 50




char* fileContent (){
     int fd = open("recipes.json", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    // Find the file size
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (!file_content) {
        perror("Memory allocation failed");
        close(fd);
        return NULL;
    }

    ssize_t bytes_read = read(fd, file_content, file_size);
    if (bytes_read == -1) {
        perror("Error reading file");
        free(file_content);
        close(fd);
        return NULL;
    }

    file_content[file_size] = '\0'; 
    close(fd); 
    return file_content;

}

Food* getKeys(cJSON *object) {
    Food* menu = (Food*)malloc(MAX_MENU_ITEM * sizeof(Food));
    cJSON *current = object->child;

    
    int foodCounter =0 ;
    while (current != NULL) {


        int ingCounter = 0 ;
        cJSON *ch = current->child;
        Ingredient *ings = (Ingredient*)malloc(MAX_MENU_ITEM * sizeof(Ingredient));
        while (ch != NULL){
            sprintf(ings[ingCounter].name , strdup(ch->string) , MAX_LEN_NAME);
            ings[ingCounter].quantity = ch-> valueint ;
            ch = ch->next;
            ingCounter += 1;
        }

        
        Food f ;
        sprintf(f.name , strdup(current->string), MAX_LEN_NAME );\
        for (int i =0 ; i < MAX_INGREDIENT_NUMBER ; i++){
            f.ingredients[i] = ings[i];

        }
        f.ingredientCount =  ingCounter;
        menu[foodCounter] = f;
        foodCounter += 1 ;
        current = current->next;
    } 

    return menu;
}


Food* parseJson (){
    char* text = fileContent();
    Food* menu = (Food*)malloc(MAX_MENU_ITEM * sizeof(Food));
    int food_count = 0;

    cJSON *json = cJSON_Parse(text); 
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        return NULL; 
    }     


    
    return getKeys(cJSON_Parse(text));

}


int numberOfFoods (){
    cJSON *object = cJSON_Parse(fileContent());
    cJSON *current = object->child;
    int n = 0 ;
    while (current != NULL) {
        n +=1 ;
        current = current->next;
    }
    return n ;

}
