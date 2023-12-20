#include <iostream>
#include <cstring>
#include <string>
#include <memory>
#include <termios.h>
#include <unistd.h>
#include "sqlite3/sqlite3.h"

using namespace std;
// Custom namespace to make sqlite3 query easier
using database = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

// Store login info
struct loginInfo {
    string status;
    string name;
    long number;
};

// #Function prototype (forward declaration)#
database open_database(const char* name);
void execute(sqlite3* db, const char* sql_query);
statement create_statement(sqlite3* db, const string &sql_query);
void parameter(sqlite3_stmt* stmt, int index, int value);
void parameter(sqlite3_stmt* stmt, int index, const std::string& value);
constexpr const char* default_schema();

void generateSpace(string variable, string same_line_output);
void clearScreen();
int getAst();
loginInfo showLoginMenu(sqlite3* db);

class student{
public:
    string name;
    string studentNumber;
    
    // Function to set student's score
    string setScore(string subject, string score) {
        // Determine grade from score
        string grade = gradeScore(score);

        string x = semester1[1][0] = {subject};
        string y = semester1[1][1] = {score};
        string z = semester1[1][2] = {grade};
        return x, y, z;
    }

    // Function to view score per semester
    void viewScore() {
        cout << "--------------------------------------------------" << endl;
        cout << "Name:\t\t\t" << name << endl;
        cout << "Student Number:\t\t" << studentNumber << endl;
        cout << "--------------------------------------------------" << endl;

        // Display student's score and grade
        for(int i = 0; i < 15; i++) {
            for(int j = 0; j < 3; j++) {
                // Don't print empty array
                if(semester1[i][j] != "") {
                    cout << semester1[i][j] << "\t\t";
                }
            }
            // New line for each rows
            if(semester1[i][0] != "") {
                    cout << endl;
                }
        }
        cout << "--------------------------------------------------" << endl;
    }

private:
    // Score per semester
    string semester1[15][3] = {"Subject", "Score", "Grade"};

    // Function to determine grades
    string gradeScore(string score) {
        string grade;
        // Convert score to int so that we can use operator
        int score_int = stoi(score);

        if(score_int > 90) {
            return grade = "A";
        }
        else if(score_int > 80) {
            return grade = "B";
        }
        else if(score_int > 70) {
            return grade = "C";
        }
        else if(score_int > 60) {
            return grade = "D";
        }
        else return grade = "E";
    }
};

int main() {
    clearScreen();

    // Create new (link existing) database.
    database db = open_database("main.db");
    // // Create (if not created yet) default sqlite tables (defining .schema)
    execute(db.get(), default_schema());


    // ##Front-End##
    showLoginMenu(db.get());


    
    

    
    // int option;   
    // mainMenu:
    // cout<<"----------------------------------------------------------------------"<<endl;
    // cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    // cout<<"1. View Score"<<endl;
    // cout<<"2. Edit Score"<<endl;
    // cout<<"3. Exit"<<endl;
    // cout<<"----------------------------------------------------------------------"<<endl;
    // cin>>option;

    // try{
    //     if(system("cls")) {
    //         throw(invalid_argument("App not running on Windows"));
    //     }
    // }
    // catch(invalid_argument){
    //     system("clear");
    // }

    // if(option == 1) {

    //     goto mainMenu;
    // }

    // else if(option == 2) {

    //     editMenu:
    //     cout<<"----------------------------------------------------------------------"<<endl;
    //     cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    //     cout<<"1. Add Score"<<endl;
    //     cout<<"2. Delete Score"<<endl;
    //     cout<<"3. Go to previous menu"<<endl;
    //     cout<<"----------------------------------------------------------------------"<<endl;
    //     cin>>option;

    //     try{
    //         if(system("cls")) {
    //             throw(invalid_argument("App not running on Windows"));
    //         }
    //     }
    //     catch(invalid_argument){
    //         system("clear");
    //     }
        

    //     if(option == 1) {

    //         goto editMenu;
    //     }
    //     else if(option == 2) {

    //         goto editMenu;
            
    //     }
    //     else if(option == 3) {

    //         goto mainMenu;
            
    //     }
    //     else {
    //         cin.ignore();
    //         cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
    //         cin.get();

    //         try{
    //         if(system("cls")) {
    //             throw(invalid_argument("App not running on Windows"));
    //             }
    //         }
    //         catch(invalid_argument){
    //             system("clear");
    //         }
    //         goto editMenu;
    //     }
    // }
    // else if(option == 3) {
        
    //     cin.ignore();
    // }
    // else {
    //     cin.ignore();
    //     cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
    //     cin.get();
        
    //     try{
    //         if(system("cls")) {
    //             throw(invalid_argument("App not running on Windows"));
    //         }
    //     }
    //     catch(invalid_argument){
    //         system("clear");
    //     }

    //     goto mainMenu;
    // }
}











        // ##List of All Function##

// Generate space for neat interface.
void generateSpace(string variable, string same_line_output) {
    // 80 is the total number of character per line in the interface
    int length = 80 - (strlen(variable.c_str()) + strlen(same_line_output.c_str()));
    for (int i = 1; i < length; i++) {
        cout << " ";
    }
}

// Clear (terminal/cmd) Screen
void clearScreen() {
    /* If the app is running on Windows, use system("cls") (to clear screen).
    Else, the app must be running on Linux. Hence, use system("clear") (to clear screen) */
    try{
        if(system("cls")) {
        throw(invalid_argument("App not running on Windows"));
        }
    }
    catch(invalid_argument){
    system("clear");
    }
}

// Replace user input to asterisk (*) whilte typing password
// From > https://dev.to/namantam1/how-to-take-hidden-password-from-terminal-in-cc-3ddd
int getAst() {
    int ast;
    // struct to hold the terminal settings
    struct termios old_settings, new_settings;
    // take default setting in old_settings
    tcgetattr(STDIN_FILENO, &old_settings);
    // make of copy of it (Read my previous blog to know 
    // more about how to copy struct)
    new_settings = old_settings;
    // change the settings for by disabling ECHO mode
    // read man page of termios.h for more settings info
    new_settings.c_lflag &= ~(ICANON | ECHO);
    // apply these new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    // now take the input in this mode
    ast = getchar();
    // reset back to default settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return ast;
}


    // #Front-End#

loginInfo showLoginMenu(sqlite3* db) {
    int login_failed = 0;
    string option;
    loginInfo login_info;
    loginMenu:
    cout << endl << endl;
    loginMenuErrMsg:
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "| Choose one option below by typing the number (0,1,2,.. )                     |" << endl;
    cout << "| 1. Login                                                                     |" << endl;
    cout << "| 2. Create Account                                                            |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|                                                                  |  0. Exit  |" << endl;
    cout << "+------------------------------------------------------------------+-----------+" << endl;
    cout << "| Input: ";
    cin >> option;

    clearScreen();

    if(option == "1") {
        int number = 0;
        string username, password;
        bool username_exist = false;

        loginOption:
        cout << endl << endl;
        loginOptionErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Insert Username: "; cin >> username;

        clearScreen();
        cout << endl << endl;
        cout << "+------------------------------------------------------------------------------+"<<endl;
        cout << "| Insert Username: " << username, generateSpace(username, "| Insert Username: "), cout << "|\n";
        cout << "| Insert Password: ";

        // Hide user input while typing password
        int asciiChar, i = 0;
        char get_password[100];
        cin.ignore();
        while ((asciiChar = getAst()) != '\n') {
            if (asciiChar == 127 || asciiChar == 8) { // handle backspace
                if (i != 0) {
                    i--;
                    printf("\b \b");
                }
            } else {
                get_password[i++] = asciiChar;
                // echo the '*' to get feel of taking password 
                cout << "*";
            }
            get_password[i] = '\0';
            password = get_password;
        }

        // Too many failed attempt to login
        if (login_failed >= 2) {
            clearScreen();
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| > Too many failed login attempt                                              |" << endl;
            loginFailed:
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| Do you want to try to login again? (y/n)                                     |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> option;
            
            login_failed = 0;

            if (option == "y" or option == "Y") {
                clearScreen();
                goto loginOption;
            } else if (option == "n" or option == "N") {
                clearScreen();
                goto loginMenu;
            } else if (option == "9") {
                clearScreen();
                goto loginMenu;
            } else if(option == "0") {
                clearScreen();
                exit(EXIT_SUCCESS);
            } else {
                clearScreen();
                cin.ignore();
                cout << "+------------------------------------------------------------------------------+" << endl;
                cout << "| > Invalid option. Please enter the available option!                         |" << endl;
                goto loginFailed;
            }
        }

        // Check if username is exist in the database
        string query = R"(
        SELECT number FROM student WHERE username = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            login_info.status = "student";
            username_exist = true;
            number = sqlite3_column_int(stmt.get(), 0);
        }
        query = R"(
        SELECT number FROM lecturer WHERE username = ?)";
        stmt = create_statement(db, query);
        parameter(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            login_info.status = "lecturer";
            username_exist = true;
            number = sqlite3_column_int(stmt.get(), 0);
        }

        if (username_exist != true) {
            login_failed += 1;
            clearScreen();
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Invalid username or password                                               |" << endl;
            goto loginOptionErrMsg;
        }
        
        // Check if the password match the username in the database
        if (login_info.status == "student") {
            query = R"(
            SELECT number, name FROM student WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            parameter(stmt.get(), 1, username);
            parameter(stmt.get(), 2, password);
            if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                login_info.number = sqlite3_column_int(stmt.get(), 0);
                login_info.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1)));
            } else {
                login_failed += 1;
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Invalid username or password                                               |" << endl;
                goto loginOptionErrMsg;
            }
        }
        else if (login_info.status == "lecturer") {
            query = R"(
            SELECT number, name FROM lecturer WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            parameter(stmt.get(), 1, username);
            parameter(stmt.get(), 2, password);
            if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                login_info.number = sqlite3_column_int(stmt.get(), 0);
                login_info.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1)));
            } else {
                login_failed += 1;
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Invalid username or password                                               |" << endl;
                goto loginOptionErrMsg;
            }
        } else {
            login_failed += 1;
            clearScreen();
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Invalid username or password                                               |" << endl;
            goto loginOptionErrMsg;
        }

        // Return login info
        if (login_info.status == "student" or login_info.status == "lecturer" and !login_info.name.empty() and login_info.number != 0) {
            return login_info;
        }
    }
    else if(option == "2") {
        string name;
        clearScreen();
        registerOption:
        cout << endl << endl;
        registerOptionErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "|                      Are you a Student or a Lecturer?                        |" << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Choose one option below by typing the number (0,1,2,.. )                     |" << endl;
        cout << "| 1. Student                                                                   |" << endl;
        cout << "| 2. Lecturer                                                                  |" << endl;
        cout << "|                                                                              |" << endl;
        cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
        cout << "| Input: ";
        cin >> option;

        if (option == "1") {
            clearScreen();
            string full_name, number, username, password;
            enterFullName:
            cout << endl << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| Please enter your Full Name!                                                 |" << endl;
            cout << "| Full Name: "; cin.ignore(); getline(cin, full_name);

            clearScreen();
            cout << endl << endl;
            fullNameConfirmErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "|                                                                              |" << endl;
            cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
            cout << "|                                                                              |" << endl;
            cout << "| Is that the correct Name? (y/n)                                              |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> option;

            if (option == "y" or option == "Y") {
                clearScreen();
                enterNumber:
                cout << endl << endl;
                enterNumberErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| / Please enter your Student Number!                                          |" << endl;
                cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                cout << "| Student Number: ";
                cin.ignore();
                getline(cin, number);
                // Make sure user input is only number
                if ((number.find_first_of("0123456789") == string::npos) == true) {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Student Number must be all numerical                                       |" << endl;
                    cout << "| What you enter: " << option << endl;
                    goto enterNumberErrMsg;
                }

                clearScreen();
                cout << endl << endl;
                numberConfirmErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|                                                                              |" << endl;
                cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                cout << "| Student Number: " << number, generateSpace(number, "| Student Number: "), cout << "|\n";
                cout << "|                                                                              |" << endl;
                cout << "| Is that the correct Student Number? (y/n)                                    |" << endl;
                cout << "|                                                                              |" << endl;
                cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                cout << "| Input: ";
                cin >> option;

                if (option == "y" or option == "Y") {
                    // Check student number in database
                    string query = "SELECT * FROM student WHERE number = ?";
                    statement stmt = create_statement(db, query);
                    parameter(stmt.get(), 1, number);

                    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                        clearScreen();
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "| > Student Number already exist. Please enter a new one!                      |" << endl;
                        goto enterNumberErrMsg;
                    }

                    clearScreen();
                    enterUsername:
                    cout << endl << endl;
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| / Please create a Username!                                                  |" << endl;
                    cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                    cout << "| Student Number: " << number, generateSpace(number, "| Student Number: "), cout << "|\n";
                    cout << "| Username: ";
                    cin >> username;

                    clearScreen();
                    cout << endl << endl;
                    usernameConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "|                                                                              |" << endl;
                    cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                    cout << "| Student Number: " << number, generateSpace(number, "| Student Number: "), cout << "|\n";
                    cout << "| Username: " << username, generateSpace(username, "| Username: "), cout << "|\n";
                    cout << "|                                                                              |" << endl;
                    cout << "| Do you want to continue with this Username? (y/n)                            |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                    cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                    cout << "| Input: ";
                    cin >> option;

                } else if (option == "n" or option == "N") {
                    clearScreen();
                    goto enterNumber;
                } else if (option == "9") {
                    clearScreen();
                    goto enterFullName;
                } else if (option == "0") {
                    clearScreen();
                    exit(EXIT_SUCCESS);
                } else {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+" << endl;
                    cout << "| > Invalid option. Please enter the available option!                         |" << endl;
                    cin.ignore();
                    goto numberConfirmErrMsg;
                }
                

            } else if (option == "n" or option == "N") {
                clearScreen();
                goto enterFullName;
            } else if (option == "9") {
                clearScreen();
                goto registerOption;
            } else if (option == "0") {
                clearScreen();
                exit(EXIT_SUCCESS);
            } else {
                clearScreen();
                cout << "+------------------------------------------------------------------------------+" << endl;
                cout << "| > Invalid option. Please enter the available option!                         |" << endl;
                cin.ignore();
                goto fullNameConfirmErrMsg;
            }

        } else if (option == "2") {
            
        } else if (option == "9") {
            clearScreen();
            goto loginMenu;
        } else if (option == "0") {
            clearScreen();
            exit(EXIT_SUCCESS);
        } else {
            clearScreen();
            cin.ignore();
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| > Invalid option! Please enter the available option.                         |" << endl;
            goto registerOptionErrMsg;
        }
    }
    else if(option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    }
    else {
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| > Invalid option. Please enter the available option!                         |" << endl;
        cin.ignore();
        goto loginMenuErrMsg;
    }

    return login_info;
}




// #Template for sqlite query#

// Open/create new database or link existing database.
database open_database(const char* name){
    sqlite3* db = nullptr;
    int rc = sqlite3_open(name, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    return database(db, sqlite3_close);
}

// Execute SQL query with sqlite3_exec() function.
void execute(sqlite3* db, const char* sql_query) {
    char* errmsg = 0;
    int rc = sqlite3_exec(db, sql_query, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        cerr << "Unable to execute query" << errmsg;
        sqlite3_free(errmsg);
        exit(EXIT_FAILURE);
    }
}

// Create (and prepare) SQL statement.
statement create_statement(sqlite3* db, const string &sql_query) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql_query.c_str(), sql_query.length(), &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Unable to create statement '" << sql_query <<"': " << sqlite3_errmsg(db);
        exit(EXIT_FAILURE);
    }
    return statement(stmt, sqlite3_finalize);
}

// Bind int parameter
void parameter(sqlite3_stmt* stmt, int index, int value) {
    if (sqlite3_bind_int(stmt, index, value) != SQLITE_OK) {
        throw(invalid_argument("Error, sqlite3_bind can't accept your value"));
    }
}

// Bind string parameter
void parameter(sqlite3_stmt* stmt, int index, const std::string& value) {
    if (sqlite3_bind_text(stmt, index, value.c_str(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
        throw(invalid_argument("Error, sqlite3_bind can't accept your value"));
    }
}

// Query to create default tables (create sql schema).
constexpr const char* default_schema() {
    return R"(
    CREATE TABLE IF NOT EXISTS student (
        number INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        name TEXT NOT NULL,
        username TEXT NOT NULL,
        password TEXT NOT NULL
    );

    CREATE UNIQUE INDEX IF NOT EXISTS number ON student(number);
    CREATE UNIQUE INDEX IF NOT EXISTS username ON student(username);

    CREATE TABLE IF NOT EXISTS lecturer (
        number INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        name TEXT NOT NULL,
        username TEXT NOT NULL,
        password TEXT NOT NULL
    );

    CREATE UNIQUE INDEX IF NOT EXISTS number ON lecturer(number);
    CREATE UNIQUE INDEX IF NOT EXISTS username ON lecturer(username);

    CREATE TABLE IF NOT EXISTS subject_list (
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        lecturer_id INTEGER NOT NULL,
        name TEXT NOT NULL,
        sks TEXT NOT NULL,
        FOREIGN KEY(lecturer_id) REFERENCES lecturer(number)
    );

    CREATE TABLE IF NOT EXISTS score (
        student_number INTEGER NOT NULL,
        semester INTEGER NOT NULL,
        subject_id INTEGER NOT NULL,
        score INTEGER NOT NULL,
        grade TEXT NOT NULL,
	FOREIGN KEY(student_number) REFERENCES student(number)
	FOREIGN KEY(subject_id) REFERENCES subject_list(id)
    );
    )";
}