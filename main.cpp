#include <iostream>
#include "sqlite3.h"
#include <string>
#include <windows.h>
#include <conio.h>
#include "PropertyManager.h"
#include "UserManager.h"
#include "DBManager.h"
using namespace std;

void textattr(int i)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void drawHeader() {
    textattr(14);
    gotoxy(15, 1); cout << "  _____  ______          _        ______  _____ _______       _______ ______ ";
    gotoxy(15, 2); cout << " |  __ \\|  ____|   /\\   | |      |  ____|/ ____|__   __|/\\   |__   __|  ____|";
    gotoxy(15, 3); cout << " | |__) | |__     /  \\  | |      | |__  | (___    | |  /  \\     | |  | |__   ";
    gotoxy(15, 4); cout << " |  _  /|  __|   / /\\ \\ | |      |  __|  \\___ \\   | | / /\\ \\    | |  |  __|  ";
    gotoxy(15, 5); cout << " | | \\ \\| |____ / ____ \\| |____  | |____ ____) |  | |/ ____ \\   | |  | |____ ";
    gotoxy(15, 6); cout << " |_|  \\_\\______/_/    \\_\\______| |______|_____/   |_/_/    \\_\\  |_|  |______|";

    textattr(11);
    gotoxy(35, 8); cout << "--- PROPERTY MANAGEMENT SYSTEM ---";
}
void drawMenu(int selected, bool firstDraw)
{
    // Only clear the full screen and draw header on the first entry
    // or after returning from another screen.
    if (firstDraw) {
        system("cls");
        drawHeader();
    }

    const char* menu[] = {
        "View Properties",
        "Login",
        "Search Properties",
        "Exit"
    };

    int y = 12;
    for (int i = 0; i < 4; i++)
    {
        gotoxy(38, y + i * 2);

        if (i == selected)
        {
            textattr(240); // Highlight: Black text on White background
            cout << "> " << menu[i] << "  "; // Extra spaces to clear old chars
        }
        else
        {
            textattr(15); // Normal: White text on Black background
            cout << "  " << menu[i] << "  ";
        }
    }
    textattr(15);
}

bool executeMenuAction(int choice, sqlite3* db, DBManager* dbManager)
{
    system("cls");
    PropertyManager pm;
    UserManager um;

    switch (choice)
    {
    case 0:
        pm.viewAllProperties(db, dbManager);
        break;

    case 1:

        if (um.login(dbManager)) {
            textattr(10); cout << "\nLogin Successful!";
        }
        break;

    case 2:
        // You can add your search function call here later
        cout << "Search functionality coming soon...";
        break;

    case 3:
        textattr(12);
        cout << "Exiting system...\n";
        return false;
    }

    if (choice != 3) {
        textattr(8);
        cout << "\n\nPress any key to return to main menu...";
        _getch();
    }
    return true;
}
void runMainMenu(sqlite3* db, DBManager* dbManager)
{
    int selected = 0;
    char key;
    bool running = true;
    bool needsFullRedraw = true; // Flag to control system("cls")

    // Hide Cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    while (running)
    {
        drawMenu(selected, needsFullRedraw);
        needsFullRedraw = false; // After the first draw, don't cls again

        key = _getch();

        if (key == 72 || key == 'w' || key == 'W') // UP
        {
            selected = (selected <= 0) ? 3 : selected - 1;
        }
        else if (key == 80 || key == 's' || key == 'S') // DOWN
        {
            selected = (selected >= 3) ? 0 : selected + 1;
        }
        else if (key == 13) // ENTER
        {

            running = executeMenuAction(selected, db, dbManager);
            needsFullRedraw = true;
        }
    }
}


int main() {
    DBManager dbManager("test.db");

    if (!dbManager.getDB()) {
        return 0;
    }

    dbManager.initializeDatabase();

    runMainMenu(dbManager.getDB(), &dbManager);

    return 0;
}
