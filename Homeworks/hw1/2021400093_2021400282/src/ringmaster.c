#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>

#define MAX_SUBJECT_NAMES 64
#define MAX_ITEMS 64
#define MAX_ACTIONS 64
#define MAX_CONDITIONS 64
#define MAX_SENTENCES 128

//Below are the structs that I use to parse and separate sentence to its parts.


//Basically corresponds to Subject in the grammar.
typedef struct {
    char *subjectName;
} SubjectNameStruct;

//Corresponds to Item in the grammar. An item has a quantifier and an identifier. For example 4 apple.
typedef struct{
    int quantifier;
    char* identifier;
} ItemStruct;

//An action consists of subjects (Ahmet and Mahmut), a keyword (buy-from), items (3 apple and 1 pear),
//an othersubject ((from) Necati), and a location (Ankara (used in other keywords))
//Some of these must be present in all actions, some are optional.
//It depends on the keyword.
typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    ItemStruct items[MAX_ITEMS];
    char *otherSubject;
    char *location;
} ActionStruct;


//A condition can consist of a subject names, a keyword, items and a location.
//For example (Ahmet and Mahmut) (has less than) (32 apple and 92 pear)
// or (Ahmet and Necati) (at) (Ankara)
typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    ItemStruct items[MAX_ITEMS];
    char *location;
} ConditionStruct;


//A sentence consists of actions and conditions.
//Actions will be executed only if conditions are true.
typedef struct {
    ActionStruct actions[MAX_ACTIONS];
    ConditionStruct conditions[MAX_CONDITIONS];
} SentenceStruct;

SentenceStruct sentences[MAX_SENTENCES];


//Questions can consist of subjects, keyword, location and item name.
//For example (Frodo) (total) (apple) ?
typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    char *location;
    char *itemName;
} QuestionStruct;

QuestionStruct question;

//A function that resets every single value in the sentences structure.
void resetSentences() {
    for (int i = 0; i < MAX_SENTENCES; i++) { //For every sentence
        for (int j = 0; j < MAX_ACTIONS; j++) { //For every action
            ActionStruct* action = &sentences[i].actions[j];
            for (int k = 0; k < MAX_SUBJECT_NAMES; k++) { //For every subject
                action->subjectNames[k].subjectName = NULL; //reset the name
            }
            action->keyword = 0; //reset the keyword
            for (int k = 0; k < MAX_ITEMS; k++) { //For every item
                action->items[k].quantifier = 0; //reset the quantifier
                action->items[k].identifier = NULL; //reset the identifier
            }
            action->otherSubject = NULL; //reset the other subject
            action->location = NULL; //reset the location
        }
        for (int j = 0; j < MAX_CONDITIONS; j++) { //For every condition
            ConditionStruct* condition = &sentences[i].conditions[j];
            for (int k = 0; k < MAX_SUBJECT_NAMES; k++) { //For every subject
                condition->subjectNames[k].subjectName = NULL; //Reset the name
            }
            condition->keyword = 0; //Reset the keyword
            for (int k = 0; k < MAX_ITEMS; k++) { //For every item
                condition->items[k].quantifier = 0; //Reset the quantifier
                condition->items[k].identifier = NULL; //Reset the identifier
            }
            condition->location = NULL; //Reset the location
        }
    }
}

//Resets the questions similar to reseting the sentences
void resetQuestions() {
    for (int i = 0; i < MAX_SUBJECT_NAMES; i++) {
        question.subjectNames[i].subjectName = NULL;
    }
    question.keyword = 0;
    question.location = NULL;
    question.itemName = NULL;
}

bool checkForDuplicates(SentenceStruct sentence) {
    //When given a sentence structure, this function checks if there are duplicate items or subjects.
    //For example the following sentences contain duplicates
    //Ahmet and Mehmet and Ahmet go to Kayseri
    //Ayşe buy 1 elma and 3 armut and 3 elma
    //Ayşe go to Ankara if Ahmet and Ahmet has less than 3 elma
    //a go to b if c has 3 elma and 3 armut and 5 elma

    // Check within actions for both subject names and item identifiers
    for(int a = 0; a < MAX_ACTIONS; a++) {
        // Check subject names
        for(int i = 0; i < MAX_SUBJECT_NAMES - 1; i++) {
            for(int j = i + 1; j < MAX_SUBJECT_NAMES; j++) {
                if(sentence.actions[a].subjectNames[i].subjectName != NULL &&
                   sentence.actions[a].subjectNames[j].subjectName != NULL &&
                   strcmp(sentence.actions[a].subjectNames[i].subjectName, sentence.actions[a].subjectNames[j].subjectName) == 0) {
                    return true; // Duplicate found
                }
            }
        }
        // Check item identifiers
        for(int i = 0; i < MAX_ITEMS - 1; i++) {
            for(int j = i + 1; j < MAX_ITEMS; j++) {
                if(sentence.actions[a].items[i].identifier != NULL &&
                   sentence.actions[a].items[j].identifier != NULL &&
                   strcmp(sentence.actions[a].items[i].identifier, sentence.actions[a].items[j].identifier) == 0) {
                    return true; // Duplicate found
                }
            }
        }
    }

    // Check within conditions for both subject names and item identifiers
    for(int c = 0; c < MAX_CONDITIONS; c++) {
        // Check subject names
        for(int i = 0; i < MAX_SUBJECT_NAMES - 1; i++) {
            for(int j = i + 1; j < MAX_SUBJECT_NAMES; j++) {
                if(sentence.conditions[c].subjectNames[i].subjectName != NULL &&
                   sentence.conditions[c].subjectNames[j].subjectName != NULL &&
                   strcmp(sentence.conditions[c].subjectNames[i].subjectName, sentence.conditions[c].subjectNames[j].subjectName) == 0) {
                    return true; // Duplicate found
                }
            }
        }
        // Check item identifiers
        for(int i = 0; i < MAX_ITEMS - 1; i++) {
            for(int j = i + 1; j < MAX_ITEMS; j++) {
                if(sentence.conditions[c].items[i].identifier != NULL &&
                   sentence.conditions[c].items[j].identifier != NULL &&
                   strcmp(sentence.conditions[c].items[i].identifier, sentence.conditions[c].items[j].identifier) == 0) {
                    return true; // Duplicate found
                }
            }
        }
    }

    return false; // No duplicates found
}


//This is a structure that is used to store the real data.
//Not related to parsing.
struct Subject{
    char *name;
    char *location;
    int *inventory;
};

//These are the initial capacities
//These capacities are allocated dynamically and doubled once they are full
int subjectCapacity = 16; //Initial capacity of subjects
int currentSubjectAmount = 0; //current subject amount is obviously 0 initially
int itemCapacity = 16; //these ara also similar to above
int currentItemAmount = 0;
int locationCapacity = 16;
int currentLocationAmount = 0;

//here is how I track the objects subjects own.
//items is an array of strings
//for example at one point it can be like
// {"elma", "armut" ,"havuç"}
//in this case subjects have an int* inventory that may look like
// {0,2,7} => this means that subject has 0 elma 2 armut and 7 havuç
char **items;
struct Subject *subjects;
//if char **locations are like the following
//{"ankara", "istanbul", "izmir"}
//and if a subjects location property = 0, it is in ankara. 1 is for istanbul and 2 is for izmir and so on.
char **locations;
//location populations is just a simple int array where an array like {0,2,12} means
//0 people are in ankara, 2 people in istanbul, 12 people in izmir
int *locationPopulations;

int* createNewInventory();
int searchItem(const char*);
void createNewItem(const char*);


void addElementToSubjects(struct Subject subject){
    //checks if the array is full
    //if it is full doubles its capacity using realloc
    //then it adds the subject to the array
    //increases the currentSubjectAmount by 1
    if(currentSubjectAmount == subjectCapacity){
        subjectCapacity *= 2;
        subjects = (struct Subject*)realloc(subjects, subjectCapacity * sizeof(struct Subject));
        if (subjects == NULL){
            perror("realloc error");
            exit(EXIT_FAILURE);
        }
    }

    (subjects)[currentSubjectAmount] = subject;
    (currentSubjectAmount)++;
}

void createNewSubject(char *name){
    //creates a new subject by using the Subject struct
    //adds it to the array of all subjects so that it is accessible later
    struct Subject newSubject = {name, "NOWHERE", createNewInventory() };
    addElementToSubjects(newSubject);
}

int searchSubject(char *name){
    //checks if there is such a subject
    //if it exists returns its index
    //if not returns -1
    for(int i = 0; i < currentSubjectAmount; i++){
        if (strcmp(name, subjects[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int* createNewInventory(){
    //creates a new inventory for a subject
    //used to initialize the inventory of a newly created subject
    //makes the inventory size = itemCapacity
    int *newInventory = (int*)malloc(itemCapacity * sizeof(int));
    if (newInventory == NULL){
        perror("realloc error");
        exit(EXIT_FAILURE);
    }
    return newInventory;
}

void subjectBuyItem(char *name, char *item, int amount){
    //checks if that type of item existed before
    //if not, creates it
    //checks if that subject exists
    //if not, creates it
    //gives the subject "amount" many of that item
    int itemResult = searchItem(item);
    if (itemResult == -1){
        createNewItem(item);
        itemResult = currentItemAmount-1;
    }
    int subjectResult = searchSubject(name);
    if(subjectResult == -1){
        createNewSubject(name);
        subjectResult = currentSubjectAmount-1;
    }
    subjects[subjectResult].inventory[itemResult] += amount;
}

void subjectSellItem(char *name, char *item, int amount){
    //checks if that type of item existed before
    //if not, creates it
    //checks if that subject exists
    //if not, creates it
    //takes from the subject "amount" many of that item if it has enough
    int itemResult = searchItem(item);
    if (itemResult == -1){
        createNewItem(item);
        return;
    }
    int subjectResult = searchSubject(name);
    if(subjectResult == -1){
        createNewSubject(name);
        return;
    }
    if (subjects[subjectResult].inventory[itemResult] >= amount){
        subjects[subjectResult].inventory[itemResult] -= amount;
    }
}

void subjectsBuyItemsFrom(ActionStruct action){

    //checking if the subject that is selling the items is also in the buyers
    //also counting the number of subjects in the meantime
    int numOfSubjects = 0;
    for (int i = 0; action.subjectNames[i].subjectName != NULL ; ++i) {
        numOfSubjects++;
        if (strcmp(action.subjectNames[i].subjectName, action.otherSubject) == 0){
            printf("INVALID");
            return;
        }
    }

    int otherSubjectResult = searchSubject(action.otherSubject);
    if (otherSubjectResult == -1){
        //if the other subject does not exist yet,
        //it surely does not have enough items to give to the buyers
        return;
    }else{
        for (int i = 0; action.items[i].identifier != NULL; ++i) {
            int itemResult = searchItem(action.items[i].identifier);
            if (itemResult == -1){
                //if the item does not exist yet,
                //the seller surely does not have enough of it to give to the buyers
                return;
            }else if (subjects[otherSubjectResult].inventory[itemResult] < numOfSubjects * action.items[i].quantifier){
                //checking if the buyer has enough of each item
                return;
            }
        }
    }

    for (int i = 0; i < numOfSubjects; ++i) {
        for (int j = 0; action.items[j].identifier != NULL; ++j) {
            //doing the sale process
            subjectBuyItem(action.subjectNames[i].subjectName, action.items[j].identifier, action.items[j].quantifier);
            subjectSellItem(action.otherSubject, action.items[j].identifier, action.items[j].quantifier);
        }
    }
}

void subjectsSellItemsTo(ActionStruct action){

    //checking if the buyer is also in sellers
    //also counting the number of subjects at the same time
    int numOfSubjects = 0;
    for (int i = 0; action.subjectNames[i].subjectName != NULL ; ++i) {
        numOfSubjects++;
        if (strcmp(action.subjectNames[i].subjectName, action.otherSubject) == 0){
            printf("INVALID");
            return;
        }

        //if the subject is not present yet, it surely has not enough items to sell
        int subjectResult = searchSubject(action.subjectNames[i].subjectName);
        if (subjectResult == -1){
            return;
        }

        for (int j = 0; action.items[j].identifier != NULL; ++j) {
            int itemResult = searchItem(action.items[j].identifier);
            if (itemResult == -1){ //if the item is not present yet, the sellers surely cannot have enough of them
                return;
            }
            if (subjects[subjectResult].inventory[itemResult] < action.items[j].quantifier){
                //checking if the sellers have enough of the item
                return;
            }
        }
    }

    for (int i = 0; action.subjectNames[i].subjectName != NULL ; ++i) {
        for (int j = 0; action.items[j].identifier != NULL; ++j){
            //doing the sale process
            subjectSellItem(action.subjectNames[i].subjectName, action.items[j].identifier, action.items[j].quantifier);
            subjectBuyItem(action.otherSubject, action.items[j].identifier, action.items[j].quantifier);
        }
    }
}

int searchItem(const char *item){
    //checks if the item exists in the items array
    //if it exists returns its index
    //else returns -1
    for(int i = 0; i < currentItemAmount; i++){
        if (strcmp(item, items[i]) == 0){
            return i;
        }
    }
    return -1;
}

void createNewItem(const char *item){
    //adds a previously non-existing item to items array
    //here non-existing means it didn't even exist in any subject
    //this is the first time the program sees such an item
    if (currentItemAmount == itemCapacity){
        itemCapacity *= 2;
        items = (char**)realloc(items, itemCapacity * sizeof(char));
        if(items == NULL){
            perror("realloc error");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i < currentSubjectAmount; i++){
            subjects[i].inventory = (int*)realloc(subjects[i].inventory, itemCapacity * sizeof(int));
            if (subjects[i].inventory == NULL){
                perror("realloc error");
                exit(EXIT_FAILURE);
            }
            // Initialize the new part of the inventory to 0
            for (int j = currentItemAmount; j < itemCapacity; j++) {
                subjects[i].inventory[j] = 0;
            }
        }
    }

    char *newItem = (char*) malloc(strlen(item) + 1);
    if (newItem == NULL) {
        perror("malloc error for new item");
        exit(EXIT_FAILURE);
    }

    strcpy(newItem, item);

    (items)[currentItemAmount] = newItem;
    currentItemAmount++;
}

void createNewLocation(char *location){
    //adds a previously non-existing location to locations array
    if (currentLocationAmount == locationCapacity){
        locationCapacity*=2;
        locations = (char**) realloc(locations, locationCapacity* sizeof(char));
        //reallocates if not enough space in locations array
        if (locations == NULL){
            perror("realloc error");
            exit(EXIT_FAILURE);
        }
        locationPopulations = (int*) realloc(locationPopulations, locationCapacity * sizeof(int));
        //also updates the location populations array accordingly
        if (locationPopulations == NULL){
            perror("realloc error");
            exit(EXIT_FAILURE);
        }
        for (int i = currentLocationAmount; i < locationCapacity; i++){
            locationPopulations[i] = 0;
        }
    }

    char *newLocation = (char*) malloc(strlen(location) + 1);
    if (newLocation == NULL){
        perror("realloc error");
        exit(EXIT_FAILURE);
    }

    strcpy(newLocation, location);

    (locations)[currentLocationAmount] = newLocation;
    currentLocationAmount++;
}

int findLocationIndex(char *location){
    //finds and returns the index of a location in locations array
    //returns -1 if it can't find it
    for(int i = 0; i < currentLocationAmount; i++){
        if (strcmp(location, locations[i]) == 0){
            return i;
        }
    }
    return -1;
}

void increasePopulationInLocation(char *location){
    //increases population in a location
    int locationIndex = findLocationIndex(location);
    locationPopulations[locationIndex]++;
}

void subjectTravel(char *name, char *location){
    //checks if the subject exists
    //if not, creates it
    //checks if the location exists
    //if not, creates it
    //updates the subject location attribute
    int subjectIndex = searchSubject(name);
    if (subjectIndex == -1){
        createNewSubject(name);
        subjectIndex = currentSubjectAmount-1;
    }
    int locationIndex = findLocationIndex(location);
    if(locationIndex == -1){
        createNewLocation(location);
    }
    subjects[subjectIndex].location = location;
    increasePopulationInLocation(location);
}

//QUESTION FUNCTIONS

int subjectsTotalItem(SubjectNameStruct names[MAX_SUBJECT_NAMES], char *item){

    //returns the amount a subjects have an item of
    int total = 0;
    int itemIndex = searchItem(item);
    if (itemIndex == -1){ //if the item does not exist yet, the subjects surely has 0 of it
        return 0;
    }
    for(int i = 0; i < MAX_SUBJECT_NAMES; i++){
        char *currentName = names[i].subjectName;
        if (currentName == NULL){
            return total;
        }
        int currentSubjectIndex = searchSubject(currentName);
        if (currentSubjectIndex == -1){
            //if that subject does not exist yet, it has 0 of the item, so we don't add anything to total, just skip to the next subject
            continue;
        }
        int amount = subjects[currentSubjectIndex].inventory[itemIndex];
        total += amount;
    }
    return total;
}

char *subjectWhere(char *name){
    //finds and returns the location of the subject
    int subjectIndex = searchSubject(name);
    if (subjectIndex == -1){ //if that subject does not exist yet
        return "NOWHERE";
    }
    return subjects[subjectIndex].location;
}

char **whoAtLocation(char *location){
    //finds the subjects at the given locations and returns an array of strings with the names in it
    char **matchedNames = NULL;
    int currentCount = 0;
    for(int i = 0; i < currentSubjectAmount; i++){
        if (strcmp(subjects[i].location, location) == 0){ //if the location matches
            matchedNames = (char**) realloc(matchedNames, (currentCount+1) * sizeof(char*));
            if (matchedNames == NULL){
                perror("realloc error");
                exit(EXIT_FAILURE);
            }
            matchedNames[currentCount] = subjects[i].name; //write the name to the array
            currentCount++; //increase the count
        }
    }

    //increase the size of the array by 1
    matchedNames = (char**) realloc(matchedNames, (currentCount+1) * sizeof(char*));
    if (matchedNames == NULL){
        perror("realloc error");
        exit(EXIT_FAILURE);
    }

    matchedNames[currentCount] = NULL;

    return matchedNames;
}

int howManyAtLocation(char *location){
    return locationPopulations[findLocationIndex(location)];
}

char** subjectTotal(char *name){
    //returns the items a subject owns
    //example output: {"3 apple", "123412 truck", "1241254124 ring"}
    int amount = 0;
    struct Subject mySubject = subjects[searchSubject(name)];
    //counts how many kinds of item the subject has in its inventory
    for (int i = 0; i < currentItemAmount; i++){
        if (mySubject.inventory[i] != 0){
            amount++;
        }
    }
    //allocates the result array according to the amount
    char** resultArray = (char**) malloc(amount+1 * sizeof(char*));
    int place = 0;
    //adds each item one by one
    for (int i = 0; i < currentItemAmount; i++){
        if (mySubject.inventory[i] !=0){
            resultArray[place] = (char*) malloc(100 * sizeof(char));
            sprintf(resultArray[place], "%d %s", mySubject.inventory[i], items[i]);
            place++;
        }
    }
    resultArray[place] = NULL;

    return resultArray;
}

int sizeOfStringArray(char **array){
    int result = 0;
    for (int i = 0; array[i] != NULL ; ++i) {
        result++;
    }
    return result;
}

//CONDITION FUNCTIONS

bool subjectsAtLocation(SubjectNameStruct names[MAX_SUBJECT_NAMES], char* location){
    //checks the condition of if the given names are in the given location
    for(int i = 0; names[i].subjectName != NULL; i++){
        if (searchSubject(names[i].subjectName) == -1){
            //if a subject does not exist yet, it surely is not in that location
            return false;
        }
        if(strcmp(subjects[searchSubject(names[i].subjectName)].location, location) != 0){
            //one false makes the whole result false
            return false;
        }
    }
    return true;
}

bool subjectsHasItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    //checks the condition of if the given subjects has given items in the given quantities
    for(int i = 0; names[i].subjectName != NULL; i++){ //for every given subject
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){ //for every given item
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

            if (quantifier == 0){
                //if the quantifier is 0, it surely is true
                continue;
            }

            if (searchSubject(names[i].subjectName) == -1){
                //if the subject does not exit yet, it surely does not have the item
                return false;
            }

            if (subjects[searchSubject(names[i].subjectName)].inventory[searchItem(identifier)] != quantifier){
                //checking the real condition
                return false;
            }
        }
    }
    return true;
}

bool subjectsHasLessThanItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    for(int i = 0; names[i].subjectName != NULL; i++){ //for every given subject
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){ //for every given item
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

            if (quantifier == 0){ //it can't be less than 0
                return false;
            }

            if (searchSubject(names[i].subjectName) == -1){ //if the subject does not exist yet, it surely has less than any amount (not 0 obv, but it was handled before)
                continue;
            }

            if (searchItem(identifier) == -1){ //if the item does not exist yet, it the subject surely has 0 of it
                continue;
            }

            if (subjects[searchSubject(names[i].subjectName)].inventory[searchItem(identifier)] >= quantifier){
                //normal comparison
                return false;
            }
        }
    }
    return true;
}

bool subjectsHasMoreThanItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    for(int i = 0; names[i].subjectName != NULL; i++){ //for every given subject
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){ //for every given item
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

            if (searchSubject(names[i].subjectName) == -1){ //if the subject does not exist yet, it cannot have an item more than an amount
                return false;
            }

            if (searchItem(identifier) == -1){ //if the item does not exist yet, it the subject surely has 0 of it
                return false;
            }

            if (subjects[searchSubject(names[i].subjectName)].inventory[searchItem(identifier)] <= quantifier){
                return false;
            }
        }
    }
    return true;
}

//PARSING

enum {
    TOKEN_EXIT,
    TOKEN_END,
    TOKEN_ERROR,
    TOKEN_AND,
    TOKEN_NOWHERE,
    TOKEN_NOTHING,
    TOKEN_NOBODY,
    TOKEN_SELL,
    TOKEN_BUY,
    TOKEN_GO,
    TOKEN_FROM,
    TOKEN_TO,
    TOKEN_AT,
    TOKEN_HAS,
    TOKEN_LESS,
    TOKEN_MORE,
    TOKEN_IF,
    TOKEN_THAN,
    TOKEN_WHERE,
    TOKEN_TOTAL,
    TOKEN_WHO,
    TOKEN_QUESTION_MARK,
    TOKEN_IDENTIFIER,
    TOKEN_QUANTIFIER
};

int peekToken();
void getNextToken();

char *input;
int current_token;

size_t max_length = 1025;

int parseSentences();

int Sentence();
int Actions();
int Action();
int Conditions();
int Condition();
int Subjects(int type);
int Subject(int type);
int Items(int type);
int Item(int type);
int Location(int type);


// Sets the current_token to the next one and pushes the pointer into the end of the token.
void getNextToken() {
    while (isspace(*input)) ++input;
    if (*input == '\0' || *input == '\n') {
        current_token = TOKEN_END;
    } else if (strncmp(input, "exit ", 5) == 0 || strncmp(input, "exit\0", 5) == 0 || strncmp(input, "exit\n", 5) == 0) {
        current_token = TOKEN_EXIT;
        input += 4;
    } else if (strncmp(input, "NOBODY ", 7) == 0 || strncmp(input, "NOBODY\0", 7) == 0 || strncmp(input, "NOBODY\n", 7) == 0) {
        current_token = TOKEN_NOBODY;
        input += 6;
    } else if ((strncmp(input, "NOWHERE ", 8) == 0 || strncmp(input, "NOWHERE\0", 8) == 0 || strncmp(input, "NOWHERE\n", 8) == 0)) {
        current_token = TOKEN_NOWHERE;
        input += 7;
    } else if (strncmp(input, "NOTHING ", 8) == 0 || strncmp(input, "NOTHING\0", 8) == 0 || strncmp(input, "NOTHING\n", 8) == 0) {
        current_token = TOKEN_NOTHING;
        input += 7;
    } else if (strncmp(input, "total ", 6) == 0 || strncmp(input, "total\0", 6) == 0 || strncmp(input, "total\n", 6) == 0) {
        current_token = TOKEN_TOTAL;
        input += 5;
    } else if (strncmp(input, "where ", 6) == 0 || strncmp(input, "where\0", 6) == 0 || strncmp(input, "where\n", 6) == 0) {
        current_token = TOKEN_WHERE;
        input += 5;
    } else if (strncmp(input, "who ", 4) == 0 || strncmp(input, "who\0", 4) == 0 || strncmp(input, "who\n", 4) == 0) {
        current_token = TOKEN_WHO;
        input += 3;
    } else if (strncmp(input, "? ", 2) == 0 || strncmp(input, "?\0", 2) == 0 || strncmp(input, "?\n", 2) == 0) {
        current_token = TOKEN_QUESTION_MARK;
        input += 1;
    } else if (strncmp(input, "and ", 4) == 0 || strncmp(input, "and\0", 4) == 0 || strncmp(input, "and\n", 4) == 0) {
        current_token = TOKEN_AND;
        input += 3;
    } else if (strncmp(input, "if ", 3) == 0 || strncmp(input, "if\0", 3) == 0 || strncmp(input, "if\n", 3) == 0) {
        current_token = TOKEN_IF;
        input += 2;
    } else if (strncmp(input, "buy ", 4) == 0 || strncmp(input, "buy\0", 4) == 0 || strncmp(input, "buy\n", 4) == 0) {
        current_token = TOKEN_BUY;
        input += 3;
    } else if (strncmp(input, "sell ", 5) == 0 || strncmp(input, "sell\0", 5) == 0 || strncmp(input, "sell\n", 5) == 0) {
        current_token = TOKEN_SELL;
        input += 4;
    } else if (strncmp(input, "to ", 3) == 0 || strncmp(input, "to\0", 3) == 0 || strncmp(input, "to\n", 3) == 0) {
        current_token = TOKEN_TO;
        input += 2;
    } else if (strncmp(input, "from ", 5) == 0 || strncmp(input, "from\0", 5) == 0 || strncmp(input, "from\n", 5) == 0) {
        current_token = TOKEN_FROM;
        input += 4;
    } else if (strncmp(input, "at ", 3) == 0 || strncmp(input, "at\0", 3) == 0 || strncmp(input, "at\n", 3) == 0) {
        current_token = TOKEN_AT;
        input += 2;
    } else if (strncmp(input, "go ", 3) == 0 || strncmp(input, "go\0", 3) == 0 || strncmp(input, "go\n", 3) == 0) {
        current_token = TOKEN_GO;
        input += 2;
    } else if (strncmp(input, "has ", 4) == 0 || strncmp(input, "has\0", 4) == 0 || strncmp(input, "has\n", 4) == 0) {
        current_token = TOKEN_HAS;
        input += 3;
    } else if (strncmp(input, "less ", 5) == 0 || strncmp(input, "less\0", 5) == 0 || strncmp(input, "less\n", 5) == 0) {
        current_token = TOKEN_LESS;
        input += 4;
    } else if (strncmp(input, "more ", 5) == 0 || strncmp(input, "more\0", 5) == 0 || strncmp(input, "more\n", 5) == 0) {
        current_token = TOKEN_MORE;
        input += 4;
    } else if (strncmp(input, "than ", 5) == 0 || strncmp(input, "than\0", 5) == 0 || strncmp(input, "than\n", 5) == 0) {
        current_token = TOKEN_THAN;
        input += 4;
    } else if (iswalpha(*input) || *input == '_') {
        current_token = TOKEN_IDENTIFIER;
        while (iswalpha(*input) || *input == '_') ++input;
    } else if (isdigit(*input)) {
        current_token = TOKEN_QUANTIFIER;
        while (isdigit(*input)) ++input;
    } else {
        current_token = TOKEN_ERROR;
    }
}


// Returns the next token, without changing the pointer position.
int peekToken() {
    while (isspace(*input)) ++input;
    if (*input == '\0' || *input == '\n') {
        return TOKEN_END;
    } else if (strncmp(input, "exit ", 5) == 0 || strncmp(input, "exit\0", 5) == 0 || strncmp(input, "exit\n", 5) == 0) {
        return TOKEN_EXIT;
    } else if (strncmp(input, "NOBODY ", 7) == 0 || strncmp(input, "NOBODY\0", 8) == 0 || strncmp(input, "NOBODY\n", 8) == 0) {
        return TOKEN_NOBODY;
    } else if ((strncmp(input, "NOWHERE ", 8) == 0 || strncmp(input, "NOWHERE\0", 8) == 0 || strncmp(input, "NOWHERE\n", 8) == 0)) {
        return TOKEN_NOWHERE;
    } else if (strncmp(input, "NOTHING ", 8) == 0 || strncmp(input, "NOTHING\0", 8) == 0 || strncmp(input, "NOTHING\n", 8) == 0) {
        return TOKEN_NOTHING;
    } else if (strncmp(input, "total ", 6) == 0 || strncmp(input, "total\0", 6) == 0 || strncmp(input, "total\n", 6) == 0) {
        return TOKEN_TOTAL;
    } else if (strncmp(input, "where ", 6) == 0 || strncmp(input, "where\0", 6) == 0 || strncmp(input, "where\n", 6) == 0) {
        return TOKEN_WHERE;
    } else if (strncmp(input, "who ", 4) == 0 || strncmp(input, "who\0", 4) == 0 || strncmp(input, "who\n", 4) == 0) {
        return TOKEN_WHO;
    } else if (strncmp(input, "? ", 2) == 0 || strncmp(input, "?\0", 2) == 0 || strncmp(input, "?\n", 2) == 0) {
        return TOKEN_QUESTION_MARK;
    } else if (strncmp(input, "and ", 4) == 0 || strncmp(input, "and\0", 4) == 0 || strncmp(input, "and\n", 4) == 0) {
        return TOKEN_AND;
    } else if (strncmp(input, "if ", 3) == 0 || strncmp(input, "if\0", 3) == 0 || strncmp(input, "if\n", 3) == 0) {
        return TOKEN_IF;
    } else if (strncmp(input, "buy ", 4) == 0 || strncmp(input, "buy\0", 4) == 0 || strncmp(input, "buy\n", 4) == 0) {
        return TOKEN_BUY;
    } else if (strncmp(input, "sell ", 5) == 0 || strncmp(input, "sell\0", 5) == 0 || strncmp(input, "sell\n", 5) == 0) {
        return TOKEN_SELL;
    } else if (strncmp(input, "to ", 3) == 0 || strncmp(input, "to\0", 3) == 0 || strncmp(input, "to\n", 3) == 0) {
        return TOKEN_TO;
    } else if (strncmp(input, "from ", 5) == 0 || strncmp(input, "from\0", 5) == 0 || strncmp(input, "from\n", 5) == 0) {
        return TOKEN_FROM;
    } else if (strncmp(input, "at ", 3) == 0 || strncmp(input, "at\0", 3) == 0 || strncmp(input, "at\n", 3) == 0) {
        return TOKEN_AT;
    } else if (strncmp(input, "go ", 3) == 0 || strncmp(input, "go\0", 3) == 0 || strncmp(input, "go\n", 3) == 0) {
        return TOKEN_GO;
    } else if (strncmp(input, "has ", 4) == 0 || strncmp(input, "has\0", 4) == 0 || strncmp(input, "has\n", 4) == 0) {
        return TOKEN_HAS;
    } else if (strncmp(input, "less ", 5) == 0 || strncmp(input, "less\0", 5) == 0 || strncmp(input, "less\n", 5) == 0) {
        return TOKEN_LESS;
    } else if (strncmp(input, "more ", 5) == 0 || strncmp(input, "more\0", 5) == 0 || strncmp(input, "more\n", 5) == 0) {
        return TOKEN_MORE;
    } else if (strncmp(input, "than ", 5) == 0 || strncmp(input, "than\0", 5) == 0 || strncmp(input, "than\n", 5) == 0) {
        return TOKEN_THAN;
    } else if (iswalpha(*input) || *input == '_') {
        return TOKEN_IDENTIFIER;
        while (iswalpha(*input) || *input == '_') ++input;
    } else if (isdigit(*input)) {
        return TOKEN_QUANTIFIER;
        while (isdigit(*input)) ++input;
    } else {
        return TOKEN_ERROR;
    }
}

char* getFirstWord(const char* inputStr) {
    // Allocate memory for the first word; +1 for the null terminator
    char* word = (char*)malloc(strlen(inputStr) + 1);

    if (word == NULL) {
        // Allocation failed
        return NULL;
    }

    int i = 0;
    while (inputStr[i] != ' ' && inputStr[i] != '\0' && inputStr[i] != '\n') {
        word[i] = inputStr[i];
        i++;
    }
    word[i] = '\0'; // Null-terminate the string

    return word;
}

// Checks whether the input is the exit command.
int parseExit() {
    if (peekToken() == TOKEN_EXIT) {
        getNextToken();
        if (peekToken() == TOKEN_END) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int questionSubjectNameNo = 0;

// Checks whether the input is a question command and also fills the question attributes meanwhile.
int parseQuestion() {
    // Initialize the counter for the number of subject names in a question
    questionSubjectNameNo = 0;

    // Save the current state of the input and token to possibly revert if parsing fails
    char *backupInput = input;
    int backupToken = current_token;

    // Check if the question starts with the keyword "who"
    if (peekToken() == TOKEN_WHO) {
        question.keyword = TOKEN_WHO;
        getNextToken();
        // Check if the "who" question is about location
        if (peekToken() == TOKEN_AT) {
            getNextToken();
            if (Location(3) == 1) {
                // Check for the closing question mark
                if (peekToken() == TOKEN_QUESTION_MARK) {
                    getNextToken();
                    return 1;  // Successfully parsed a "who at location?" question
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        int success = 0;
        // Attempt to parse a subject sequence for other types of questions
        if (Subjects(3) == 1) {
            questionSubjectNameNo = 0;
            // Check if the question is about the total count of an item
            if (peekToken() == TOKEN_TOTAL) {
                getNextToken();
                if (Item(3) == 1) {
                    question.keyword = TOKEN_TOTAL + TOKEN_IDENTIFIER;
                    // Check for the closing question mark
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        success = 1;
                    }
                }
            }
        }

        if (success == 1) {
            return 1;  // Successfully parsed a "total of an item?" question
        } else {
            // Restore the input and token state if parsing failed
            input = backupInput;
            current_token = backupToken;
            questionSubjectNameNo = 0;

            // Attempt to parse questions about the location or total possessions of a single subject
            if (Subject(3) == 1) {
                if (peekToken() == TOKEN_WHERE) {
                    question.keyword = TOKEN_WHERE;
                    getNextToken();
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        return 1;  // Successfully parsed a "where is subject?" question
                    } else {
                        return 0;
                    }
                } else if (peekToken() == TOKEN_TOTAL) {
                    question.keyword = TOKEN_TOTAL;
                    getNextToken();
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        return 1;  // Successfully parsed a "total possessions of subject?" question
                    } else {
                        return 0;
                    }
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
}


int sentenceNo = 0; // kaçıncı cümlede
int actionNo = 0; // kaçıncı action
int actionSubjectNameNo = 0; // kaçıncı subject
int actionItemsNo = 0; //kaçıncı item
int conditionNo = 0; // kaçıncı condition
int conditionSubjectNameNo = 0; // kaçıncı subject
int conditionItemsNo = 0; // kaçıncı item

// Checks whether the input is a sentence type command and fills all the attributes meanwhile.
int parseSentences() {
    // Try to parse a single sentence using the Sentence() function
    if (Sentence() == 1) {
        sentenceNo++;  // Increment the count of successfully parsed sentences
        // Reset action, condition, and question indices for the new sentence
        actionNo = 0;
        actionSubjectNameNo = 0;
        actionItemsNo = 0;
        conditionNo = 0;
        conditionSubjectNameNo = 0;
        conditionItemsNo = 0;
        questionSubjectNameNo = 0;

        // Check for an 'and' indicating there are multiple sentences to be parsed
        if (peekToken() == TOKEN_AND) {
            getNextToken();  // Consume the 'and' token
            return parseSentences();  // Recursively call to parse subsequent sentences
        } else if (peekToken() == TOKEN_END) {
            getNextToken();  // Consume the end token indicating the end of input
            return 1;  // Return success after all sentences are parsed
        } else {
            return 0;  // If sentences do not terminate properly, return error
        }
    } else {
        return 0;  // If a sentence fails to parse, return error
    }
}


int Sentence() {
    if (Actions() == 1) {
        if (peekToken() == TOKEN_IF) {
            getNextToken();
            return Conditions();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

// Checking whether there is an actions sequence, starting from the current input pointer position.
int Actions() {
    if (Action() == 1) {
        actionNo++;
        actionSubjectNameNo = 0;
        actionItemsNo = 0;

        // Saving the pointer and token position.
        char *backup = input;
        int backup_token = current_token;

        // Checking whether there is potentially additional actions:
        if (peekToken() == TOKEN_AND) {
            getNextToken();
            if (Action() == 1) {
                actionSubjectNameNo = 0;
                actionItemsNo = 0;
                input = backup;
                current_token = backup_token;
                getNextToken();
                return Actions();
            } else {
                actionSubjectNameNo = 0;
                actionItemsNo = 0;
                input = backup;
                current_token = backup_token;
                return 1;
            }
        } else {
            actionSubjectNameNo = 0;
            actionItemsNo = 0;
            return 1;
        }
    } else {
        return 0;
    }
}


// Checking whether there is an action ahead of the current pointer position. Sets the action type, subjects and objects of the action. 
int Action() {
    // First check if there are any subjects specified as part of the action
    if (Subjects(0) == 1) {
        actionSubjectNameNo = 0;  // Reset the subject name counter for this action
        // Check if the next token indicates a 'go' action
        if (peekToken() == TOKEN_GO) {
            getNextToken();  // Consume the 'go' token
            if (peekToken() == TOKEN_TO) {
                getNextToken();  // Consume the 'to' token
                sentences[sentenceNo].actions[actionNo].keyword = TOKEN_GO;  // Set the action keyword to GO
                return Location(0);  // Parse the location to complete the 'go to location' action
            } else {
                return 0;  // If 'to' is not found after 'go', it's a parsing error
            }
        }
            // Check if the action is a 'buy' action
        else if (peekToken() == TOKEN_BUY) {
            sentences[sentenceNo].actions[actionNo].keyword = TOKEN_BUY;  // Set the action keyword to BUY
            getNextToken();  // Consume the 'buy' token
            if (Items(0) == 1) {  // Check if items are specified after 'buy'
                if (peekToken() == TOKEN_FROM) {
                    sentences[sentenceNo].actions[actionNo].keyword = TOKEN_FROM;  // Set the action keyword to BUY FROM
                    getNextToken();  // Consume the 'from' token
                    return Subject(2);  // Parse the subject from which items are bought
                }
                return 1;  // If no 'from' is specified, it is just a buy action without specifying from whom
            } else {
                return 0;  // If no items are specified, it's a parsing error
            }
        }
            // Check if the action is a 'sell' action
        else if (peekToken() == TOKEN_SELL) {
            sentences[sentenceNo].actions[actionNo].keyword = TOKEN_SELL;  // Set the action keyword to SELL
            getNextToken();  // Consume the 'sell' token
            if (Items(0) == 1) {  // Check if items are specified after 'sell'
                if (peekToken() == TOKEN_TO) {
                    sentences[sentenceNo].actions[actionNo].keyword = TOKEN_TO;  // Set the action keyword to SELL TO
                    getNextToken();  // Consume the 'to' token
                    return Subject(2);  // Parse the subject to whom items are sold
                } else {
                    return 1;  // If no 'to' is specified, it is just a sell action without specifying to whom
                }
            } else {
                return 0;  // If no items are specified, it's a parsing error
            }
        }
            // If no actionable token is found after subjects are specified
        else if (peekToken() == TOKEN_END) {
            return 0;  // Indicates parsing should stop if end of input is reached
        }
        return 0;  // General failure case if none of the above actions match
    }
    else {
        return 0;  // Return failure if no subjects are initially parsed
    }
}



int Conditions() {
    if (Condition() == 1) {
        conditionNo++;
        conditionSubjectNameNo = 0;
        conditionItemsNo = 0;

        char *backup = input;
        int backup_token = current_token;


        // Checking whether there can be another condition following. 
        if (peekToken() == TOKEN_AND) {
            getNextToken();
            if (Condition() == 1) {                 // There is at least one condition too. Keep searching recursively.
                conditionSubjectNameNo = 0;
                conditionItemsNo = 0;
                input = backup;
                current_token = backup_token;
                getNextToken();
                return Conditions();
            } else {                                // Although there is an "and" token, no condition following. Take the conditions before, reload the pointer position and return 1.
                conditionSubjectNameNo = 0;
                conditionItemsNo = 0;
                input = backup;
                current_token = backup_token;
                return 1;
            }
        } else {                                    // There can't be any additional condition because there is no "and" token following.
            conditionSubjectNameNo = 0;
            conditionItemsNo = 0;
            return 1;
        }
    } else {
        return 0;
    }
}


// Checking whether there is a condition ahead of the current pointer position. Sets the condition type, subjects and objects of the condition. 
int Condition() {
    if (Subjects(1) == 1) {
        conditionSubjectNameNo = 0;
        if (peekToken() == TOKEN_AT) {
            sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_AT;
            getNextToken();
            return Location(1);
        }

        else if (peekToken() == TOKEN_HAS) {
            sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS;
            getNextToken();
            if (peekToken() == TOKEN_LESS) {
                getNextToken();
                if (peekToken() == TOKEN_THAN) {
                    sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS + TOKEN_LESS;
                    getNextToken();
                    return Items(1);
                } else {
                    return 0;
                }
            }

            else if (peekToken() == TOKEN_MORE) {
                getNextToken();
                if (peekToken() == TOKEN_THAN) {
                    sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS + TOKEN_MORE;
                    getNextToken();
                    return Items(1);
                } else {
                    return 0;
                }
            }

            else if (Items(1) == 1) {
                return 1;
            }

            else {
                return 0;
            }
        } else{
            return 0;
        }
    }

    else {
        return 0;
    }
}

// Checking whether there is a subjects sequence starting from the current pointer position. 
// type0: Subjects control in an Action call
// type1: Subjects control in a Condition call
// type3: Subjects control in a Question call
int Subjects(int type) { 
    if (Subject(type) == 1) {
        if (type == 0){
            actionSubjectNameNo++;
        }else if(type == 1){
            conditionSubjectNameNo++;
        } else if(type == 3){
            questionSubjectNameNo++;
        }
        if (peekToken() == TOKEN_AND) {  
            getNextToken();
            return Subjects(type);
        } else {
            return 1;
        }
    }

    else {
        return 0;
    }
}


// Checking if the next token can be a Subject.
// type0: Subject check in an Action call
// type1: Subject check in a Condition call
// type2: Subject check in an Action call, but those are not the subject of the action. Instead, they are objects (sell from X, buy to Y situations).
// type3: Subject check in a Question call.
int Subject(int type) {
    if (peekToken() == TOKEN_IDENTIFIER) {
        if (type == 0){
            sentences[sentenceNo].actions[actionNo].subjectNames[actionSubjectNameNo].subjectName = getFirstWord(input);
        }else if (type == 1){
            sentences[sentenceNo].conditions[conditionNo].subjectNames[conditionSubjectNameNo].subjectName = getFirstWord(input);
        }else if (type == 2){
            sentences[sentenceNo].actions[actionNo].otherSubject = getFirstWord(input);
        }else if (type == 3) {
            question.subjectNames[questionSubjectNameNo].subjectName = getFirstWord(input);
        }
        getNextToken();
        return 1;
    } else {
        return 0;
    }
}

// Checking if there is an Items sequence following the current pointer position. Types are the same as subjects, except there is no type2.
int Items(int type) {

    if (Item(type) == 1) {
        if (type == 0){
            actionItemsNo++;
        }else if (type == 1){
            conditionItemsNo++;
        }
        char *backup = input;
        int backup_token = current_token;   // saving our current input pointer position

        if (peekToken() == TOKEN_AND) {
            getNextToken();
            if (peekToken() != TOKEN_QUANTIFIER) {  // checking whether the upcoming token is an item
                input = backup;
                current_token = backup_token;                     // if not, reload the original pointer position and return 1
                return 1;
            } else {
                return Items(type);                     // if so, keep checking other items in this sequence.
            }
        } else {
            return 1;
        }
    } else {
        return 0; // Parsing failed
    }
}


// Checking if the next part of the input is an Item. 
int Item(int type) {
    if (type == 3) {
        if (peekToken() == TOKEN_IDENTIFIER) {
            char *inputName = getFirstWord(input);
            question.itemName = inputName;
            getNextToken();
            return 1;
        }
    } else {
        if (peekToken() == TOKEN_QUANTIFIER) {
            char *quantifier = getFirstWord(input);
            getNextToken();
            if (peekToken() == TOKEN_IDENTIFIER) {
                char *identifier = getFirstWord(input);
                if (type == 0){
                    sentences[sentenceNo].actions[actionNo].items[actionItemsNo].quantifier = atoi(quantifier);
                    sentences[sentenceNo].actions[actionNo].items[actionItemsNo].identifier = identifier;
                }else if (type == 1){
                    sentences[sentenceNo].conditions[conditionNo].items[conditionItemsNo].quantifier = atoi(quantifier);
                    sentences[sentenceNo].conditions[conditionNo].items[conditionItemsNo].identifier = identifier;
                }
                getNextToken();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return 0;
}


// Checking if the next part of the input is a Location.
int Location(int type) {
    if (peekToken() == TOKEN_IDENTIFIER) {
        if (type == 0){
            sentences[sentenceNo].actions[actionNo].location = getFirstWord(input);
            getNextToken();
            return 1;
        }else if (type == 1){
            sentences[sentenceNo].conditions[conditionNo].location = getFirstWord(input);
            getNextToken();
            return 1;
        }else if(type == 3){
            question.location = getFirstWord(input);
            getNextToken();
            return 1;
        }
        return 0;
    } else {
        return 0;
    }
}

bool checkConditions(SentenceStruct sentence){
    // Iterate through each condition in the sentence.
    for (int i = 0; sentence.conditions[i].keyword != 0; ++i) {
        // Check if the condition involves subjects being at a specific location.
        if (sentence.conditions[i].keyword == TOKEN_AT){
            if (!subjectsAtLocation(sentence.conditions[i].subjectNames, sentence.conditions[i].location)){
                return false;  // Return false if any subject is not at the specified location.
            }
        }
            // Check if the condition involves subjects possessing specific items.
        else if (sentence.conditions[i].keyword == TOKEN_HAS){
            if (!subjectsHasItems(sentence.conditions[i].subjectNames, sentence.conditions[i].items)){
                return false;  // Return false if subjects do not have the specified items.
            }
        }
            // Check if the condition specifies that subjects have less than a certain quantity of items.
        else if (sentence.conditions[i].keyword == TOKEN_HAS + TOKEN_LESS){
            if (!subjectsHasLessThanItems(sentence.conditions[i].subjectNames, sentence.conditions[i].items)){
                return false;  // Return false if any subject has more than the specified item quantity.
            }
        }
            // Check if the condition specifies that subjects have more than a certain quantity of items.
        else if (sentence.conditions[i].keyword == TOKEN_HAS + TOKEN_MORE){
            if (!subjectsHasMoreThanItems(sentence.conditions[i].subjectNames, sentence.conditions[i].items)){
                return false;  // Return false if any subject has less or equal to the specified item quantity.
            }
        }
    }
    return true;  // Return true if all conditions are met.
}


void executeActions(SentenceStruct sentence){
    // Iterate through each action in the sentence.
    for (int i = 0; sentence.actions[i].keyword != 0; ++i) {
        ActionStruct action = sentence.actions[i];

        // If the action is to buy items, process the buying for each subject and item specified.
        if (action.keyword == TOKEN_BUY) {
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                for (int k = 0; action.items[k].identifier != NULL; ++k) {
                    subjectBuyItem(action.subjectNames[j].subjectName, action.items[k].identifier, action.items[k].quantifier);
                }
            }
        }
            // If the action involves buying items from another subject, call the specific function for this scenario.
        else if (action.keyword == TOKEN_FROM) {
            subjectsBuyItemsFrom(action);
        }
            // If the action is to sell items, ensure that each subject has the items to sell before proceeding.
        else if (action.keyword == TOKEN_SELL) {
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                for (int k = 0; action.items[k].identifier != NULL; ++k) {
                    int subjectResult = searchSubject(action.subjectNames[j].subjectName);
                    if (subjectResult == -1) {
                        return;  // Subject not found, exit the action execution.
                    }
                    int itemResult = searchItem(action.items[k].identifier);
                    if (itemResult == -1) {
                        return;  // Item not found, exit the action execution.
                    }
                    if (subjects[subjectResult].inventory[itemResult] < action.items[k].quantifier) {
                        return;  // Insufficient items to sell, exit the action execution.
                    }
                }
            }
            // After verifying item availability, process the selling action for each subject and item.
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                for (int k = 0; action.items[k].identifier != NULL; ++k) {
                    subjectSellItem(action.subjectNames[j].subjectName, action.items[k].identifier, action.items[k].quantifier);
                }
            }
        }
            // If the action involves selling items to another subject, call the specific function for this scenario.
        else if (action.keyword == TOKEN_TO) {
            subjectsSellItemsTo(action);
        }
            // If the action is to move a subject to a different location, process the travel for each subject specified.
        else if (action.keyword == TOKEN_GO) {
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                subjectTravel(action.subjectNames[j].subjectName, action.location);
            }
        }
    }
}


void executeParsedSentences(){
    // First pass: check for duplicate items or subjects within each parsed sentence.
    for (int j = 0; j < sentenceNo; ++j) {
        SentenceStruct sentence = sentences[j];

        if (checkForDuplicates(sentence)){
            printf("INVALID\n");  // Print "INVALID" if any duplicates are found and stop processing.
            return;
        }
    }

    // Second pass: execute actions for each sentence if conditions are met.
    for (int i = 0; i < sentenceNo; ++i) {
        SentenceStruct sentence = sentences[i];

        // Check if there are any conditions associated with the sentence's actions.
        if (sentence.conditions[0].keyword != 0) { // There is at least one condition
            if (!checkConditions(sentence)) {  // Evaluate conditions, proceed only if true.
                printf("OK\n");  // Print "OK" and stop processing if conditions are not met.
                return;
            }
        }

        // If conditions are met or no conditions exist, execute the actions.
        executeActions(sentence);
    }

    // If all sentences are processed without early termination due to unmet conditions, print "OK".
    printf("OK\n");
}


void executeQuestion() {
    // Handle a query for the total count of an item across specified subjects.
    if (question.keyword == TOKEN_TOTAL + TOKEN_IDENTIFIER) {
        printf("%d\n", subjectsTotalItem(question.subjectNames, question.itemName));
    }
        // Handle a query for the current location of a specific subject.
    else if (question.keyword == TOKEN_WHERE) {
        printf("%s\n", subjectWhere(question.subjectNames[0].subjectName));
    }
        // Handle a query for listing all subjects present at a specific location.
    else if (question.keyword == TOKEN_WHO) {
        char **result = whoAtLocation(question.location);
        if (result[0] == NULL) {
            printf("NOBODY");
        } else {
            printf("%s", result[0]);
        }
        // Loop through the rest of the subjects and format them into a list.
        for (int i = 1; result[i] != NULL; ++i) {
            printf(" and %s", result[i]);
        }
        printf("\n");
    }
        // Handle a query for a summary of all items a subject possesses.
    else if (question.keyword == TOKEN_TOTAL) {
        if (searchSubject(question.subjectNames[0].subjectName) == -1) {
            printf("NOTHING\n");
            return;
        }
        char **result = subjectTotal(question.subjectNames[0].subjectName);
        if (result[0] == NULL) {
            printf("NOTHING");
        } else {
            printf("%s", result[0]);
        }
        // Loop through and format each item in the subject's possession.
        for (int i = 1; result[i] != NULL; ++i) {
            printf(" and %s", result[i]);
        }
        printf("\n");
    }
}


void parseAll() {
    resetSentences();
    resetQuestions();
    sentenceNo = 0;
    char *backupInput = input;
    int backupToken = current_token;
    if (parseExit() == 1) {                    
        exit(0);
    } else if (parseQuestion() == 1) {            // Input is not an exit command. Check if it is a question.
        if (peekToken() == TOKEN_END) {          
            executeQuestion();                    // It is a valid question. Execute the operations accordingly.
        } else {
            printf("INVALID\n");                  // It was supposed to be a question but is not valid. 
        }
    } else {
        input = backupInput;                      // Relocate the pointer back into the start of the input.
        current_token = backupToken;
        if (parseSentences() == 1) {    
            executeParsedSentences();             // It is a valid sentence command. Execute the operations accordingly.
        } else {
            printf("INVALID\n");                  // It is syntatically incorrect. Prints INVALID.
        }
    }
}

int main() {
    char inputStr[1025];  // Buffer for user input.

    // Allocate initial memory for items. Each pointer size assumed to be that of an int by mistake.
    items = (char**)malloc(itemCapacity * sizeof(int));
    if (items == NULL){
        perror("malloc error");  // Print error and exit if memory allocation fails.
        exit(EXIT_FAILURE);
    }

    // Allocate memory for subjects array.
    subjects = (struct Subject*)malloc(subjectCapacity * sizeof(struct Subject));
    if (subjects == NULL){
        perror("malloc error");  // Print error and exit if memory allocation fails.
        exit(EXIT_FAILURE);
    }

    // Allocate memory for location pointers.
    locations = (char**) malloc(locationCapacity * sizeof(char*));
    if (locations == NULL){
        perror("malloc error");  // Print error and exit if memory allocation fails.
        exit(EXIT_FAILURE);
    }

    // Allocate memory for tracking population counts at each location.
    locationPopulations = (int*) malloc(locationCapacity * sizeof(int));
    if (locationPopulations == NULL){
        perror("malloc error");  // Print error and exit if memory allocation fails.
        exit(EXIT_FAILURE);
    }

    // Main loop to keep the program running and processing input.
    while (true){

        // Allocate memory for user input.
        input = (char *) malloc(max_length * sizeof(char));
        if (input == NULL) {
            perror("malloc error");
            exit(EXIT_FAILURE);
        }

        printf(">> ");  // Prompt user for input.
        fflush(stdout);  // Ensure the prompt is displayed immediately.
        if (fgets(input, max_length, stdin) != NULL){  // Read input from user.
            if (parseExit() == 1) {  // Check if the input is an exit command.
                break;  // Exit the loop and terminate the program.
            }else{
                parseAll();  // Parse and process the command.
            }
        }
    }

    // Free allocated memory before exiting the program.
    free(items);
    free(subjects);
    free(locations);
    return 0;
}
