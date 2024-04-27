/*
Taylor Winn
Assignment 5
April 2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXLEN 19
#define TABLESIZE 300007

typedef struct item {
    char name[MAXLEN+1];
    int quantity;
    int saleprice;
} item;

typedef struct node {
    item* iPtr;
    struct node* next;
} node;

typedef struct hashtable {
    node** lists;
    int size;
} hashtable;

int hashfunc(char* name, int size) {
    int len = strlen(name);
    int res = 0;
    for (int i=0; i<len; i++){
        res = (1151*res + (name[i]-'a'))%size;
    }
return res;
}

int complexity = 0; // global variable to keep track of the number of operations performed
node * hash_table[TABLESIZE];   // creating hash table of pre-defined size

void initialize_table(){    
    for(int i = 0; i < TABLESIZE; i++){     // initializing the elements within the hash table to NULL
        hash_table[i] = NULL;
    }
}

bool TableInsert(node * item){  // function for inserting a value into the table
    if(item == NULL){
        return false;   // if the item is NULL there is nothing to insert, so we return the boolean value false
    }
    int index = hashfunc(item->iPtr->name, TABLESIZE);  // the index within the table is equivilent to the element's hash value
    item->next = hash_table[index];
    hash_table[index] = item;
    return true;    // returning true since there was a value we were able to assign
}

node * TableSearch(char * item){    // function for finding an element in the table
    int index = hashfunc(item, TABLESIZE);  // retrieving element's hash value
    node * temp = hash_table[index];    // temp value to traverse through the linked list stored at the hash index (if there is one)
    int comp = 0;
    while(temp != NULL && strcmp(temp->iPtr->name, item) != 0){ // standard linked list traversal
        temp = temp->next;
        comp++;
    }  
    complexity += (comp + 1);   // incrimenting the count of operations excecuted
    return temp;    // return the item struct value
}


int main(int argc, char *argv[]){

    initialize_table(); // initializing the table to NULL to make way for our new values

    FILE *file = NULL;  // file pointer
    file = fopen(argv[1], "r"); // command line argument for file
    if(file == NULL){       
        printf("cannot open file"); // tedious condition just in case
        return 1;
    } 

    int numCommands = 0;
    int Funds = 100000;  // initial amount the store has to spend
    fscanf(file, "%d", &numCommands);
    item * NewItem = NULL;  // creating a new item struct value for later use
    
    for(int i = 0; i < numCommands; i++){

        char command[13];   // making space for largest size of command variable (size of change_price + 1)
        char item[MAXLEN + 1];  // creating space for a new item struct value to store read in values
        
        fscanf(file, "%s", command);    
        if(strcmp("buy", command) == 0){    // first we need to read in the command to check which case is going to be executed (since we only have 3)
            int quantity;
            int price;
            fscanf(file, "%s %d %d", item, &quantity, &price);

            node * PreExisting = TableSearch(item); // if the item being searched is already in inventory, add to it. if not, create with read-in values
            if(PreExisting != NULL){
                PreExisting->iPtr->quantity += quantity;
                Funds -= price; // since they are buying merchendise, the money is outgoing. thus, we subtract the price of the good from overall funds
                printf("%s %d %d\n", item, PreExisting->iPtr->quantity, Funds);
            } else {
                NewItem = malloc(sizeof(item)); // creating space for a new item
                strcpy(NewItem->name, item);
                NewItem->quantity = quantity;   // since it is NULL, we can just copy the input values 
                NewItem->saleprice = price;

                node * newNode = malloc(sizeof(node));  // we also need to make space for the new item in both the hash table and the inner linked list
                newNode->iPtr = NewItem;
                newNode->next = NULL;
                TableInsert(newNode);   // inserting into the hash table
                Funds -= price;
                printf("%s %d %d\n", item, NewItem->quantity, Funds);
            }

        } else if (strcmp("sell", command) == 0){   
            int quantity = 0;   
            fscanf(file, "%s %d", item, &quantity);

            node * PreExisting = TableSearch(item);
            
            if(PreExisting->iPtr->quantity >= quantity){    // if there is more supply than demand, we can just decriment the supply
                PreExisting->iPtr->quantity -= quantity;    
                Funds += (PreExisting->iPtr->saleprice * quantity); // since the store is selling, money is coming in. thus, we add to their overall funds
                printf("%s %d %d\n", item, PreExisting->iPtr->quantity, Funds);
                
            } else {    // for the case in which the demand is greater than the supply
                Funds += (PreExisting->iPtr->quantity * PreExisting->iPtr->saleprice);  
                PreExisting->iPtr->quantity = 0;    // you cant sell more than you have (in a perfect scenario), so we just equate supply to 0
                printf("%s %d %d\n", item, PreExisting->iPtr->quantity, Funds);
                
            }

        } else if (strcmp("change_price", command) == 0){
            int price = 0;
            fscanf(file, "%s %d", item, &price);

            node * PreExisting = TableSearch(item); // just finding the index and changing the price
            PreExisting->iPtr->saleprice = price;

        } else {
            printf("\ncommand not recognized"); // default statement
        }

    }

printf("%d\n", Funds);  // printing the final amount of money the store has after all transactions
printf("%d", complexity);

fclose(file);
return 0;

}


