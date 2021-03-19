#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <list>
#include <string.h>
#include <nlohmann/json.hpp>
#include <json/json.h>
#include <string>

using namespace std;
using json = nlohmann::json;

sqlite3 *DATABASE_HANDLER;
int DATABASE_CONNECTION;
char *Z_ERR_MSG = 0;
string courseName = "";
string studentFirstName = "";
string studentLastName = "";

map<int, char> asciiTable = {
    {10, '0'}, {11, '1'}, {12, '2'}, {13, '3'}, {14, '4'}, {15, '5'}, {16, '6'}, {17, '7'}, {18, '8'}, {19, '9'},
    {20, 'A'}, {21, 'B'}, {22, 'C'},{23, 'D'},{24, 'E'},{25, 'F'},{26, 'G'},{27, 'H'},{28, 'I'},{29, 'J'},
    {30, 'K'}, {31, 'L'}, {32, 'M'}, {33, 'N'}, {34, 'O'}, {35, 'P'}, {36, 'Q'}, {37, 'R'}, {38, 'S'}, {39, 'T'},
    {40, 'U'}, {41, 'V'}, {42, 'W'}, {43, 'X'}, {44, 'Y'}, {45, 'Z'}
};

struct Course{
    int courseCodeLetters;
    int courseCode;
};

Course asciiToInt(char* stringToConvert){
    string concatenatedString = "";
    Course course;
    int integer;
    
    for(int i = 0; i < strlen(stringToConvert); ++i) {
        for(map<int, char>::iterator iterator = asciiTable.begin(); iterator != asciiTable.end(); iterator++){
            if(toupper(stringToConvert[i]) == iterator->second)
                concatenatedString += to_string(iterator->first) ;
        }
    }

    istringstream(concatenatedString.substr(0,6)) >> course.courseCodeLetters;
    istringstream(concatenatedString.substr(6,concatenatedString.length()-6)) >> course.courseCode;
    return course;
}

string intToAscii(int intToConvert){
    string concatenatedString = to_string(intToConvert);
    char str[concatenatedString.length()+1];
    string resultString;
    
    snprintf(str, concatenatedString.length() + 1, "%d", intToConvert);
    for(int i = 0; i < strlen(str) ; i +=2) {
        for(map<int, char>::iterator iterator = asciiTable.begin(); iterator != asciiTable.end(); iterator++){
            if(stoi(string(1, str[i])+str[i+1]) == iterator->first)
                resultString += iterator->second;
        }
    }

    return resultString;
}

Json::Value parseStudentInformations(int studentCode, string grade, string firstName, string lastName)
{
    Json::Value student;

    student["nom"] = lastName;
    student["prenom"] = firstName;
    student["matricule"] = studentCode;
    student["note"] = grade;

    return student;
}

Json::Value parseCourseInformations(string courseName, int courseTrimester, string courseNumber, string grade)
{   
    Json::Value course;

    course["nom"] = courseName;
    course["sigle"] = courseNumber;
    course["trimestre"] = courseTrimester;
    course["note"] = grade;
    return course;
}

string selectNameFromStudent(int studentCode, bool isFirstName)
{
    sqlite3_stmt *statement;
    string studentName;
    isFirstName ? DATABASE_CONNECTION = sqlite3_prepare_v2(DATABASE_HANDLER, "SELECT FIRSTNAME FROM STUDENT WHERE STUDENTCODE = ?;", 53, &statement, NULL)
                : DATABASE_CONNECTION = sqlite3_prepare_v2(DATABASE_HANDLER, "SELECT LASTNAME FROM STUDENT WHERE STUDENTCODE = ?;", 52, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, studentCode);

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        studentName = reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
    }

    sqlite3_finalize(statement);

    return studentName;
}


int selectBlockIdFromCourses(char *courseNumber, int courseTrimester)
{
    sqlite3_stmt *statement;
    int blockId = NULL;
        Course course = asciiToInt(courseNumber);


    DATABASE_CONNECTION = sqlite3_prepare_v2(DATABASE_HANDLER, "SELECT IDBLOCK FROM BLOCK WHERE COURSENUMBER = ? AND COURSECODE = ? AND COURSETRIMESTER = ?;", 92, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, course.courseCodeLetters);
    sqlite3_bind_int(statement, 2, course.courseCode);
    sqlite3_bind_int(statement, 3, courseTrimester);
    try
    {
        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            blockId = sqlite3_column_int(statement, 0);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    sqlite3_finalize(statement);

    return blockId;
}

int selectBlockIdFromCourseAndStudent(
    int studentCode,
    char *courseNumber,
    int trimester)
{
    sqlite3_stmt *statement;
    int blockId = NULL;

    Course course = asciiToInt(courseNumber);
    DATABASE_CONNECTION = sqlite3_prepare_v2(DATABASE_HANDLER, "SELECT IDBLOCK FROM BLOCK WHERE COURSENUMBER = ? AND COURSECODE = ? AND COURSETRIMESTER = ? AND STUDENTCODE = ?;", 112, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, course.courseCodeLetters);
    sqlite3_bind_int(statement, 2, course.courseCode);
    sqlite3_bind_int(statement, 3, trimester);
    sqlite3_bind_int(statement, 4, studentCode);
    
    try
    {
        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            blockId = sqlite3_column_int(statement, 0);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    sqlite3_finalize(statement);

    return blockId;
}

void createBlockchainTables()
{
    char *sqlCreateBlockTable =
        "CREATE TABLE BLOCK("
        "IDBLOCK INT  NOT NULL,"
        "STUDENTCODE INT NOT NULL,"
        "COURSENUMBER INT NOT NULL,"
        "COURSECODE INT NOT NULL,"
        "COURSETRIMESTER    INT     NOT NULL,"
        "GRADE              TEXT    NOT NULL,"
        "PRIMARY KEY (STUDENTCODE, COURSENUMBER, COURSECODE, IDBLOCK),"
        "FOREIGN KEY (STUDENTCODE) REFERENCES STUDENT(STUDENTCODE) ,"
        "FOREIGN KEY (COURSENUMBER) REFERENCES COURSE(COURSENUMBER));";

    DATABASE_CONNECTION = sqlite3_exec(DATABASE_HANDLER, sqlCreateBlockTable, NULL, 0, &Z_ERR_MSG);

    if (DATABASE_CONNECTION != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", Z_ERR_MSG);
        sqlite3_free(Z_ERR_MSG);
    }
    else
    {
        fprintf(stdout, "Table Block created successfully\n");
    }
    
    char *sqlCreateCourseTable =
        "CREATE TABLE COURSE("
        "COURSENUMBER       INT     NOT NULL,"
        "COURSECODE         INT     NOT NULL,"
        "COURSETRIMESTER    INT     NOT NULL,"
        "COURSENAME         TEXT    NOT NULL,"
        "PDF                BLOB    NOT NULL,"
        "PRIMARY KEY (COURSENUMBER, COURSECODE, COURSETRIMESTER));";

    DATABASE_CONNECTION = sqlite3_exec(DATABASE_HANDLER, sqlCreateCourseTable, NULL, 0, &Z_ERR_MSG);

    if (DATABASE_CONNECTION != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", Z_ERR_MSG);
        sqlite3_free(Z_ERR_MSG);
    }
    else
    {
        fprintf(stdout, "Table Course created successfully\n");
    }

    char *sqlCreateStudentTable =
        "CREATE TABLE STUDENT("
        "STUDENTCODE INT    PRIMARY KEY     NOT NULL,"
        "FIRSTNAME                  TEXT    NOT NULL,"
        "LASTNAME                   TEXT    NOT NULL);";

    DATABASE_CONNECTION = sqlite3_exec(DATABASE_HANDLER, sqlCreateStudentTable, NULL, 0, &Z_ERR_MSG);

    if (DATABASE_CONNECTION != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", Z_ERR_MSG);
        sqlite3_free(Z_ERR_MSG);
    }
    else
    {
        fprintf(stdout, "Table Student created successfully\n");
    }
}

void initializeBlockchainDatabase(const char* databaseFilename)
{

    DATABASE_CONNECTION = sqlite3_open(databaseFilename, &DATABASE_HANDLER);

    if (DATABASE_CONNECTION != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(DATABASE_HANDLER));
        return;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    createBlockchainTables();
}

void deleteBlockchainData(int blockId)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(DATABASE_HANDLER, "DELETE FROM COURSE WHERE EXISTS (SELECT BLOCK.COURSENUMBER FROM BLOCK WHERE BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER AND BLOCK.IDBLOCK = ?);", 232, &statement, NULL);
    
    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }
    
    sqlite3_bind_int(statement, 1, blockId);
    sqlite3_step(statement);
    sqlite3_finalize(statement);

    sqlite3_prepare_v2(DATABASE_HANDLER, "DELETE FROM STUDENT WHERE EXISTS (SELECT BLOCK.STUDENTCODE FROM BLOCK WHERE BLOCK.STUDENTCODE = STUDENT.STUDENTCODE AND BLOCK.IDBLOCK = ?);", 139, &statement, NULL);
    
    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }
    
    sqlite3_bind_int(statement, 1, blockId);
    sqlite3_step(statement);
    sqlite3_finalize(statement);

    sqlite3_prepare_v2(DATABASE_HANDLER, "DELETE FROM BLOCK WHERE IDBLOCK = ?;", 36, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, blockId);
    sqlite3_step(statement);
    sqlite3_finalize(statement);
}

void insertBlockchainData(
    int studentCode,
    char *firstNameStudent,
    char *lastNameStudent,
    char *courseNumber,
    int trimester,
    char *courseName,
    char *grade,
    int blockId)
{
    sqlite3_stmt *statement;

    Course course = asciiToInt(courseNumber);

    sqlite3_prepare_v2(DATABASE_HANDLER, "INSERT INTO COURSE (COURSENUMBER,COURSECODE,COURSETRIMESTER,COURSENAME,PDF) VALUES (?,?,?,?,?);", 95, &statement, NULL);
    if (statement != NULL)
    {
        sqlite3_bind_int(statement, 1, course.courseCodeLetters);
        sqlite3_bind_int(statement, 2, course.courseCode);
        sqlite3_bind_int(statement, 3, trimester);
        sqlite3_bind_text(statement, 4, courseName, 50, SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 5, "", 150000, SQLITE_TRANSIENT);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
    }

    sqlite3_prepare_v2(DATABASE_HANDLER, "INSERT INTO STUDENT (STUDENTCODE,FIRSTNAME,LASTNAME) VALUES (?,?,?);", 68, &statement, NULL);
    if (statement != NULL)
    {
        sqlite3_bind_int(statement, 1, studentCode);
        sqlite3_bind_text(statement, 2, firstNameStudent, 16, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 3, lastNameStudent, 15, SQLITE_TRANSIENT);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
    }

    sqlite3_prepare_v2(DATABASE_HANDLER, "INSERT INTO BLOCK (IDBLOCK,STUDENTCODE,COURSENUMBER,COURSECODE,COURSETRIMESTER,GRADE) VALUES (?,?,?,?,?,?);", 107, &statement, NULL);
    if (statement != NULL)
    {
        sqlite3_bind_int(statement, 1, blockId);
        sqlite3_bind_int(statement, 2, studentCode);
        sqlite3_bind_int(statement, 3, course.courseCodeLetters);
        sqlite3_bind_int(statement, 4, course.courseCode);
        sqlite3_bind_int(statement, 5, trimester);
        sqlite3_bind_text(statement, 6, grade, 5, SQLITE_TRANSIENT);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
    }
}

void insertPdfInCourseTable(char* pdf, char* courseNumber, int trimester){
    sqlite3_stmt *statement;

    Course course = asciiToInt(courseNumber);
    sqlite3_prepare_v2(DATABASE_HANDLER, "UPDATE COURSE SET PDF = ? WHERE COURSENUMBER = ? AND COURSECODE = ? AND COURSETRIMESTER = ?;", 92, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                    << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    if (statement != NULL)
    {   
        sqlite3_bind_blob(statement, 1, pdf, 150000, SQLITE_TRANSIENT);
        sqlite3_bind_int(statement, 2, course.courseCodeLetters);
        sqlite3_bind_int(statement, 3, course.courseCode);
        sqlite3_bind_int(statement, 4, trimester);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
    }
}

list<Json::Value> selectCoursesFromStudentCode(int studentCode, int trimester = 0, char *courseNumber = "")
{
    sqlite3_stmt *statement;
    list<Json::Value> jsonCourseList;

    if (trimester != 0 && courseNumber == "")
    {
        DATABASE_CONNECTION = sqlite3_prepare_v2(
            DATABASE_HANDLER,
            "SELECT BLOCK.COURSENUMBER, BLOCK.COURSECODE, BLOCK.COURSETRIMESTER, GRADE, COURSENAME, FIRSTNAME, LASTNAME FROM BLOCK INNER JOIN COURSE ON BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER INNER JOIN STUDENT ON BLOCK.STUDENTCODE = STUDENT.STUDENTCODE WHERE BLOCK.STUDENTCODE = ? AND BLOCK.COURSETRIMESTER = ?;",
            392,
            &statement,
            NULL);

        if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
        {
            std::cerr << "Impossible to Prepare Select Statement!\n"
                      << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
            exit(1);
        }

        sqlite3_bind_int(statement, 1, studentCode);
        sqlite3_bind_int(statement, 2, trimester);
    }
    else if (courseNumber != "" && trimester == 0)
    {   
        Course course = asciiToInt(courseNumber);
        DATABASE_CONNECTION = sqlite3_prepare_v2(
            DATABASE_HANDLER,
            "SELECT BLOCK.COURSENUMBER, BLOCK.COURSECODE, BLOCK.COURSETRIMESTER, GRADE, COURSENAME, FIRSTNAME, LASTNAME FROM BLOCK INNER JOIN COURSE ON BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER INNER JOIN STUDENT ON BLOCK.STUDENTCODE = STUDENT.STUDENTCODE WHERE BLOCK.COURSENUMBER = ? AND BLOCK.COURSECODE = ? AND BLOCK.STUDENTCODE = ?;",
            414,
            &statement,
            NULL);

        if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
        {
            std::cerr << "Impossible to Prepare Select Statement!\n"
                      << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
            exit(1);
        }

        sqlite3_bind_int(statement, 1, course.courseCodeLetters);
        sqlite3_bind_int(statement, 2, course.courseCode);
        sqlite3_bind_int(statement, 3, studentCode);
    }
    else if (courseNumber != "" && trimester != 0)
    {   
        Course course = asciiToInt(courseNumber);
        DATABASE_CONNECTION = sqlite3_prepare_v2(
            DATABASE_HANDLER,
            "SELECT BLOCK.COURSENUMBER, BLOCK.COURSECODE, BLOCK.COURSETRIMESTER, GRADE, COURSENAME, FIRSTNAME, LASTNAME FROM BLOCK INNER JOIN COURSE ON BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER INNER JOIN STUDENT ON BLOCK.STUDENTCODE = STUDENT.STUDENTCODE WHERE BLOCK.COURSENUMBER = ? AND BLOCK.COURSECODE = ? AND BLOCK.STUDENTCODE = ? AND BLOCK.COURSETRIMESTER = ?;",
            444,
            &statement,
            NULL);

        if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
        {
            std::cerr << "Impossible to Prepare Select Statement!\n"
                      << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
            exit(1);
        }

        sqlite3_bind_int(statement, 1, course.courseCodeLetters);
        sqlite3_bind_int(statement, 2, course.courseCode);
        sqlite3_bind_int(statement, 3, studentCode);
        sqlite3_bind_int(statement, 4, trimester);
    }
    else
    {
        DATABASE_CONNECTION = sqlite3_prepare_v2(
            DATABASE_HANDLER,
            "SELECT BLOCK.COURSENUMBER, BLOCK.COURSECODE, BLOCK.COURSETRIMESTER, GRADE, COURSENAME, FIRSTNAME, LASTNAME FROM BLOCK INNER JOIN COURSE ON BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER INNER JOIN STUDENT ON BLOCK.STUDENTCODE = STUDENT.STUDENTCODE WHERE BLOCK.STUDENTCODE = ?;",
            362,
            &statement,
            NULL);

        if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
        {
            std::cerr << "Impossible to Prepare Select Statement!\n"
                      << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
            exit(1);
        }

        sqlite3_bind_int(statement, 1, studentCode);
    }

    while (sqlite3_step(statement) == SQLITE_ROW)
    {       
        jsonCourseList.push_back(parseCourseInformations(reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)),
                                                         sqlite3_column_int(statement, 2),
                                                         intToAscii(sqlite3_column_int(statement, 0)) + intToAscii(sqlite3_column_int(statement, 1)),
                                                         reinterpret_cast<const char *>(sqlite3_column_text(statement, 3))
                                                         ));
        if (studentFirstName.empty() && studentLastName.empty()) {
            studentFirstName = reinterpret_cast<const char *>(sqlite3_column_text(statement, 5));
            studentLastName = reinterpret_cast<const char *>(sqlite3_column_text(statement, 6));
        }
    }

    sqlite3_finalize(statement);

    return jsonCourseList;
}

list<Json::Value> selectStudentsFromCourseNumber(char *courseNumber, int trimester)
{
    sqlite3_stmt *statement;
    map<int, string> studentMap;
    list<Json::Value> studentsJson;
    Course course = asciiToInt(courseNumber);
    
    DATABASE_CONNECTION = sqlite3_prepare_v2(
        DATABASE_HANDLER, 
        "SELECT STUDENTCODE, GRADE, COURSENAME FROM BLOCK INNER JOIN COURSE ON BLOCK.COURSENUMBER = COURSE.COURSENUMBER AND BLOCK.COURSECODE = COURSE.COURSECODE AND BLOCK.COURSETRIMESTER = COURSE.COURSETRIMESTER WHERE BLOCK.COURSENUMBER = ? AND BLOCK.COURSECODE = ? AND BLOCK.COURSETRIMESTER = ?;", 
        287, 
        &statement, 
        NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, course.courseCodeLetters);
    sqlite3_bind_int(statement, 2, course.courseCode);
    sqlite3_bind_int(statement, 3, trimester);
    try
    {
        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            studentMap.insert(pair<int, string>(sqlite3_column_int(statement, 0), reinterpret_cast<const char *>(sqlite3_column_text(statement, 1))));
            if(courseName.empty()){
                courseName = reinterpret_cast<const char *>(sqlite3_column_text(statement, 2));
            }
        }

        for (map<int, string>::iterator iterator = studentMap.begin(); iterator != studentMap.end(); ++iterator)
        {
            studentsJson.push_back(parseStudentInformations(iterator->first,
                                                            iterator->second,
                                                            selectNameFromStudent(iterator->first, true),
                                                            selectNameFromStudent(iterator->first, false)
                                                            ));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    sqlite3_finalize(statement);

    return studentsJson;
}

string selectPdfFromCourse(char* courseNumber, int trimester){
    sqlite3_stmt *statement;
    string pdf = "";
    Course course = asciiToInt(courseNumber);

    DATABASE_CONNECTION = sqlite3_prepare_v2(DATABASE_HANDLER, "SELECT PDF FROM COURSE WHERE COURSENUMBER = ? AND COURSECODE = ? AND COURSETRIMESTER = ?;", 213, &statement, NULL);

    if (DATABASE_CONNECTION != SQLITE_OK || statement == NULL)
    {
        std::cerr << "Impossible to Prepare Select Statement!\n"
                  << sqlite3_errmsg(DATABASE_HANDLER) << std::endl;
        exit(1);
    }

    sqlite3_bind_int(statement, 1, course.courseCodeLetters);
    sqlite3_bind_int(statement, 2, course.courseCode);
    sqlite3_bind_int(statement, 3, trimester);
    try
    {
        while (sqlite3_step(statement) == SQLITE_ROW)
        {   
            pdf = reinterpret_cast<const char *>(sqlite3_column_blob(statement, 0));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    sqlite3_finalize(statement);

    return pdf;
}
