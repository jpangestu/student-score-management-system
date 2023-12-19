#include <iostream>
#include <cstring>
#include <memory>
#include "sqlite3/sqlite3.h"

using namespace std;
// Custom namespace to make sqlite3 query easier
using database = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

// #Function prototype (forward declaration)#
database open_database(const char* name);
void execute(sqlite3* db, const char* sql_query);
statement create_statement(sqlite3* db, const string &sql_query);
void parameter(sqlite3_stmt* stmt, int index, const std::string& value);
constexpr const char* default_schema();

void spaceGenerate(int numOfChar);
void showLoginMenu(sqlite3* db);


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

void spaceGenerate(int numOfChar) {
    for (int i = 0; i < numOfChar; i++) {
        cout << " ";
    }
}

// #Front-End

void showLoginMenu(sqlite3* db) {
    int option;
    string username, password;

    loginMenu:
    cout<<"+------------------------------------------------------------------------------+"<<endl;
    cout<<"| Choose one option below by typing the corresponding number (0,1,2,.. ):      |"<<endl;
    cout<<"| 1. Login                                                                     |"<<endl;
    cout<<"| 2. Create Account                                                            |"<<endl;
    cout<<"|                                                                              |"<<endl;
    cout<<"|                                                                  |  0. Exit  |"<<endl;
    cout<<"+------------------------------------------------------------------+-----------+"<<endl;
    cin>>option;

    /* If the app is running on Windows, use system("cls") (to clear screen).
    Else, the app must be running on Linux. Hence, use system("clear") (to clear screen) */
    try{
        system("cls");
        throw(invalid_argument("App not running on Windows"));
        
    }
    catch(invalid_argument){
        system("clear");
    }

    if(option == 1) {
        int number = 0;
        string session = "";
        bool username_exist = false;

        cout << "+------------------------------------------------------------------------------+"<<endl;
        loginOption:
        cout << "| Insert Username: ";
        cin >> username;

        cout << "| Insert Password: ";
        cin >> password;

        // Check if username is exist in the database
        string query = R"(
        SELECT number FROM student WHERE username = ?)";
        statement stmt = create_statement(db, query);
        parameter(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            session = "student";
            username_exist = true;
            number = sqlite3_column_int(stmt.get(), 0);
        }
        query = R"(
        SELECT number FROM lecturer WHERE username = ?)";
        stmt = create_statement(db, query);
        parameter(stmt.get(), 1, username);
        if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
            session = "lecturer";
            username_exist = true;
            number = sqlite3_column_int(stmt.get(), 0);
        }

        if (username_exist != true) {
            try{
                if(system("cls")) {
                throw(invalid_argument("App not running on Windows"));
                }
            }
            catch(invalid_argument){
            system("clear");
            }
            cout << "+------------------------------------------------------------------------------+"<<endl;
            cout << "| > Invalid username or password!                                              |" << endl;
            cout << "+------------------------------------------------------------------------------+"<<endl;
            goto loginOption;
        }
        
        // Check if the password match the username in the database
        if (session == "student") {
            query = R"(
            SELECT number FROM student WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            parameter(stmt.get(), 1, username);
            parameter(stmt.get(), 2, password);
            if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
                try{
                    if(system("cls")) {
                    throw(invalid_argument("App not running on Windows"));
                    }
                }
                catch(invalid_argument){
                system("clear");
                }
                cout << "> Invalid username or password!" << endl;
                goto loginOption;
            }
        }
        else if (session == "lecturer") {
            query = R"(
            SELECT number FROM lecturer WHERE username = ? AND password = ?)";
            stmt = create_statement(db, query);
            parameter(stmt.get(), 1, username);
            parameter(stmt.get(), 2, password);
            if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
                try{
                    if(system("cls")) {
                    throw(invalid_argument("App not running on Windows"));
                    }
                }
                catch(invalid_argument){
                system("clear");
                }
                cout << "> Invalid username or password!" << endl;
                goto loginOption;
            }
        } else {
            try{
                if(system("cls")) {
                throw(invalid_argument("App not running on Windows"));
                }
            }
            catch(invalid_argument){
            system("clear");
            }
            cout << "> Invalid username or password!" << endl;
            goto loginOption;
        }
        

    }
    else if(option == 2) {
        string name;
        cout << "Insert Your Name: ";
        cin >> name;
    }
    else if(option == 0) {
        
        cin.ignore();
    }
    else {
        cin.ignore();
        cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
        cin.get();
        
        try{
            if(system("cls")) {
                throw(invalid_argument("App not running on Windows"));
            }
        }
        catch(invalid_argument){
            system("clear");
        }

        goto loginMenu;
    }
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

void parameter(sqlite3_stmt* stmt, int index, const std::string& value) {
    if (sqlite3_bind_text(stmt, index, value.c_str(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
        throw(invalid_argument("Error, sqlite3_bind can't accept your value"));
    }
};

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