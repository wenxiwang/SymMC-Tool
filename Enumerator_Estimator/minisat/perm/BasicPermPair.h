//
// Created by huyang on 1/10/20.
//

#ifndef CPP_BASICPERMPAIR_H
#define CPP_BASICPERMPAIR_H

#include "dep.h"

class BasicPermPair {
    public:
    list<int> headPair;
    list<int> first;
    list<int> second;
    BasicPermPair();
    //BasicPermPair(list<int>& headPair);
    BasicPermPair(BasicPermPair&);
    BasicPermPair(BasicPermPair&&) noexcept;
    list<int>& getHeadPair();
    void print();
};


#endif //CPP_BASICPERMPAIR_H
