
#include "parse_message.h"

#include <stdio.h>
#include <string>

#include <array>
#include <vector>

#include <sys/socket.h>


ResponseType parse_message(const std::array<unsigned char, 512> msg, ParsedMessage& pmes)
{
    std::string target;
    const unsigned char* pos {msg.begin()};
//    find id
    target = "2:id20:";
    if ((pos = std::search(msg.begin(), msg.end(), target.begin(), target.end())) == msg.end())
    {
        return ResponseType::Error;
    }
    pos += target.length();
    std::copy(pos, pos+20, pmes.id.begin());

    target = "5:nodes";
    if ((pos = std::search(msg.begin(), msg.end(), target.begin(), target.end())) != msg.end())
    {
        pos += target.length();
        target = ":";
        const unsigned char* colon_pos {std::search(pos, msg.end(), target.begin(), target.end())};
        if (colon_pos == msg.end())
            return ResponseType::Error;
        std::string ln_str {pos, colon_pos};
        int compacts_len {std::stoi(ln_str)};
        if (compacts_len > (msg.end()-pos))
            return ResponseType::Error;
        pos = colon_pos+1;

        for (int i {0}; i < compacts_len/26; i++)
        {
            Child child;
            pmes.children.push_back(child);

            std::copy(pos, pos+20, pmes.children.at(i).id.data());
            std::copy(pos+20, pos+24, pmes.children.at(i).ip.data());
            std::copy(pos+24, pos+26, pmes.children.at(i).port.data());

            pos += 26;
        }
        return ResponseType::Nodes;
    }

    target = "6:valuesl";
    if ((pos = std::search(msg.begin(), msg.end(), target.begin(), target.end())) != msg.end())
    {
        pos += target.length();
        target = "ee1:t2:aa";
        const unsigned char* end_pos {std::search(pos, msg.end(), target.begin(), target.end())};
        if (end_pos == msg.end())
            return ResponseType::Error;
        auto values_len {end_pos - pos};
        for (int i {0}; i < values_len/8; i++)
        {
            PeerInfo peer_info;
            pmes.peers.push_back(peer_info);

            std::copy(pos+2, pos+6, pmes.peers.at(i).ip.data());
            std::copy(pos+6, pos+8, pmes.peers.at(i).port.data());

            pos += 8;
        }

        return ResponseType::Peers;
    }

    return ResponseType::Error;

}