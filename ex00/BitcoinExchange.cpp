#include "BitcoinExchange.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cctype>

BitcoinExchange::BitcoinExchange()
{
    loadDatabase();
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &src) : _db(src._db) {
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &src)
{
    if (this != &src)
        _db = src._db;
    return *this;
}

BitcoinExchange::~BitcoinExchange() {
}


void BitcoinExchange::loadDatabase()
{
    std::ifstream file("data.csv");
    if (!file.is_open())
        throw std::runtime_error("Error: could not open database file.");

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::size_t comma = line.find(',');
        if (comma == std::string::npos)
            continue;

        std::string date    = line.substr(0, comma);
        std::string rateStr = line.substr(comma + 1);

        char       *end;
        float       rate = static_cast<float>(std::strtod(rateStr.c_str(), &end));
        if (*end != '\0' && *end != '\r')
            continue;

        _db[date] = rate;
    }
}

bool BitcoinExchange::isValidDate(const std::string &date)
{
    if (date.size() != 10)
        return false;
    if (date[4] != '-' || date[7] != '-')
        return false;

    for (std::size_t i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    int year  = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day   = std::atoi(date.substr(8, 2).c_str());

    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > 31)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        daysInMonth[1] = 29;

    return day <= daysInMonth[month - 1];
}


float BitcoinExchange::parseValue(const std::string &raw)
{
    std::size_t start = raw.find_first_not_of(" \t");
    if (start == std::string::npos)
        throw std::runtime_error("empty value");

    const std::string str = raw.substr(start);

    char       *end;
    double      val = std::strtod(str.c_str(), &end);

    if (end == str.c_str() || (*end != '\0' && *end != '\r'))
        throw std::runtime_error("bad value");

    return static_cast<float>(val);
}


void BitcoinExchange::exchange(const std::string &filename) const
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::size_t sep = line.find(" | ");
        if (sep == std::string::npos)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date     = line.substr(0, sep);
        std::string valueStr = line.substr(sep + 3);

        if (!isValidDate(date))
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        float value;
        try
        {
            value = parseValue(valueStr);
        }
        catch (...)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (value < 0.0f)
        {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }
        if (value > 1000.0f)
        {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        std::map<std::string, float>::const_iterator it = _db.lower_bound(date);

        if (it == _db.end() || it->first != date)
        {
            if (it == _db.begin())
            {
                std::cerr << "Error: bad input => " << date << std::endl;
                continue;
            }
            --it;
        }

        std::cout << date << " => " << value << " = " << value * it->second
                  << std::endl;
    }
}
