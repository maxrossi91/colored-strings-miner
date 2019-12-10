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

#define _PRINT_IMPQ

#include "../miner/miner.h"
#include "../utils/dblogger.h"

using namespace std;

void print(const char* X, char y, vector<pair<pair<int,int>,int>> *F){
  for(auto elem : *F){
    pair<int,int> substr = elem.first;
    // cout << substr.first << " " << substr.second<<" ";
    cout << elem.second << " " << y <<"-unique: ";
    // for(int i = substr.first ; i <substr.first + substr.second; ++i){
    // for(int i = substr.first + substr.second-1 ; i >=substr.first; --i){
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
  OUTPUT,
  BOOST
};

static std::map<std::string, OptionValue> s_mapStringValues;

inline void initialize_map(){
  s_mapStringValues["-x"] = OptionValue::ALPHA_X_SIZE;
  s_mapStringValues["-c"] = OptionValue::ALPHA_C_SIZE;
  s_mapStringValues["-n"] = OptionValue::TEXT_LENGTH;
  s_mapStringValues["-s"] = OptionValue::SEED;
  s_mapStringValues["-o"] = OptionValue::OUTPUT;
  s_mapStringValues["-b"] = OptionValue::BOOST;
}

int main(int argc, char const *argv[]){

  initialize_map();

  std::string usage("usage: " + std::string(argv[0]) + " [-x alpha x] [-c alpha c] [-n text length] [-s seed] [-o output] [-b boost]\n\n"
    + "Finds all the minimum (y,d)-unique substring of a random generated text of a given length.\n"
    + "          alpha x: [integer] - the size of the alphabet of the text smaller than or equals to "+ std::to_string(ALPHA) +". (default "+ std::to_string(ALPHA_X_SIZE_DEF) +")\n"
    + "          alpha c: [integer] - the size of the alphabet of colors smaller than or equals to "+ std::to_string(ALPHA) +". (default "+ std::to_string(ALPHA_C_SIZE_DEF) +")\n"
    + "      text length: [integer] - the length of the text . (default "+ std::to_string(TEXT_LENGTH_DEF) +")\n"
    + "             seed: [integer] - the seed of the random generator. (default 0)\n"
    + "           output: [boolean] - output the result. (default true)\n"
    + "            boost: [boolean] - enable the boost. (default true)\n");
  // Check arguments.
  if (argc != 1 && argc != 3 && argc != 5 && argc != 7 && argc != 9 && argc != 11 && argc != 13) {
    cerr << usage;
    exit(-1);
  }

  int n = TEXT_LENGTH_DEF;
  int alpha_x = ALPHA_X_SIZE_DEF;
  int alpha_c = ALPHA_C_SIZE_DEF;
  int seed = 0;
  bool output = true;
  bool boost = true;

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
        case OptionValue::BOOST:
          if(std::string(argv[i+1]).compare("false")==0)
            boost = false;
        break;
        default:
          cerr << usage;
          exit(-1);
      }
  }





  boost  = false;

  int nProp = 0;
  vector<pair<pair<int,int>,int>> F;
  string X = "acacacbacab";
  string C = "xyxzxyzyxxz";
  n = X.length();

  reverse(X.begin(),X.end());

  char y = 'y';
  int n_y = 0;
  // Count the number of y's
  for(int i = 0; i < n;++i){
    if(C[i] == y) n_y++;
  }
  boost_options(boost);
  // Time start
  auto t_start = chrono::high_resolution_clock::now();
  nProp = mine(X.c_str(),C.c_str(),n,y,&F);
  // Time end
  auto t_end = chrono::high_resolution_clock::now();
  cout << "X: 3\nC: 3\nN: " << n << "\nN_y: " << n_y << '\n';
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
  if(output) print(X.c_str(),y,&F);

}
