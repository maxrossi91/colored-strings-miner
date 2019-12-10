////////////////////////////////////////////////////////////////////////////////
// miner.cc
//   Implementation of the y pattern miner for colored strings.
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

#include "miner.h"
#include "indexMaxPQ.h"
#include <sdsl/suffix_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include <algorithm>
#include <queue>


using namespace sdsl;
using namespace std;

template <class cst_t, class node_t=typename cst_t::node_type, class char_t=typename cst_t::char_type>
class miner{
public:

  void set_boost(bool value){
    boost = value;
  }

  int mine(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
           int _n, unsigned long int y, std::vector<std::pair<std::pair<int,int>,int>>* F){
     // Convert the colors string into an int_vector
     string tmp_file = ram_file_name(util::to_string(util::pid())+"_"+util::to_string(util::id()));
     store_to_file(C, tmp_file);
     int_vector<> tmp_C;
     load_vector_from_file(tmp_C, tmp_file,num_bytes_C);
     ram_fs::remove(tmp_file);

     n = _n;

     // Build the Compressed Suffix Treee (CST) for X
     construct_im(cst, X, num_bytes_X);

     // Build the RMQ support for characte y in C
     bit_vector tmp_cy(n);
     for(int i = 0; i < n; ++i) tmp_cy[i] = (tmp_C[i] == y);
     // tmp_cy[n-1] = 1;
     rank_support_v<> tmp_rcy(&tmp_cy);
     bit_vector::select_1_type tmp_scy(&tmp_cy);
     cy = tmp_cy;
     rcy = tmp_rcy;
     scy = tmp_scy;
     res = F;

     range_maximum_sct<>::type tmp_rMq(&cst.csa);
     rMq = tmp_rMq;


     int n_nodes = cst.nodes();
     to_be_reported.push_back(vector<int>(n_nodes,n));
     to_be_reported.push_back(vector<int>(n_nodes,n));

     // Computes the BFS numbering of the CST and its inverse
     int global_bfs = n_nodes-1;
     bfs_to_v.resize(n_nodes);
     ids_to_bfs.resize(n_nodes);
     bfs_to_ids.resize(n_nodes);
     alphabet.resize(n_nodes);

     set<char_t> tmp_alphabet;
     // for(int i = 0; i < n; ++i) tmp_alphabet.insert(X[i]);
     for(auto child: cst.children(cst.root())){
       if((int)cst.sn(child) == n) continue;
       tmp_alphabet.insert(cst.edge(child,1));
     }

     queue<node_t> f;
     f.push(cst.root());
     while(!f.empty()){
       node_t v = f.front();
       f.pop();
       int node_id = cst.id(v);
       bfs_to_v[global_bfs] = v;
       bfs_to_ids[global_bfs] = node_id;
       ids_to_bfs[node_id] = global_bfs--;
       // Build the Weiner link character for each node
       for(auto c: tmp_alphabet){
         node_t wl = cst.wl(v,c);
         if(cst.root() != wl) alphabet[node_id].insert(c);
       }
       if(!cst.is_leaf(v))
         for(auto child:cst.children(v)) f.push(child);
     }


     // global_max_min.resize(n_nodes,n_nodes);
     impq.init(n_nodes);

     for(int i = 0; i < n; ++i){
       int isn = cst.csa.isa[i];
       node_t u = cst.select_leaf(isn+1);
       assert(cst.sn(u) == i);
       // global_max_min[cst.id(u)] = i+1;
       // q.push(make_pair(i+1,ids_to_bfs[cst.id(u)]));
       impq.push(ids_to_bfs[cst.id(u)],i+1);
     }

     node_t v = cst.root();
     highest_d(v,n);

     while(process()){}

     return res->size();
   }


protected:
  // Internal functions

  // Arguments:
  //   v = a node in the suffix tree,
  //   l = the upper bound on the porition of the rightmost y,
  // Returns:
  //   the highest value of d < l such that all leaves in the subtree rooted in v
  //   have a y d positions to the right.
  int highest_d(node_t v,int l){
    // if(l <= 0) return -1;
    int min_d = l-1;
    if(cst.is_leaf(v)){ // Is a leaf
      int sn = n-cst.sn(v)-1; // the bitvector of the coloring is not reversed
      if(sn+l<=n && sn >= 0){
        int rank = rcy(sn+l);
        min_d = (rank?scy(rank):-1) - sn;
      }
            // cout << "v.id: " << cst.id(v) << " min_d: "<< min_d <<" sn: "<<sn<< endl;
    }else{ // Is an inner node
      // Check if d > max in subtree
      int max_val = cst.csa[rMq(cst.lb(v),cst.rb(v))];
      if(l-1 <= max_val){
        bool all_leaves = boost;
        bool first = true;
        bool all_eq = true;
        for(auto child: cst.children(v)){
          // cout << "v.id: " << cst.id(v) << " min_d: "<< min_d << endl;
          int d = highest_d(child, l);
          if(first) min_d = d, first = false;
          if(min_d != d)all_eq = false;
          if(d < min_d) swap(d,min_d);
          if(!cst.is_leaf(child)) all_leaves = false;
        }
        if(all_leaves){ // Let us try to understand if we can skipp this and use the same algorithm for both cases.
          // cout << "all leaves" << endl;
          while(!all_eq && min_d>=0){
            first = true;
            l = min_d+1;
            for(auto child: cst.children(v)){
              if(first) all_eq = true, first = false;
              // cout << "v.id: " << cst.id(v) << " min_d: "<< min_d << endl;
              int d = highest_d(child, l);
              if(min_d != d)all_eq = false;
              if(d < min_d) min_d = d;//swap(d,min_d);
            }
          }
        }
      }
    }
    // global_max_min[cst.id(v)] = min_d;
    // q.push(make_pair(min_d,ids_to_bfs[cst.id(v)]));
    impq.push(ids_to_bfs[cst.id(v)],min_d);
    return min_d;
  }

  int find_second_max_val(int lb,int rb){
    int pos_max = rMq(lb,rb);
    if(pos_max == lb) return cst.csa[rMq(pos_max+1,rb)];
    else if(pos_max == rb) return cst.csa[rMq(lb,pos_max-1)];
    else return max(cst.csa[rMq(lb,pos_max-1)],cst.csa[rMq(pos_max+1,rb)]);
  }

  // Returns:
  //   true if some element in the priority queue has been processed.
  //   false if the priority queue is empty.
  bool process(){
    // if(q.empty()) return false;
    // pair<int,int> current = q.top();
    // q.pop();
    if(impq.is_empty()) return false;

    // // Print the priority queue
    // cout << impq.print();
    // // Print the left minimal ds
    // for(int i = 0; i < 2; ++i){
    //   for(auto elem: to_be_reported[i])
    //     cout << setw(2) << elem << " ";
    //   cout << endl;
    // }
    // cout << endl;

    pair<int,int> current = impq.get_max();

    node_t v = bfs_to_v[current.second];
    int node_id = bfs_to_ids[current.second];
    int d = current.first;
    if(d<0) return true;

    // if(global_max_min[node_id] == d){
      // 1. Check whether v is a leaf or not
      if(!cst.is_leaf(v)){
        // 2.REPORT
        node_t slink_p = cst.sl(v);
        node_t slink_pp = cst.parent(slink_p);
        while(cst.depth(cst.parent(v)) < cst.depth(slink_pp)+1 && slink_p != cst.root()){
          slink_p = slink_pp;
          slink_pp = cst.parent(slink_p);
        }
        bool report = (to_be_reported[(d+1)%2][cst.id(slink_p)] != d+1);
        // Check if d > max in subtree
        int second_max_val = find_second_max_val(cst.lb(v),cst.rb(v));
        if(report && res!= NULL && d <= second_max_val && cst.root() != v){
            res->push_back(make_pair(make_pair(n-second_max_val-1,cst.depth(cst.parent(v))+1),d));
        }
          // // 3. Inibhit children and Winer link children
          // if((to_be_reported[d%2][node_id] == d) || report){
          //   for(auto child : cst.children(v)){
          //     if(!cst.is_leaf(child)){
          //       to_be_reported[d%2][cst.id(child)] = d;
          //     }
          //   }
          //   for(auto c : alphabet[node_id]){
          //     node_t u = cst.wl(v,c);
          //     if(d>0 && ((cst.depth(v)+1 - cst.depth(cst.parent(u))) <= (cst.depth(v) - cst.depth(cst.parent(v)))))
          //     to_be_reported[(d-1)%2][cst.id(u)] = d-1;
          //   }
          // }

      }
      // 4.COMPUTE NEXT VALUE
      int new_d = highest_d(v, d);
      // 5.PROPAGARE MIN UP TO THE TREE
      while(v != cst.root() && impq.get_key(ids_to_bfs[cst.id(v)]) >= new_d){
        // global_max_min[cst.id(v)] = new_d;
        impq.push(ids_to_bfs[cst.id(v)], new_d);
        v = cst.parent(v);
      }
      if(v == cst.root() && impq.get_key(ids_to_bfs[cst.id(v)]) >= new_d){
        // global_max_min[cst.id(v)] = new_d;
        impq.push(ids_to_bfs[cst.id(v)], new_d);
      }
    // }else{
    //   current.first = global_max_min[node_id];
    //   if(current.first >= 0) q.push(current);
    // }

    // to_be_reported[d%2][node_id] = true;
    to_be_reported[d%2][node_id] = d;
    return true;
  }


private:
  cst_t cst; // Compressed Suffix Tree
  bit_vector cy; // Bitvector with 1s in position of y
  rank_support_v<> rcy; // rank support for cy
  bit_vector::select_1_type scy; // select support for cy
  int n; // length of the text

  indexMaxPQ impq;

  vector<node_t> bfs_to_v;
  vector<int> bfs_to_ids;
  vector<int> ids_to_bfs;
  vector<set<char_t>> alphabet;
  std::vector<std::pair<std::pair<int,int>,int>>* res;
  // if d is even then I can use [0]
  // if d is even then I can use [1]
  vector<vector<int>> to_be_reported;
  // vector<bit_vector> to_be_reported;
  // Range maximum query over csa in cst
  range_maximum_sct<>::type rMq;

  const char* text;

  bool boost;

};

miner<cst_sct3<> > miner_char;
miner<cst_sct3<csa_bitcompressed<int_alphabet<> > >  > miner_int;

// Wrapper functions
int mine(const char *X, const char *C, int _n,
         char y, std::vector<std::pair<std::pair<int,int>,int>>* F){
           return miner_char.mine(X,1,C,1,_n,y,F);
         }

int mine_int(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
         int _n, unsigned long int y, std::vector<std::pair<std::pair<int,int>,int>>* F){
           return miner_int.mine(X,num_bytes_X,C,num_bytes_C,_n,y,F);
         }

void boost_options(bool value){
  miner_char.set_boost(value);
  miner_int.set_boost(value);
}
