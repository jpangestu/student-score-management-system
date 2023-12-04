#include <iostream>
#include <sqlite3.h>
#include <SQLiteCpp.h>
using namespace std;
using namespace SQLite;

// Function prototype (forward declaration).
constexpr const char* default_schema();

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
    Database db("main.db");
    // // Create (if not created yet) default sqlite tables (defining .schema)
    Statement stmt(db, default_schema());
    Statement stmt2(db, R"(INSERT INTO lecturer (name, username, password)
    VALUES ('Walter White', 'walterwhite', 'walterwhite');
    INSERT INTO lecturer (name, username, password)
    VALUES ("Gus Fring", 'gusfring', 'gusfring');
    INSERT INTO lecturer (name, username, password)
    VALUES ("Saul Goodman", 'saulgoodman', 'saulgoodman');
    INSERT INTO subject_list (lecturer_id, name, sks)
    VALUES (1, 'Chemistry', 2);
    INSERT INTO subject_list (lecturer_id, name, sks)
    VALUES (1, 'Entrepreneurship', 2);
    INSERT INTO subject_list (lecturer_id, name, sks)
    VALUES (2, 'Entrepreneurship', 2);
    INSERT INTO subject_list (lecturer_id, name, sks)
    VALUES (3, 'Business Law', 4);
    INSERT INTO subject_list (lecturer, name, sks)
    VALUES (3, 'Entrepreneurship', 2);
    INSERT INTO student (number, name, username, password)
    VALUES (232301, 'Jesse Pinkman', 'jessepinkman', 'jessepinkman');
    INSERT INTO student (number, name, username, password)
    VALUES (232302, 'Todd Alquist', 'toddalquist', 'toddalquist');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232301, 1, 1, 78, 'B');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232301, 1, 2, 85, 'A');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232301, 1, 4, 67, 'C');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232302, 1, 1, 63, 'C');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232302, 1, 3, 72, 'B');
    INSERT INTO score (student_number, semester, subject_id, score, grade)
    VALUES (232302, 1, 4, 62, 'C');)");

    Statement query(db, "SELECT * FROM score;");

    while (query.executeStep())
    {
        // Demonstrate how to get some typed column value
        int         student_number  = query.getColumn(0);
        const char* semester        = query.getColumn(1);
        const char* subject_id      = query.getColumn(2);
        int         score           = query.getColumn(3);
        const char* grade           = query.getColumn(4);
        
        cout << "Student Number: " << student_number << ", " << semester
        << ", " << subject_id << ", " << score << ", " << grade << endl;
    }




    // ##Front-End##
    int option, number;
    string username = "John", password, name, subject_lectured[9];

    loginMenu:
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    cout<<"1. Login"<<endl;
    cout<<"2. Create Account"<<endl;
    cout<<"3. Exit"<<endl;
    cout<<"----------------------------------------------------------------------"<<endl;
    cin>>option;

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

    if(option == 1) {
        cout << "Insert Username: ";
        cin >> username;
        // command cmd(db, R"(SELECT student.username, lecturer.username FROM student
        // INNER JOIN student ON student.username = lecturer.username WHERE username = ?)");
        // cmd.binder() << username;
        // cmd.execute();

        cout << "Insert Password: ";
        cin >> password;


    }
    else if(option == 2) {
        cout << "Insert Your Name: ";
        cin >> name;
    }
    else if(option == 3) {
        
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


    mainMenu:
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
    cout<<"1. View Score"<<endl;
    cout<<"2. Edit Score"<<endl;
    cout<<"3. Exit"<<endl;
    cout<<"----------------------------------------------------------------------"<<endl;
    cin>>option;

    try{
        if(system("cls")) {
            throw(invalid_argument("App not running on Windows"));
        }
    }
    catch(invalid_argument){
        system("clear");
    }

    if(option == 1) {

        goto mainMenu;
    }

    else if(option == 2) {

        editMenu:
        cout<<"----------------------------------------------------------------------"<<endl;
        cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
        cout<<"1. Add Score"<<endl;
        cout<<"2. Delete Score"<<endl;
        cout<<"3. Go to previous menu"<<endl;
        cout<<"----------------------------------------------------------------------"<<endl;
        cin>>option;

        try{
            if(system("cls")) {
                throw(invalid_argument("App not running on Windows"));
            }
        }
        catch(invalid_argument){
            system("clear");
        }
        

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

            try{
            if(system("cls")) {
                throw(invalid_argument("App not running on Windows"));
                }
            }
            catch(invalid_argument){
                system("clear");
            }
            goto editMenu;
        }
    }
    else if(option == 3) {
        
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

        goto mainMenu;
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
	FOREIGN KEY(student_number) REFERENCES student(student_number)
	FOREIGN KEY(subject_id) REFERENCES subject_list(id)
    );
    )";
}