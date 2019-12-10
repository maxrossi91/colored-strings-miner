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
#include <fstream>
#include <sstream>

#include "../baseline/baseline.h"
#include "../utils/dblogger.h"


extern unsigned long long int ccount;
extern unsigned long long int cnomono;

using namespace std;

void print(vector<string> v_X, vector<string> v_C, vector<pair<pair<int,int>,int>> *F){
  string x;
  for(auto elem : *F){
    pair<int,int> substr = elem.first;
    // cout << substr.first << " " << substr.second<<" ";
    x = v_C[substr.first + elem.second];
    cout << elem.second << " " << x <<"-unique: ";
    // for(int i = substr.first ; i <substr.first + substr.second; ++i){
    // for(int i = substr.first + substr.second-1 ; i >=substr.first; --i){
    if(substr.first < 0){cout << endl;continue;}
    for(int i = substr.first - substr.second+1 ; i <=substr.first; ++i){
      cout << v_X[i] << " ";
    }
    cout << '\n';
  }
}

enum OptionValue{
  NOT_DEFINED,
  COLOR_VARIABLE,
  COLOR_VALUE,
  OUTPUT
};

static std::map<std::string, OptionValue> s_mapStringValues;

inline void initialize_map(){
  s_mapStringValues["-c"] = OptionValue::COLOR_VARIABLE;
  s_mapStringValues["-v"] = OptionValue::COLOR_VALUE;
  s_mapStringValues["-o"] = OptionValue::OUTPUT;
}

int main(int argc, char const *argv[]){

  initialize_map();

  std::string usage("usage: " + std::string(argv[0]) + " [file name] [-c color variable name] [-v color variable value] [-o output]\n\n"
    + "Finds all the minimum (y,d)-unique substring of the trace in [file name] in mangrove1 format.\n"
    + "            file name: [string]  - the file name of the .mangrove1 and .variables1 file\n"
    + "  color variable name: [string]  - the name of the output variable that has to be used as color value. (default the first variable in the list)\n"
    + " color variable value: [integer] - the value of the output variable that we want to mine. (default the first value of the variable)\n"
    + "               output: [boolean] - output the result. (default true)\n");
  // Check arguments.
  if (argc != 2 && argc != 4 && argc != 6 && argc != 8 && argc != 10) {
    cerr << usage;
    exit(-1);
  }

  bool output = true;

  string color_variable_name;
  unsigned long int color_variable_value = 0;
  bool color_variable_value_flag = false;

  string file_name(argv[1]);

  ifstream mangrove_file(file_name + ".mangrove1");
  ifstream variables_file(file_name + ".variables1");

  // if the bool entry:
  //  - false -> input
  //  - true  -> output
  vector<pair<string,bool> > variables;
  // Read the variables file
  string variable;
  string type;
  int n_in = 0;
  int n_out = 0;
  variables_file >> variable >> type;
  while(!variables_file.eof()){
    if(type.compare("out")==0) n_out ++;
    else n_in++;
    variables.push_back(make_pair(variable,(type.compare("out")==0)));
    variables_file >> variable >> type;
  }

  // find the first output variable
  for(auto elem: variables){
    if(elem.second) color_variable_name = elem.first;
    if(elem.second) break;
  }


  for(int i = 2; i < argc; i+=2){
      std::string option(argv[i]);
      switch(s_mapStringValues[option]){
        case OptionValue::COLOR_VARIABLE:
          color_variable_name = std::string(argv[i+1]);
        break;
        case OptionValue::COLOR_VALUE:
          color_variable_value = std::stoi(argv[i+1]);
          color_variable_value_flag = true;
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



  int n_var,n;
  mangrove_file >> n_var >>n ;
  string dummy;
  getline(mangrove_file,dummy);

  vector<vector<string> > input_traces(n_in);
  vector<string> v_X(n,"");
  vector<string> v_C;

  map<string,unsigned long int> color_alphabet;
  unsigned long int color_alphabet_size = 1;
  int curr_in = 0;
  // extract the variables color_alphabets
  for(unsigned int i = 0; i < variables.size(); ++i){
    string line;
    getline(mangrove_file,line);
    istringstream ss(line.c_str());
    if(variables[i].second){
      // Output sequence
      if((variables[i].first).compare(color_variable_name) == 0){
        // It is the correct variable;
        for(string each; getline(ss,each,' ');){
          auto it = color_alphabet.find(each);
          if(it==color_alphabet.end()){
            v_C.push_back(to_string(color_alphabet_size));
            color_alphabet.insert(make_pair(each,color_alphabet_size++));
          }else{
            v_C.push_back(to_string(it->second));
          }
        }
      }
    }else{
      // Input sequence
      for(string each; getline(ss,each,' '); input_traces[curr_in].push_back(each));
      curr_in++;
    }

  }

  map<string,unsigned long int> alphabet;
  unsigned long int alphabet_size = 1;
  for(int i = 0; i < n; ++i){
    string character;
    for(unsigned int j = 0; j < input_traces.size(); ++j){
      character += input_traces[j][i] + " ";
    }
    auto it = alphabet.find(character);
    if(it==alphabet.end()){
      v_X[i] = to_string(alphabet_size);
      alphabet.insert(make_pair(character,alphabet_size++));
    }else{
      v_X[i] = to_string(it->second);
    }
  }


  int nProp = 0;
  vector<pair<pair<int,int>,int>> F;

  reverse(v_X.begin(),v_X.end());
  string X;
  string C;
  for(int i = 0; i < n; ++i){
    X+= v_X[i] + " ";
    C+= v_C[i] + " ";
  }
  if(!color_variable_value_flag) color_variable_value = stoi(v_C[0]);
  else color_variable_value = color_alphabet[to_string(color_variable_value)];
  // reverse(C.begin(),C.end());
  string y = to_string(color_variable_value);
  int n_y = 0;
  // Count the number of y's
  for(int i = 0; i < n;++i){
    if(v_C[i].compare(y) == 0) n_y++;
  }
  // Time start
  auto t_start = chrono::high_resolution_clock::now();
  nProp = baseline_mine_int(X.c_str(),'d',C.c_str(),'d',n,color_variable_value,&F);
  // Time end
  cout << "ccount: " << ccount << endl;
  cout << "cnomono: " << cnomono << endl;
  auto t_end = chrono::high_resolution_clock::now();
  cout << "X: " << alphabet_size << "\nC: " << color_alphabet_size-1 << "\nN: " << n << "\nN_y: " << n_y << '\n';
  cout << fixed << setprecision(2) << "Wall clock time passed: "
          << chrono::duration<double, milli>(t_end-t_start).count()
          << " (ms) - " << chrono::duration<double, ratio<1>>(t_end-t_start).count()
          << " (s) - " << chrono::duration<double, ratio<60>>(t_end-t_start).count()
          << " (min)\n";
  cout << "Number of propositions: " << nProp << endl;
  // nProp = mine(X.c_str(),C.c_str(),n,'Y',&F);
  reverse(v_X.begin(),v_X.end());
  string tmp_X;
  for(int i = 0; i < n; ++i) tmp_X += v_X[i] + " ";
  // reverse(C.begin(),C.end());
  if(output) cout << C << endl << tmp_X <<endl;
  // reverse(X.begin(),X.end());
  sort(F.begin(),F.end());
  if(output) print(v_X,v_C,&F);

  logger::entry entry;
  entry.algo = logger::AlgoType::ALGO_BASELINE;
  entry.data = logger::DataType::DATA_REAL;
  entry.N = n;
  entry.C =color_alphabet_size -1;
  entry.X = alphabet_size;
  entry.File_Name = file_name;
  entry.Color_Name = color_variable_name;
  entry.Color_Value = color_variable_value;
  entry.Ny = n_y;
  entry.N_propositions = nProp;
  entry.time_ms = to_string(chrono::duration<double, milli>(t_end-t_start).count());
  entry.time_s = to_string(chrono::duration<double, ratio<1>>(t_end-t_start).count());
  entry.time_min = to_string(chrono::duration<double, ratio<60>>(t_end-t_start).count());
  entry.time_h = to_string(chrono::duration<double, ratio<3600>>(t_end-t_start).count());
  logger log("../logs/logs.db");
  log.write(entry);
}
