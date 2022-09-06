//
// Created by huyang on 1/10/20.
//

#include "BasicPermPair.h"

BasicPermPair::BasicPermPair() {

}

BasicPermPair::BasicPermPair(BasicPermPair& e) {
    headPair = e.headPair;
    first = e.first;
    second = e.second;
}

BasicPermPair::BasicPermPair(BasicPermPair&& e) noexcept:
        headPair(std::move(e.headPair)),
        first(std::move(e.first)),
        second(std::move(e.second))
{

}

/*BasicPermPair::BasicPermPair(list<int>& headPair) {
    this->headPair = headPair;
}*/

list<int>& BasicPermPair::getHeadPair() {
    return this->headPair;
}

void BasicPermPair::print() {
    for (int &i : this->headPair) {
        cout << "head " << i << endl;
    }
    cout << "first:";
    for (int &i : this->first) {
        cout << " " << i;
    }
    cout << endl;
    cout << "secnd:";
    for (int &i : second) {
        cout << " " << i;
    }
    cout << endl;
}