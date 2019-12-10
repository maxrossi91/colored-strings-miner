////////////////////////////////////////////////////////////////////////////////
// baseline.cc
//   Implementation of the y pattern baseline miner for colored strings.
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

#include "baseline.h"
#include <sdsl/suffix_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include <algorithm>
#include <queue>


unsigned long long int ccount = 0;
unsigned long long int cnomono = 0;

using namespace sdsl;
using namespace std;

#define ANY_COLOR 0
#define NO_MONO 1
#define MONO 2

template <class cst_t, class node_t=typename cst_t::node_type, class char_t=typename cst_t::char_type>
class baseline_miner{
public:

  int mine(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
           int _n, char_t* y, std::vector<std::pair<std::pair<int,int>,int>>* F)
    {
      n = _n;
      // Convert the colors string into an int_vector
      string tmp_file = ram_file_name(util::to_string(util::pid())+"_"+util::to_string(util::id()));
      store_to_file(C, tmp_file);
      load_vector_from_file(colors, tmp_file,num_bytes_C);
      ram_fs::remove(tmp_file);

      // Build the Compressed Suffix Treee (CST) for X
      construct_im(cst, X, num_bytes_X);
      // Build a range Maximum query on SA
      range_maximum_sct<>::type tmp_rMq(&cst.csa);
      rMq = tmp_rMq;

      res = F;

      int n_nodes = cst.nodes();
      colors_of_nodes[0] = vector<pair<char,char_t>>(n_nodes,make_pair(0,0));
      colors_of_nodes[1] = vector<pair<char,char_t>>(n_nodes,make_pair(0,0));

      bit_vector not_report(n);
      // For all d = 0,...,n
      for(int d = n; d >=0; --d){
        // std::cerr << "\n res:" << res->size() << "d: " << d << '\n'<< "node ids:";
        color(cst.root(),d,y);
      }
      // std::cerr << '\n';

      return res->size();
    }

protected:
  int find_second_max_val(int lb,int rb){
    int pos_max = rMq(lb,rb);
    if(pos_max == lb) return cst.csa[rMq(pos_max+1,rb)];
    else if(pos_max == rb) return cst.csa[rMq(lb,pos_max-1)];
    else return max(cst.csa[rMq(lb,pos_max-1)],cst.csa[rMq(pos_max+1,rb)]);
  }

  // Arguments:
  //   v = node in the suffix tree that has to be colored.
  //   d = the distance such that the node has to be colored.
  //   *y = a pointer to the color taken into account;
  // Returns:
  //   the first char refers to he type of return:
  //   - 0: any color;
  //   - 1: no color;
  //   - 2: the color in the secon char.

  pair<char,char_t> color(node_t v, int d, char_t *y){
    ccount++;
      // If v is a leaf, than report its color if it appear in the text,
      // otherwise report any color.
      if(cst.is_leaf(v)){
        // std::cerr << cst.id(v) << " ";
        int sn = n-cst.sn(v)-1;
        if(sn+d<n && sn >= 0){
          if(y != NULL){
            if(colors[sn+d] == *y) colors_of_nodes[d%2][cst.id(v)] = make_pair(MONO,colors[sn+d]);
            else colors_of_nodes[d%2][cst.id(v)] = make_pair(NO_MONO,colors[sn+d]);
          }
          else colors_of_nodes[d%2][cst.id(v)] = make_pair(MONO,colors[sn+d]);
          return colors_of_nodes[d%2][cst.id(v)];
        }
        // Assume that 0 stands for any color.
        colors_of_nodes[d%2][cst.id(v)] = make_pair(ANY_COLOR,0);
        return colors_of_nodes[d%2][cst.id(v)];
      }
      pair<char,char_t> col = make_pair(ANY_COLOR,0);
      bool at_least_one_mono = false;
      // for all children of v
      for(auto child: cst.children(v)){
        // get the colors of all childs
        pair<char,char_t> current = color(child,d,y);
        // we check whether all the children colors are either any color or
        // they have the same color.
        if(current.first == MONO) at_least_one_mono = true;
        if(col.first == NO_MONO) continue;
        if(current.first == ANY_COLOR) continue;
        if(current.first == NO_MONO){
          col.first = NO_MONO; // the node v has no colors
          continue;
        }
        if(current.first == MONO){
          if(col.first == ANY_COLOR){
            col = current;
          }else if(current.second != col.second){
            col.first = NO_MONO; // the node v node has no colors;
          }else{
            col.second = current.second;
          }
        }
      }

      if(col.first == NO_MONO && at_least_one_mono){
        cnomono++;
        for(auto child: cst.children(v)){
          // get the colors of all childs
          pair<char,char_t> current = colors_of_nodes[d%2][cst.id(child)];
          if(current.first == MONO && !cst.is_leaf(child)){ // it should be to_be_reported
            // check whether its slink parent is monochrome
            node_t slink_p = cst.sl(child);
            node_t slink_pp = cst.parent(slink_p);
            while(cst.depth(v) < cst.depth(slink_pp)+1 && slink_p != cst.root()){
              slink_p = slink_pp;
              slink_pp = cst.parent(slink_p);
            }
            pair<char,char_t> slink_p_col = colors_of_nodes[(d+1)%2][cst.id(slink_p)];
            if(slink_p_col.first == MONO && slink_p_col.second == current.second){
              // skip
              continue;
            }
            // Report
            int second_max_val = find_second_max_val(cst.lb(child),cst.rb(child));
            if(res!= NULL && d <= second_max_val ){
              res->push_back(make_pair(make_pair(n-second_max_val-1,cst.depth(cst.parent(child))+1),d));
              // res->push_back(make_pair(make_pair(n-cst.sn(cst.lb(child))-1,cst.depth(cst.parent(child))+1),d));
            }
          }
        }
      }

      // std::cerr << cst.id(v) << " ";

      colors_of_nodes[d%2][cst.id(v)] = col;
      return colors_of_nodes[d%2][cst.id(v)];
  }

private:
  cst_t cst; // Compressed Suffix Tree
  int n; // length of the text

  std::vector<std::pair<std::pair<int,int>,int>>* res;
  // Range maximum query over csa in cst
  range_maximum_sct<>::type rMq;
  // Color string pointer
  int_vector<> colors;
  // Colors of each node:
  // if d is even then I can use [0]
  // if d is even then I can use [1]
  vector<pair<char,char_t>> colors_of_nodes[2];

};

baseline_miner<cst_sct3<> > baseline_miner_char;
baseline_miner<cst_sct3<csa_bitcompressed<int_alphabet<> > >  > baseline_miner_int;
typedef cst_sct3<>::char_type baseline_miner_char_color_t;
typedef cst_sct3<csa_bitcompressed<int_alphabet<> > >::char_type baseline_miner_int_color_t;

// Wrapper functions
int baseline_mine(const char *X, const char *C, int _n,
         char y, std::vector<std::pair<std::pair<int,int>,int>>* F){
           baseline_miner_char_color_t tmp_y = y;
           return baseline_miner_char.mine(X,1,C,1,_n,&tmp_y,F);
         }

int baseline_mine_all(const char *X, const char *C, int _n,
          std::vector<std::pair<std::pair<int,int>,int>>* F){
           return baseline_miner_char.mine(X,1,C,1,_n,NULL,F);
         }

int baseline_mine_int(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
         int _n, unsigned long int y, std::vector<std::pair<std::pair<int,int>,int>>* F){
           baseline_miner_int_color_t tmp_y = y;
           return baseline_miner_int.mine(X,num_bytes_X,C,num_bytes_C,_n,&tmp_y,F);
         }

int baseline_mine_all_int(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
         int _n, std::vector<std::pair<std::pair<int,int>,int>>* F){
           return baseline_miner_int.mine(X,num_bytes_X,C,num_bytes_C,_n,NULL,F);
         }










//}
