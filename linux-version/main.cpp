#include <iostream>
#include <string>
#include "sqlite3/sqlite3.h"
using namespace std;

// Defining student class
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
    // Open sqlite database (create new database if not exist)
    char** error_msg;
    sqlite3 *db;
    int rc = sqlite3_open("main_database.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    const char* sql_deflt_table = "CREATE TABLE IF NOT EXISTS student (" \
        "number INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "name TEXT NOT NULL," \
        "username TEXT NOT NULL," \
        "password TEXT NOT NULL" \
    ");" \
    \
    "CREATE UNIQUE INDEX IF NOT EXISTS number ON student(number);" \
    "CREATE UNIQUE INDEX IF NOT EXISTS username ON student(username);" \
    \
    "CREATE TABLE IF NOT EXISTS subject_list (" \
        "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "name TEXT NOT NULL," \
        "sks TEXT NOT NULL);" \
    \
    "CREATE UNIQUE INDEX IF NOT EXISTS name ON subject_list(name);" \
    \
    "CREATE TABLE IF NOT EXISTS lecturer (" \
        "number INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "name TEXT NOT NULL," \
        "subject_lectured TEXT NOT NULL," \
        "username TEXT NOT NULL," \
        "password TEXT NOT NULL," \
        "FOREIGN KEY (subject_lectured) REFERENCES subject_list(name)" \
    ");"\
    \
    "CREATE UNIQUE INDEX IF NOT EXISTS number ON lecturer(number);" \
    "CREATE UNIQUE INDEX IF NOT EXISTS username ON lecturer(username);" \
    \
    "CREATE TABLE IF NOT EXISTS score ("\
        "student_name TEXT NOT NULL," \
        "student_number INTEGER NOT NULL," \
        "semester INTEGER NOT NULL," \
        "subject TEXT NOT NULL," \
        "score INTEGER NOT NULL," \
        "grade INTEGER NOT NULL," \
        "FOREIGN KEY(student_name) REFERENCES student(name)," \
        "FOREIGN KEY(student_number) REFERENCES student(student_number)" \
    ");";

    rc = sqlite3_exec(db, sql_deflt_table, NULL, NULL, error_msg);

    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", error_msg);
      sqlite3_free(error_msg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }




    // Front-End
    int option;
    student student1;
    student1.name = "Jesse Pinkman";
    student1.studentNumber = "22.22.0022";

    // Main Menu
    mainMenu:
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    cout<<"1. View Score"<<endl;
    cout<<"2. Edit Score"<<endl;
    cout<<"3. Exit"<<endl;
    cout<<"----------------------------------------------------------------------"<<endl;
    cin>>option;
    system("clear");

    // If View is selected from the Main Menu
    if(option == 1) {
        student1.viewScore();

        cout << endl;
        cout << endl;

        goto mainMenu;
    }
    // If Edit is selected from the Main Menu
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
            string subject, score;
            cout << "Please input the subject: ";
            cin >> subject;
            cout << "Please input the score: ";
            cin >> score;
            

            student1.setScore(subject, score);
            student1.viewScore();

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
        sqlite3_close(db);
    }
    else {
        cin.ignore();
        cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
        cin.get();
        system("clear");
        goto mainMenu;
    }

    sqlite3_close(db);
}