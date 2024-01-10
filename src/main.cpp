#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <windows.h>
#include <iomanip>
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
void clearScreen();
loginInfo showLoginMenu(sqlite3* db);
bool showStudentMenu(sqlite3* db, loginInfo login_info);
bool showLecturerMenu(sqlite3* db, loginInfo login_info);


const char* dir = R"(D:\.alprog\final-uas-fix\student-score-management-system\main.db)";

int main() {
    clearScreen();

    

    // Create new (link existing) database.
    database db = open_database("main.db");
    // Create (if not created yet) default sqlite tables (defining .schema)
    execute(db.get(), default_schema());


    // ##Front-End##
    loginMenu:
    loginInfo loginInfo = showLoginMenu(db.get());

    if (loginInfo.status == "lecturer") {
        if (showLecturerMenu(db.get(), loginInfo) == true) {
            clearScreen();
            goto loginMenu;
        }
    } else {
        if (showStudentMenu(db.get(), loginInfo) == true) {
            clearScreen();
            goto loginMenu;
        }
    }
    

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

static int callback(void* NotUsed, int argc, char** argv, char** azColName){

    for (int i = 0; i < argc; i++) {
		// column name and value
		cout << azColName[i] << ": " << argv[i] << endl;
	}

	cout << endl;

	return 0;
}

static int selectData(const char* s){

    sqlite3* DB;
	char* messageError;

	string sql = "SELECT subject_list.name, score.score, score.grade FROM subject_list FULL OUTER JOIN score ON subject_list.id = score.subject_id WHERE score.student_number = ? AND score.semester = ?;";

	int exit = sqlite3_open(s, &DB);
	/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
	exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

	if (exit != SQLITE_OK) {
		cerr << "Error in selectData function." << endl;
		sqlite3_free(messageError);
	}
	else
		cout << "Records selected Successfully!" << endl;

	return 0;
}

// void retrieve(){
//     string query = "SELECT subject_list.name, score.score, score.grade FROM subject_list FULL OUTER JOIN score ON subject_list.id = score.subject_id WHERE score.student_number = ? AND score.semester = ?;";
//     string result = sqlite3_prepare16_v2(db, query.c_str(), -1, &stmt, NULL);
//     statement stmt = create_statement(db, query);
//     parameter(stmt.get(), 1, login_info.number);
//     parameter(stmt.get(), 2, n_smt);
// }


    // #Front-End#
// Login & register menu
// Return user's name, student/lecturer number, and user's status inside of loginInfo (struct)
loginInfo showLoginMenu(sqlite3* db) {
    string option;
    loginInfo login_info;
    loginMenu:
    cout << endl << endl;
    loginMenuErrMsg:
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "| > Choose one option below by typing the number (0,1,2,.. )                   |" << endl;
    cout << "| 1. Login                                                                     |" << endl;
    cout << "| 2. Register                                                                  |" << endl;
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
        cout << "| > Insert your username                                                       |" << endl;
        cout << "| Username:    ";
        getline(cin, username);

        clearScreen();
        cout << endl << endl;
        cout << "+------------------------------------------------------------------------------+"<<endl;
        cout << "| > Insert your password                                                       |" << endl;
        cout << printUI("| Username:    ", username);
        cout << "| Password:    ";
        password = getpassword("");

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
                clearScreen();
                goto loginFailed;
                
            }
        } else {
            clearScreen();
            loginFailed:
            cout << printUIErr("Invalid username or password");
            loginFailedErrMsg:
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                                                              |" << endl;
            cout << printUI("| Username:    ", username);
            cout << printUI("| Password:    ", printAsterisk(password.length()));
            cout << "|                                                                              |" << endl;
            cout << "| > Do you want to try to login again? (Y/n)                                   |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            getline(cin, option);

            if (option == "y" || option == "Y" || option.length() == 0) {
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
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto loginFailedErrMsg;
            }
        }
    }
    else if(option == "2") {
        string status, full_name, str_number, username, password, password2;
        long long number;
        clearScreen();
        registerOption:
        cout << endl << endl;
        registerOptionErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| > Are you a Student or a Lecturer?                                           |" << endl;
        cout << "| 1. Student                                                                   |" << endl;
        cout << "| 2. Lecturer                                                                  |" << endl;
        cout << "|                                                                              |" << endl;
        cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
        cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
        cout << "| Input: ";
        getline(cin, option);

        // Get status
        if (option == "1") {
            clearScreen();
            status = "student";
        } else if (option == "2") {
            clearScreen();
            status = "lecturer";
        } else if (option == "7") {
            clearScreen();
            goto loginMenu;
        } else if (option == "9") {
            clearScreen();
            goto loginMenu;
        } else if (option == "0") {
            clearScreen();
            exit(EXIT_SUCCESS);
        } else {
            clearScreen();
            cout << printUIErr("Invalid option. Please enter the available option!");
            goto registerOptionErrMsg;
        }

        enterFullName:
        cout << endl << endl;
        enterFullNameErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| > Please enter your Full Name!                                               |" << endl;
        cout << "| Full Name:           ";
        getline(cin, full_name);

        // Full_name must be all letters and at least 1 character
        if(regex_match(full_name, regex("([a-zA-Z .]*)")) == false) { 
            clearScreen();
            cout << printUIErr("Full Name must be all letters");
            goto enterFullNameErrMsg;
        } else if (oneCharExist(full_name) == false) {
            clearScreen();
            cout << printUIErr("Full Name cannot be empty. Please insert at least 1 character");
            goto enterFullNameErrMsg;
        }

        full_name = toDefaultTextStyle(full_name);

        clearScreen();
        cout << endl << endl;
        fullNameConfirmErrMsg:
        cout << "+------------------------------------------------------------------------------+"<<endl;
        cout << "|                                                                              |" << endl;
        cout << printUI("| Full Name:           ", full_name);
        cout << "|                                                                              |" << endl;
        cout << "| > Do you want to continue with this Name? (Y/n)                              |" << endl;
        cout << "|                                                                              |" << endl;
        cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
        cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
        cout << "| Input: ";
        getline(cin, option);

        if (option == "y" || option == "Y" || option.length() == 0) {
            clearScreen();
            enterNumber:
            cout << endl << endl;
            enterNumberErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Please enter your Student/Lecturer Number!                                 |" << endl;
            cout << printUI("| Full Name:           ", full_name);
            cout << "| Number:              ";
            getline(cin, str_number);

            // Make sure user input is only number
            if(regex_match(str_number, regex("([0-9]*)")) == false) { 
                clearScreen();
                cout << printUIErr("Student/Lecturer Number must be all numerical with no space");
                goto enterNumberErrMsg;
            } else if (str_number.length() > 16) {
                clearScreen();
                cout << printUIErr("Student/Lecturer Number maximum length is 16");
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
                cout << printUIErr("Student/Lecturer Number already exist. Please enter a different one!");
                goto enterNumberErrMsg;
            }

            clearScreen();
            cout << endl << endl;
            numberConfirmErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "|                                                                              |" << endl;
            cout << printUI("| Full Name:           ", full_name);
            cout << printUI("| Number:              ", str_number);
            cout << "|                                                                              |" << endl;
            cout << "| > Do you want to continue with this Number? (Y/n)                            |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
            cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            getline(cin, option);

            if (option == "y" || option == "Y" || option.length() == 0) {
                clearScreen();
                enterUsername:
                cout << endl << endl;
                enterUsernameErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "| > Please create a Username!                                                  |" << endl;
                cout << printUI("| Full Name:           ", full_name);
                cout << printUI("| Number:              ", str_number);
                cout << "| Username:            ";
                getline(cin, username);

                // Make sure username only contains letters, numbers, and underscores with no space
                if(regex_match(username, regex("([a-zA-Z0-9_]*)")) == false) { 
                    clearScreen();
                    cout << printUIErr("Username can only contains numbers, letters, and underscores");
                    goto enterUsernameErrMsg;
                } else if (username.length() > 32) {
                    clearScreen();
                    cout << printUIErr("Username maximum length is 32 character");
                    goto enterUsernameErrMsg;
                } else if (username.length() < 6) {
                    clearScreen();
                    cout << printUIErr("Username must be at least 6 character");
                    goto enterUsernameErrMsg;
                }

                // Check username in database
                query = "SELECT username FROM person WHERE username = ?";
                statement stmt = create_statement(db, query);
                parameter(stmt.get(), 1, username);

                if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                    clearScreen();
                    cout << printUIErr("Username already exist. Please enter a different one!");
                    goto enterUsernameErrMsg;
                }

                clearScreen();
                cout << endl << endl;
                usernameConfirmErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|                                                                              |" << endl;
                cout << printUI("| Full Name:           ", full_name);
                cout << printUI("| Number:              ", str_number);
                cout << printUI("| Username:            ", username);
                cout << "|                                                                              |" << endl;
                cout << "| > Do you want to continue with this Username? (Y/n)                          |" << endl;
                cout << "|                                                                              |" << endl;
                cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
                cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
                cout << "| Input: ";
                getline(cin, option);

                if (option == "y" || option == "Y" || option.length() == 0) {
                    clearScreen();
                    enterPassword:
                    cout << endl << endl;
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Please create a password!                                                  |" << endl;
                    cout << printUI("| Full Name:           ", full_name);
                    cout << printUI("| Number:              ", str_number);
                    cout << printUI("| Username:            ", username);
                    cout << "| Password:            ";
                    getline(cin, password);

                    clearScreen();
                    enterPassword2:
                    cout << endl << endl;
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "| > Please create a password!                                                  |" << endl;
                    cout << printUI("| Full Name:           ", full_name);
                    cout << printUI("| Number:              ", str_number);
                    cout << printUI("| Username:            ", username);
                    cout << printUI("| Password:            ", printAsterisk(password.length()));
                    cout << "| Confirm Password:    ";
                    getline(cin, password2);

                    if (password != password2) {
                        clearScreen();
                        cout << printUIErr("Password does not match");
                        passwordMissmatch:
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "|                                                                              |" << endl;
                        cout << printUI("| Full Name:           ", full_name);
                        cout << printUI("| Number:              ", str_number);
                        cout << printUI("| Username:            ", username);
                        cout << printUI("| Password:            ", printAsterisk(password.length()));
                        cout << printUI("| Confirm Password:    ", password2);
                        cout << "|                                                                              |" << endl;
                        cout << "| > Do you want to enter a new password? (Y/n)                                 |" << endl;
                        cout << "|                                                                              |" << endl;
                        cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
                        cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
                        cout << "| Input: ";
                        getline(cin, option);

                        if (option == "y" || option == "Y" || option.length() == 0) {
                            clearScreen();
                            goto enterPassword;
                        } else if (option == "n" || option == "N") {
                            clearScreen();
                            goto enterPassword2;
                        } else if (option == "7") {
                            clearScreen();
                            goto loginMenu;
                        } else if (option == "9") {
                            clearScreen();
                            goto enterPassword;
                        } else if (option == "0") {
                            clearScreen();
                            exit(EXIT_SUCCESS);
                        } else {
                            clearScreen();
                            cout << printUIErr("Invalid option. Please enter the available option!");
                            goto passwordMissmatch;
                        }

                    }

                    clearScreen();
                    cout << endl << endl;
                    passwordConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+"<<endl;
                    cout << "|                                                                              |" << endl;
                    cout << printUI("| Full Name:           ", full_name);
                    cout << printUI("| Number:              ", str_number);
                    cout << printUI("| Username:            ", username);
                    cout << printUI("| Password:            ", printAsterisk(password.length()));
                    cout << "|                                                                              |" << endl;
                    cout << "| > Is all the data above is correct? (Y/n)                                    |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|  7. Prev. Menu  |                                    |  9. Back  |  0. Exit  |" << endl;
                    cout << "+-----------------+------------------------------------+-----------+-----------+" << endl;
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
                            cout << printUIErr("Accout successfully created");
                            goto loginMenuErrMsg;
                        } else {
                            clearScreen();
                            cout << printUIErr("Error, can't create account");
                            goto loginMenuErrMsg;
                        }
                        
                    } else if (option == "n" || option == "N") {
                        clearScreen();
                        goto enterPassword;
                    } else if (option == "7") {
                        clearScreen();
                        goto loginMenu;
                    } else if (option == "9") {
                        clearScreen();
                        goto enterUsername;
                    } else if (option == "0") {
                        clearScreen();
                        exit(EXIT_SUCCESS);
                    } else {
                        clearScreen();
                        cout << printUIErr("Invalid option. Please enter the available option!");
                        goto passwordConfirmErrMsg;
                    }
                
                // If-else from enter username
                } else if (option == "n" || option == "N") {
                    clearScreen();
                    goto enterUsername;
                } else if (option == "7") {
                    clearScreen();
                    goto loginMenu;
                } else if (option == "9") {
                    clearScreen();
                    goto enterNumber;
                } else if (option == "0") {
                    clearScreen();
                    exit(EXIT_SUCCESS);
                } else {
                    clearScreen();
                    cout << printUIErr("Invalid option. Please enter the available option!");
                    goto usernameConfirmErrMsg;
                }
                
            // If-else from enter (student) number
            } else if (option == "n" || option == "N") {
                clearScreen();
                goto enterNumber;
            } else if (option == "7") {
                clearScreen();
                goto loginMenu;
            } else if (option == "9") {
                clearScreen();
                goto enterFullName;
            } else if (option == "0") {
                clearScreen();
                exit(EXIT_SUCCESS);
            } else {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto numberConfirmErrMsg;
            }
            
        // If-else from enter enter status & full name
        } else if (option == "n" || option == "N") {
            clearScreen();
            goto enterFullName;
        } else if (option == "7") {
            clearScreen();
            goto loginMenu;
        } else if (option == "9") {
            clearScreen();
            goto registerOption;
        } else if (option == "0") {
            clearScreen();
            exit(EXIT_SUCCESS);
        } else {
            clearScreen();
            cout << printUIErr("Invalid option. Please enter the available option!");
            goto fullNameConfirmErrMsg;
        }

    // If-else from login menu   
    } else if (option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    } else {
        clearScreen();
        cout << printUIErr("Invalid option. Please enter the available option!");
        goto loginMenuErrMsg;
    }
    return login_info;
}

// Student menu
// Return bool to check whether the user want to go back to login & register menu or not (true if yes)
bool showStudentMenu(sqlite3* db, loginInfo login_info) {
    string option;
    string login_number = to_string(login_info.number);
    menuMhs:
    clearScreen();
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "|                             WELCOME HOME STUDENT                             |" << endl;
    cout << "|                                                                              |" << endl;
    cout << printUI("| Student Number   :   ", login_number);
    cout << printUI("| Name             :   ", login_info.name);
    cout << "|                                                                              |" << endl;
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "|                                    MENU                                      |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "| 1. Show your semester score                                                  |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "+------------------------------------------------------------------+-----------+" << endl;
    cout << "|                                                                  |  0. Exit  |" << endl;
    cout << "+------------------------------------------------------------------+-----------+" << endl;
    cout << "| Input: ";
    cin >> option;

    if (option=="1")
    {
        int n_smt;

        clearScreen();
        menuMhsSmt:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "|                                                                              |" << endl;
        cout << printUI("| Student Number   :   ", login_number);
        cout << printUI("| Name             :   ", login_info.name);
        cout << "|                                                                              |" << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| Input n semester: ";
        cin >> n_smt;
        cout << "+------------------------------------------------------------------------------+" << endl;

        sqlite3_stmt *stmt;

        string str_log_num = to_string(login_info.number);
        string str_smt = to_string(n_smt);

        string sql = "SELECT subject_list.name, score.score, score.grade FROM subject_list FULL OUTER JOIN score ON subject_list.id = score.subject_id WHERE score.student_number = " + str_log_num + " AND score.semester = " + str_smt + ";";

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        clearScreen();
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "|                                                                              |" << endl;
        cout << printUI("| Student Number   :   ", login_number);
        cout << printUI("| Name             :   ", login_info.name);
        cout << "|                                                                              |" << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;

        if (n_smt < 9)
        {
            cout << "|                           YOUR SEMESTER "<<n_smt<<" SCORE                             |" << endl;
        } else{
            cout << "|                           YOUR SEMESTER "<<n_smt<<" SCORE                             |" << endl;
        }

        cout << "+--------------------------------------------------------------+-------+-------+" << endl;

        if (rc != SQLITE_OK) {
            cout << "error: " << sqlite3_errmsg(db) << endl;
        }
        cout << left << setw(63) << "| SUBJECT" << "| " << setw(6) << "SCORE" << "| " << setw(6) << "GRADE" << "|" << endl;
        cout << "+--------------------------------------------------------------+-------+-------+" << endl;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            cout << "| " << left << setw(61) << sqlite3_column_text(stmt, 0) << "| " << setw(6) << sqlite3_column_int(stmt, 1) << "| " << setw(6) << sqlite3_column_text(stmt, 2) << "|" << endl;
        }

        cout << "+--------------------------------------------------------------+-------+-------+" << endl;
        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
        cout << "|                                                      |  9. Menu  |  0. Exit  |" << endl;
        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
        cout << "| Input: ";
        cin >> option;


        if (option=="9")
        {
            clearScreen();
            goto menuMhs;
        }
        else if (option=="0")
        {
            exit(EXIT_SUCCESS);
        } 
    }
    else if (option=="0")
    {
        exit(EXIT_SUCCESS);
    } 
    else 
    {
        clearScreen();
        cout << printUIErr("Invalid option. Please enter the available option!");
        goto menuMhs;
    }
    return false;
} 

// Lecturer menu
// Return bool to check whether the user want to go back to login & register menu or not (true if yes)
bool showLecturerMenu(sqlite3* db, loginInfo login_info) {
    string option;
    int int_opt, sub_id;
    string login_number = to_string(login_info.number);
    clearScreen();
    cout << endl << endl;
    menuDsn:
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "|                            WELCOME HOME LECTURER                             |" << endl;
    cout << "|                                                                              |" << endl;
    cout << printUI("| Lecturer Number  :   ", login_number);
    cout << printUI("| Name             :   ", login_info.name);
    cout << "|                                                                              |" << endl;
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "|                                    MENU                                      |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "| 1. Show your student score                                                   |" << endl;
    cout << "| 2. Add student score                                                         |" << endl;
    cout << "| 3. Edit student score                                                        |" << endl;
    cout << "| 4. Delete student score                                                      |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "+------------------------------------------------------------------+-----------+" << endl;
    cout << "|                                                                  |  0. Exit  |" << endl;
    cout << "+------------------------------------------------------------------+-----------+" << endl;
    cout << "| Input: ";
    cin >> option;

    if(option=="1") {
        menuShow:
        clearScreen();
        cout << endl << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "|                                    SHOW                                      |" << endl;
        cout << "+------------------------------------------------------------------------------+" << endl;

        string query = R"(SELECT count(id) FROM subject_list WHERE lecturer_number = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, login_info.number);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {

            int rows = get_int_value(stmt.get(), 0);
            
            menuShowInput:
            clearScreen();
            cout << endl << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                    SHOW                                      |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                             CHOOSE YOUR CLASS                                |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            
            sqlite3_stmt *stmt;
            string str_log_num = to_string(login_info.number);
            string sql = "SELECT id, name FROM subject_list WHERE lecturer_number = " + str_log_num;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                cout << "error: " << sqlite3_errmsg(db) << endl;
            }
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                cout << "| " << sqlite3_column_int(stmt, 0) << ". " << left << setw(74) << sqlite3_column_text(stmt, 1) <<"|"<< endl;
            } 

            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> int_opt;

            if (int_opt==9)
            {
                clearScreen();
                goto menuDsn;
            }
            else if (int_opt==0)
            {
                exit(EXIT_SUCCESS);
            } 
            else if(int_opt>rows){
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto menuShowInput;
            }
            else if (int_opt==1)
            {
                sub_id = int_opt;
            }
            else if (int_opt==2)
            {
                sub_id = int_opt;
            }
            else if (int_opt==3)
            {
                sub_id = int_opt;                                    
            }
            else 
            {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto menuShowInput;
            }

            clearScreen();
            cout << endl << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                    SHOW                                      |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                   SCORE                                      |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            
            // sqlite3_stmt *stmt;
            string str_sub_id=to_string(sub_id);
            string sql2 = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id=" + str_sub_id + " ORDER BY score.student_number";
            int rc2 = sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt, NULL);

            if (rc2 != SQLITE_OK) {
                cout << "error: " << sqlite3_errmsg(db) << endl;
            }
            cout << left << setw(9) << "| NIM" << "| " << setw(52) << "NAME" << "| " << setw(6) << "SCORE" << "| " << setw(6) << "GRADE" << "|" << endl;

            while ((rc2 = sqlite3_step(stmt)) == SQLITE_ROW) {
                cout << "| " << left << setw(7) << sqlite3_column_int(stmt,0) << "| " << setw(52) << sqlite3_column_text(stmt, 1) << "| " << setw(6) << sqlite3_column_int(stmt, 2) << "| " << setw(6) << sqlite3_column_text(stmt, 3) << "|" << endl;
            }    
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> option;

            if (option=="9")
            {
                clearScreen();
                goto menuShowInput;
            }
            else if (option=="0")
            {
                exit(EXIT_SUCCESS);
            } 
        }

        
    }
    else if(option=="2") {
        string str_smt, str_name, str_nim, str_score, str_grade, str_sub;
        int sub_id, smt, score;
        long long nim;

        clearScreen();
        chooseClass:
        cout << endl << endl;
        chooseClassErrMsg:
        string query = R"(SELECT count(id) FROM subject_list WHERE lecturer_number = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, login_info.number);

        if(sqlite3_step(stmt.get()) == SQLITE_ROW){
            
            int rows = get_int_value(stmt.get(), 0);

            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                    ADD                                       |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                             CHOOSE YOUR CLASS                                |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            
            sqlite3_stmt *stmt;
            string str_log_num = to_string(login_info.number);
            string sql = "SELECT id, name FROM subject_list WHERE lecturer_number = " + str_log_num;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                cout << "error: " << sqlite3_errmsg(db) << endl;
            }
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                cout << "| " << sqlite3_column_int(stmt, 0) << ". " << left << setw(74) << sqlite3_column_text(stmt, 1) <<"|"<< endl;
            } 

            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> sub_id;

            if (sub_id == 9) {
                clearScreen();
                goto menuDsn;
            } else if (sub_id == 0) {
                exit(EXIT_SUCCESS);
            } else if(sub_id > rows) {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto chooseClassErrMsg;
            }

            clearScreen();
            menuAddSmt:
            cout << endl << endl;
            menuAddSmtErrMsg:
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "|      Please enter the Semester!                                              |" << endl;
            cout << "| Semester         :   ";
            cin >> smt;

            str_smt = to_string(smt);

            if (smt > 0 && smt < 15)
            {
                clearScreen();
                menuAddNim:
                cout << endl << endl;
                menuAddNimErrMsg:
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|      Please enter the Student Number!                                        |" << endl;
                cout << printUI("| Semester         :   ", str_smt);
                cout << "| Student Number   :   ";
                cin >> nim;
                str_nim = to_string(nim);

                string query = "SELECT number, name FROM person WHERE status = 'student' AND number = ?";
                statement stmt = create_statement(db, query);
                parameter(stmt.get(), 1, nim);

                if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                    string query = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id=? AND score.semester=? AND score.student_number=? ORDER BY score.student_number;";
                    statement stmt = create_statement(db, query);
                    parameter(stmt.get(), 1, sub_id);
                    parameter(stmt.get(), 2, smt);
                    parameter(stmt.get(), 3, nim);

                    if (sqlite3_step(stmt.get()) == SQLITE_ROW)
                    {
                        clearScreen();
                        cout << printUIErr("This Student Number already has a score. Please enter a different one!");
                        goto menuAddNimErrMsg;
                    }

                    str_name = get_text_value(stmt.get(), 1);
                    
                    clearScreen();
                        cout << endl << endl;
                        nimConfirmErrMsg:
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "|                                                                              |" << endl;
                        cout << printUI("| Semester         :   ", str_smt);
                        cout << printUI("| Student Number   :   ", str_nim);
                        cout << printUI("| Student Name     :   ", str_name);
                        cout << "|                                                                              |" << endl;
                        cout << "| Do you want to continue with this student? (Y/n)                             |" << endl;
                        cout << "|                                                                              |" << endl;
                        cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                        cout << "| Input: ";
                        cin >> option;

                        
                        if(option == "y" || option == "Y" || option.length() == 0) {

                            clearScreen();
                            menuAddScore:
                            cout << endl << endl;
                            menuAddScoreErrMsg:
                            cout << "+------------------------------------------------------------------------------+"<<endl;
                            cout << "|      Please enter the Score!                                                 |" << endl;
                            cout << printUI("| Semester         :   ", str_smt);
                            cout << printUI("| Student Number   :   ", str_nim);
                            cout << printUI("| Student Name     :   ", str_name);
                            cout << "| Score            :   ";
                            cin >> score;
                            str_score = to_string(score);

                            
                            if (score > 100) {
                                clearScreen();
                                cout << printUIErr("Invalid input. Please enter another value!");
                                goto menuAddScoreErrMsg;                                        
                            } else if (score > 80) {
                                str_grade = "A";
                            } else if (score > 60) {
                                str_grade = "B";
                            } else if (score > 40) {
                                str_grade = "C";
                            } else if (score > 20) {
                                str_grade = "D";
                            } else if (score > 0) {
                                str_grade = "E";
                            } else if (score == 0) {
                                str_grade = "-";
                            } else {
                                clearScreen();
                                cout << printUIErr("Invalid input. Please enter another value!");
                                goto menuAddScoreErrMsg;
                            }

                            clearScreen();
                            scoreConfirm:
                            cout << endl << endl;
                            scoreConfirmErrMsg:
                            cout << "+------------------------------------------------------------------------------+"<<endl;
                            cout << "|                                                                              |" << endl;
                            cout << printUI("| Semester         :   ", str_smt);
                            cout << printUI("| Student Number   :   ", str_nim);
                            cout << printUI("| Student Name     :   ", str_name);
                            cout << printUI("| Score            :   ", str_score);
                            cout << printUI("| Grade            :   ", str_grade);
                            cout << "|                                                                              |" << endl;
                            cout << "| Do you want to continue with this score and grade? (Y/n)                     |" << endl;
                            cout << "|                                                                              |" << endl;
                            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                            cout << "| Input: ";
                            cin >> option;

                            if (option == "y" || option == "Y" || option.length() == 0)
                            {
                                // Create Account
                                query = R"(INSERT INTO score (student_number, semester, subject_id, score, grade)
                                VALUES (?, ?, ?, ?, ?))";
                                statement stmt = create_statement(db, query);
                                parameter(stmt.get(), 1, nim);
                                parameter(stmt.get(), 2, smt);
                                parameter(stmt.get(), 3, sub_id);
                                parameter(stmt.get(), 4, score);
                                parameter(stmt.get(), 5, str_grade);

                                // sqlite3_step will return SQLITE_DONE when all the statements are successfully executed 
                                if (sqlite3_step(stmt.get()) == SQLITE_DONE) {
                                    clearScreen();
                                    cout << printUIErr("Student's score successfully added");
                                    goto menuDsn;
                                } else {
                                    clearScreen();
                                    cout << printUIErr("Error, can't add student's score");
                                    goto menuDsn;
                                }
                            //elif ngisi score
                            } else if (option == "n" || option == "N") {
                                clearScreen();
                                goto menuAddScore;
                            } else if (option == "9") {
                                clearScreen();
                                goto menuAddNim;
                            } else if (option == "0") {
                                clearScreen();
                                exit(EXIT_SUCCESS);
                            } else {
                                clearScreen();
                                cout << printUIErr("Invalid option. Please enter the available option!");
                                goto scoreConfirmErrMsg;
                            }

                        // elif ngisi nim
                        } else if (option == "n" || option == "N") {
                            clearScreen();
                            goto menuAddNim;
                        } else if (option == "9") {
                            clearScreen();
                            goto menuAddSmt;
                        } else if (option == "0") {
                            clearScreen();
                            exit(EXIT_SUCCESS);
                        } else {
                            clearScreen();
                            cout << printUIErr("Invalid option. Please enter the available option!");
                            goto nimConfirmErrMsg;
                        }
                    


                } 
                else{
                    clearScreen();
                    cout << printUIErr("Student Number doesn't exist. Please enter a different one!");
                    goto menuAddNimErrMsg;

                }
            } else {
                clearScreen();
                cout << printUIErr("Semester doesn't exist. Please enter a different one!");
                goto menuAddSmtErrMsg;
            }
        } else {
            clearScreen();
            goto menuDsn;
        }
    }
    else if(option=="3"){

        string str_smt, str_name, str_nim, str_score, str_grade, str_sub_id, str_n_score, str_n_grade;
        int sub_id, smt, score, n_score;
        long long nim;

        clearScreen();
        chooseClassEdit:
        cout << endl << endl;
        chooseClassEditErrMsg:
        string query = R"(SELECT count(id) FROM subject_list WHERE lecturer_number = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, login_info.number);

        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {

            int rows = get_int_value(stmt.get(), 0);

            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                    EDIT                                      |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                             CHOOSE YOUR CLASS                                |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            
            sqlite3_stmt *stmt;
            string str_log_num = to_string(login_info.number);
            string sql = "SELECT id, name FROM subject_list WHERE lecturer_number = " + str_log_num;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                cout << "error: " << sqlite3_errmsg(db) << endl;
            }
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                cout << "| " << sqlite3_column_int(stmt, 0) << ". " << left << setw(74) << sqlite3_column_text(stmt, 1) <<"|"<< endl;
            } 

            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> sub_id;

            str_sub_id = to_string(sub_id);

            if (sub_id == 9) {
                clearScreen();
                goto menuDsn;
            } else if (sub_id == 0) {
                exit(EXIT_SUCCESS);
            } else if(sub_id > rows) {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto chooseClassEditErrMsg;
            }

            // clearScreen();
            menuAddSmtEdit:
            // cout << endl << endl;
            inputSmtEditErrMsg:
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|      Please enter the Semester!                                              |" << endl;
            cout << "| Semester         :   ";
            cin >> smt;

            str_smt = to_string(smt);

            if (smt > 0 && smt < 15) {

                clearScreen();
                inputNimEdit:
                cout << endl << endl;
                inputNimEditErrMsg:
                cout << "+------------------------------------------------------------------------------+" << endl;
                cout << "|                                  EDIT SCORE                                  |" << endl;
                cout << "+------------------------------------------------------------------------------+" << endl;
                
                // sqlite3_stmt *stmt;
                string sql = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id = " + str_sub_id + " AND score.semester = " + str_smt + " ORDER BY score.student_number";
                // cout << sql << endl;
                int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

                if (rc != SQLITE_OK) {
                    cout << "error: " << sqlite3_errmsg(db) << endl;
                }
                cout << left << setw(9) << "| NIM" << "| " << setw(52) << "NAME" << "| " << setw(6) << "SCORE" << "| " << setw(6) << "GRADE" << "|" << endl;

                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                    cout << "| " << left << setw(7) << sqlite3_column_int(stmt,0) << "| " << setw(52) << sqlite3_column_text(stmt, 1) << "| " << setw(6) << sqlite3_column_int(stmt, 2) << "| " << setw(6) << sqlite3_column_text(stmt, 3) << "|" << endl;
                }    
                
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|      Please Enter the Student Number!                                        |" << endl;
                cout << "| Student Number   :   ";
                cin >> nim;
                str_nim = to_string(nim);

                // string query = "SELECT number, name FROM person WHERE status = 'student' AND number = ?";
                string query = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id = ? AND score.semester = ? AND score.student_number = ?";
                statement stmt = create_statement(db, query);
                parameter(stmt.get(), 1, sub_id);
                parameter(stmt.get(), 2, smt);
                parameter(stmt.get(), 3, nim);

                if (sqlite3_step(stmt.get()) == SQLITE_ROW)
                {
                    str_name = get_text_value(stmt.get(), 1);
                    score = get_int_value(stmt.get(), 2);
                    str_grade = get_text_value(stmt.get(), 3);

                    str_score = to_string(score);

                    clearScreen();
                    inputNimConfirm:
                    cout << endl << endl;
                    inputNimConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+" << endl;
                    cout << "|                                   EDIT SCORE                                 |" << endl;
                    cout << "+------------------------------------------------------------------------------+" << endl; 
                    cout << "|                                                                              |" << endl;
                    cout << printUI("| Semester         :   ", str_smt);
                    cout << printUI("| Student Number   :   ", str_nim);
                    cout << printUI("| Student Name     :   ", str_name);
                    cout << printUI("| Score            :   ", str_score);
                    cout << printUI("| Grade            :   ", str_grade);
                    cout << "|                                                                              |" << endl;
                    cout << "| Do you want to continue with this student? (Y/n)                             |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                    cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                    cout << "| Input: ";
                    cin >> option;

                    if(option == "y" || option == "Y" || option.length() == 0) {

                        clearScreen();
                        newScore:
                        cout << endl << endl;
                        newScoreErrMsg:
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "|      Please enter the new score!                                             |" << endl;
                        cout << printUI("| Semester         :   ", str_smt);
                        cout << printUI("| Student Number   :   ", str_nim);
                        cout << printUI("| Student Name     :   ", str_name);
                        cout << printUI("| Score            :   ", str_score);
                        cout << printUI("| Grade            :   ", str_grade);
                        cout << "| New Score        :   ";
                        cin >> n_score;
                        str_n_score = to_string(n_score);
                        
                        if (n_score > 100) {
                            clearScreen();
                            cout << printUIErr("Invalid input. Please enter another value!");
                            goto newScoreErrMsg;                                        
                        } else if (n_score > 80) {
                            str_n_grade = "A";
                        } else if (n_score > 60) {
                            str_n_grade = "B";
                        } else if (n_score > 40) {
                            str_n_grade = "C";
                        } else if (n_score > 20) {
                            str_n_grade = "D";
                        } else if (n_score > 0) {
                            str_n_grade = "E";
                        } else if (n_score == 0) {
                            str_n_grade = "-";
                        } else {
                            clearScreen();
                            cout << printUIErr("Invalid input. Please enter another value!");
                            goto newScoreErrMsg;
                        }

                        clearScreen();
                        updateConfirm:
                        cout << endl << endl;
                        updateConfirmErrMsg:
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "|                                                                              |" << endl;
                        cout << printUI("| Semester         :   ", str_smt);
                        cout << printUI("| Student Number   :   ", str_nim);
                        cout << printUI("| Student Name     :   ", str_name);
                        cout << printUI("| Presvious Score  :   ", str_score);
                        cout << printUI("| Previous Grade   :   ", str_grade);
                        cout << printUI("| New Score        :   ", str_n_score);
                        cout << printUI("| New Grade        :   ", str_n_grade);
                        cout << "|                                                                              |" << endl;
                        cout << "| Do you want to continue with this new score and grade? (Y/n)                 |" << endl;
                        cout << "|                                                                              |" << endl;
                        cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                        cout << "| Input: ";
                        cin >> option;

                        if (option == "y" || option == "Y" || option.length() == 0)
                        {
                            // Update Score
                            query = R"(UPDATE score SET score = ?, grade = ? WHERE student_number = ?)";
                            statement stmt = create_statement(db, query);
                            parameter(stmt.get(), 1, n_score);
                            parameter(stmt.get(), 2, str_n_grade);
                            parameter(stmt.get(), 3, nim);

                            // sqlite3_step will return SQLITE_DONE when all the statements are successfully executed 
                            if (sqlite3_step(stmt.get()) == SQLITE_DONE) {
                                clearScreen();
                                cout << printUIErr("Student's score successfully updated");
                                goto menuDsn;
                            } else {
                                clearScreen();
                                cout << printUIErr("Error, can't update student's score");
                                goto menuDsn;
                            }
                        //elif ngisi score
                        } else if (option == "n" || option == "N") {
                            clearScreen();
                            goto newScore;
                        } else if (option == "9") {
                            clearScreen();
                            goto inputNimEdit;
                        } else if (option == "0") {
                            clearScreen();
                            exit(EXIT_SUCCESS);
                        } else {
                            clearScreen();
                            cout << printUIErr("Invalid option. Please enter the available option!");
                            goto updateConfirmErrMsg;
                        }

                    } else if (option == "n" || option == "N") {
                        clearScreen();
                        goto inputNimEdit;
                    } else if (option == "9") {
                        clearScreen();
                        goto inputNimEdit;
                    } else if (option == "0") {
                        clearScreen();
                        exit(EXIT_SUCCESS);
                    } else {
                        clearScreen();
                        cout << printUIErr("Invalid option. Please enter the available option!");
                        goto inputNimConfirmErrMsg;
                    }

                } else {
                    clearScreen();
                    cout << printUIErr("Student doesn't exist. Please enter a different one!");
                    goto inputNimEditErrMsg;
                }

            } else {
                clearScreen();
                cout << printUIErr("Semester doesn't exist. Please enter a different one!");
                goto chooseClassEditErrMsg;
            }

        } else {
            clearScreen();
            goto menuDsn;
        }
        
    }
    else if(option=="4"){
        string str_smt, str_name, str_nim, str_score, str_grade, str_sub_id, str_n_score, str_n_grade;
        int sub_id, smt, score, n_score;
        long long nim;

        clearScreen();
        chooseClassDel:
        cout << endl << endl;
        chooseClassDelErrMsg:
        string query = R"(SELECT count(id) FROM subject_list WHERE lecturer_number = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, login_info.number);

        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {

            int rows = get_int_value(stmt.get(), 0);

            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                                   DELETE                                     |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|                             CHOOSE YOUR CLASS                                |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            
            sqlite3_stmt *stmt;
            string str_log_num = to_string(login_info.number);
            string sql = "SELECT id, name FROM subject_list WHERE lecturer_number = " + str_log_num;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                cout << "error: " << sqlite3_errmsg(db) << endl;
            }
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                cout << "| " << sqlite3_column_int(stmt, 0) << ". " << left << setw(74) << sqlite3_column_text(stmt, 1) <<"|"<< endl;
            } 

            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------+-----------+-----------+" << endl;
            cout << "| Input: ";
            cin >> sub_id;

            str_sub_id = to_string(sub_id);

            if (sub_id == 9) {
                clearScreen();
                goto menuDsn;
            } else if (sub_id == 0) {
                exit(EXIT_SUCCESS);
            } else if(sub_id > rows) {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto chooseClassDelErrMsg;
            }

            // clearScreen();
            menuAddSmtDel:
            // cout << endl << endl;
            inputSmtDelErrMsg:
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "|      Please enter the Semester!                                              |" << endl;
            cout << "| Semester         :   ";
            cin >> smt;

            str_smt = to_string(smt);

            if (smt > 0 && smt < 15) {

                clearScreen();
                inputNimDel:
                cout << endl << endl;
                inputNimDelErrMsg:
                cout << "+------------------------------------------------------------------------------+" << endl;
                cout << "|                                   DELETE                                     |" << endl;
                cout << "+------------------------------------------------------------------------------+" << endl;
                
                // sqlite3_stmt *stmt;
                string sql = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id = " + str_sub_id + " AND score.semester = " + str_smt + " ORDER BY score.student_number";
                // cout << sql << endl;
                int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

                if (rc != SQLITE_OK) {
                    cout << "error: " << sqlite3_errmsg(db) << endl;
                }
                cout << left << setw(9) << "| NIM" << "| " << setw(52) << "NAME" << "| " << setw(6) << "SCORE" << "| " << setw(6) << "GRADE" << "|" << endl;

                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                    cout << "| " << left << setw(7) << sqlite3_column_int(stmt,0) << "| " << setw(52) << sqlite3_column_text(stmt, 1) << "| " << setw(6) << sqlite3_column_int(stmt, 2) << "| " << setw(6) << sqlite3_column_text(stmt, 3) << "|" << endl;
                }    
                
                cout << "+------------------------------------------------------------------------------+"<<endl;
                cout << "|      Please Enter the Student Number!                                        |" << endl;
                cout << "| Student Number   :   ";
                cin >> nim;
                str_nim = to_string(nim);

                // string query = "SELECT number, name FROM person WHERE status = 'student' AND number = ?";
                string query = "SELECT score.student_number, person.name, score.score, score.grade FROM score FULL OUTER JOIN person ON score.student_number = person.number WHERE score.subject_id = ? AND score.semester = ? AND score.student_number = ?";
                statement stmt = create_statement(db, query);
                parameter(stmt.get(), 1, sub_id);
                parameter(stmt.get(), 2, smt);
                parameter(stmt.get(), 3, nim);

                if (sqlite3_step(stmt.get()) == SQLITE_ROW)
                {
                    str_name = get_text_value(stmt.get(), 1);
                    score = get_int_value(stmt.get(), 2);
                    str_grade = get_text_value(stmt.get(), 3);

                    str_score = to_string(score);

                    clearScreen();
                    inputNimDelConfirm:
                    cout << endl << endl;
                    inputNimDelConfirmErrMsg:
                    cout << "+------------------------------------------------------------------------------+" << endl;
                    cout << "|                                   EDIT SCORE                                 |" << endl;
                    cout << "+------------------------------------------------------------------------------+" << endl; 
                    cout << "|                                                                              |" << endl;
                    cout << printUI("| Semester         :   ", str_smt);
                    cout << printUI("| Student Number   :   ", str_nim);
                    cout << printUI("| Student Name     :   ", str_name);
                    cout << printUI("| Score            :   ", str_score);
                    cout << printUI("| Grade            :   ", str_grade);
                    cout << "|                                                                              |" << endl;
                    cout << "| Do you want to continue with this student? (Y/n)                             |" << endl;
                    cout << "|                                                                              |" << endl;
                    cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                    cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                    cout << "| Input: ";
                    cin >> option;

                    if(option == "y" || option == "Y" || option.length() == 0) {

                        clearScreen();
                        deleteConfirm:
                        cout << endl << endl;
                        deleteConfirmErrMsg:
                        cout << "+------------------------------------------------------------------------------+"<<endl;
                        cout << "|                                                                              |" << endl;
                        cout << printUI("| Semester         :   ", str_smt);
                        cout << printUI("| Student Number   :   ", str_nim);
                        cout << printUI("| Student Name     :   ", str_name);
                        cout << printUI("| Score            :   ", str_score);
                        cout << printUI("| Grade            :   ", str_grade);
                        cout << "|                                                                              |" << endl;
                        cout << "| Do you want to delete this student score? (Y/n)                              |" << endl;
                        cout << "|                                                                              |" << endl;
                        cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
                        cout << "+------------------------------------------------------+-----------+-----------+" << endl;
                        cout << "| Input: ";
                        cin >> option;

                        if (option == "y" || option == "Y" || option.length() == 0)
                        {
                            // Update Score
                            query = R"(DELETE FROM score WHERE student_number = ?)";
                            statement stmt = create_statement(db, query);
                            parameter(stmt.get(), 1, nim);

                            // sqlite3_step will return SQLITE_DONE when all the statements are successfully executed 
                            if (sqlite3_step(stmt.get()) == SQLITE_DONE) {
                                clearScreen();
                                cout << printUIErr("Student's score successfully deleted");
                                goto menuDsn;
                            } else {
                                clearScreen();
                                cout << printUIErr("Error, can't delete student's score");
                                goto menuDsn;
                            }

                        //elif ngisi score
                        } else if (option == "n" || option == "N") {
                            clearScreen();
                            goto inputNimDelConfirm;
                        } else if (option == "9") {
                            clearScreen();
                            goto inputNimDelConfirm;
                        } else if (option == "0") {
                            clearScreen();
                            exit(EXIT_SUCCESS);
                        } else {
                            clearScreen();
                            cout << printUIErr("Invalid option. Please enter the available option!");
                            goto deleteConfirmErrMsg;
                        }

                    } else if (option == "n" || option == "N") {
                        clearScreen();
                        goto inputNimDelConfirm;
                    } else if (option == "9") {
                        clearScreen();
                        goto inputNimDelConfirm;
                    } else if (option == "0") {
                        clearScreen();
                        exit(EXIT_SUCCESS);
                    } else {
                        clearScreen();
                        cout << printUIErr("Invalid option. Please enter the available option!");
                        goto inputNimDelConfirmErrMsg;
                    }

                } else {
                    clearScreen();
                    cout << printUIErr("Student doesn't exist. Please enter a different one!");
                    goto inputNimDelErrMsg;
                }

            } else {
                clearScreen();
                cout << printUIErr("Semester doesn't exist. Please enter a different one!");
                goto inputSmtDelErrMsg;
            }

        } else {
            clearScreen();
            goto menuDsn;
        }
    }
    return false;
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