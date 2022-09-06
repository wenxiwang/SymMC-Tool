//
// Created by huyang on 1/10/20.
//

#ifndef CPP_PERMPAIR_H
#define CPP_PERMPAIR_H

#include "dep.h"

class PermPair {
    public:
    list<list<int>*> first;
    list<list<int>*> second;
    PermPair();
    PermPair(const PermPair&);
    PermPair(PermPair&&) noexcept;
    PermPair& operator=(const PermPair&);

    void addFirst(list<int>* first);
    void addSecond(list<int>* second);
    void print();
};


#endif //CPP_PERMPAIR_H
