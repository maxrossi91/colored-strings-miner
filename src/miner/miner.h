////////////////////////////////////////////////////////////////////////////////
// miner.h
//   The main header for the miner algorithm. This files needs to be included
//   to use the miner algorithm.
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

#ifndef __MINER_HH
#define __MINER_HH

#include <vector>
#include <inttypes.h>

// NOTE: I am assuming that X is the reverse of the text and C is NOT the reverse
//       of the coloring.
//
// Arguments:
//   X[0..n-1] = input string,
//   C[0..n-1] = input string colors,
//   y = a color,
//   F = a pointer (can to be NULL) to a container storing the output
//     y-unique minimal patterns as a sequence of pairs (pattern, distance)
//     where pattern is a substring (pos,len) of X and distance stores the
//     position of the color y with respect to pos + len -1.
// Returns:
//   the number of minimal pattern in the parsing of X.
int mine(const char *X, const char *C, int _n,
         char y, std::vector<std::pair<std::pair<int,int>,int> >* F);

 // NOTE: I am assuming that X is the reverse of the text and C is NOT the reverse
 //       of the coloring.
 //
 // Arguments:
 //   X[0..n-1] = input string,
 //   num_bytes_X = the number of bystes of each character of X
 //   C[0..n-1] = input string colors,
 //   num_bytes_C = the number of bystes of each character of C
 //   y = a color,
 //   F = a pointer (can to be NULL) to a container storing the output
 //     y-unique minimal patterns as a sequence of pairs (pattern, distance)
 //     where pattern is a substring (pos,len) of X and distance stores the
 //     position of the color y with respect to pos + len -1.
 // Returns:
 //   the number of minimal pattern in the parsing of X.
int mine_int(const char *X, uint8_t num_bytes_X, const char *C, uint8_t num_bytes_C,
         int _n, unsigned long int y, std::vector<std::pair<std::pair<int,int>,int> >* F);

 // Arguments:
 //   value = true to enable the boost option, false otherwise..
void boost_options(bool value);

#endif // __MINER_HH
