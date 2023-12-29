#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <windows.h>
#include "sqlite3/sqlite3.h"
#include "sqlite_query.h"

using namespace std;

// Store login info
struct loginInfo {
    string status;
    string name;
    long long number;
};

// #Function prototype (forward declaration)#
constexpr const char* default_schema();
string printUI(string message, string variable);
string printAsterisk(int length);
bool oneCharExist(string input);
string toDefaultTextStyle(string input);
void clearScreen();
loginInfo showLoginMenu(sqlite3* db);

int main() {
    clearScreen();

    // Create new (link existing) database.
    database db = open_database("main.db");
    // Create (if not created yet) default sqlite tables (defining .schema)
    execute(db.get(), default_schema());


    // ##Front-End##
    loginInfo loginInfo = showLoginMenu(db.get());

    cout << loginInfo.status << endl;
    cout << loginInfo.name << endl;
    cout << loginInfo.number << endl;

}










        // ##List of All Functions##

// Generate interface with dynamic spaces.
string printUI(string message, string variable) {
    // String concatenation
    string s = message + variable;
    // 80 is the total number of character per line in the interface
    size_t length = 80 - (variable.length() + message.length());
    for (size_t i = 1; i < length; i++) {
        s += " ";
        if (i == length - 1) {
            s += "|\n";
        }
    }
    return s;
}

string printUI(string option) {
    string s;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (option == "strip") {
        s = "+------------------------------------------------------------------------------+\n";
    } else if (option == "blank") {
        s = "|                                                                              |\n";
    } else if (option == "bottom menu") {
        s = "|                                                      |  9. Back  |  0. Exit  |\n";
        s += "+------------------------------------------------------+-----------+-----------+\n";
        s += "| Input: ";
    }
    else {
        s = "UI Function Error! Option is not incorrect";
    }
    return s;
}

string printUIErr(string error_message) {
    string s= "+------------------------------------------------------------------------------+\n| > ";
    s += error_message;
    // 80 is the total number of character per line in the interface
    size_t length = 80 - (error_message.length() + 4); // +4 is for counting the "| > "
    for (size_t i = 1; i < length; i++) {
        s += " ";
        if (i == length - 1) {
            s += "|\n";
        }
    }  
    return s;
}

// Replace user input (password) with *
// From -> https://cplusplus.com/forum/general/3570/
string getpassword( const string& prompt) {
    string result;

    // Set the console mode to no-echo, not-line-buffered input
    DWORD mode, count;
    HANDLE ih = GetStdHandle( STD_INPUT_HANDLE  );
    HANDLE oh = GetStdHandle( STD_OUTPUT_HANDLE );
    if (!GetConsoleMode( ih, &mode ))
    throw runtime_error(
        "getpassword: You must be connected to a console to use this program.\n"
        );
    SetConsoleMode( ih, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT) );

    // Get the password string
    WriteConsoleA( oh, prompt.c_str(), prompt.length(), &count, NULL );
    char c;
    while (ReadConsoleA( ih, &c, 1, &count, NULL) && (c != '\r') && (c != '\n')) {
        if (c == '\b') {
            if (result.length())
                {
                WriteConsoleA( oh, "\b \b", 3, &count, NULL );
                result.erase( result.end() -1 );
                }
            } else {
            WriteConsoleA( oh, "*", 1, &count, NULL );
            result.push_back( c );
            }
        }
    // Restore the console mode
    SetConsoleMode( ih, mode );
    return result;
}

// Print asterisk as many as a string variable length
string printAsterisk(size_t length) {
    string s;
    for (int i = 0; i < length; i++) {
        s += "*";
    }
    return s;
}

// Make sure there is at least 1 character
bool oneCharExist(string input) {
    bool flag = false;
    for (int i = 0; i < input.length(); i++) {
        if (input[i] >= 'a' && input[i] <= 'z' || input[i] >= 'A' && input[i] <= 'Z' ) {
            flag = true;
        }
    }
    return flag;
}

// Remove unnecessary space and capitalize first character of each word
string toDefaultTextStyle(string input) {
    /* regex_replace -> replace multiple spaces (2nd argument) in input (1st argument)
    with 1 space (3rd argument) */
    string s = regex_replace(input, regex("\\s+"), " "); // \\s+ means more than 1 space (1 space is allowed)
    
    if (s[0] == ' ') {
        s.erase(0, 1);
    }

    // Capitalize the first letter of each word and set the rest to lowercase
        for (int x = 0; x < s.length(); x++)
        {
            if (x == 0)
            {
                s[x] = toupper(s[x]);
            }
            else if (s[x - 1] == ' ')
            {
                s[x] = toupper(s[x]);
            }
            else {
                s[x] = tolower(s[x]);
            }
        }
    return s;
}

// Clear (terminal/cmd) Screen
void clearScreen() {
    /* If the app is running on Windows, use system("cls") (to clear screen).
    Else, the app must be running on Linux. Hence, use system("clear") (to clear screen) */
    try{
        system("cls");
    }
    catch(invalid_argument){
    system("clear");
    }
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
    getline(cin, option);

    clearScreen();

    if(option == "1") {
        string username, password;

        loginOption:
        cout << endl << endl;
        loginOptionErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Insert Username: ";
        getline(cin, username);

        clearScreen();
        cout << endl << endl;
        cout << "+------------------------------------------------------------------------------+"<<endl;
        cout << printUI("| Insert Username: ", username);
        cout << "| Insert Password: ";
        password = getpassword("");

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

            if (option == "y" || option == "Y") {
                clearScreen();
                goto loginOption;
            } else if (option == "n" || option == "N") {
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

        // Check if username and password is exist in the database
        string query = R"(
        SELECT number FROM person WHERE username = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            query = R"(
            SELECT number, name, status FROM person WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            parameter(stmt.get(), 1, username);
            parameter(stmt.get(), 2, password);
            if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                login_info.number = get_int_value(stmt.get(), 0);
                login_info.name = get_text_value(stmt.get(), 1);
                login_info.status = get_text_value(stmt.get(), 2);

                // Login successful. Return login info
                clearScreen();
                return login_info;
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
    }
    else if(option == "2") {
        string status, full_name, str_number, username, password;
        long long number;
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
        getline(cin, option);

        // Get status
        if (option == "1") {
            clearScreen();
            status = "student";
        } else if (option == "2") {
            clearScreen();
            status = "lecturer";
        } else if (option == "9") {
            clearScreen();
            goto loginMenu;
        } else if (option == "0") {
            clearScreen();
            exit(EXIT_SUCCESS);
        } else {
            clearScreen();
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| > Invalid option. Please enter the available option!                         |" << endl;
            goto registerOptionErrMsg;
        }

        enterFullName:
        cout << endl << endl;
        enterFullNameErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Please enter your Full Name!                                                 |" << endl;
        cout << "| Full Name: ";
        getline(cin, full_name);

        // Full_name must be all letters and at least 1 character
        if(regex_match(full_name, regex("([a-zA-Z .]*)")) == false) { 
            clearScreen();
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Full Name must be all letters                                              |" << endl;
            goto enterFullNameErrMsg;
        } else if (oneCharExist(full_name) == false) {
            clearScreen();
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Full Name cannot be empty. Please insert at least 1 character              |" << endl;
            goto enterFullNameErrMsg;
        }

        full_name = toDefaultTextStyle(full_name);

        clearScreen();
        cout << endl << endl;
        fullNameConfirmErrMsg:
        cout << "+------------------------------------------------------------------------------+"<<endl;
        cout << "|                                                                              |" << endl;
        cout << printUI("| Full Name: ", full_name);
        cout << "|                                                                              |" << endl;
        cout << "| Is that the correct Name? (Y/n)                                              |" << endl;
        cout << "|                                                                              |" << endl;
        cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
        cout << "| Input: ";
        getline(cin, option);

        if (option == "y" || option == "Y" || option.length() == 0) {
            clearScreen();
            enterNumber:
            cout << endl << endl;
            enterNumberErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| / Please enter your Student Number!                                          |" << endl;
            cout << printUI("| Full Name: ", full_name);
            cout << "| Student Number: ";
            getline(cin, str_number);

            // Make sure user input is only number
            if(regex_match(str_number, regex("([0-9]*)")) == false) { 
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Student Number must be all numerical with no space                         |" << endl;
                goto enterNumberErrMsg;
            } else if (str_number.length() > 16) {
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Student Number maximum length is 16                                        |" << endl;
                goto enterNumberErrMsg;
            }

            stringstream toint(str_number);
            toint >> number;

            // Check student number in database
            string query = "SELECT number FROM person WHERE number = ?";
            statement stmt = create_statement(db, query);
            parameter(stmt.get(), 1, number);

            if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Student Number already exist. Please enter a different one!                |" << endl;
                goto enterNumberErrMsg;
            }

            clearScreen();
            cout << endl << endl;
            numberConfirmErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "|                                                                              |" << endl;
            cout << printUI("| Full Name: ", full_name);
            cout << printUI("| Student Number: ", str_number);
            cout << "|                                                                              |" << endl;
            cout << "| Is that the correct Student Number? (Y/n)                                    |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            getline(cin, option);

            if (option == "y" || option == "Y" || option.length() == 0) {
                clearScreen();
                enterUsername:
                cout << endl << endl;
                enterUsernameErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| / Please create a Username!                                                  |" << endl;
                cout << printUI("| Full Name: ", full_name);
                cout << printUI("| Student Number: ", str_number);
                cout << "| Username: ";
                getline(cin, username);

                // Make sure username can only contains letters, numbers, and underscores with no space
                if(regex_match(username, regex("([a-zA-Z0-9_]*)")) == false) { 
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Username can only contains numbers, letters, and underscores               |" << endl;
                    goto enterUsernameErrMsg;
                } else if (username.length() > 32) {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Username maximum length is 32 character                                    |" << endl;
                    goto enterUsernameErrMsg;
                } else if (username.length() < 6) {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Username must be at least 6 character                                      |" << endl;
                    goto enterUsernameErrMsg;
                }

                // Check username in database
                query = "SELECT username FROM person WHERE username = ?";
                statement stmt = create_statement(db, query);
                parameter(stmt.get(), 1, username);

                if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Username already exist. Please enter a different one!                      |" << endl;
                    goto enterUsernameErrMsg;
                }

                clearScreen();
                cout << endl << endl;
                usernameConfirmErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|                                                                              |" << endl;
                cout << printUI("| Full Name: ", full_name);
                cout << printUI("| Student Number: ", str_number);
                cout << printUI("| Username: ", username);
                cout << "|                                                                              |" << endl;
                cout << "| Do you want to continue with this Username? (Y/n)                            |" << endl;
                cout << "|                                                                              |" << endl;
                cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                cout << "| Input: ";
                getline(cin, option);

                if (option == "y" || option == "Y" || option.length() == 0) {
                    clearScreen();
                    enterPassword:
                    cout << endl << endl;
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| / Please create a password!                                                  |" << endl;
                    cout << printUI("| Full Name: ", full_name);
                    cout << printUI("| Student Number: ", str_number);
                    cout << printUI("| Username: ", username);
                    cout << "| Password: ";

                    // Hide user input
                    password = getpassword("");

                    clearScreen();
                    cout << endl << endl;
                    passwordConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "|                                                                              |" << endl;
                    cout << printUI("| Full Name: ", full_name);
                    cout << printUI("| Student Number: ", str_number);
                    cout << printUI("| Username: ", username);
                    cout << printUI("| Password: ", printAsterisk(password.length()));
                    cout << "|                                                                              |" << endl;
                    cout << "| Is all the data above is correct? (Y/n)                                      |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                    cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                    cout << "| Input: ";
                    getline(cin, option);

                    if (option == "y" || option == "Y" || option.length() == 0) {
                        // Create Account
                        query = R"(INSERT INTO person (number, name, username, password, status)
                        VALUES (?, ?, ?, ?, ?))";
                        statement stmt = create_statement(db, query);
                        parameter(stmt.get(), 1, number);
                        parameter(stmt.get(), 2, full_name);
                        parameter(stmt.get(), 3, username);
                        parameter(stmt.get(), 4, password);
                        parameter(stmt.get(), 5, status);

                        // sqlite3_step will return SQLITE_DONE when all the statements are successfully executed 
                        if (sqlite3_step(stmt.get()) == SQLITE_DONE) {
                            clearScreen();
                            cout << "+------------------------------------------------------------------------------+" << endl;
                            cout << "| > Accout successfully created                                                |" << endl;
                            goto loginMenuErrMsg;
                        } else {
                            clearScreen();
                            cout << "+------------------------------------------------------------------------------+" << endl;
                            cout << "| > Error, can't create account                                                |" << endl;
                            goto loginMenuErrMsg;
                        }
                        
                    } else if (option == "n" || option == "N") {
                        clearScreen();
                        goto enterPassword;
                    } else if (option == "9") {
                        clearScreen();
                        goto enterUsername;
                    } else if (option == "0") {
                        clearScreen();
                        exit(EXIT_SUCCESS);
                    } else {
                        clearScreen();
                        cout << "+------------------------------------------------------------------------------+" << endl;
                        cout << "| > Invalid option. Please enter the available option!                         |" << endl;
                        goto passwordConfirmErrMsg;
                    }
                
                // If-else from enter username
                } else if (option == "n" || option == "N") {
                    clearScreen();
                    goto enterUsername;
                } else if (option == "9") {
                    clearScreen();
                    goto enterNumber;
                } else if (option == "0") {
                    clearScreen();
                    exit(EXIT_SUCCESS);
                } else {
                    clearScreen();
                    cout << "+------------------------------------------------------------------------------+" << endl;
                    cout << "| > Invalid option. Please enter the available option!                         |" << endl;
                    goto usernameConfirmErrMsg;
                }
                
            // If-else from enter (student) number
            } else if (option == "n" || option == "N") {
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
                goto numberConfirmErrMsg;
            }
            
        // If-else from enter enter status & full name
        } else if (option == "n" || option == "N") {
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
            goto fullNameConfirmErrMsg;
        }

    // If-else from login menu   
    } else if (option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    } else {
        clearScreen();
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| > Invalid option! Please enter the available option.                         |" << endl;
        goto loginMenuErrMsg;
    }
    return login_info;
}

// Query to create default sql tables (schema).
constexpr const char* default_schema() {
    return R"(
    CREATE TABLE IF NOT EXISTS person (
        number INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        name TEXT NOT NULL,
        username TEXT NOT NULL,
        password TEXT NOT NULL,
        status TEXT NOT NULL
    );

    CREATE UNIQUE INDEX IF NOT EXISTS number ON person(number);
    CREATE UNIQUE INDEX IF NOT EXISTS username ON person(username);

    CREATE TABLE IF NOT EXISTS subject_list (
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        lecturer_number INTEGER NOT NULL,
        name TEXT NOT NULL,
        sks TEXT NOT NULL,
        FOREIGN KEY(lecturer_number) REFERENCES person(number)
    );

    CREATE TABLE IF NOT EXISTS score (
        student_number INTEGER NOT NULL,
        semester INTEGER NOT NULL,
        subject_id INTEGER NOT NULL,
        score INTEGER NOT NULL,
        grade TEXT NOT NULL,
	FOREIGN KEY(student_number) REFERENCES person(number)
	FOREIGN KEY(subject_id) REFERENCES subject_list(id)
    );
    )";
}