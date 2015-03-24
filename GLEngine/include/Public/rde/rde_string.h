#ifndef RDESTL_STRING_H
#define RDESTL_STRING_H

#include "Core.h"
#include "basic_string.h"
#include "hash.h"

namespace rde
{
typedef basic_string<char>	string;

template<typename E, class TAllocator, typename TStorage>
struct hash<basic_string<E, TAllocator, TStorage> >
{
    hash_value_t operator()(const basic_string<E, TAllocator, TStorage>& x) const 
    {
        // Derived from: http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-talk/142054
        hash_value_t h = 0;
		for (typename basic_string<E, TAllocator, TStorage>::size_type p = 0; p < x.length(); ++p) 
		{
            h = x[p] + (h<<6) + (h<<16) - h;
        }
        return h & 0x7FFFFFFF;
    }
};
/*
string operator+(const string& lhs, const string& rhs)
{
	string str;

	str.reserve(lhs.length() + rhs.length());
	str += lhs;
	str += rhs;
	return str;
}
string operator+(const string::value_type* lhs, const string& rhs)
{
	string str;
	str.reserve(strlen(lhs) + rhs.length());
	str.append(lhs);
	str += rhs;
	return str;
}
*/

string format(const char* fmt, ...);
string to_string(int i);
string to_string(unsigned int u);
string to_string(float f);
string to_string(uint64 i);

}

#endif // RDESTL_STRING_H
