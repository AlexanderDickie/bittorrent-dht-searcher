
#include "searcher.h"

#include "parse_message.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>

#include <array>
#include <vector>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>


int distance(nodeid_t id, nodeid_t ih)
{
    int out {0};
    for (int i {0}; i < 20; i++)
    {
        out += (std::bitset<8>(id.at(i) ^ ih.at(i))).count();
    }
    return out;
}

//return byte array of a get peers request message
std::vector<unsigned char> mk_getpeers_req(const nodeid_t id, const nodeid_t ih)
{
    std::string add;
    std::vector<unsigned char> out;

    add = "d1:ad2:id20:";
    out.insert(out.end(), add.begin(), add.end());

    out.insert(out.end(), id.begin(), id.end());

    add = "9:info_hash20:";
    out.insert(out.end(), add.begin(), add.end());

    out.insert(out.end(), ih.begin(), ih.end());

    add = "e1:q9:get_peers1:t2:aa1:y1:qe";
    out.insert(out.end(), add.begin(), add.end());

    return out;
}

void print_num_peers(std::set<PeerInfo> peers)
{
    std::cout << "\r" << "Number of peers found so far = " << peers.size() << std::flush;

}

void print_peers(std::set<PeerInfo> peers)
{
    std::cout << '\n';
    for (auto peer: peers)
    {
        std::cout << "ip = ";
        for (auto c : peer.ip)
        {
            std::cout << static_cast<int>(c) << '.';
        }
        std::cout << "\b ";

        uint16_t port;
        memcpy(&port, peer.port.data(), sizeof(port));
        std::cout << "port = " << port << '\n';

    }
}


Searcher::Searcher(int _fd, nodeid_t _ih, nodeid_t _my_id)
    {
        fd = _fd;
        ih = _ih;
        my_id = _my_id;

        std::vector<unsigned char> msg_vec {mk_getpeers_req(my_id, ih)};
        msg = msg_vec.data();
        msg_len = msg_vec.size();

        //send bootstrap get_peers request
        std::string hostname{"67.215.246.10"};
        uint16_t port = 6881;
        sockaddr_in dest;
        dest.sin_family = AF_UNSPEC;
        dest.sin_port = htons(port);
        dest.sin_addr.s_addr = inet_addr(hostname.c_str());
        sendto(fd, msg, msg_len, 0, reinterpret_cast<sockaddr *>(&dest), sizeof(dest));

        std::cout << "Press return to finish" << '\n';
        print_num_peers(peers);

        listener();
    }

//mutates our 8 closest nodes vector, and our peer info set
//iteratively searches the dht looking for the closest nodes ids to our ih
//stores peer information that it finds on the way
int Searcher::listener()
{
        fd_set read_fds;
        fd_set ui_fd;

        //wait 3 sec before returning peers
        timeval tv;
        tv.tv_sec = 4;
        tv.tv_usec = 0;

        timeval ui_tv;
        ui_tv.tv_sec = 0;
        ui_tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        FD_ZERO(&ui_fd);
        FD_SET(0, &ui_fd);

        while (select(fd+1
                      , &read_fds, NULL, NULL, &tv))
        {
            //check for return key press
            FD_SET(0, &ui_fd);
            if (select(1, &ui_fd, NULL, NULL, &ui_tv)){
                print_peers(peers);
                return 0;
            }

            //check if socket is ready to read from
            if (FD_ISSET(fd, &read_fds))
            {

                //read message from socket
                sockaddr from;
                socklen_t from_len {sizeof(from)};

                unsigned char buf[512];
                int num_bytes {0};
                if ((num_bytes=recvfrom(fd, buf, sizeof buf, 0, &from, &from_len)) == -1)
                    break;
                std::array<unsigned char, 512> msg_ar;
                std::copy(buf, buf+512, msg_ar.data());


                //parse message
                ParsedMessage pmes {};
                ResponseType resp;

                if ((resp = parse_message(msg_ar, pmes)) != ResponseType::Nodes) {
                    if (resp == ResponseType::Peers)
                    {
                        for (auto peer : pmes.peers)
                            peers.insert(peer);
                        add_node_to_closest(pmes, from);
                        print_num_peers(peers);
                    }

                    continue;
                }
                //for adding bootstrap to have_sent
                if (!have_sent.contains(pmes.id))
                    have_sent.insert(pmes.id);


                //send children get_peers request
                for (auto child: pmes.children)
                {
                    if ((!have_sent.contains(child.id)))
                    {
                        sockaddr_in dest {};
                        dest.sin_family = AF_UNSPEC;
                        //we received the ip and port bytes in network order, so directly copy them
                        memcpy(&dest.sin_port, &child.port, sizeof dest.sin_port);
                        memcpy(&dest.sin_addr.s_addr, &child.ip, sizeof dest.sin_addr.s_addr);

                        have_sent.insert(child.id);

                        sendto(fd, msg, msg_len, 0, reinterpret_cast<sockaddr *>(&dest), sizeof(dest));

                    }
                }
            }
        }

        print_peers(peers);
        return 0;

}

//mutates nodes, adding new node to them if it is one of the 8 closes nodes
void Searcher::add_node_to_closest(const ParsedMessage& pmes, const sockaddr& from) {

    Node new_node{pmes.id, from};

    int idx{0};
    for (auto node: nodes) {
        if (new_node.id == node.id)
            break;
        else if (distance(ih, new_node.id) < distance(ih, node.id)) {
            nodes.insert(nodes.begin() + idx, new_node);
            break;
        }
        idx++;
    }
    if (idx == nodes.size())
        nodes.push_back(new_node);
    if (nodes.size() == 9)
        nodes.pop_back();
}





