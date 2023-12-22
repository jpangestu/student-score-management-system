#include <iostream>
#include <termios.h>
#include <unistd.h>
using namespace std;

// **Linux-only**

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

int main() {
    // Hide user input while typing password
    string password;
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
}