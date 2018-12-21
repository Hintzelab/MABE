
#pragma once

#include <cmath> // log2
#include <algorithm> // transform
#include <numeric> // accumulate
#include <iterator> // back_inserter, begin, end
#include <vector>

namespace loss {
  using std::vector;

  namespace priv {
    // this private namespace is used for helper functions that shouldn't be easily viewable by a user of the `loss` namespace
    template <typename T>
    double plog_poverphat(T p, T phat) {
      using std::log2;
      return static_cast<double>(p) * log2( static_cast<double>(p) / static_cast<double>(phat) );
    }
  }
  
  template <typename T>
  double cross_entropy(vector<T> output, vector<T> target) {
    using std::accumulate; using std::transform; using std::begin; using std::end; using std::back_inserter;
    vector<double> entropies;
    transform( begin(output), end(output), begin(target), back_inserter(entropies), priv::plog_poverphat<double> );
    return accumulate( begin(entropies), end(entropies), 0.0 );
  }
  
  // more loss functions here...
}
