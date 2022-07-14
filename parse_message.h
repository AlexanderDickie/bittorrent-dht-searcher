
#include <stdio.h>
#include <array>
#include <vector>

#include <sys/socket.h>

#ifndef UNTITLED_PARSE_MESSAGE_H
#define UNTITLED_PARSE_MESSAGE_H
using nodeid_t = std::array<unsigned char, 20>;
using ip_t = std::array<unsigned char, 4>;
using port_t = std::array<unsigned char, 2>;

struct Node
{
    nodeid_t id {};
    sockaddr addr {};
};

struct Child
{
    nodeid_t id {};
    ip_t ip {};
    port_t port {};
};

struct PeerInfo
{
    ip_t ip {};
    port_t port {};
};
inline bool operator<(const PeerInfo& lhs, const PeerInfo& rhs)
{
    return lhs.ip < rhs.ip;
}

enum ResponseType
{
    Nodes,
    Peers,
    Error,
};

struct ParsedMessage
{
    ResponseType type {};
    std::array<unsigned char, 20> id {};

    std::vector<Child> children {};
    std::vector<PeerInfo> peers {};
};

ResponseType parse_message(std::array<unsigned char, 512> msg, ParsedMessage& pmes);
#endif //UNTITLED_PARSE_MESSAGE_H
