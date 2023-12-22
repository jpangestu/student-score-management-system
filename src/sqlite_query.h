using database = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

database open_database(const char* name);
void execute(sqlite3* db, const char* sql_query);
statement create_statement(sqlite3* db, const std::string &sql_query);
void parameter(sqlite3_stmt* stmt, int index, int value);
void parameter(sqlite3_stmt* stmt, int index, const std::string& value);
int get_int_value(sqlite3_stmt* stmt, int indexColumn);
std::string get_text_value(sqlite3_stmt* stmt, int indexColumn);