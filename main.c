#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>


/**
 * Define the database file
*/
const char *const USERS_FILE = "./database/users.csv";
const char *const QUESTION_FILE = "./database/questions.csv";
const char *const ANSWER_FILE = "./database/answer.csv";
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
#define MAX_LINE_LENGTH 5500



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
    char question_id[50];
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

void get_answers(const char *file_path, const char *question_id) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    Answer answer;
    // Print table headers
    printf("%-5s | %-60s | %-20s | %-20s | %-10s\n", "ID", "Answer", "Date", "Username", "Question ID");
    printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%[^,],%[^,],%[^,],%[^\n]", &answer.id, answer.username, answer.answer, answer.date, answer.question_id);
        if (strcmp(answer.question_id, question_id) == 0) {
            // Print each row in a formatted manner
            printf("%-5d | %-60s | %-20s | %-20s | %-10s\n", answer.id, answer.username, answer.answer, answer.date, answer.question_id);
        }
    }

    fclose(file);
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

void print_questions() {
    FILE *file = fopen(QUESTION_FILE, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int maxColumnWidth[6] = {5, 30, 30, 20, 15, 10};

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        char tempLine[MAX_LINE_LENGTH];
        strcpy(tempLine, line);

        char *token = strtok(tempLine, ",");
        int columnCount = 0;
        while (token != NULL && columnCount < 6) {
            int tokenLength = strlen(token);
            if (tokenLength > maxColumnWidth[columnCount]) {
                maxColumnWidth[columnCount] = tokenLength;
            }
            token = strtok(NULL, ",");
            columnCount++;
        }
    }

    fclose(file);
    file = fopen(QUESTION_FILE, "r");

    printf("%-*s ", maxColumnWidth[0], "ID");
    printf("%-*s ", maxColumnWidth[1], "TITLE");
    printf("%-*s ", maxColumnWidth[2], "DESCRIPTION");
    printf("%-*s ", maxColumnWidth[3], "DATE");
    printf("%-*s ", maxColumnWidth[4], "USERNAME");
    printf("%-*s\n", maxColumnWidth[5], "USER ID");

    printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file) != NULL) {
        char tempLine[MAX_LINE_LENGTH];
        strcpy(tempLine, line);

        char *token = strtok(tempLine, ",");
        int columnCount = 0;
        while (token != NULL && columnCount < 6) {
            printf("%-*s ", maxColumnWidth[columnCount], token);
            token = strtok(NULL, ",");
            columnCount++;
        }
        printf("\n");
    }

    fclose(file);
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


void get_app_name() {
    printf(" _______  _______  ______    __   __  ___   __    _  _______  ___        _______  _______  ___      ___   _ \n");
    printf("|       ||       ||    _ |  |  |_|  ||   | |  |  | ||   _   ||   |      |       ||   _   ||   |    |   | | |\n");
    printf("|_     _||    ___||   | ||  |       ||   | |   |_| ||  |_|  ||   |      |_     _||  |_|  ||   |    |   |_| |\n");
    printf("  |   |  |   |___ |   |_||_ |       ||   | |       ||       ||   |        |   |  |       ||   |    |      _|\n");
    printf("  |   |  |    ___||    __  ||       ||   | |  _    ||       ||   |___     |   |  |       ||   |___ |     |_ \n");
    printf("  |   |  |   |___ |   |  | || ||_|| ||   | | | |   ||   _   ||       |    |   |  |   _   ||       ||    _  |\n");
    printf("  |___|  |_______||___|  |_||_|   |_||___| |_|  |__||__| |__||_______|    |___|  |__| |__||_______||___| |_|\n");
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

        get_app_name();
        printf("----------------------------\n");
        printf("1. Login\n");
        printf("2. Create Account\n");
        printf("3. Help\n");
        printf("4. Exit\n");
        printf("----------------------------\n");

        printf("Choose an option to proceed: ");
        choice = get_user_choice();

        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please enter a number between 1 and 4.\n");
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
            help_screen();
            break;
        case 4:
            printf("Exiting the Terminal Talk. Goodbye!\n");
            exit(0);
            break;
    }
}

/**
 * @name: Help Screen
*/
void help_screen(int isAuthenticated) {
    clear_screen();
    get_app_name();
    printf("----------------------------\n");
    printf("Welcome to the Terminal Talk App Help section!\n");
    printf("This app allows you to participate in discussions and interact with other users.\n");
    printf("Here are some key features and how to use them:\n");
    printf("1. Login: If you already have an account, select this option to log in.\n");
    printf("2. Create Account: If you are new to the Terminal Talk App, select this option to create a new account.\n");
    printf("3. Help: You're here! This option displays information on how to use the app.\n");
    printf("4. Exit: Select this option to exit the Terminal Talk App.\n");
    printf("----------------------------\n");
    printf("Once logged in, you can:\n");
    printf("- Create new posts to start discussions.\n");
    printf("- Go to feed to see all the problems.\n");
    printf("- Jump to the single problem.\n");
    printf("- You can add a solution to a particular question\n");
    printf("If you have any further questions or encounter any issues, feel free to contact our support team.\n");
    printf("----------------------------\n");
    printf("Press any key to return to the main menu...");
    while (getchar() != '\n');
    getchar();
    if (isAuthenticated) {
        home_screen();
    } else {
        bootstrap_screen();
    }
}

/**
 * @name: Choose options
*/
int get_user_choice(void) {
    int choice;

    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
        printf("Invalid choice. Please enter a number between 1 and 4: ");
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
        get_app_name();
        printf("----------------------------\n");
        printf("1. Go to Fourm\n");
        printf("2. Write a problem?\n");
        printf("3. Help\n");
        printf("4. Exit\n");
        printf("----------------------------\n");

        printf("Choose an option to proceed: ");
        choice = get_user_choice();

        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please enter a number between 1 and 4.\n");
        }
    } while (choice < 1 || choice > 4);

    
    switch (choice) {
        case 1:
            forum_screen();
            break;
        case 2:
            write_problem_screen();
            break;
        case 3:
            help_screen(1);
            break;
        case 4:
            printf("Exiting the Terminal Talk App. Goodbye!\n");
            exit(0);
            break;
    }
}

/**
 * @name: Forum Screen
*/
void forum_screen() {
    clear_screen();
    printf("Forum! \n");
    print_questions();
    char choice;
    do {
        printf("Enter 'h' to go home or enter question ID to view it (e.g., 1): ");
        if (scanf(" %c", &choice) != 1) {
            printf("Invalid input. Please try again.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        if (choice == 'h') {
            home_screen();
        } else if (choice >= '0' && choice <= '9') {
            int question_id = choice - '0';
            view_question(question_id);
            break;
        } else {
            printf("Invalid input. Please try again.\n");
        }
    } while (1);

}




/**
 * @name: View Question Screen
*/
void view_question(int id) {
    clear_screen();
    char id_str[200];
    snprintf(id_str, sizeof(id_str), "%d", id); 
    char* question = get_data_by_value(QUESTION_FILE, id_str);
    

    // generate a question id like `question-3`
    char formatted_question_id[20];
    sprintf(formatted_question_id, "question-%d", id);



    if (question != NULL) {
        char* title = get_value_by_column(question, 2);
        char* description = get_value_by_column(question, 3);
        char* date = get_value_by_column(question, 4);
        char* username = get_value_by_column(question, 5);

        printf("QUESTION: %s\n", title);
        printf("DESCRIPTION: %s\n", description);
        printf("AUTHOR: %s\n", username);
        printf("PUBLISH DATE: %s\n", date);

        printf("\nANSWERS:\n");
        get_answers(ANSWER_FILE, formatted_question_id);
        printf("\n");

        char choice;
        while (1) {
        printf("Enter 'a' for ADD solution or 'f' for go to FORUM: ");
        scanf(" %c", &choice);
        getchar();

        if (choice == 'a') {
            add_answer(id);
        } else if (choice == 'f') {
            forum_screen();
        } else {
            printf("Invalid input!\n");
        }
    }

        free(question);
    }else {
        printf("Question not found.\n");
    }
}

/**
 * @name: Add answer to a question
*/
void add_answer(int question_id){
    Answer ans;

    int rows = count_rows(ANSWER_FILE);
    ans.id = rows + 1;

    printf("Enter the answer: ");
    fgets(ans.answer, sizeof(ans.answer), stdin);
    strtok(ans.answer, "\n");
    printf("Answer: %s\n", ans.answer);

    char* current_date = getCurrentDate();
    strcpy(ans.date, current_date);
    printf("Date: %s\n", ans.date);

    strcpy(ans.username, logged_in_user.username);
    printf("Username: %s\n", ans.username);

    // Format the question_id as "question-x"
    char formatted_question_id[20]; // Assuming maximum 20 characters for the formatted ID
    sprintf(formatted_question_id, "question-%d", question_id);
    printf("Formatted Question ID: %s\n", formatted_question_id);

    char answer_string[99999];

    sprintf(answer_string, "%d,%s,%s,%s,%s",
        ans.id,
        ans.answer,
        ans.date,
        ans.username,
        formatted_question_id
    );

    bool isAddedAnswer = create(ANSWER_FILE, answer_string, strlen(answer_string));
    if (isAddedAnswer) {
        printf("Successfully added the answer!\n");
        // wait 2 seconds and call view_question function
        sleep(2);
        view_question(question_id);
    } else {
        printf("Something went wrong!\n");
        home_screen();
    }
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

    char questionString[99999];

    sprintf(questionString, "%d,%s,%s,%s,%s,%d",
        new_question.id,
        new_question.title,
        new_question.description,
        new_question.date,
        new_question.username,
        new_question.user_id
    );

    bool isCreatedQuestion = create(QUESTION_FILE, questionString, strlen(questionString));
    if (isCreatedQuestion) {
        char choice;
        printf("Question has been added! Want to go to FORUM? (y/n)");
        scanf("%c", &choice);
        if (choice == 'y') {
            forum_screen();
        } else {
            home_screen();
        }
    } else {
        printf("Somethign wen't wrong!");
        home_screen();
    }

}



int main () {
    bootstrap_screen();
    return 0;
}