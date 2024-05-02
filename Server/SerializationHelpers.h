// Kyryl Sydorov, 2024

#pragma once

#include <iostream>
#include <vector>

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    out << vec.size() << ' ';
    for (const T& elem : vec)
    {
        out << elem << ' ';
    }
    return out;
}

template <typename T>
std::istream& operator>>(std::istream& in, std::vector<T>& vec)
{
    size_t size;
    in >> size;
    vec.resize(size);
    for (T& elem : vec)
    {
        in >> elem;
    }
    return in;
}