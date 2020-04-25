//
// Created by andrey yanpolsky on 4/23/20.
//

#ifndef CPP_TIME_WINDOW_H
#define CPP_TIME_WINDOW_H

#include <tuple>
#include <queue>

using Price = long long;
using Time = long long;

class time_window {
public:
    using Data = std::tuple<Price, Time>;
    std::priority_queue<Data> pq_;
private:

};


#endif //CPP_TIME_WINDOW_H
