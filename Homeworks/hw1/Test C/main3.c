#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SUBJECT_NAMES 32
#define MAX_ITEMS 32
#define MAX_ACTIONS 16
#define MAX_CONDITIONS 16
#define MAX_SENTENCES 32

typedef struct {
    char *subjectName;
} SubjectNameStruct;

typedef struct{
    int quantifier;
    char* identifier;
} ItemStruct;

typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    ItemStruct items[MAX_ITEMS];
    char *otherSubject;
    char *location;
} ActionStruct;

typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    ItemStruct items[MAX_ITEMS];
    char *location;
} ConditionStruct;

typedef struct {
    ActionStruct actions[MAX_ACTIONS];
    ConditionStruct conditions[MAX_CONDITIONS];
} SentenceStruct;

SentenceStruct sentences[MAX_SENTENCES];

typedef struct {
    SubjectNameStruct subjectNames[MAX_SUBJECT_NAMES];
    int keyword;
    char *location;
    char *itemName;
} QuestionStruct;

QuestionStruct question;

void resetSentences() {
    for (int i = 0; i < MAX_SENTENCES; i++) {
        for (int j = 0; j < MAX_ACTIONS; j++) {
            ActionStruct* action = &sentences[i].actions[j];
            for (int k = 0; k < MAX_SUBJECT_NAMES; k++) {
                action->subjectNames[k].subjectName = NULL;
            }
            action->keyword = 0;
            for (int k = 0; k < MAX_ITEMS; k++) {
                action->items[k].quantifier = 0;
                action->items[k].identifier = NULL;
            }
            action->otherSubject = NULL;
            action->location = NULL;
        }
        for (int j = 0; j < MAX_CONDITIONS; j++) {
            ConditionStruct* condition = &sentences[i].conditions[j];
            for (int k = 0; k < MAX_SUBJECT_NAMES; k++) {
                condition->subjectNames[k].subjectName = NULL;
            }
            condition->keyword = 0;
            for (int k = 0; k < MAX_ITEMS; k++) {
                condition->items[k].quantifier = 0;
                condition->items[k].identifier = NULL;
            }
            condition->location = NULL;
        }
    }
}

struct Subject{
    char *name;
    char *location;
    //int RowNumber;
    int *inventory;
};

int subjectCapacity = 16;
int currentSubjectAmount = 0;
int itemCapacity = 16;
int currentItemAmount = 0;
int locationCapacity = 16;
int currentLocationAmount = 0;

char **items;
struct Subject *subjects;
char **locations;
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
        itemResult = currentItemAmount-1;
        return;
    }
    int subjectResult = searchSubject(name);
    if(subjectResult == -1){
        createNewSubject(name);
        subjectResult = currentSubjectAmount-1;
        return;
    }
    if (subjects[subjectResult].inventory[itemResult] >= amount){
        subjects[subjectResult].inventory[itemResult] -= amount;
    }
}

void subjectsBuyItemsFrom(ActionStruct action){
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
        return;
    }else{
        for (int i = 0; action.items[i].identifier != NULL; ++i) {
            int itemResult = searchItem(action.items[i].identifier);
            if (itemResult == -1){
                return;
            }else if (subjects[otherSubjectResult].inventory[itemResult] < numOfSubjects * action.items[i].quantifier){
                return;
            }
        }
    }

    for (int i = 0; i < numOfSubjects; ++i) {
        for (int j = 0; action.items[j].identifier != NULL; ++j) {
            subjectBuyItem(action.subjectNames[i].subjectName, action.items[j].identifier, action.items[j].quantifier);
            subjectSellItem(action.otherSubject, action.items[j].identifier, action.items[j].quantifier);
        }
    }
}

void subjectsSellItemsTo(ActionStruct action){
    int numOfSubjects = 0;
    for (int i = 0; action.subjectNames[i].subjectName != NULL ; ++i) {
        numOfSubjects++;
        if (strcmp(action.subjectNames[i].subjectName, action.otherSubject) == 0){
            printf("INVALID");
            return;
        }
        int subjectResult = searchSubject(action.subjectNames[i].subjectName);
        if (subjectResult == -1){
            return;
        }

        for (int i = 0; action.items[i].identifier != NULL; ++i) {
            int itemResult = searchItem(action.items[i].identifier);
            if (itemResult == -1){
                return;
            }
            if (subjects[subjectResult].inventory[itemResult] < action.items[i].quantifier){
                return;
            }
        }
    }

    for (int i = 0; action.subjectNames[i].subjectName != NULL ; ++i) {
        for (int j = 0; action.items[j].identifier != NULL; ++j){
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

/*int subjectsTotalItem(char **names, int numberOfNames, char *item){
    int total = 0;
    int itemIndex = searchItem(item);
    for(int i = 0; i < numberOfNames; i++){
        char *currentName = names[i];
        int currentSubjectIndex = searchSubject(currentName);
        int amount = subjects[currentSubjectIndex].inventory[itemIndex];
        total += amount;
    }
    return total;
}*/

int subjectsTotalItem(SubjectNameStruct names[MAX_SUBJECT_NAMES] ,char *item){
    int total = 0;
    int itemIndex = searchItem(item);
    if (itemIndex == -1){
        return 0;
    }
    for(int i = 0; i < MAX_SUBJECT_NAMES; i++){
        char *currentName = names[i].subjectName;
        int currentSubjectIndex = searchSubject(currentName);
        int amount = subjects[currentSubjectIndex].inventory[itemIndex];
        total += amount;
    }
    return total;
}

char *subjectWhere(char *name){
    int subjectIndex = searchSubject(name);
    return subjects[subjectIndex].location;
}

char **whoAtLocation(char *location){
    char **matchedNames = NULL;
    int currentCount = 0;
    for(int i = 0; i < currentSubjectAmount; i++){
        if (strcmp(subjects[i].location, location) == 0){
            matchedNames = (char**) realloc(matchedNames, (currentCount+1) * sizeof(char*));
            if (matchedNames == NULL){
                perror("realloc error");
                exit(EXIT_FAILURE);
            }
            matchedNames[currentCount] = subjects[i].name;
            currentCount++;
        }
    }

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
    int amount = 0;
    struct Subject mySubject = subjects[searchSubject(name)];
    for (int i = 0; i < currentItemAmount; i++){
        if (mySubject.inventory[i] != 0){
            amount++;
        }
    }
    char** resultArray = (char**) malloc(amount+1 * sizeof(char*));
    int place = 0;
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

/*bool subjectsAtLocation(char **names, char* location){
    for(int i = 0; names[i] != NULL; i++){
        if(strcmp(subjects[searchSubject(names[i])].location, location) != 0){
            return false;
        }
    }
    return true;
}*/

bool subjectsAtLocation(SubjectNameStruct names[MAX_SUBJECT_NAMES], char* location){
    for(int i = 0; names[i].subjectName != NULL; i++){
        if(strcmp(subjects[searchSubject(names[i].subjectName)].location, location) != 0){
            return false;
        }
    }
    return true;
}

/*bool subjectsHasItems(char **names, char **itemNames){
    for(int i = 0; names[i] != NULL; i++){
        for(int j = 0; itemNames[j] != NULL; j++){
            long quantity;
            char *item;

            quantity = strtol(itemNames[j], &item, 10);

            while (*item == ' ') item++;

            if (subjects[searchSubject(names[i])].inventory[searchItem(item)] != quantity){
                return false;
            }
        }
    }
    return true;
}*/

bool subjectsHasItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    for(int i = 0; names[i].subjectName != NULL; i++){
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

            if (subjects[searchSubject(names[i].subjectName)].inventory[searchItem(identifier)] != quantifier){
                return false;
            }
        }
    }
    return true;
}

/*bool subjectsHasLessThanItems(char** names, char**itemNames){
    for(int i = 0; names[i] != NULL; i++){
        for(int j = 0; itemNames[j] != NULL; j++){
            long quantity;
            char *item;

            quantity = strtol(itemNames[j], &item, 10);

            while (*item == ' ') item++;

            if (subjects[searchSubject(names[i])].inventory[searchItem(item)] >= quantity){
                return false;
            }
        }
    }
    return true;
}*/

bool subjectsHasLessThanItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    for(int i = 0; names[i].subjectName != NULL; i++){
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

            if (subjects[searchSubject(names[i].subjectName)].inventory[searchItem(identifier)] >= quantifier){
                return false;
            }
        }
    }
    return true;
}

/*bool subjectsHasMoreThanItems(char** names, char**itemNames){
    for(int i = 0; names[i] != NULL; i++){
        for(int j = 0; itemNames[j] != NULL; j++){
            long quantity;
            char *item;

            quantity = strtol(itemNames[j], &item, 10);

            while (*item == ' ') item++;

            if (subjects[searchSubject(names[i])].inventory[searchItem(item)] <= quantity){
                return false;
            }
        }
    }
    return true;
}*/

bool subjectsHasMoreThanItems(SubjectNameStruct names[MAX_SUBJECT_NAMES], ItemStruct itemsStruct[MAX_ITEMS]){
    for(int i = 0; names[i].subjectName != NULL; i++){
        for(int j = 0; itemsStruct[j].identifier != NULL; j++){
            long quantifier = itemsStruct[j].quantifier;
            char *identifier = itemsStruct[j].identifier;

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
    TOKEN_SELL,
    TOKEN_BUY,
    TOKEN_GOTO,
    TOKEN_FROM,
    TOKEN_TO,
    TOKEN_AT,
    TOKEN_HAS,
    TOKEN_LESS_THAN,
    TOKEN_MORE_THAN,
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

size_t max_length = 1024;

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

void getNextToken() {

    while (isspace(*input)) ++input;
    if (*input == '\0') {
        current_token = TOKEN_END;
    } else if (strncmp(input, "exit", 4) == 0) {
        current_token = TOKEN_EXIT;
        input += 4;
    } else if (strncmp(input, "total", 5) == 0) {
        current_token = TOKEN_TOTAL;
        input += 5;
    } else if (strncmp(input, "where", 5) == 0) {
        current_token = TOKEN_WHERE;
        input += 5;
    } else if (strncmp(input, "Who", 3) == 0) {
        current_token = TOKEN_WHO;
        input += 3;
    } else if (strncmp(input, "?", 1) == 0) {
        current_token = TOKEN_QUESTION_MARK;
        input += 1;
    } else if (strncmp(input, "and", 3) == 0) {
        current_token = TOKEN_AND;
        input += 3;
    } else if (strncmp(input, "if", 2) == 0) {
        current_token = TOKEN_IF;
        input += 2;
    } else if (strncmp(input, "buy", 3) == 0) {
        current_token = TOKEN_BUY;
        input += 3;
    } else if (strncmp(input, "sell", 4) == 0) {
        current_token = TOKEN_SELL;
        input += 4;
    } else if (strncmp(input, "to", 2) == 0) {
        current_token = TOKEN_TO;
        input += 2;
    } else if (strncmp(input, "from", 4) == 0) {
        current_token = TOKEN_FROM;
        input += 4;
    } else if (strncmp(input, "at", 2) == 0) {
        current_token = TOKEN_AT;
        input += 2;
    } else if (strncmp(input, "go to", 5) == 0) {
        current_token = TOKEN_GOTO;
        input += 5;
    } else if (strncmp(input, "has", 3) == 0) {
        current_token = TOKEN_HAS;
        input += 3;
    } else if (strncmp(input, "less than", 9) == 0) {
        current_token = TOKEN_LESS_THAN;
        input += 9;
    } else if (strncmp(input, "more than", 9) == 0) {
        current_token = TOKEN_MORE_THAN;
        input += 9;
    } else if (isalpha(*input) || *input == '_') {
        current_token = TOKEN_IDENTIFIER;
        while (isalpha(*input) || *input == '_') ++input;
    } else if (isdigit(*input)) {
        current_token = TOKEN_QUANTIFIER;
        while (isdigit(*input)) ++input;
    } else {
        current_token = TOKEN_ERROR;
    }
}

int peekToken() {
    while (isspace(*input)) ++input;
    if (*input == '\0') {
        return TOKEN_END;
    } else if (strncmp(input, "exit", 4) == 0) {
        return TOKEN_EXIT;
    } else if (strncmp(input, "total", 5) == 0) {
        return TOKEN_TOTAL;
    } else if (strncmp(input, "where", 5) == 0) {
        return TOKEN_WHERE;
    } else if (strncmp(input, "Who", 3) == 0) {
        return TOKEN_WHO;
    } else if (strncmp(input, "?", 1) == 0) {
        return TOKEN_QUESTION_MARK;
    } else if (strncmp(input, "and", 3) == 0) {
        return TOKEN_AND;
    } else if (strncmp(input, "if", 2) == 0) {
        return TOKEN_IF;
    } else if (strncmp(input, "buy", 3) == 0) {
        return TOKEN_BUY;
    } else if (strncmp(input, "sell", 4) == 0) {
        return TOKEN_SELL;
    } else if (strncmp(input, "to", 2) == 0) {
        return TOKEN_TO;
    } else if (strncmp(input, "from", 4) == 0) {
        return TOKEN_FROM;
    } else if (strncmp(input, "at", 2) == 0) {
        return TOKEN_AT;
    } else if (strncmp(input, "go to", 5) == 0) {
        return TOKEN_GOTO;
    } else if (strncmp(input, "has", 3) == 0) {
        return TOKEN_HAS;
    } else if (strncmp(input, "less than", 9) == 0) {
        return TOKEN_LESS_THAN;
    } else if (strncmp(input, "more than", 9) == 0) {
        return TOKEN_MORE_THAN;
    } else if (isalpha(*input) || *input == '_') {
        return TOKEN_IDENTIFIER;
        while (isalpha(*input) || *input == '_') ++input;
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

int parseQuestion() {

    char *backupInput = input;
    int backupToken = current_token;

    if (peekToken() == TOKEN_WHO) {
        question.keyword = TOKEN_WHO;
        getNextToken();
        if (peekToken() == TOKEN_AT) {
            getNextToken();
            if (Location(3) == 1) {
                if (peekToken() == TOKEN_QUESTION_MARK) {
                    getNextToken();
                    return 1;
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

    else {
        int success = 0;
        if (Subjects(3) == 1) {
            if (peekToken() == TOKEN_TOTAL) {
                getNextToken();
                if (Item(3) == 1) {
                    question.keyword = TOKEN_TOTAL + TOKEN_IDENTIFIER;
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        success = 1;
                    }
                }
            }
        }

        if (success == 1) {
            return 1;
        }

        else {
            input = backupInput;
            current_token = backupToken;

            if (Subject(3) == 1) {
                if (peekToken() == TOKEN_WHERE) {
                    question.keyword = TOKEN_WHERE;
                    getNextToken();
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        return 1;
                    } else {
                        return 0;
                    }
                }

                else if (peekToken() == TOKEN_TOTAL) {
                    question.keyword = TOKEN_TOTAL;
                    getNextToken();
                    if (peekToken() == TOKEN_QUESTION_MARK) {
                        getNextToken();
                        return 1;
                    } else if (Item(3) == 1) {
                        question.keyword = TOKEN_TOTAL + TOKEN_IDENTIFIER;
                        if (peekToken() == TOKEN_QUESTION_MARK) {
                            getNextToken();
                            return 1;
                        } else {
                            return 0;
                        }
                    } else {
                        return 0;
                    }
                }

                else {
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

int parseSentences() {
    if (Sentence() == 1) {
        sentenceNo++;
        actionNo = 0;
        actionSubjectNameNo = 0;
        actionItemsNo = 0;
        conditionNo = 0;
        conditionSubjectNameNo = 0;
        conditionItemsNo = 0;
        if (peekToken() == TOKEN_AND) {
            getNextToken();
            return parseSentences();
        } else if (peekToken() == TOKEN_END) {
            getNextToken();
            return 1;
        } else{
            return 0;
        }
    } else {
        return 0;
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

int Actions() {
    if (Action() == 1) {
        actionNo++;
        char *backup = input;
        int backup_token = current_token;

        if (peekToken() == TOKEN_AND) {
            getNextToken();
            if (Action() == 1) {
                input = backup;
                current_token = backup_token;
                getNextToken();
                return Actions();
            } else {
                input = backup;
                current_token = backup_token;
                return 1;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int Action() {
    if (Subjects(0) == 1) {


        if (peekToken() == TOKEN_GOTO) {
            sentences[sentenceNo].actions[actionNo].keyword = TOKEN_GOTO;
            getNextToken();
            return Location(0);
        }

        else if (peekToken() == TOKEN_BUY) {
            sentences[sentenceNo].actions[actionNo].keyword = TOKEN_BUY;
            getNextToken();
            if (Items(0) == 1) {
                if (peekToken() == TOKEN_FROM) {
                    sentences[sentenceNo].actions[actionNo].keyword = TOKEN_FROM;
                    getNextToken();
                    return Subject(2);
                }

                return 1;

            } else {
                return 0;
            }
        }

        else if (peekToken() == TOKEN_SELL) {
            sentences[sentenceNo].actions[actionNo].keyword = TOKEN_SELL;
            getNextToken();

            if (Items(0) == 1) {
                if (peekToken() == TOKEN_TO) {
                    sentences[sentenceNo].actions[actionNo].keyword = TOKEN_TO;
                    getNextToken();
                    return Subject(2);
                } else {
                    return 1;
                }
            }

            else {
                return 0;
            }
        }

        else if (peekToken() == TOKEN_END) {
            return 0;
        }
    }

    else {
        return 0;
    }
}

int Conditions() {
    if (Condition() == 1) {
        conditionNo++;

        char *backup = input;
        int backup_token = current_token;

        if (peekToken() == TOKEN_AND) {
            getNextToken();
            if (Condition() == 1) {
                input = backup;
                current_token = backup_token;
                getNextToken();
                return Conditions();
            } else {
                input = backup;
                current_token = backup_token;
                return 1;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int Condition() {
    if (Subjects(1) == 1) {
        if (peekToken() == TOKEN_AT) {
            sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_AT;
            getNextToken();
            return Location(1);
        }

        else if (peekToken() == TOKEN_HAS) {
            sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS;
            getNextToken();
            if (peekToken() == TOKEN_LESS_THAN) {
                sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS + TOKEN_LESS_THAN;
                getNextToken();
                return Items(1);
            }

            else if (peekToken() == TOKEN_MORE_THAN) {
                sentences[sentenceNo].conditions[conditionNo].keyword = TOKEN_HAS + TOKEN_LESS_THAN;
                getNextToken();
                return Items(1);
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

int Subjects(int type) { //0 action 1 condition 3 question
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

int Subject(int type) { //0 action 1 condition 2 otherSubject 3 question
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

int Item(int type) {
    if (type == 3) {
        if (peekToken() == TOKEN_IDENTIFIER) {
            char *inputName = getFirstWord(input);
            question.itemName = inputName;
            getNextToken();
            if (peekToken() != TOKEN_END) {
                return 0;
            } else {
                return 1;
            }
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
}

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
            if (peekToken() == TOKEN_END) {
                return 1;
            } else {
                return 0;
            }
        }
    } else {
        return 0;
    }
}

bool checkConditions(SentenceStruct sentence){
    for (int i = 0; sentence.conditions[i].keyword != 0; ++i) {
        if (sentence.conditions[conditionNo].keyword == TOKEN_AT){
            return subjectsAtLocation(sentence.conditions[conditionNo].subjectNames, sentence.conditions[conditionNo].location);
        }else if (sentence.conditions[conditionNo].keyword == TOKEN_HAS){
            return subjectsHasItems(sentence.conditions[conditionNo].subjectNames, sentence.conditions[conditionNo].items);
        }else if (sentence.conditions[conditionNo].keyword == TOKEN_HAS + TOKEN_LESS_THAN){
            return subjectsHasLessThanItems(sentence.conditions[conditionNo].subjectNames, sentence.conditions[conditionNo].items);
        }else if (sentence.conditions[conditionNo].keyword == TOKEN_HAS + TOKEN_MORE_THAN){
            return subjectsHasMoreThanItems(sentence.conditions[conditionNo].subjectNames, sentence.conditions[conditionNo].items);
        }
    }
}

void executeActions(SentenceStruct sentence){
    for (int i = 0; sentence.actions[i].keyword != 0; ++i) {
        ActionStruct action = sentence.actions[i];
        if (action.keyword == TOKEN_BUY){
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                for (int k = 0; action.items[k].identifier != NULL ; ++k) {
                    subjectBuyItem(action.subjectNames[j].subjectName, action.items[k].identifier, action.items[k].quantifier);
                }
            }
        }else if (action.keyword == TOKEN_FROM){
            subjectsBuyItemsFrom(action);
        }else if (action.keyword == TOKEN_SELL){
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                for (int k = 0; action.items[k].identifier != NULL ; ++k) {
                    subjectSellItem(action.subjectNames[j].subjectName, action.items[k].identifier, action.items[k].quantifier);
                }
            }
        }else if (action.keyword == TOKEN_TO){
            subjectsSellItemsTo(action);
        }else if (action.keyword == TOKEN_GOTO){
            for (int j = 0; action.subjectNames[j].subjectName != NULL; ++j) {
                subjectTravel(action.subjectNames[j].subjectName, action.location);
            }
        }
    }
}

void executeParsedSentences(){
    for (int i = 0; i < sentenceNo; ++i) {
        SentenceStruct sentence = sentences[i];
        //check the condition of the action
        if (sentence.conditions[0].keyword != 0){ //the action has a condition
            if (!checkConditions(sentence)){
                return;
            }
        }
        executeActions(sentence);
    }
}

void executeQuestion(){
    if (question.keyword == TOKEN_TOTAL + TOKEN_IDENTIFIER){
        printf("%d\n", subjectsTotalItem(question.subjectNames, question.itemName));
    }else if (question.keyword == TOKEN_WHERE){
        printf("%s\n", subjectWhere(question.subjectNames[0].subjectName));
    }else if (question.keyword == TOKEN_WHO){
        char **result = whoAtLocation(question.location);
        if (result[0] == NULL){
            printf("NOBODY");
        }else{
            printf("%s", result[0]);
        }
        for (int i = 1; result[i] != NULL ; ++i) {
            printf(" and %s", result[i]);
        }
        printf("\n");
    }else if (question.keyword == TOKEN_TOTAL){
        char **result = subjectTotal(question.subjectNames[0].subjectName);
        if (result[0] == NULL){
            printf("NOBODY");
        }else{
            printf("%s", result[0]);
        }
        for (int i = 1; result[i] != NULL ; ++i) {
            printf(" and %s", result[i]);
        }
        printf("\n");
    }
}

int parse(){
    resetSentences();
    sentenceNo = 0;
    int result = parseSentences();
    if (result == 1){
        executeParsedSentences();
    }
    return result;
}

void parseAll() {
    resetSentences();
    sentenceNo = 0;
    char *backupInput = input;
    int backupToken = current_token;
    if (parseExit() == 1) {
        exit(0);
    } else if (parseQuestion() == 1) {
        executeQuestion();
    } else {
        input = backupInput;
        current_token = backupToken;
        if (parseSentences() == 1) {
            executeParsedSentences();
        } else {
            printf("INVALID!\n");
        }
    }
}

int main() {
    char inputStr[1024];

    items = (char**)malloc(itemCapacity * sizeof(int));
    if (items == NULL){
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    subjects = (struct Subject*)malloc(subjectCapacity * sizeof(struct Subject));
    if (subjects == NULL){
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    locations = (char**) malloc(locationCapacity * sizeof(char*));
    if (locations == NULL){
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    locationPopulations = (int*) malloc(locationCapacity * sizeof(int));
    if (locationPopulations == NULL){
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    while (true){

        input = (char *) malloc(max_length * sizeof(char));
        if (input == NULL) {
            printf("malloc error");
            exit(EXIT_FAILURE);
        }

        printf(">> ");
        if (fgets(input, max_length, stdin) != NULL){
            if (parseExit() == 1) {
                break;
            }else{
                /*if (parseSentences() == 1) {
                    printf("VALID!\n");
                } else {
                    printf("INVALID!\n");
                }*/

                parseAll();

                printf("%s\n",subjectWhere("Ahmet"));

                for(int i = 0; i < currentSubjectAmount; i++){
                    struct Subject mySubject = subjects[i];
                    printf("Subject %d is: %s\n", i ,mySubject.name);
                    for(int j = 0; j < currentItemAmount; j++){
                        printf("He has %d of item %d\n", mySubject.inventory[j], j);
                    }
                }

                //parse();
            }
        }
    }

    //
    //
    //TEST KISMI BURANIN AŞAĞISI TAMAMEN TEST ETMEK İÇİN
    //
    //

    char *everybody[] = {"Ahmet", "Mahmut", NULL};

    /*createNewSubject("Ahmet");
    createNewSubject("Mahmut");
    subjectBuyItem("Ahmet", "Çorap", 3);
    subjectBuyItem("Mahmut", "Çorap", 1);
    subjectBuyItem("Mahmut", "Ayakkabı", 7);

    subjectSellItem("Mahmut", "Ayakkabı", 5);*/

    //printf("Total number of that item: %d\n", subjectsTotalItem(everybody, 2, "Ayakkabı"));

    /*subjectTravel("Ahmet", "Kayseri");
    subjectTravel("Mahmut", "Kayseri");
    subjectTravel("Necati", "Kayseri");
    subjectTravel("Tahsin", "Kayseri");*/

    /*printf("%s\n",subjectWhere("Necati"));

    printf("%d\n", howManyAtLocation("Kayseri"));

    for (int i = 0; i < howManyAtLocation("Kayseri"); i++) {
        printf("%s\n", whoAtLocation("Kayseri")[i]);
    }*/

    /*char **mysubjtotal = subjectTotal("Mahmut");
    for (int i = 0; mysubjtotal[i] != NULL; ++i) {
        printf("%s\n", mysubjtotal[i]);
    }*/

    //subjectTravel("Mahmut", "Anamur");

    /*if (subjectsAtLocation(everybody, "Kayseri")){
        printf("e\n");
    }*/

    char* itemss[] = {"2 Çorap", NULL};

    subjectSellItem("Ahmet", "Çorap", 1);
    subjectBuyItem("Mahmut", "Çorap", 1);

    /*if (subjectsHasItems(everybody,itemss)){
        printf("z\n");
    }*/

    char *denemeisim[] = {"Necati","Tahsin",NULL};
    char* denemeitems[] = {"2 Çorap", "999 Terlik", NULL};

    subjectBuyItem("Necati","Çorap", 1);
    subjectBuyItem("Necati","Terlik", 5);
    subjectBuyItem("Tahsin","Çorap", 1);
    subjectBuyItem("Tahsin","Terlik", 5);


    /*if (subjectsHasItems(denemeisim,denemeitems)){
        printf("m\n");
    }*/

    /*if(subjectsHasLessThanItems(denemeisim,denemeitems)){
        printf("K\n");
    }*/

    /*for(int i = 0; i < currentSubjectAmount; i++){
        struct Subject mySubject = subjects[i];
        printf("Subject %d is: %s\n", i ,mySubject.name);
        for(int j = 0; j < currentItemAmount; j++){
            printf("He has %d of item %d\n", mySubject.inventory[j], j);
        }
    }
     */

    free(items);
    free(subjects);
    free(locations);
    return 0;
}