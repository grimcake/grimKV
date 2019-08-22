#pragma once
#include <string>
using namespace std;

class DataNode
{
public:
    DataNode(string server_ip, int server_port, int id);
    ~DataNode();
    void startup();
    void start();
    void sendDataInfo(); // 新的datanode向server发送信息
    void sendHeartInfo(); // 向server发送心跳信息
    void dealServerCmd(); // 处理管理节点发过来的请求
private:
    int id_;
    int server_port_;
    string server_ip_;
    int sock_fd_;
};