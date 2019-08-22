#include <map>

using namespace std;

class ConsistentHash
{
friend class Server;
public:
    ConsistentHash(int node_num, int virtual_node_num);
    ~ConsistentHash();

    void Initialize();
    int getServerIndex(const string& key);
    void deleteNode(const int index);
    void addNewNode(const int index);

private:
    map<int, int> server_nodes_; // 虚拟节点，key是哈希值，value是机器的index
    int node_num_; // 真实机器节点个数
    int virtual_node_num_; // 每个机器节点关联的虚拟节点个数
};
