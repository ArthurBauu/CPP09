#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <stdexcept>

class PmergeMe {
public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    void parseArgs(int argc, char** argv);
    void sort();
    void printResults() const;

private:
    std::vector<int>    _vec;
    std::deque<int>     _deq;
    std::vector<int>    _original;
    double              _vecTime;
    double              _deqTime;

    void fordJohnsonVec(std::vector<int>& arr);
    void fordJohnsonDeq(std::deque<int>& arr);
};



#endif