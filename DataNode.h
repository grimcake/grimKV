#pragma once
#include <string>
#include "./HashCache.h"
using namespace std;

class HashCache;
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
    string Get(string key); //根据key获取value
    void Put(string key, string value); // 将key-value对存入内存中
private:
    int id_;
    int server_port_;
    string server_ip_;
    int sock_fd_;
    HashCache cache_;
};