#include "UserManager.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstring>
#include "ConsoleUtils.h"

#define NORMAL_PEN 0x07
#define HIGHLIGHTED_PEN 0x70

using namespace std;

extern bool isLoggedIn;
extern int currentUserId;
extern string currentUserEmail;


void LeftMove(int* current)
{
    if (*current > 0) (*current)--;
}

void RightMove(int* current, int* last)
{
    if (*current < *last) (*current)++;
}

void PressedHome(int* current, int* last)
{
    *current = 0;
}

void PressedEnd(int* current, int* last)
{
    *current = *last;
}

void DeleteChar(char* line, int* current, int* last)
{
    if (*current > *last) return;
    for (int i = *current; i < *last; i++)
        line[i] = line[i + 1];
    line[*last] = ' ';
    if (*last > 0) (*last)--;
}

void Backspace(char* line, int* current, int* last)
{
    if (*current > 0)
    {
        (*current)--;
        for (int i = *current; i < *last; i++)
            line[i] = line[i + 1];
        line[*last] = ' ';
        if (*last > 0) (*last)--;
    }
}



char** multiLineEditor(int xpos, int ypos, int l, char sr[], char er[], int lineno)
{
    char** lines = new char*[lineno];
    int* lasts = new int[lineno];

    for (int i = 0; i < lineno; i++)
    {
        lines[i] = new char[l + 1];
        memset(lines[i], ' ', l);
        lines[i][l] = '\0';
        lasts[i] = 0;
    }

    int currentLine = 0, currentChar = 0;
    char ch;
    bool done = false;

    while (!done)
    {
        for (int i = 0; i < lineno; i++)
        {
            gotoxy(xpos, ypos + (i * 2));
            textattr(HIGHLIGHTED_PEN);
            // Mask password line (line index 1)
            if (i == 1)
            {
                for (int j = 0; j < strlen(lines[i]); j++)
                {
                    if (lines[i][j] != ' ')
                        cout << '*';
                    else
                        cout << ' ';
                }
            }
            else
            {
                cout << lines[i];
            }
        }

        textattr(NORMAL_PEN);
        gotoxy(xpos + currentChar, ypos + (currentLine * 2));

        ch = _getch();

        if (ch == -32) // Arrow keys
        {
            ch = _getch();
            switch (ch)
            {
                case 72: if (currentLine > 0) currentLine--; break; // Up
                case 80: if (currentLine < lineno - 1) currentLine++; break; // Down
                case 75: LeftMove(&currentChar); break; // Left
                case 77: RightMove(&currentChar, &lasts[currentLine]); break; // Right
                case 71: PressedHome(&currentChar, &lasts[currentLine]); break; // Home
                case 79: PressedEnd(&currentChar, &lasts[currentLine]); break; // End
                case 83: DeleteChar(lines[currentLine], &currentChar, &lasts[currentLine]); break; // Delete
            }
        }
        else
        {
            switch (ch)
            {
                case 8: Backspace(lines[currentLine], &currentChar, &lasts[currentLine]); break;
                case 13: // Enter
                    if (currentLine == lineno - 1) done = true;
                    else { currentLine++; currentChar = 0; }
                    break;
                default:
                    if (ch >= sr[currentLine] && ch <= er[currentLine] && lasts[currentLine] < l)
                    {
                        for (int i = lasts[currentLine]; i > currentChar; i--)
                            lines[currentLine][i] = lines[currentLine][i - 1];
                        lines[currentLine][currentChar++] = ch;
                        lasts[currentLine]++;
                    }
            }
        }
    }

    // Null terminate lines
    for (int i = 0; i < lineno; i++)
        lines[i][lasts[i]] = '\0';

    delete[] lasts;
    return lines;
}



string trim(const char* s)
{
    string str(s);

    size_t start = str.find_first_not_of(' ');
    if (start != string::npos)
        str = str.substr(start);
    else
        return "";

    while (!str.empty() && str.back() == ' ')
        str.pop_back();

    return str;
}



bool UserManager::login(sqlite3* db)
{
    system("cls"); // Clear console
    int width = 40, height = 10;
    int startX = 15, startY = 5;

    // Draw frame
    for (int y = startY; y <= startY + height; y++)
    {
        gotoxy(startX, y);
        for (int x = startX; x <= startX + width; x++)
        {
            if (y == startY || y == startY + height) cout << "-"; // top/bottom
            else if (x == startX || x == startX + width) cout << "|"; // sides
            else cout << " ";
        }
    }

    // Title
    gotoxy(startX + 12, startY); textattr(HIGHLIGHTED_PEN);
    cout << " LOGIN";
    textattr(NORMAL_PEN);

    // Labels
    gotoxy(startX + 2, startY + 3); cout << "Email: ";
    gotoxy(startX + 2, startY + 5); cout << "Password:  ";

    // Set allowed chars
    char sr[] = { ' ', ' ' };
    char er[] = { '~', '~' };

    // Start editor at positions next to labels
    char** input = multiLineEditor(startX + 13, startY + 3, 25, sr, er, 2);

    string email = trim(input[0]);
    string password = trim(input[1]);

    // Free memory
    for (int i = 0; i < 2; i++) delete[] input[i];
    delete[] input;

    if (email.empty() || password.empty())
    {
        gotoxy(startX + 2, startY + height + 2);
        cout << "Invalid email or password!\n";
        return false;
    }

    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE email=? AND password=?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        gotoxy(startX + 2, startY + height + 2);
        cout << "Database error!\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        currentUserId = sqlite3_column_int(stmt, 0);
        currentUserEmail = email;
        isLoggedIn = true;

        gotoxy(startX + 2, startY + height + 2);
        cout << "Login successful!\n";
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    gotoxy(startX + 2, startY + height + 2);
    cout << "Wrong email or password!\n";
    return false;
}
