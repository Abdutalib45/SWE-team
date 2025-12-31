#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include "PropertyManager.h"
#include "UserManager.h"
#include "sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include "ConsoleUtils.h"
#define NORMAL_PEN 0x07
#define HIGHLIGHTED_PEN 0x70
using namespace std;




extern bool isLoggedIn;
extern string currentUserEmail;


class AdminManager {
public:
    void viewPropertiesByOwner(sqlite3* db) {
    int ownerId;
    cout << "Enter Owner ID: ";
    cin >> ownerId;

    std::string sql = "SELECT p.id, p.name, p.location, p.price, p.type, p.isAvailable, o.name "
                      "FROM properties p "
                      "JOIN owners o ON p.owner_id = o.owner_id "
                      "WHERE o.owner_id = " + std::to_string(ownerId) + ";";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement.\n";
        return;
    }

    vector<Property> props;
    vector<string> ownerNames;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Property p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = (const char*)sqlite3_column_text(stmt, 1);
        p.location = (const char*)sqlite3_column_text(stmt, 2);
        p.price = sqlite3_column_double(stmt, 3);
        p.type = (const char*)sqlite3_column_text(stmt, 4);
        p.available = sqlite3_column_int(stmt, 5);
        props.push_back(p);

        string ownerName = (const char*)sqlite3_column_text(stmt, 6);
        ownerNames.push_back(ownerName);
    }
    sqlite3_finalize(stmt);

    if (props.empty()) {
        cout << "No properties found for this owner.\n";
        _getch();
        return;
    }


    system("cls");
    setAttr(11);
    cout << "====== Properties for Owner: " << ownerNames[0] << " ======\n\n";
    setAttr(240);
    cout << " ID    | NAME                | LOCATION          | PRICE              | TYPE      | AVAILABLE\n";
    setAttr(15);

    for (int i = 0; i < (int)props.size(); i++) {
        cout << setw(5) << props[i].id << " | ";
        cout << setw(18) << props[i].name << " | ";
        cout << setw(16) << props[i].location << " | ";
        cout << "$" << setw(16) << fixed << setprecision(2) << props[i].price << " | ";
        cout << setw(8) << props[i].type << " | ";
        cout << (props[i].available ? "Yes" : "No") << "\n";
    }

    _getch();
}


    void lockUnlockProperty(sqlite3* db) {
        int id;
        cout << "Enter Property ID to lock/unlock: ";
        cin >> id;

        sqlite3_stmt* stmt;
        string sql = "SELECT isAvailable FROM properties WHERE id=" + to_string(id);
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int available = sqlite3_column_int(stmt,0);
                sqlite3_finalize(stmt);

                string sqlUpdate = "UPDATE properties SET isAvailable=" + to_string(available == 1 ? 0 : 1) + " WHERE id=" + to_string(id);
                if (sqlite3_exec(db, sqlUpdate.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK) {
                    cout << "Property " << (available == 1 ? "locked" : "unlocked") << " successfully.\n";
                } else {
                    cout << "Failed to update property.\n";
                }
                _getch();
            } else {
                cout << "Property not found.\n";
                sqlite3_finalize(stmt);
                _getch();
            }
        }
    }
  void updateProperty(sqlite3* db) {
    int id;
    cout << "Enter Property ID to update: ";
    cin >> id;
    cin.ignore();

    sqlite3_stmt* stmt;
    string sql = "SELECT id, name, location, price, type, isAvailable, InfoNumber FROM properties WHERE id=" + to_string(id);
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            Property p;
            p.id = sqlite3_column_int(stmt,0);
            p.name = (const char*)sqlite3_column_text(stmt,1);
            p.location = (const char*)sqlite3_column_text(stmt,2);
            p.price = sqlite3_column_double(stmt,3);
            p.type = (const char*)sqlite3_column_text(stmt,4);
            p.available = sqlite3_column_int(stmt,5);
            p.infoNumber = (const char*)sqlite3_column_text(stmt,6);

            sqlite3_finalize(stmt);

            int lineno = 5;
            int maxLen = 50;
            char sr[5] = {32,32,32,32,32};
            char er[5] = {126,126,126,126,126};
            char** editedLines;

            string fields[5] = {"Name", "Location", "Price", "Type", "Contact"};
            string values[5] = {p.name, p.location, to_string(p.price), p.type, p.infoNumber};

            // ⁄—÷ «·‹ labels Ê«·ﬁÌ„ «·ﬁœÌ„…
            int startX = 5;
            for(int i=0;i<lineno;i++) {
                gotoxy(startX, 5 + i*2);
                cout << fields[i] << " (last value: " << values[i] << "): ";
            }

            // «Õ”» »œ«Ì… „ﬂ«‰ «·ﬂ «»… »ÕÌÀ Ì»ﬁÏ »⁄œ ÿÊ· «·‹ label + ÿÊ· «·ﬁÌ„… «·ﬁœÌ„… + 3
            int editorX[5];
            for(int i=0;i<lineno;i++)
                editorX[i] = startX + fields[i].length() + values[i].length() + 15;

            // «” œ⁄«¡ «·‹ multiLineEditor
            editedLines = multiLineEditor(editorX[0], 5, maxLen, sr, er, lineno);

            string newName = strlen(editedLines[0]) > 0 ? string(editedLines[0]) : p.name;
            string newLocation = strlen(editedLines[1]) > 0 ? string(editedLines[1]) : p.location;
            double newPrice = strlen(editedLines[2]) > 0 ? stod(editedLines[2]) : p.price;
            string newType = strlen(editedLines[3]) > 0 ? string(editedLines[3]) : p.type;
            string newContact = strlen(editedLines[4]) > 0 ? string(editedLines[4]) : p.infoNumber;

            string sqlUpdate = "UPDATE properties SET name='" + newName +
                                "', location='" + newLocation +
                                "', price=" + to_string(newPrice) +
                                ", type='" + newType +
                                "', InfoNumber='" + newContact +
                                "' WHERE id=" + to_string(p.id);

            if (sqlite3_exec(db, sqlUpdate.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK)
                cout << "Property updated successfully.\n";
            else
                cout << "Failed to update property.\n";

            for(int i=0;i<lineno;i++) delete[] editedLines[i];
            delete[] editedLines;

            _getch();
        } else {
            cout << "Property not found.\n";
            sqlite3_finalize(stmt);
            _getch();
        }
    }
}


};

#endif
