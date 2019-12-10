////////////////////////////////////////////////////////////////////////////////
// test-miner.cc
//   Wrapper to test the mine function.
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

#include<iostream>
#include<vector>
#include<algorithm>
#include <chrono>
#include <iomanip>
#include <map>

#include "../baseline/baseline.h"
#include "../utils/dblogger.h"

using namespace std;

void print(const char* X,const char* C, vector<pair<pair<int,int>,int>> *F){
  char x;
  for(auto elem : *F){
    pair<int,int> substr = elem.first;
    // cout << substr.first << " " << substr.second<<" ";
    // for(int i = substr.first ; i <substr.first + substr.second; ++i){
    // for(int i = substr.first + substr.second-1 ; i >=substr.first; --i){
    x = C[substr.first + elem.second];
    cout << elem.second << " " << x <<"-unique: ";
    // cout << elem.second << " " << y <<"-unique: ";
    if(substr.first < 0){cout << endl;continue;}
    for(int i = substr.first - substr.second+1 ; i <=substr.first; ++i){
      cout << X[i];
    }
    cout << '\n';
  }
}

#define ALPHA 36
#define ALPHA_X_SIZE_DEF 36
#define ALPHA_C_SIZE_DEF 36
#define TEXT_LENGTH_DEF 10000


enum OptionValue{
  NOT_DEFINED,
  ALPHA_X_SIZE,
  ALPHA_C_SIZE,
  TEXT_LENGTH,
  SEED,
  OUTPUT
};

static std::map<std::string, OptionValue> s_mapStringValues;

inline void initialize_map(){
  s_mapStringValues["-x"] = OptionValue::ALPHA_X_SIZE;
  s_mapStringValues["-c"] = OptionValue::ALPHA_C_SIZE;
  s_mapStringValues["-n"] = OptionValue::TEXT_LENGTH;
  s_mapStringValues["-s"] = OptionValue::SEED;
  s_mapStringValues["-o"] = OptionValue::OUTPUT;
}

int main(int argc, char const *argv[]){

  initialize_map();

  std::string usage("usage: " + std::string(argv[0]) + " [-x alpha x] [-c alpha c] [-n text length] [-s seed] [-o output]\n\n"
    + "Finds all the minimum (y,d)-unique substring of a random generated text of a given length.\n"
    + "          alpha x: [integer] - the size of the alphabet of the text smaller than or equals to "+ std::to_string(ALPHA) +". (default "+ std::to_string(ALPHA_X_SIZE_DEF) +")\n"
    + "          alpha c: [integer] - the size of the alphabet of colors smaller than or equals to "+ std::to_string(ALPHA) +". (default "+ std::to_string(ALPHA_C_SIZE_DEF) +")\n"
    + "      text length: [integer] - the length of the text . (default "+ std::to_string(TEXT_LENGTH_DEF) +")\n"
    + "             seed: [integer] - the seed of the random generator. (default 0)\n"
    + "           output: [boolean] - output the result. (default true)\n");
  // Check arguments.
  if (argc != 1 && argc != 3 && argc != 5 && argc != 7 && argc != 9 && argc != 11) {
    cerr << usage;
    exit(-1);
  }

  int n = TEXT_LENGTH_DEF;
  int alpha_x = ALPHA_X_SIZE_DEF;
  int alpha_c = ALPHA_C_SIZE_DEF;
  int seed = 0;
  bool output = true;

  for(int i = 1; i < argc; i+=2){
      std::string option(argv[i]);
      switch(s_mapStringValues[option]){
        case OptionValue::ALPHA_X_SIZE:
          alpha_x = std::stoi(argv[i+1]);
          if(alpha_x > ALPHA){
            cerr << usage;
            exit(-1);
          }
        break;
        case OptionValue::ALPHA_C_SIZE:
          alpha_c = std::stoi(argv[i+1]);
          if(alpha_c > ALPHA){
            cerr << usage;
            exit(-1);
          }
        break;
        case OptionValue::TEXT_LENGTH:
          n = std::stoi(argv[i+1]);
        break;
        case OptionValue::SEED:
          seed = std::stoi(argv[i+1]);
        break;
        case OptionValue::OUTPUT:
          if(std::string(argv[i+1]).compare("false")==0)
            output = false;
        break;
        default:
          cerr << usage;
          exit(-1);
      }
  }



  // char alphaX[ALPHA_X_SIZE] = {'A','B','C','D'};
  // char alphaC[ALPHA_C_SIZE] = {'X', 'Y', 'Z'};
  char alphaX[ALPHA] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9'};
  char alphaC[ALPHA] = {'Z','Y','X','W','V','U','T','S','R','Q','P','O','N','M','L','K','J','I','H','G','F','E','D','C','B','A','9','8','7','6','5','4','3','2','1','0'};
  srand(seed);
  // srand(time(NULL));

  int nProp = 0;
  vector<pair<pair<int,int>,int>> F;
  string X(n,'x');
  string C(n,'x');
  for(int i = 0; i < n; ++i){
    X[i] = alphaX[rand()%alpha_x];
    C[i] = alphaC[rand()%alpha_c];
  }
  reverse(X.begin(),X.end());
  // reverse(C.begin(),C.end());
  char y = 'Z';
  int n_y = 0;
  // Count the number of y's
  for(int i = 0; i < n;++i){
    if(C[i] == y) n_y++;
  }
  // Time start
  auto t_start = chrono::high_resolution_clock::now();
  nProp = baseline_mine(X.c_str(),C.c_str(),n,y,&F);
  // Time end
  auto t_end = chrono::high_resolution_clock::now();
  cout << "X: " << alpha_x << "\nC: " << alpha_c << "\nN: " << n << "\nN_y: " << n_y << '\n';
  cout << fixed << setprecision(2) << "Wall clock time passed: "
          << chrono::duration<double, milli>(t_end-t_start).count()
          << " (ms) - " << chrono::duration<double, ratio<1>>(t_end-t_start).count()
          << " (s) - " << chrono::duration<double, ratio<60>>(t_end-t_start).count()
          << " (min)\n";
  cout << "Number of propositions: " << nProp << endl;
  // nProp = mine(X.c_str(),C.c_str(),n,'Y',&F);
  reverse(X.begin(),X.end());
  // reverse(C.begin(),C.end());
  if(output) cout << C << endl << X <<endl;
  // reverse(X.begin(),X.end());
  sort(F.begin(),F.end());
  if(output) print(X.c_str(),C.c_str(),&F);

  logger::entry entry;
  entry.algo = logger::AlgoType::ALGO_BASELINE;
  entry.data = logger::DataType::DATA_RANDOM;
  entry.N = n;
  entry.C = alpha_c;
  entry.X = alpha_x;
  entry.Seed = seed;
  entry.Color_Value = y;
  entry.Ny = n_y;
  entry.N_propositions = nProp;
  entry.time_ms = to_string(chrono::duration<double, milli>(t_end-t_start).count());
  entry.time_s = to_string(chrono::duration<double, ratio<1>>(t_end-t_start).count());
  entry.time_min = to_string(chrono::duration<double, ratio<60>>(t_end-t_start).count());
  entry.time_h = to_string(chrono::duration<double, ratio<3600>>(t_end-t_start).count());
  logger log("../logs/logs.db");
  log.write(entry);
}
