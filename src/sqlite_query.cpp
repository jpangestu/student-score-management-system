#include <iostream>
#include <memory>
#include "sqlite3/sqlite3.h"
#include "sqlite_query.h"

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
        std::cerr << "Unable to execute query" << errmsg;
        sqlite3_free(errmsg);
        exit(EXIT_FAILURE);
    }
}

// Create (and prepare) SQL statement.
statement create_statement(sqlite3* db, const std::string &sql_query) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql_query.c_str(), sql_query.length(), &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Unable to create statement '" << sql_query <<"': " << sqlite3_errmsg(db);
        exit(EXIT_FAILURE);
    }
    return statement(stmt, sqlite3_finalize);
}

// Bind int variable to a prepared sqlite3_stmt*
void bindVariable(sqlite3_stmt* stmt, int index, int value) {
    if (sqlite3_bind_int(stmt, index, value) != SQLITE_OK) {
        throw(std::invalid_argument("Error! sqlite3_bind_int from bindVariable() function can't accept your value"));
    }
}

// Bind string variable to a prepared sqlite3_stmt*
void bindVariable(sqlite3_stmt* stmt, int index, const std::string& value) {
    std::string value2 = value;
    if (sqlite3_bind_text(stmt, index, value2.c_str(), value2.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
        throw(std::invalid_argument("Error! sqlite3_bind_text from bindVariable() function can't accept your value"));
    }
}

// Get int value from SQLite table
int get_int_value(sqlite3_stmt* stmt, int indexColumn) {
    return sqlite3_column_int(stmt, indexColumn);
}

// Get string/text value from SQLite table
std::string get_text_value(sqlite3_stmt* stmt, int indexColumn) {
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, indexColumn)));
}

// Get total number of rows from sqlite query
// Use SELECT count(*) in the query
int getTotalRow(sqlite3_stmt* stmt) {
    int total_row;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total_row = get_int_value(stmt, 0);
    } else {

    }
    return total_row;
}