////////////////////////////////////////////////////////////////////////////////
// dblogger.hh
//   Utils to log the experimental results in a SQLite database.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Zsuzsanna Lipták, Simon J. Puglisi and Massimiliano Rossi
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sqlite3.h>


using namespace std;
class logger{
public:

  enum AlgoType{
    ALGO_NONE,
    ALGO_BASELINE,
    ALGO_BASELINE_ALL,
    ALGO_SKIPPING,
    ALGO_SKIPPING_NO_BOOST,
    Algo_MAX = ALGO_SKIPPING_NO_BOOST
  };

  enum DataType{
    DATA_NONE,
    DATA_RANDOM,
    DATA_REAL,
    Data_MAX = DATA_REAL
  };

  // Entry type
  struct entry{
    AlgoType algo;
    DataType data;
    int N;
    int X;
    int C;
    int Seed;
    string File_Name;
    string Color_Name;
    int Color_Value;
    int Ny;
    int N_propositions;
    string time_ms;
    string time_s;
    string time_min;
    string time_h;
  };

  logger(const char* db_name){

    string sql;
    char *zErrMsg = 0;
    int rc;
    /* Open database */
    rc = sqlite3_open(db_name, &db);

    if( rc ) {
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    } else {
       // fprintf(stdout, "Opened database successfully\n");
    }


    /* Create TABLE */
    sql = "CREATE TABLE IF NOT EXISTS EXECUTION_TIMES ("\
	"ID	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"\
  "DATE	TEXT NOT NULL,"\
	"ALGO_TYPE	TEXT NOT NULL,"\
	"TEXT_TYPE	TEXT NOT NULL,"\
	"N	INTEGER NOT NULL,"\
	"C	INTEGER NOT NULL,"\
	"X	INTEGER NOT NULL,"\
	"SEED	INTEGER,"\
	"TRACE_NAME	TEXT,"\
	"COLOR_NAME	TEXT,"\
	"COLOR_VALUE	INTEGER,"\
	"N_Y	INTEGER,"\
	"N_PROPERTIES	INTEGER NOT NULL,"\
	"EXECUTION_TIME_MS	REAL NOT NULL,"\
	"EXECUTION_TIME_S	REAL NOT NULL,"\
	"EXECUTION_TIME_MIN	REAL NOT NULL,"\
	"EXECUTION_TIME_H	REAL NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);



  }

  ~logger(){
    sqlite3_close(db);
  }

  void write(entry data){
    /* Create SQL statement */
    string sql;
    char *zErrMsg = 0;
    int rc;

    sql = "INSERT INTO EXECUTION_TIMES (DATE,ALGO_TYPE,TEXT_TYPE,N,C,X,SEED,TRACE_NAME,COLOR_NAME,COLOR_VALUE,N_Y,N_PROPERTIES,EXECUTION_TIME_MS,EXECUTION_TIME_S,EXECUTION_TIME_MIN,EXECUTION_TIME_H) "  \
    "VALUES ( datetime('now', 'localtime'), '" + (data.algo == AlgoType::ALGO_BASELINE?string("Baseline") : (data.algo == AlgoType::ALGO_BASELINE_ALL?string("Baseline_all") : (data.algo == AlgoType::ALGO_SKIPPING? string("Skipping") : string("Skipping_no_boost"))))
    +"', '"+ (data.data == DataType::DATA_RANDOM?string("Random") : string("Real"))
    +"', " + to_string(data.N) + ", " + to_string(data.C) +", "+ to_string(data.X)
    +", " + to_string(data.Seed)+ ", '"+data.File_Name +"', '"+ data.Color_Name +"', " + to_string(data.Color_Value)
    +", "+ to_string(data.Ny) + ", " + to_string(data.N_propositions) + ", "+ data.time_ms + ", "+ data.time_s + ", "+ data.time_min + ", "+ data.time_h+ " ); ";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    while (rc == SQLITE_LOCKED || rc == SQLITE_BUSY){
      rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    }

    if( rc != SQLITE_OK ){
      cerr << sql << endl;
      // cerr << SQLITE_LOCKED << endl;
      // cerr << rc << endl;
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      // fprintf(stdout, "Records created successfully\n");
    }

  }
protected:
  static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    // int i;
    // for(i = 0; i<argc; i++) {
    //   printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    // }
    // printf("\n");
    // NtD
    return 0;
  }

private:
  sqlite3 *db;

};
