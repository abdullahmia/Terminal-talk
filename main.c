#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
const int QUESTION_DESCRIPTION = 2;
const int QUESTION_DATE = 3;
const int QUESTION_USERNAME = 4;
const int QUESTION_USER_ID = 5;
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
    printf("Found value: %s\n", is_user);
    if (is_user != NULL) {
        // check the password is valid or not
        char* db_password = get_value_by_column(is_user, 3);
        if (strcmp(db_password, password) == 0) {
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
    printf("Write down you problem on Forum!");

    char title[500];

    printf("Enter question title: ");
    clear_input_buffer();
    fgets(title, sizeof(title), stdin);
    strtok(title, "\n");

    printf("%s", title);
    
}


int main () {
    bootstrap_screen();
    return 0;
}