//
// Created by huyang on 1/10/20.
//

#include "PermPair.h"

PermPair::PermPair() {

}

PermPair::PermPair(PermPair&& o) noexcept:
    first(std::move(o.first)),
    second(std::move(o.second)) {

}


void PermPair::addFirst(list<int>* first) {
    this->first.emplace_back(first);
}

void PermPair::addSecond(list<int>* second) {
    this->second.emplace_back(second);
}

void PermPair::print() {
    cout << "first:" << endl;
    for(auto & f : this->first) {
        cout << "{";
        for(int ff : *f) {
            cout << ff << ",";
        }
        cout << "}" << endl;
    }

    cout << "second:" << endl;
    for(auto & f : this->second) {
        cout << "{";
        for(int ff : *f) {
            cout << ff << ",";
        }
        cout << "}" << endl;
    }
}

PermPair& PermPair::operator=(const PermPair& e) {
    this->first = e.first;
    this->second = e.second;
    return *this;
}

PermPair::PermPair(const PermPair& e) {
    this->first = e.first;
    this->second = e.second;
}
