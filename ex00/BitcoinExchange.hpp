#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange
{
public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange &src);
    BitcoinExchange &operator=(const BitcoinExchange &src);
    ~BitcoinExchange();

    void exchange(const std::string &filename) const;

private:
    std::map<std::string, float> _db;

    void              loadDatabase();
    static bool       isValidDate(const std::string &date);
    static float      parseValue(const std::string &str);
};

#endif
