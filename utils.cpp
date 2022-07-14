#include "utils.h"
#include <vector>
#include <string_view>
#include <iostream>

int hex_to_int(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    else if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    else if (hex >= 'a' && hex <= 'a')
        return hex - 'a';
}

std::vector<unsigned char> hexs_to_bytes(std::string_view hex)
{
    assert(hex.size() % 2 == 0);
    std::vector<unsigned char> out;
    int inter_sm {0};

    for (int i {0}; char c : hex)
    {
        if ((i % 2)==0)
            inter_sm = hex_to_int(c);
        else
            out.push_back(inter_sm * 16 + hex_to_int(c));

        i++;
    }
    return out;
}







