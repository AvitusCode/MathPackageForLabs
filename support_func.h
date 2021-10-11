#pragma once


// Исправить функцию
namespace support
{
template<typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value)
{
    for (; first < last; ++first) 
    {
        *first = value;
    }
}
}
