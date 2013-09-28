/* 
 * File:   Utils.hpp
 * Author: Piotr Brendel
 */

#ifndef UTILS_HPP
#define	UTILS_HPP

#include <iostream>

template <typename T>
std::ostream &operator<<(std::ostream &str, const std::vector<T> &v)
{
    for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); i++)
    {
        if (i != v.begin())
        {
            std::cout<<" ";
        }
        std::cout<<*i;
    }
    return str;
}

#endif	/* UTILS_HPP */

