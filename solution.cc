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


inline int ToIdx(char c) {
  return c - 'a';
}


struct SAMNode {
  // longest(v)
  int length_ = 0;
  // suffix link
  SAMNode *parent_ = nullptr;
  // transitions a-z.
  vector<SAMNode *> children_{26, nullptr};
};


SAMNode *AddedLastCharacterToSAM(SAMNode *root, SAMNode *last, char c) {
  auto cur = new SAMNode;
  cur->length_ = last->length_ + 1;

  auto p = last;
  while (p && !p->children_[ToIdx(c)]) {
    p->children_[ToIdx(c)] = cur;
    p = p->parent_;
  }

  if (p == nullptr) {
    cur->parent_ = root;
  } else {
    auto q = p->children_[ToIdx(c)];
    if (p->length_ + 1 == q->length_) {
      // continue.
      cur->parent_ = q;
    } else {
      // discontinue.
      auto split_q = new SAMNode;

      split_q->length_ = p->length_ + 1;
      split_q->parent_ = q->parent_;
      split_q->children_ = q->children_;

      while (p && p->children_[ToIdx(c)] == q) {
        p->children_[ToIdx(c)] = split_q;
        p = p->parent_;
      }

      q->parent_ = split_q;
      cur->parent_ = split_q;
    }
  }
  return cur;
}


void PrintSAMNode(SAMNode *node) {
  cout << "--------------------------------" << endl;
  cout << "ptr: " << node << endl;
  cout << "parent: " << node->parent_ << endl;
  cout << "length: " << node->length_ << endl;
  for (char c = 'a'; c <= 'z'; ++c) {
    auto child = node->children_[ToIdx(c)];
    if (child) {
      cout << "child " << c << ": " << child << endl;
    }
  }
}


void PrintSAMTree(SAMNode *root) {
  vector<SAMNode *> cur_level, next_level;
  unordered_set<SAMNode *> searched;

  cur_level.push_back(root);
  int level = 1;

  while (!cur_level.empty()) {
    cout << "###############################" << endl;
    cout << "# Level: " << level << endl;
    cout << "###############################" << endl;

    for (auto node : cur_level) {
      PrintSAMNode(node);
      for (char c = 'a'; c <= 'z'; ++c) {
        auto child = node->children_[ToIdx(c)];
        if (child && searched.count(child) == 0) {
          next_level.push_back(child);
          searched.insert(child);
        }
      }
    }

    cout << endl << endl;

    swap(cur_level, next_level);
    next_level.clear();
    ++level;
  }
}


void TopologySort(
    SAMNode *node,
    unordered_set<SAMNode *> &searched,
    list<SAMNode *> &ret) {

  if (searched.count(node) > 0) {
    return;
  }

  searched.insert(node);
  for (int idx = 0; idx < 26; ++idx) {
    auto child = node->children_[idx];
    if (child) {
      TopologySort(child, searched, ret);
    }
  }
  ret.push_front(node);
}


void CreateDOTSource(vector<SAMNode *> nodes) {
  unordered_map<SAMNode *, string> max_substr, min_substr, node_id;
  // init.
  auto root = nodes.front();
  min_substr[root] = max_substr[root] = "";
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
    for (char c = 'a'; c <= 'z'; ++c) {
      auto child = node->children_[ToIdx(c)];
      if (child == nullptr) {
        continue;
      }
      fout << "  " << node_id[node] << " -> " << node_id[child]
           << " [label=\"" << c << "\"]"
           << endl;

      // update min_substr, max_substr.
      if (max_substr.count(child) == 0
          || max_substr[node].size() + 1 > max_substr[child].size()) {
        max_substr[child] = max_substr[node] + string(1, c);
      }
      if (min_substr.count(child) == 0
          || min_substr[node].size() + 1 < min_substr[child].size()) {
        min_substr[child] = min_substr[node] + string(1, c);
      }
    }

    // suffix link.
    if (node->parent_) {
      fout << "  " << node_id[node] << " -> " << node_id[node->parent_]
           << " [color=\"blue\"]"
           << endl;
    }

    // label.
    fout << "  " << node_id[node]
         << " [label=\"";
    if (i == 0) {
      fout << "nil";
    } else {
      fout << max_substr[node] << "\\n" << min_substr[node];
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

  auto root = new SAMNode;
  auto last = root;

  for (char c : s) {
    last = AddedLastCharacterToSAM(root, last, c);
  }

  PrintSAMTree(root);

  unordered_set<SAMNode *> searched;
  list<SAMNode *> ret;
  TopologySort(root, searched, ret);
  CreateDOTSource(vector<SAMNode *>(ret.begin(), ret.end()));
}
