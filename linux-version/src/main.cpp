#include <iostream>
#include <memory>
#include "sqlite3/sqlite3.h"

using namespace std;
// Custom namespace to make sqlite3 query easier
using database = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

// Function prototype (forward declaration).
database open_database(const char* name);
void execute(sqlite3* db, const char* sql_query);
statement create_statement(sqlite3* db, const string &sql_query);
constexpr const char* default_schema();


int main() {
    // Link existing (create new) database.
    database db = open_database("main.db");
    execute(db.get(), default_schema());

    string sql_query = R"(SELECT student_number, student.name, semester,
    subject_list.name, lecturer, score, grade FROM score
    INNER JOIN subject_list ON subject_list.id = score.subject_id
    INNER JOIN student ON student.number = score.student_number;)";
    statement stmt = create_statement(db.get(), sql_query);

    // Execute sql statement, and while there are rows returned, print ID
    bool found = false;
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt.get())) == SQLITE_ROW) {
        cout << "Student Number: " << sqlite3_column_int(stmt.get(), 0);
        cout << "Student Name: " << sqlite3_column_int(stmt.get(), 0);
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        //this error handling could be done better, but it works
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db.get()));
        printf("ret_code = %d\n", ret_code);
    }

    printf("entry %s\n", found ? "found" : "not found");

    //release resources
    sqlite3_finalize(stmt.get());
    sqlite3_close(db.get());



    // ##Front-End##
    // Main Menu
    int option;
    mainMenu:
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    cout<<"1. View Score"<<endl;
    cout<<"2. Edit Score"<<endl;
    cout<<"3. Exit"<<endl;
    cout<<"----------------------------------------------------------------------"<<endl;
    cin>>option;
    system("clear");

    if(option == 1) {
        
        goto mainMenu;
    }
    // Edit Menu
    else if(option == 2) {

        editMenu:
        cout<<"----------------------------------------------------------------------"<<endl;
        cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
        cout<<"1. Add Score"<<endl;
        cout<<"2. Delete Score"<<endl;
        cout<<"3. Go to previous menu"<<endl;
        cout<<"----------------------------------------------------------------------"<<endl;
        cin>>option;
        system("clear");

        if(option == 1) {
            
            goto editMenu;
        }
        else if(option == 2) {

            goto editMenu;
            
        }
        else if(option == 3) {

            goto mainMenu;
            
        }
        else {
            cin.ignore();
            cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
            cin.get();
            system("clear");
            goto editMenu;
        }
    }
    else if(option == 3) {
        
        cin.ignore();
        sqlite3_close(db.get());
    }
    else {
        cin.ignore();
        cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
        cin.get();
        system("clear");
        goto mainMenu;
    }

    sqlite3_close(db.get());
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
        lecturer INTEGER NOT NULL,
        name TEXT NOT NULL,
        sks TEXT NOT NULL,
        FOREIGN KEY(lecturer) REFERENCES lecturer(number)
    );

    CREATE TABLE IF NOT EXISTS score (
        student_number INTEGER NOT NULL,
        semester INTEGER NOT NULL,
        subject_id INTEGER NOT NULL,
        score INTEGER NOT NULL,
        grade TEXT NOT NULL,
	FOREIGN KEY(student_number) REFERENCES student(student_number)
	FOREIGN KEY(subject_id) REFERENCES subject_list(id)
    );
    )";
}