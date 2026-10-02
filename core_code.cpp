#include <bits/stdc++.h>
using namespace std;
class rate_limiter {
  int request_limit = 10;
  unordered_map<string, deque<chrono::steady_clock::time_point>> data;

public:
  bool check(string token) {
    auto now = chrono::steady_clock::now();

    auto &dq = data[token];
    while (!dq.empty() && now > dq.front() + chrono::seconds(60))
      dq.pop_front();
    if (dq.size() >= request_limit) {
      return false;
    } else {
      dq.push_back(now);
      return true;
    }
  }
};
