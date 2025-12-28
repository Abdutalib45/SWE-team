#include <iostream>
#include "sqlite3.h"
#include <string>

// Callback function to handle query results
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << (azColName[i] ? azColName[i] : "NULL") << " = "
                  << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

int main() {
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    const char* db_name = "test.db";

    // 1. Open Database
    rc = sqlite3_open(db_name, &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    } else {
        std::cout << "Opened database successfully\n";
    }

    // 2. Create SQL statement
    std::string sql = "CREATE TABLE IF NOT EXISTS greetings ("  \
                      "id INTEGER PRIMARY KEY AUTOINCREMENT," \
                      "message TEXT NOT NULL);" \
                      "INSERT INTO greetings (message) VALUES ('Hello World from SQLite!');" \
                      "SELECT * FROM greetings;";

    // 3. Execute SQL statement
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Operation done successfully\n";
    }

    // 4. Close Database
    sqlite3_close(db);
    return 0;
}
