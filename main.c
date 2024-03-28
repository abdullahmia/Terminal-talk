#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/**
 * Define the database file
*/
const char *const USERS_FILE = "users.csv";
const char *const QUESTION_FILE = "questions.csv";
const char *const ANSWER_FILE = "answer.csv";
const int USER_ID = 1;
const int USER_NAME_COLUMN = 2;
const int USER_PASSWORD_COLUMN = 3;
const int USER_BIRTHDATE_COLUMN = 4;
const int QUESTION_ID = 1;
const int QUESTION_TITLE= 2;
const int QUESTION_DESCRIPTION = 3;
const int QUESTION_DATE = 4;
const int QUESTION_USERNAME = 5;
const int QUESTION_USER_ID = 6;
const int ANSER_ID = 1;
const int ANSER_USERNAME = 2;
const int ANSER_ANSWER = 3;
const int ANSER_DATE = 4;
const int ANSER_QUESTION_ID = 5;

#define MAX_LINE_LENGTH 1024


/**
 * <-- STRUCTURE OF DATA -->
 * 
*/
typedef struct
{
    int id;
    char username[50];
    char password[50];
    char birth_date[50];
} Account;

typedef struct {
    int id;
    char title[1000];
    char description[5000];
    char date[50];
    char username[50];
    int user_id;
} Question;

typedef struct {
    int id;
    char username[50];
    char answer[5000];
    char date[50];
    int question_id;
} Answer;

typedef struct {
    int id;
    char username[100];
} Looged_User;

Looged_User logged_in_user;

/**
 * <-- ORM UTILITY FUNCTIONS -->
*/
bool create(const char *filename, const void *data, size_t data_size) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fprintf(file, "%s\n", (char *)data);
        fclose(file);
        return true;
    } else {
        return false;
    }
}

char* get_data_by_value(const char* file_path, const char* value) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        char* original_line = strdup(line);
        char* token = strtok(line, ",");
        while (token != NULL) {
            if (strcmp(token, value) == 0) {
                fclose(file);
                char* matched_line = strdup(original_line);
                free(original_line);
                return matched_line;
            }
            token = strtok(NULL, ",");
        }
        free(original_line);
    }

    fclose(file);
    return NULL;
}

char* get_value_by_column(const char* row_data, int column) {
    char* copy = strdup(row_data);
    char* token = strtok(copy, ",");

    for (int i = 1; i < column && token != NULL; i++) {
        token = strtok(NULL, ",");
    }

    if (token == NULL) {
        free(copy);
        return NULL;
    }

    char* value = strdup(token);
    free(copy);
    return value;
}


int count_rows(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return -1;
    }

    int count = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file) != NULL) {
        count++;
    }

    fclose(file);
    return count;
}


/**
 * <-- UTILITY DATE & TIME FUNCTIONS -->
*/
char* getCurrentDate() {
    static char dateString[11]; // "YYYY-MM-DD\0"
    
    time_t currentTime;
    struct tm *localTime;
    
    // Get the current time
    currentTime = time(NULL);
    
    // Convert the current time to the local time
    localTime = localtime(&currentTime);
    
    // Format the date string
    sprintf(dateString, "%04d-%02d-%02d", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday);

    return dateString;
}


/**
 * @name: Clear screen
*/

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


/**
 * @name: Bootstrap Screen
*/
void bootstrap_screen() {

    int choice;

    do {
        clear_screen();

        printf("Welcome to the Forum App!\n");
        printf("----------------------------\n");
        printf("1. Login\n");
        printf("2. Create Account\n");
        printf("3. Forgot Password\n");
        printf("4. Help\n");
        printf("5. Exit\n");
        printf("----------------------------\n");

        printf("Choose an option to proceed: ");
        choice = get_user_choice();

        if (choice < 1 || choice > 5) {
            printf("Invalid choice. Please enter a number between 1 and 5.\n");
        }
    } while (choice < 1 || choice > 5);

    switch (choice) {
        case 1:
            login_screen();
            break;
        case 2:
            register_screen();
            break;
        case 3:
            forgot_password_screen();
            break;
        //TODO: Add cases for 4 and 5 (Help and Exit)
    }
}

/**
 * @name: Choose options
*/
int get_user_choice(void) {
    int choice;

    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
        printf("Invalid choice. Please enter a number between 1 and 5: ");
        scanf("%*[^\n]");
    }

    return choice;
}

/**
 * @name: Login Screen
*/
void login_screen() {

    char username[100];
    char password[100];

    printf("Enter username: ");
    clear_input_buffer();
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    strtok(password, "\n");

    // Check the user user is exist of not
    const char* search_valu = "abdullah";
    char* is_user = get_data_by_value(USERS_FILE, username);
    if (is_user != NULL) {
        // check the password is valid or not
        char* db_password = get_value_by_column(is_user, 3);
        if (strcmp(db_password, password) == 0) {
            char* get_user_id = get_value_by_column(is_user, 1);
            int user_id = atoi(get_user_id);
            char* get_username = get_value_by_column(is_user, 2);
            logged_in_user.id = user_id;
            strcpy(logged_in_user.username, get_username);
            home_screen();
        } else {
            printf("Password is incorrect. Please wait 2 seconds.\n");
            sleep(2); 
            login_screen();
        }
    } else {
        printf("There is no such user! Do you want to register account? (y/n)");
        char choice;
        scanf("%c", &choice);
        if (choice == 'y'){
            register_screen();
        }else{
            bootstrap_screen();
        }
    }
}



/**
 * @name: Clear input buffer
*/
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


/**
 * @name: Register Screen
*/
void register_screen() {
    Account newAccount;

    int rows = count_rows(USERS_FILE);
    newAccount.id = rows + 1;

    printf("Enter username: "); // TODO: It should be validate that username is already exist or not.
    clear_input_buffer();
    fgets(newAccount.username, sizeof(newAccount.username), stdin);
    strtok(newAccount.username, "\n");

    printf("Enter password: ");
    fgets(newAccount.password, sizeof(newAccount.password), stdin);
    strtok(newAccount.password, "\n");

    printf("Enter birth date(dd/mm/yyyy): "); // Todo: Validate the date formate
    fgets(newAccount.birth_date, sizeof(newAccount.birth_date), stdin);
    strtok(newAccount.birth_date, "\n");


    char accountString[500];
    sprintf(accountString, "%d,%s,%s,%s",
            newAccount.id,
            newAccount.username,
            newAccount.password,
            newAccount.birth_date
        );

    bool isCreateUser = create(USERS_FILE, accountString, strlen(accountString));
    if (isCreateUser)
    {
        char choice;
        printf("Account has been created successfully. Do you want to login? (y/n): ");
        scanf("%c", &choice);
        if (choice == 'y'){
            login_screen();
        }else{
            bootstrap_screen();
        }
    } else {
        printf("Something went wrong! Do you want to try again? (y/n): ");
        char choice;
        scanf("%c", &choice);
        if (choice == 'y'){
            register_screen();
        }else{
            bootstrap_screen();
        }
    }
}

/**
 * @name: Forgot Password Screen
*/
void forgot_password_screen() {

}

/**
 * @name: Home Screen
*/
void home_screen() {

    int choice;

    do {
        clear_screen();
        printf("Welcome to the Forum App!\n");
        printf("----------------------------\n");
        printf("1. Go to Fourm\n");
        printf("2. Write a problem?\n");
        printf("3. Help\n");
        printf("4. Exit\n");
        printf("----------------------------\n");

        printf("Choose an option to proceed: ");
        choice = get_user_choice();

        if (choice < 1 || choice > 5) {
            printf("Invalid choice. Please enter a number between 1 and 5.\n");
        }
    } while (choice < 1 || choice > 5);

    
    switch (choice) {
        case 1:
            forum_screen();
            break;
        case 2:
            write_problem_screen();
            break;
        //TODO: Add cases for 4 and 5 (Help and Exit)
    }
}

/**
 * @name: Forum Screen
*/
void forum_screen() {
    printf("Forum!");
}

/**
 * @name: Write a problem Screen
*/
void write_problem_screen() {
    printf("Write down your problem on the forum!\n");
    printf("Welcome, %s!\n", logged_in_user.username);

    Question new_question;

    int rows = count_rows(QUESTION_FILE);
    new_question.id = rows + 1;

    printf("Enter question title: ");
    clear_input_buffer();
    fgets(new_question.title, sizeof(new_question.title), stdin);
    strtok(new_question.title, "\n");

    printf("Enter the description: ");
    fgets(new_question.description, sizeof(new_question.description), stdin);
    strtok(new_question.description, "\n");

    char* current_date = getCurrentDate();
    strcpy(new_question.date, current_date);

    strcpy(new_question.username, logged_in_user.username);
    new_question.user_id = logged_in_user.id;

    char questionString[999999999];
    sprintf(questionString, "%d,%s,%s,%s,%s,%d",
            new_question.id,
            new_question.title,
            new_question.description,
            new_question.date,
            new_question.username,
            new_question.user_id
        );

        printf("Question ID: %d\n", new_question.id);
        printf("Title: %s\n", new_question.title);
        printf("Description: %s\n", new_question.description);
        printf("Date: %s\n", new_question.date);
        printf("Username: %s\n", new_question.username);
        printf("User ID: %d\n", new_question.user_id);

// Print the concatenated string
    printf("Concatenated String: %s\n", questionString);


    // bool is_createNew_question = create(QUESTION_FILE, questionString, strlen(questionString));
    // if (is_createNew_question) {
    //     char choice;
    //     printf("Question has been posted. Do you want to go to the Forum? (y/n): ");
    //     scanf(" %c", &choice);  // Added space before %c to consume whitespace characters
    //     if (choice == 'y'){
    //         forum_screen();
    //     } else {
    //         home_screen();
    //     }
    // } else {
        printf("Something went wrong!");
        home_screen();
    // }
}



int main () {
    bootstrap_screen();
    return 0;
}