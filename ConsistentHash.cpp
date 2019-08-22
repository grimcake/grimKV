#include "ConsistentHash.h"
#include "tools.h"
#include <sstream>

using namespace std;

ConsistentHash::ConsistentHash(int node_num, int virtual_node_num) : node_num_(node_num),
                                                                     virtual_node_num_(virtual_node_num)
{
}

ConsistentHash::~ConsistentHash()
{
    server_nodes_.clear();
}

void ConsistentHash::Initialize()
{
    for (int i = 0; i < node_num_; i++)
    {
        for (int j = 0; j < virtual_node_num_; j++)
        {
            stringstream node_key;
            node_key << "shared-" << i << "-node-" << j;
            int partion = Hash(node_key.str());
            server_nodes_.insert(pair<int, int>(partion, i));
        }
    }
}

int ConsistentHash::getServerIndex(const string &key)
{
    int partition = Hash(key);
    map<int, int>::iterator it = server_nodes_.lower_bound(partition);
    if (it == server_nodes_.end())
    {
        return server_nodes_.begin()->second;
    }
    return it->second;
}

void ConsistentHash::deleteNode(const int index)
{
    for (int j = 0; j < virtual_node_num_; j++)
    {
        stringstream node_key;
        node_key << "shared-" << index << "-node-" << j;
        int partition = Hash(node_key.str());
        map<int, int>::iterator it = server_nodes_.find(partition);
        if(it != server_nodes_.end()) 
        {
            server_nodes_.erase(it);
        }
    }
}

void ConsistentHash::addNewNode(const int index)
{
    for (int j = 0; j < virtual_node_num_; j++)
    {
        stringstream node_key;
        node_key << "shared-" << index << "-node-" << j;
        int partition = Hash(node_key.str());
        map<int, int>::iterator it = server_nodes_.find(partition);
        server_nodes_.insert(pair<int, int>(partition, index));
    }
    node_num_++;
}