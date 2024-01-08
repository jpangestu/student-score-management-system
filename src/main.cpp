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
void clearScreen();
loginInfo showLoginMenu(sqlite3* db);
bool showStudentMenu(sqlite3* db, loginInfo loginInfo);
bool showLecturerMenu(sqlite3* db, loginInfo loginInfo);

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
    string s= "+------------------------------------------------------------------------------+\n| !> ";
    s += error_message;
    // 80 is the total number of character per line in the interface
    size_t length = 80 - (error_message.length() + 5); // +4 is for counting the "| > "
    for (size_t i = 1; i < length; i++) {
        s += " ";
        if (i == length - 1) {
            s += "|\n";
        }
    }  
    return s;
}

void printUIErr(string error_message, int max_table_length) {
    for (int i = 0; i < max_table_length; i++) {
        if (i == 0) {
            cout << "+";
        } else if (i == max_table_length - 1) {
            cout << "+\n";
        } else {
            cout << "-";
        }
    }

    string s = "| !> ";
    s += error_message;
    for (int i = 0; i < max_table_length; i++) {
        if (i == 0) {
            cout << "| !> " << error_message;
        } else if (i == max_table_length - 1) {
            cout << "|\n";
        } else {
            cout << " ";
        }
    }
    cout << s;
}

// Print out (one) table data *part of printTableData() function
void printColumn(string column_title, size_t max_length, bool left) {
    if (left == false) {
        cout << column_title;
        for (size_t i = 0; i < max_length - column_title.length(); i++) {
            cout << " ";

            if (i == max_length - column_title.length() - 1) {
                cout << " | ";
            }
        }
        if (column_title.length() == max_length) {
            cout << " | ";
        }
    } else {
        cout << "| " << column_title;
        for (size_t i = 0; i < max_length - column_title.length(); i++) {
            cout << " ";

            if (i == max_length - column_title.length() - 1) {
                cout << " | ";
            }
        }
        if (column_title.length() == max_length) {
            cout << " | ";
        }
    }
}

// Print out strip & plus *part of printTableData() function
void printStrip(size_t max_length, bool left) {
    if (left == true) {
        cout << "+";
        for (size_t i = 0; i < max_length + 2; i++) {
            cout << "-";

            if (i == max_length + 1) {
                cout << "+";
            }
        }
    } else {
        for (size_t i = 0; i < max_length + 2; i++) {
            cout << "-";

            if (i == max_length + 1) {
                cout << "+";
            }
        }
    }
}

// Print out data from your SQLite table
// Requires prepared sqlite3_stmt and total row of your SQLite table (do SELECT count(*) ...)
int printTableData(sqlite3_stmt* stmt, unsigned int total_row) {
    unsigned int total_column = sqlite3_column_count(stmt);

    // Data (column value) is stored in two different array because the data type different
    int int_data[total_row][total_column];
    string str_data[total_row][total_column];
    
    // Store/get column name from the sqlite table
    string column_name[total_column];
    for (int i = 0; i < total_column; i++) {
        column_name[i] = sqlite3_column_name(stmt, i);
    }

    // Set column name character length as the default max column value character length
    size_t max_col_value_length[total_column];
    for (size_t i = 0; i < total_column; i++) {
        max_col_value_length[i] = column_name[i].length();
    }

    // Get column value from sqlite table
    int index = 0;
    // The number of times sqlite3_step will return SQLITE_ROW will match the total_row
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < total_column; i++) {
            if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER) {
                int_data[index][i] = get_int_value(stmt, i);

                // Update max column value character length value (if needed)
                if (to_string(int_data[index][i]).length() > max_col_value_length[i]) {
                    max_col_value_length[i] = to_string(int_data[index][i]).length();
                }
            }
            if (sqlite3_column_type(stmt, i) == SQLITE_TEXT) {
                str_data[index][i] = get_text_value(stmt, i);

                // Update max column value character length value (if needed)
                if (str_data[index][i].length() > max_col_value_length[i]) {
                    max_col_value_length[i] = str_data[index][i].length();
                }
            }
        }
        index++;
    } 

    // Print out top strip
    for (int i = 0; i < total_column; i++) {
        bool left = false;
        if (i == 0) {
            left = true;
        }
        printStrip(max_col_value_length[i], left);
    }
    cout << endl;

    // Print out column name/title
        for (int i = 0; i < total_column; i++) {
        bool left = false;
        if (i == 0) {
            left = true;
        }
        printColumn(column_name[i], max_col_value_length[i], left);
    }
    cout << endl;

    // Print out strip after column name
    for (int i = 0; i < total_column; i++) {
        bool left = false;
        if (i == 0) {
            left = true;
        }
        printStrip(max_col_value_length[i], left);
    }
    cout << endl;

    // Print out column value
    for (int i = 0; i < total_row; i++) {
        for (int j = 0; j < total_column; j++) {
            // Check data existence because the data are stored in two different array
            bool left = false;
            if (!str_data[i][j].empty()) {
                if (j == 0) {
                    left = true;
                }
                printColumn(str_data[i][j], max_col_value_length[j], left);
            } else {
                if (j == 0) {
                    left = true;
                }
                printColumn(to_string(int_data[i][j]), max_col_value_length[j], left);
            }
        }
        cout << endl;
    }

    // Print out bottom strip
    for (int i = 0; i < total_column; i++) {
        bool left = false;
        if (i == 0) {
            left = true;
        }
        printStrip(max_col_value_length[i], left);
    }
    cout << endl;

    // Count max table character length (all column value length + all the spaces and '|'
    // 2 is for counting the "| " for each leftmost column in printColumn()
    int max_table_length = 2;

    for (size_t i = 0; i < total_column; i++) {
        max_table_length += max_col_value_length[i] + 3; // 3 is for counting the " | " in printColumn()
    }
    // - 1 is for ignoring the space after "+" or "|" in the rightmost column from printColumn()
    max_table_length -= 1;
    return max_table_length;
}

// Print out display option from student menu dynamically
void printStudentDisOpt(int max_table_length) {
    string exit = "  0. Exit  ", logout = "  7. Logout  ";

    // Print out option display
    for (int i = 0; i < max_table_length; i++) {
        if (i == 0) {
            cout << "|" << logout;
            i += logout.length(); 
        } else if (i == max_table_length - exit.length() - 1) { // - 1 is for counting the "|"
            cout << exit << "|";
            break;
        } else {
            cout << " ";
        }
    }
    cout << endl;

    // Print out bottom border
    for (int i = 0; i < max_table_length; i++) {
        if (i == 0) {
            cout << "+";
        } else if (i == max_table_length - 1) {
            cout << "+";
        } else {
            cout << "-";
        }
    }
    cout << endl;

    cout << "| Input: ";
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

// Print asterisk as many as a string variable length (to avoid printing out user's password)
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
    for (size_t i = 0; i < input.length(); i++) {
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
// Login & register menu
// Return user's name, student/lecturer number, and user's status inside of loginInfor (struct)
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
        bindVariable(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            query = R"(
            SELECT number, name, status FROM person WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            bindVariable(stmt.get(), 1, username);
            bindVariable(stmt.get(), 2, password);
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
            bindVariable(stmt.get(), 1, number);

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
                bindVariable(stmt.get(), 1, username);

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
                        bindVariable(stmt.get(), 1, number);
                        bindVariable(stmt.get(), 2, full_name);
                        bindVariable(stmt.get(), 3, username);
                        bindVariable(stmt.get(), 4, password);
                        bindVariable(stmt.get(), 5, status);

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
bool showStudentMenu(sqlite3* db, loginInfo loginInfo) {
    string query = R"(
    SELECT count(*)
    FROM score
    INNER JOIN subject_list ON score.subject_id = subject_list.id
    INNER JOIN person ON subject_list.lecturer_number = person.number
    WHERE score.student_number = ?
    )";
    statement stmt = create_statement(db, query);
    bindVariable(stmt.get(), 1, loginInfo.number);

    int total_row = getTotalRow(stmt.get());
    string option;

    // If there's no score recorded
    if (total_row == 0) {
        clearScreen();
        cout << printUIErr("Scores data not found");
        noScoreConfirmErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "|                                                                              |" << endl;
        cout << "|  7. Logout  |                                                    |  0. Exit  |" << endl;
        cout << "+-------------+----------------------------------------------------+-----------+" << endl;
        cout << "| Input: ";
        getline(cin, option);

        if (option == "7") {
            clearScreen();
            return true;
        } else if (option == "0") {
            clearScreen();
            exit(EXIT_SUCCESS);
        } else {
            clearScreen();
            cout << printUIErr("Invalid option. Please enter the available option!");
            goto noScoreConfirmErrMsg;
        }
    }

    query = R"(
    SELECT score.semester AS "Semester", subject_list.name AS "Subject", person.name AS "Lecturer", score.score AS "Score", score.grade AS "Grade"
    FROM score
    INNER JOIN subject_list ON score.subject_id = subject_list.id
    INNER JOIN person ON subject_list.lecturer_number = person.number
    WHERE score.student_number = ?
    ORDER BY subject_list.name;
    )";

    stmt = create_statement(db, query);
    bindVariable(stmt.get(), 1, loginInfo.number);

    // Print out scores
    clearScreen();
    studentMenu:
    cout << endl << endl;
    studentMenuErrMsg:
    int max_table_length = printTableData(stmt.get(), total_row);
    printStudentDisOpt(max_table_length);
    getline(cin, option);

    if (option == "7") {
        clearScreen();
        return true;
    } else if (option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    } else {
        clearScreen();
        printUIErr("Invalid option. Please enter the available option!", max_table_length);
        goto studentMenuErrMsg;
    }

    return false;
}

// Lecturer menu
// Return bool to check whether the user want to go back to login & register menu (true if yes)
bool showLecturerMenu(sqlite3* db, loginInfo loginInfo) {
    // Get lecturer's subject list
    string query = R"(SELECT count(name) FROM subject_list WHERE lecturer_number = ?)";
    statement stmt = create_statement(db, query);
    bindVariable(stmt.get(), 1, loginInfo.number);
    int total_row = getTotalRow(stmt.get());
    string subject_list[total_row];

    string option;
    lecturerMenu:
    cout << endl << endl;
    lecturerMenuErrMsg:
    cout << "+------------------------------------------------------------------------------+" << endl;
    cout << "| 1. View Score                                                                |" << endl;
    cout << "| 2. Add/Edit Score                                                            |" << endl;
    cout << "|                                                                              |" << endl;
    cout << "|  7. Logout  |                                        |  9. Back  |  0. Exit  |" << endl;
    cout << "+-------------+----------------------------------------+-----------+-----------+" << endl;
    cout << "| Input: ";
    getline(cin, option);

    if (option == "1") {        
        // If there's no subject lectured by the lecturer
        if (total_row == 0) {
            clearScreen();
            cout << printUIErr("Currently, there's no subject lectured by you.");
            cout << "+------------------------------------------------------------------------------+" << endl;
            system("pause");
            clearScreen();
            goto lecturerMenu;
        }

        query = R"(
        SELECT name FROM subject_list WHERE lecturer_number = ?)";
        stmt = create_statement(db, query);
        bindVariable(stmt.get(), 1, loginInfo.number);
        int i = 0;
        while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            subject_list[i] = get_text_value(stmt.get(), 0);
            i++;
        }

        // Output all scores from all subjects lectured by lecturer
        query = R"(
        SELECT count(*)
        FROM score
        INNER JOIN person ON score.student_number = person.number
        INNER JOIN subject_list ON score.subject_id = subject_list.id
        INNER JOIN class_member ON score.student_number = class_member.student_number
        INNER JOIN class ON class.id = class_member.class_id
        WHERE subject_list.lecturer_number = ?;
        )";
        stmt = create_statement(db, query);
        bindVariable(stmt.get(), 1, loginInfo.number);

        total_row = getTotalRow(stmt.get());

        // If there's no score recorded
        if (total_row == 0) {
            clearScreen();
            cout << printUIErr("Scores data not found");
            cout << "+------------------------------------------------------------------------------+" << endl;
            system("pause");
            clearScreen();
            goto lecturerMenu;
        }

        query = R"(
        SELECT score.semester AS "Semester", subject_list.name AS "Subject", person.name AS "Student Name", score.student_number AS "Student Number", class.name AS "Class", score.score AS "Score", score.grade AS "Grade"
        FROM score
        INNER JOIN person ON score.student_number = person.number
        INNER JOIN subject_list ON score.subject_id = subject_list.id
        INNER JOIN class_member ON score.student_number = class_member.student_number
        INNER JOIN class ON class.id = class_member.class_id
        WHERE subject_list.lecturer_number = ?
        ORDER BY subject_list.name;)";

        stmt = create_statement(db, query);
        bindVariable(stmt.get(), 1, loginInfo.number);

        printTableData(stmt.get(), total_row);
        clearScreen();
        viewScoreOption:
        cout << endl << endl;
        printTableData(stmt.get(), total_row);
        cout << endl;
        viewScoreOptionErrMsg:
        system("pause");
        clearScreen();
        goto lecturerMenu;
    } else if (option == "2") {
        int number;
        clearScreen();
        addEditScoreOption:
        cout << endl << endl;
        addEditScoreOptionErrMsg:
        cout << "+------------------------------------------------------------------------------+" << endl;
        cout << "| > Insert Student Number                                                      |" << endl;
        cout << "| Student Number: ";
        cin >> number;

        // Check student in the database
        query = "SELECT * FROM person WHERE number = ?;";
        stmt = create_statement(db, query);
        bindVariable(stmt.get(), 1, number);
        if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
            clearScreen();
            cout << printUIErr("Student is not in the database");
            studentConfirmErrMsg:
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| > Insert Student Number                                                      |" << endl;
            cout << printUI("| Student Number: ", to_string(number));
            cout << "|                                                                              |" << endl;
            cout << "| > Do you want to re-insert student number? (Y/n)                             |" << endl;
            cout << "|                                                                              |" << endl;
            cout << "|                                                      |  9. Back  |  0. Exit  |" << endl;
            cout << "+------------------------------------------------------------------------------+" << endl;
            cout << "| Input: ";
            cin.ignore();
            getline(cin, option);

            if (option == "y" || option == "Y" || option.length() == 0) {
                clearScreen();
                goto addEditScoreOption;
            } else if (option == "n" || option == "N" || option == "9") {
                clearScreen();
                goto lecturerMenu;
            } else if (option == "0") {
                clearScreen();
                exit(EXIT_SUCCESS);
            } else {
                clearScreen();
                cout << printUIErr("Invalid option. Please enter the available option!");
                goto studentConfirmErrMsg;
            }
        }

        // Check if student is in one of lecturer's subject/class
        query = R"(
            SELECT * FROM class_member
            WHERE student_number = ?
            AND class_id IN (SELECT class_id FROM subject_list WHERE lecturer_number = ?);
        )";
        stmt = create_statement(db, query);
        bindVariable(stmt.get(), 1, number);
        bindVariable(stmt.get(), 2, loginInfo.number);
        if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
            clearScreen();
            cout << printUIErr("Student is not in your class");
            goto studentConfirmErrMsg;
        }
        
        // Show student scores (if exist)
        query = R"(
            SELECT * FROM subject_list
            JOIN score ON subject_list.id = score.subject_id
            WHERE score.student_number = 232301 AND subject_list.lecturer_number = 1121001;
        )";
    } else if (option == "3") {
        clearScreen();
        
    } else if (option == "7") {
        clearScreen();
        return true;
    } else if(option == "0") {
        clearScreen();
        exit(EXIT_SUCCESS);
    } else if(option == "6969") {
        int number, classid;
        string option, name, username, password, status, temp;
        secretMenu:
        cout << "1. Add student (automatically generate random score and class)\n2. Add Subject\n9. back\n\n";
        getline(cin, option);

        if (option == "1") {
            cout << "Insert Name: ";
            getline(cin, name);
            name = toDefaultTextStyle(name);

            cout << "Insert Number: ";
            getline(cin, temp);
            number = stoi(toDefaultTextStyle(temp));

            cout << "Insert Username: ";
            getline(cin, username);

            cout << "Insert Password: ";
            getline(cin, password);
            
            cout << "Insert Status: ";
            getline(cin, status);

            cout << name << number << username << password << status << endl;

            query = "INSERT INTO person (number, name, username, password, status) VALUES (?, ?, ?, ?, ?);";
            stmt = create_statement(db, query);
            bindVariable(stmt.get(), 1, number);
            bindVariable(stmt.get(), 2, name);
            bindVariable(stmt.get(), 3, username);
            bindVariable(stmt.get(), 4, password);
            bindVariable(stmt.get(), 5, status);
            if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
                clearScreen();
                cout << "Error while inserting student to database\n";
                goto secretMenu;
            }

            // Show all class in database
            int total_row;
            stmt = create_statement(db, "SELECT count(*) FROM class;");
            total_row = getTotalRow(stmt.get());
            stmt = create_statement(db, "SELECT * FROM class;");
            printTableData(stmt.get(), total_row);

            cout << endl << "Insert Class ID: ";
            getline(cin, temp);
            classid = stoi(temp);

            stmt = create_statement(db, "INSERT INTO class_member (class_id, student_number) VALUES (?, ?);");
            bindVariable(stmt.get(), 1, classid);
            bindVariable(stmt.get(), 2, number);
            if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
                clearScreen();
                cout << "Error while inserting class to database\n";
                goto secretMenu;
            }
            
            // Get all subject in the class
            stmt = create_statement(db, "SELECT count(*) FROM subject_list WHERE class_id = ?;");
            bindVariable(stmt.get(), 1, classid);
            int total_subject_in_class = getTotalRow(stmt.get());
            string all_subject_id[total_subject_in_class];
            stmt = create_statement(db, "SELECT id FROM subject_list WHERE class_id = ?;");
            bindVariable(stmt.get(), 1, classid);
            
            int i = 0;
            while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
                all_subject_id[i] = get_int_value(stmt.get(), 0);
            }

            // Insert random scores
            int range = 100 - 61 + 1;
            for (int i = 0; i < total_subject_in_class; i++) {
                int random_number = rand() % range + 61;
                string grade;
                if (random_number > 80) {
                    grade = "A";
                } else if (random_number > 70 && random_number < 80) {
                    grade = "B";
                } else {
                    grade = "C";
                }
                stmt = create_statement(db, "INSERT INTO score (student_number, semester, subject_id, score, grade) VALUES (?, ?, ?, ?, ?);");
                bindVariable(stmt.get(), 1, number);
                bindVariable(stmt.get(), 1, 1);
                bindVariable(stmt.get(), 1, all_subject_id[i]);
                bindVariable(stmt.get(), 1, random_number);
                bindVariable(stmt.get(), 1, grade);

            }

        } else if (option == "2") {
            clearScreen();
            cout << "not available";
            goto secretMenu;
        } else if (option == "9") {
            clearScreen();
            goto lecturerMenu;
        } else {
            clearScreen();
            goto secretMenu;
        }
        
    } else {
        clearScreen();
        cout << printUIErr("Invalid option. Please enter the available option!");
        goto lecturerMenuErrMsg;
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

    CREATE TABLE IF NOT EXISTS class(
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        name TEXT NOT NULL,
        short_name TEXT NOT NULL,
        year INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS class_member(
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        class_id INTEGER NOT NULL,
        student_number INTEGER NOT NULL,
        FOREIGN KEY(student_number) REFERENCES person(number)
        FOREIGN KEY(class_id ) REFERENCES class(id)
    );

    CREATE TABLE IF NOT EXISTS subject_list (
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        lecturer_number INTEGER NOT NULL,
        name TEXT NOT NULL,
        class_id INTEGER NOT NULL,
        sks TEXT NOT NULL,
        FOREIGN KEY(lecturer_number) REFERENCES person(number)
        FOREIGN KEY(class_id ) REFERENCES class(id)
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