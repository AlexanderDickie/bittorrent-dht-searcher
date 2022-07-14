#include "parse_message.h"

#include <stdio.h>
#include <string>

#include <array>
#include <vector>
#include <set>

#ifndef UNTITLED_SEARCHER_H
#define UNTITLED_SEARCHER_H
class Searcher
{
public:
    int fd {};
    nodeid_t my_id {};
    std::array<unsigned char, 20> ih;

    std::vector<Node> nodes {};
    std::set<nodeid_t> have_sent {};
    std::set<PeerInfo> peers {};

    const unsigned char* msg;
    size_t msg_len;

    Searcher(int _fd, nodeid_t _ih, nodeid_t _my_id);

    void add_node_to_closest(const ParsedMessage& pmes, const sockaddr& from);
    int listener();


};
#endif //UNTITLED_SEARCHER_H
