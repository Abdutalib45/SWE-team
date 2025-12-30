#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

struct UserSession {
    int userId = -1;
    string email = "";
    bool isLoggedIn = false;
    bool isAdmin = false;
};

class UserManager {
public:
    // This allows us to access the user from anywhere
    static UserSession currentUser;

    bool login(sqlite3* db) {
        system("cls");
        string email, pass;
        cout << "\n\n\t--- LOGIN SYSTEM ---" << endl;
        cout << "\tEmail: "; cin >> email;
        cout << "\tPassword: "; cin >> pass;

        //just for test
        if (email == "user" && pass == "123") {
            currentUser.userId = 1;
            currentUser.email = email;
            currentUser.isLoggedIn = true;
            currentUser.isAdmin = false;
            return true;
        }
        return false;
    }

    void logout() {
        currentUser = UserSession();
    }
};

// Initialize static member
UserSession UserManager::currentUser;

#endif
