#pragma once

#include <string>
#include <vector>
#include <unordered_map>

const int PAGE_SIZE = 2;
const int CACHE_SIZE = 2;
const int DISK_SIZE = 20;

class Page;
class Node;

// 单个数据节点
class Node
{
public:
    std::string key_;
    std::string value_;
    Page* page_; //所属页
};

// 一页
class Page
{
public:
    int file_num_; // 页对应文件序号
    bool lock_;
    bool dirty_; // 标记页是否被修改
    bool is_putpage_;
    Node data_[PAGE_SIZE];
    Page* next;
    Page* prev;
    Page() :
    lock_(false),
    dirty_(false),
    is_putpage_(false)
    {}
};

class HashCache
{
public:
    HashCache(int capacity = CACHE_SIZE);
    ~HashCache();
    void save_cache();
    std::string get(std::string key);
    void put(std::string key, std::string value);
private:
    Page* entries_; // 初始分配的所有Page
    std::vector<Page*> free_entries_; // 存储空闲page
    std::unordered_map<std::string, Node*> hash_map_; // 存储节点散列表，用于快速索引
    std::unordered_map<std::string, int> key_file_map_; // 存储key和其对应文件的映射

    Page* get_new_page(); // 得到新的一页
    Page* put_page_; // 用来标记写入的page
    bool put_page_existed_; 
    int new_file_index_; // 新建文件序号，初始为-1
    Page *head_, *tail_; // 双向列表的头尾，实现LRUCache
    void detach(Page* page); // 卸载该页
    void attach(Page* page); // 将该页连接到头部

    // 搜素key，返回节点，失败返回NULL
    Node* search(std::string key);
    // 加载文件到page
    int load_file_to_page(int file_num, Page* &page, std::unordered_map<std::string, Node*> &hash_map);
    // 将page保存到文件
    int save_page_to_file(Page* &page);
    // 将该页内容从page中释放
    void erase_page_of_cache(Page* &page, std::unordered_map<std::string, Node*> &hash_map);

    // 获取当前已经用到的文件序号
    int load_new_file_index(const char* save_file_index="file_index.dat");
    // 将程序结束时的文件序号保存到文件中
    void save_new_file_index(const char* save_file_index="file_index.dat");

    // 加载key和key存储文件的映射
    void load_key_file_map(const char* key_filename="key_file.dat");
    // 将key-file_num推入key_file_map, 并存入文件
    void push_key_to_keymap(Page* &page, const char* key_filename="key_file.dat");
};