#include <iostream>
#include <cstring>
#include <string>
#include <memory>
#include "sqlite3/sqlite3.h"
#include "sqlite_query.h"

using namespace std;

// Store login info
struct loginInfo {
    string status;
    string name;
    long number;
};

// #Function prototype (forward declaration)#
constexpr const char* default_schema();
void generateSpace(string variable, string same_line_output);
void printAsterisk(int length);
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

// Generate space for neat interface.
void generateSpace(string variable, string same_line_output) {
    // 80 is the total number of character per line in the interface
    size_t length = 80 - (strlen(variable.c_str()) + strlen(same_line_output.c_str()));
    for (int i = 1; i < length; i++) {
        cout << " ";
    }
}

// Print asterisk as many as a string variable length
void printAsterisk(size_t length) {
    for (int i = 0; i < length; i++) {
        cout << "*";
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
        string username, password;

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
        cin.ignore();
        getline(cin, password);
        

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
        string status, full_name, number, username, password;
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
            cin.ignore();
            goto registerOptionErrMsg;
        }

        enterFullName:
        cout << endl << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Please enter your Full Name!                                                 |" << endl;
        cout << "| Full Name: ";
        cin.ignore();
        getline(cin, full_name);

        // TODO: Capitalize first alphabet in each words

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

        if (option == "y" || option == "Y") {
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

            // TODO: Improve the algorithm to make sure all input is number
            // Make sure user input is only number
            if ((number.find_first_of("0123456789") == string::npos) == true) {
                clearScreen();
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Student Number must be all numerical                                       |" << endl;
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

            if (option == "y" || option == "Y") {
                // Check student number in database
                string query = "SELECT number FROM person WHERE number = ?";
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
                cin.ignore();
                getline(cin, username);

                // TODO: Make sure username has no space

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

                if (option == "y" || option == "Y") {
                    //input password
                    clearScreen();
                    enterPassword:
                    cout << endl << endl;
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| / Please create a password!                                                  |" << endl;
                    cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                    cout << "| Student Number: " << number, generateSpace(number, "| Student Number: "), cout << "|\n";
                    cout << "| Username: " << username, generateSpace(username, "| Username: "), cout << "|\n";
                    cout << "| Password: ";
                    cin.ignore();
                    getline(cin, password);

                    clearScreen();
                    cout << endl << endl;
                    passwordConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "|                                                                              |" << endl;
                    cout << "| Full Name: " << full_name, generateSpace(full_name, "| Full Name: "), cout << "|\n";
                    cout << "| Student Number: " << number, generateSpace(number, "| Student Number: "), cout << "|\n";
                    cout << "| Username: " << username, generateSpace(username, "| Username: "), cout << "|\n";
                    cout << "| Password: ", printAsterisk(password.length()), generateSpace(password, "| Password: "), cout << "|\n";
                    cout << "|                                                                              |" << endl;
                    cout << "| Is all the data above is correct? (y/n)                                      |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                    cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                    cout << "| Input: ";
                    cin >> option;

                    if (option == "y" || option == "Y") {
                        clearScreen();
                        cout << "+------------------------------------------------------------------------------+" << endl;
                        cout << "| > Accout successfully created                                                |" << endl;
                        cin.ignore();
                        goto loginMenuErrMsg;
                    } else if (option == "n" || option == "N") {
                        clearScreen();
                        goto registerOption;
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
                        cin.ignore();
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
                    cin.ignore();
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
                cin.ignore();
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
            cin.ignore();
            goto fullNameConfirmErrMsg;
        }

    // If-else from login menu   
    } else if (option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    } else {
        clearScreen();
        cin.ignore();
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