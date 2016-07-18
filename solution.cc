// {{{ $VIMCODER$ <-----------------------------------------------------
// vim:filetype=cpp:foldmethod=marker:foldmarker={{{,}}}

#include <algorithm>
#include <array>
#include <bitset>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// }}}

const string kInputFilename = "input.txt";
const string kOutputFilename = "output.txt";

ifstream fin(kInputFilename);
ofstream fout(kOutputFilename);


#define DEFINE_ACCESSOR_AND_MUTATOR(type, name) \
  type name() {                                 \
    return name ## _;                           \
  }                                             \
  void set_ ## name(type val) {                 \
    name ## _ = val;                            \
  }                                             \


class State {
 public:
  using TransType = unordered_map<char, State *>;

  DEFINE_ACCESSOR_AND_MUTATOR(int, maxlen)
  DEFINE_ACCESSOR_AND_MUTATOR(int, minlen)
  DEFINE_ACCESSOR_AND_MUTATOR(int, first_endpos)
  DEFINE_ACCESSOR_AND_MUTATOR(State *, link)
  DEFINE_ACCESSOR_AND_MUTATOR(bool, accept)
  DEFINE_ACCESSOR_AND_MUTATOR(TransType, trans)

  bool has_trans(char c) const {
    return trans_.count(c) > 0;
  }
  State *trans(char c) const {
    return trans_.at(c);
  }
  void set_trans(char c, State *v) {
    trans_[c] = v;
  }

 private:
  int maxlen_ = 0;
  int minlen_ = 0;
  int first_endpos_ = -1;

  State *link_ = nullptr;
  unordered_map<char, State *> trans_;

  bool accept_ = false;
};


State *AddSymbolToSAM(State *start, State *last, char c) {
  auto cur = new State;
  cur->set_maxlen(last->maxlen() + 1);
  cur->set_first_endpos(last->first_endpos() + 1);

  auto p = last;
  while (p && !p->has_trans(c)) {
    p->set_trans(c, cur);
    p = p->link();
  }

  if (p == nullptr) {
    cur->set_link(start);
    cur->set_minlen(1);
    return cur;
  }

  auto q = p->trans(c);
  if (p->maxlen() + 1 == q->maxlen()) {
    cur->set_link(q);
    cur->set_minlen(q->maxlen() + 1);
  } else {
    auto sq = new State;
    sq->set_maxlen(p->maxlen() + 1);
    sq->set_trans(q->trans());
    sq->set_first_endpos(q->first_endpos());
    
    while (p && p->has_trans(c) && p->trans(c) == q) {
      p->set_trans(c, sq);
      p = p->link();
    }

    sq->set_link(q->link());
    sq->set_minlen(sq->link()->maxlen() + 1);

    q->set_link(sq);
    q->set_minlen(sq->maxlen() + 1);

    cur->set_link(sq);
    cur->set_minlen(sq->maxlen() + 1);
  }

  return cur;
}


State *CreateSAM(const string &T) {
  auto start = new State;
  auto last = start;

  for (char c : T) {
    last = AddSymbolToSAM(start, last, c);
  }
  
  while (last != start) {
    last->set_accept(true);
    last = last->link();
  }

  return start;
}


void TopologySort(
    State *node,
    unordered_set<State *> &searched,
    vector<State *> &nodes) {

  if (searched.count(node) > 0) {
    return;
  }
  searched.insert(node);

  for (auto &tran : node->trans()) {
    auto child = tran.second;
    TopologySort(child, searched, nodes);
  }

  nodes.push_back(node);
}


void CreateDOTSource(const vector<State *> &nodes, const string &text) {

  // init node id.
  unordered_map<State *, string> node_id;
  for (int i = 0; i < nodes.size(); ++i) {
    node_id[nodes[i]] = "t" + to_string(i);
  }

  // head
  fout << "digraph G {" << endl;
  fout << "  rankdir=LR" << endl;
  fout << "  node[shape=\"box\"]" << endl;

  for (int i = 0; i < nodes.size(); ++i) {
    auto node = nodes[i];

    // transitions.
    vector<pair<char, State *>> ordered_trans(
        node->trans().begin(), node->trans().end()
        );
    sort(ordered_trans.begin(), ordered_trans.end());

    for (auto &tran : ordered_trans) {
      char c = tran.first;
      auto child = tran.second;

      fout << "  " << node_id[node] << " -> " << node_id[child]
           << " [label=\"" << c << "\"]"
           << endl;
    }

    // suffix link.
    if (node->link()) {
      fout << "  " << node_id[node] << " -> " << node_id[node->link()]
           << " [color=\"blue\"]"
           << endl;
    }

    // label.
    fout << "  " << node_id[node]
         << " [label=\"";
    if (i == 0) {
      fout << "empty";
    } else {

      auto longest = text.substr(
          node->first_endpos() - node->maxlen() + 1,
          node->maxlen());
      auto shortest = text.substr(
          node->first_endpos() - node->minlen() + 1,
          node->minlen());

      fout << longest << "\\n" << shortest;
    }
    fout << "\"]"
         << endl;
  }

  // tail
  fout << "}" << endl;
  fout.close();
}


int main() {
  string T;
  fin >> T;

  auto start = CreateSAM(T);

  unordered_set<State *> searched;
  vector<State *> nodes;

  TopologySort(start, searched, nodes);
  reverse(nodes.begin(), nodes.end());

  CreateDOTSource(nodes, T);
}
