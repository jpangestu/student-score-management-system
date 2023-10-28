#include <iostream>
#include <string.h>
using namespace std;

// Defining student class
class student{
public:
    string name;
    int npm;

    string setScore(string semester, string matkul, string nilai) {
        string x = semester1[1][0] = {matkul};
        string y = semester1[1][1] = {nilai};
        return x, y;
    }

    void viewScore(string semester) {
        for(int i = 0; i < 15; i++) {
            for(int j = 0; j < 3; j++) {

                if(semester1[i][j] != "") {
                    cout << semester1[i][j] << "\t";
                }
            }
            if(semester1[i][0] != "") {
                    cout << endl;
                }
        }
    }

    // Nilai per semester
    string semester1[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester2[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester3[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester4[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester5[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester6[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester7[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
    string semester8[15][3] = {"Mata Kuliah", "Nilai", "Grade"};
};


int main() {
    int option;
    student student1;

    // Main menu/window
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

        viewMenu:
        cout<<"----------------------------------------------------------------------"<<endl;
        cout<<"Choose one option below by typing the corresponding number (1,2,3...):"<<endl;
        cout<<"1. Per Semester"<<endl;
        cout<<"2. Semua Semester"<<endl;
        cout<<"3. Go to previous menu"<<endl;
        cout<<"----------------------------------------------------------------------"<<endl;
        cin>>option;
        system("clear");

        if(option == 1) {
            string semester;
            cout << "Silahkan masukkan semester: ";
            cin >> semester;
            

            student1.viewScore(semester);

            goto viewMenu;
        }
        else if(option == 2) {

            goto viewMenu;
            
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
            string matkul, nilai, semester;
            cout << "Silahkan masukkan semester: ";
            cin >> semester;
            cout << "Silahkan masukkan mata kuliah: ";
            cin >> matkul;
            cout << "Silahkan masukkan nilai: ";
            cin >> nilai;
            

            student1.setScore(semester, matkul, nilai);
            student1.viewScore("semester1");

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
    }
    else {
        cin.ignore();
        cout<<"Please enter the corresponding option!\nType enter to go back to the menu"<<endl;
        cin.get();
        system("clear");
        goto mainMenu;
    }
}


// Create/add new student's score
// int createScroe (string studentName, string subject) {

// }


// Read/access student's score



// Update/edit student's score



// Delete student's score