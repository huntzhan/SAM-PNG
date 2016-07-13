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


struct State {
  // range of substrings.
  int max_length_ = 0;
  int min_length_ = 0;

  // first endpoint.
  int endpoint_ = -1;

  // suffix link
  State *suffix_link_ = nullptr;

  // transitions a-z.
  unordered_map<char, State *> trans_;
};


State *AddedLastCharacterToSAM(State *root, State *last, char c) {
  auto cur = new State;
  cur->max_length_ = last->max_length_ + 1;
  cur->endpoint_ = last->endpoint_ + 1;

  auto p = last;
  while (p && p->trans_.count(c) == 0) {
    p->trans_[c] = cur;
    cur->min_length_ = p->min_length_ + 1;
    p = p->suffix_link_;
  }

  if (p == nullptr) {
    cur->suffix_link_ = root;
  } else {
    auto q = p->trans_[c];
    if (p->max_length_ + 1 == q->max_length_) {
      // continue.
      cur->suffix_link_ = q;
    } else {
      // discontinue.
      auto split_q = new State;

      split_q->max_length_ = p->max_length_ + 1;
      split_q->endpoint_ = q->endpoint_;

      split_q->suffix_link_ = q->suffix_link_;
      split_q->trans_ = q->trans_;

      while (p && p->trans_.count(c) > 0 && p->trans_[c] == q) {
        p->trans_[c] = split_q;
        split_q->min_length_ = p->min_length_ + 1;
        p = p->suffix_link_;
      }

      q->suffix_link_ = split_q;
      q->min_length_ = split_q->max_length_ + 1;

      cur->suffix_link_ = split_q;
    }
  }
  return cur;
}


void TopologySort(
    State *node,
    unordered_set<State *> &searched,
    vector<State *> &nodes) {

  if (searched.count(node) > 0) {
    return;
  }
  searched.insert(node);

  for (auto &tran : node->trans_) {
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
        node->trans_.begin(), node->trans_.end()
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
    if (node->suffix_link_) {
      fout << "  " << node_id[node] << " -> " << node_id[node->suffix_link_]
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
          node->endpoint_ - node->max_length_ + 1,
          node->max_length_);
      auto shortest = text.substr(
          node->endpoint_ - node->min_length_ + 1,
          node->min_length_);

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
  string s;
  fin >> s;

  auto root = new State;
  auto last = root;

  for (char c : s) {
    last = AddedLastCharacterToSAM(root, last, c);
  }

  unordered_set<State *> searched;
  vector<State *> nodes;

  TopologySort(root, searched, nodes);
  reverse(nodes.begin(), nodes.end());

  CreateDOTSource(nodes, s);
}
