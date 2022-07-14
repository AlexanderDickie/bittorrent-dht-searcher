#include "network.h"
#include "parse_message.h"
#include "searcher.h"
#include "utils.h"

#include <iostream>
#include <string>

#include <array>
#include <vector>

#define MY_ID "12121212121212121212"
#define IH "5AEE14ED6777A25F316A89118110F20F72465183"

int main()
{
    int fd;
    std::string ih_string;
    std::vector<unsigned char> ih_vec;
    nodeid_t ih_ar;

    std::string my_id_s {MY_ID};
    nodeid_t my_id;

    std::cout << "Enter a  torrent's infohash (in hex format) " << '\n';
    std::cin >> ih_string;
    ih_vec = hexs_to_bytes(ih_string);
    std::copy(ih_vec.begin(), ih_vec.end(), ih_ar.begin());

    std::copy(my_id_s.begin(),my_id_s.end(),my_id.begin());

    if ((fd = mk_bind_socket("6881")) == -1)
    {
        std::cout << "error making/binding socket"<< '\n';
        return 1;
    }

    Searcher s {fd, ih_ar, my_id};
    return 0;
}

//x





