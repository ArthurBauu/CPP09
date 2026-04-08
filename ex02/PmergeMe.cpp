#include "PmergeMe.hpp"
#include <algorithm>
#include <iomanip>
#include <climits>
#include <cstdlib>
#include <cctype>
#include <sys/time.h>


PmergeMe::PmergeMe() : _vecTime(0), _deqTime(0) {}


PmergeMe::PmergeMe(const PmergeMe& o)
    : _vec(o._vec), _deq(o._deq), _original(o._original),
      _vecTime(o._vecTime), _deqTime(o._deqTime) {}


PmergeMe& PmergeMe::operator=(const PmergeMe& o) {
    if (this != &o) {
        _vec      = o._vec;
        _deq      = o._deq;
        _original = o._original;
        _vecTime  = o._vecTime;
        _deqTime  = o._deqTime;
    }
    return *this;
}


PmergeMe::~PmergeMe() {}


void PmergeMe::parseArgs(int argc, char** argv) {
    if (argc < 2)
        throw std::invalid_argument("Error");
    for (int i = 1; i < argc; i++) {
        std::string s(argv[i]);
        if (s.empty())
            throw std::invalid_argument("Error");
        for (size_t j = 0; j < s.size(); j++) {
            if (!isdigit((unsigned char)s[j]))
                throw std::invalid_argument("Error");
        }
        long val = std::strtol(s.c_str(), NULL, 10);
        if (val <= 0 || val > INT_MAX)
            throw std::invalid_argument("Error");
        _vec.push_back((int)val);
        _deq.push_back((int)val);
        _original.push_back((int)val);
    }
}


static int jacobsthal(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int c = b + 2 * a;
        a = b;
        b = c;
    }
    return b;
}

// Trie FordJonhson : crée des pair, stock le dernier élement si nombre impair d'element, trie les pair a = plus petit, b = plus grand
// reorganise les pair par rapport a l'élement le plus grand, crée le trie final avec a0, b0, b1...
// ajoute les a1, a2 etc en partant de leur pair et en descendant en utilisant l'ordre de jackobstall
// ajoute l'élement impaire si besoin. 


static double getTimeMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}


void PmergeMe::fordJohnsonVec(std::vector<int>& arr) {
    size_t n = arr.size();
    if (n <= 1) return;

    bool hasStraggler = (n % 2 != 0);
    int  straggler    = 0;
    if (hasStraggler) {
        straggler = arr.back();
        arr.pop_back();
        n--;
    }

    // create pairs
    std::vector<std::pair<int, int> > pairs;
    pairs.reserve(n / 2);
    for (size_t i = 0; i < n; i += 2) {
        int a = arr[i], b = arr[i + 1];
        if (a > b) std::swap(a, b);
        pairs.push_back(std::make_pair(a, b));
    }

    // Recursively sort pairs bigger element
    std::vector<int> bVals;
    bVals.reserve(pairs.size());
    for (size_t i = 0; i < pairs.size(); i++)
        bVals.push_back(pairs[i].second);
    fordJohnsonVec(bVals);

    // Reorder pairs
    std::vector<std::pair<int, int> > sortedPairs;
    sortedPairs.reserve(pairs.size());
    std::vector<bool> used(pairs.size(), false);
    for (size_t i = 0; i < bVals.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (!used[j] && pairs[j].second == bVals[i]) {
                sortedPairs.push_back(pairs[j]);
                used[j] = true;
                break;
            }
        }
    }
    pairs = sortedPairs;

    // Build main chain, starting with a0
    std::vector<int> chain;
    chain.reserve(n + 1);
    chain.push_back(pairs[0].first);
    for (size_t i = 0; i < pairs.size(); i++)
        chain.push_back(pairs[i].second);

    // Insert remaining smaller elements (pend) in Jacobsthal order
    size_t pendSize = pairs.size() - 1;
    int    k        = 2;
    size_t groupStart = 0;
    while (groupStart < pendSize) {
        size_t groupEnd = std::min((size_t)(jacobsthal(k) - 1), pendSize - 1);
        for (int pi = (int)groupEnd; pi >= (int)groupStart; pi--) {
            int val   = pairs[pi + 1].first;
            int bound = pairs[pi + 1].second;
            std::vector<int>::iterator boundIt =
                std::lower_bound(chain.begin(), chain.end(), bound);
            std::vector<int>::iterator pos =
                std::lower_bound(chain.begin(), boundIt, val);
            chain.insert(pos, val);
        }
        groupStart = groupEnd + 1;
        k++;
    }

    if (hasStraggler) {
        std::vector<int>::iterator pos =
            std::lower_bound(chain.begin(), chain.end(), straggler);
        chain.insert(pos, straggler);
    }

    arr = chain;
}


void PmergeMe::fordJohnsonDeq(std::deque<int>& arr) {
    size_t n = arr.size();
    if (n <= 1) return;

    bool hasStraggler = (n % 2 != 0);
    int  straggler    = 0;
    if (hasStraggler) {
        straggler = arr.back();
        arr.pop_back();
        n--;
    }

    std::deque<std::pair<int, int> > pairs;
    for (size_t i = 0; i < n; i += 2) {
        int a = arr[i], b = arr[i + 1];
        if (a > b) std::swap(a, b);
        pairs.push_back(std::make_pair(a, b));
    }

    std::deque<int> bVals;
    for (size_t i = 0; i < pairs.size(); i++)
        bVals.push_back(pairs[i].second);
    fordJohnsonDeq(bVals);

    std::deque<std::pair<int, int> > sortedPairs;
    std::vector<bool> used(pairs.size(), false);
    for (size_t i = 0; i < bVals.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (!used[j] && pairs[j].second == bVals[i]) {
                sortedPairs.push_back(pairs[j]);
                used[j] = true;
                break;
            }
        }
    }
    pairs = sortedPairs;

    std::deque<int> chain;
    chain.push_back(pairs[0].first);
    for (size_t i = 0; i < pairs.size(); i++)
        chain.push_back(pairs[i].second);

    size_t pendSize = pairs.size() - 1;
    int    k        = 2;
    size_t groupStart = 0;
    while (groupStart < pendSize) {
        size_t groupEnd = std::min((size_t)(jacobsthal(k) - 1), pendSize - 1);
        for (int pi = (int)groupEnd; pi >= (int)groupStart; pi--) {
            int val   = pairs[pi + 1].first;
            int bound = pairs[pi + 1].second;
            std::deque<int>::iterator boundIt =
                std::lower_bound(chain.begin(), chain.end(), bound);
            std::deque<int>::iterator pos =
                std::lower_bound(chain.begin(), boundIt, val);
            chain.insert(pos, val);
        }
        groupStart = groupEnd + 1;
        k++;
    }

    if (hasStraggler) {
        std::deque<int>::iterator pos =
            std::lower_bound(chain.begin(), chain.end(), straggler);
        chain.insert(pos, straggler);
    }

    arr = chain;
}


void PmergeMe::sort() {
    double t1 = getTimeMicros();
    fordJohnsonVec(_vec);
    _vecTime = getTimeMicros() - t1;

    double t2 = getTimeMicros();
    fordJohnsonDeq(_deq);
    _deqTime = getTimeMicros() - t2;
}


void PmergeMe::printResults() const {
    std::cout << "Before:";
    for (size_t i = 0; i < _original.size(); i++)
        std::cout << " " << _original[i];
    std::cout << "\n";

    std::cout << "After:";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << "\n";

    std::cout << std::fixed << std::setprecision(5)
              << "Time to process a range of " << _original.size()
              << " elements with std::vector : " << _vecTime << " us\n"
              << "Time to process a range of " << _original.size()
              << " elements with std::deque  : " << _deqTime << " us\n";
}
