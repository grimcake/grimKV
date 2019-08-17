#include <iostream>
#include <fstream>
#include "HashCache.h"

// 表示文件是否存在于hash_map中
bool file_accessed[DISK_SIZE] = {false};

HashCache::HashCache(int capacity)
{
    new_file_index_ = load_new_file_index();
    load_key_file_map();
    put_page_ = NULL;
    put_page_existed_ = false;

    // 为cache分配内存
    entries_ = new Page[capacity];
    for(int i = 0; i<capacity; i++) 
    {
        for(int j = 0; j<PAGE_SIZE; j++)
        {
            // 将该页的地址标记到该页的每一个节点中
            (entries_+i)->data_[j].page_ = entries_+i;
        }
        // 页如空闲队列
        free_entries_.push_back(entries_+i);
    }
    // 构建双向链表
    head_ = new Page;
    tail_ = new Page;
    head_->prev = NULL;
    head_->next = tail_;
    tail_->prev = head_;
    tail_->next = NULL;
}

HashCache::~HashCache()
{
    save_cache();
    delete head_;
    delete tail_;
}

// 查找key：现在hansh_map中找；不成功再查找key_file_map,查找key所在的文件
// 在文件中如果查找成功，则将文件中的数据保留到hash_map中，所属页连接到双向链表头；失败则删除其对应信息
// get操作好put操作都要调用该函数
Node* HashCache::search(std::string key)
{
    Node* node = hash_map_[key];
    Page* page = NULL;
    if(node)
    {
        // 在hash_map中查找成功
        page = node->page_;
        detach(page);
        attach(page);
    }
    else
    {
        int file_num = key_file_map_[key];
        if(file_num == 0)
        {
            return NULL;
        }
        page = get_new_page();
        if(load_file_to_page(file_num, page, hash_map_) == -1)
        {
            // 加载失败，重新将该页置为空闲
            free_entries_.push_back(page);
        }
        if(node = hash_map_[key]) 
        {
            attach(page);
        }
    }
    return node;
}


std::string HashCache::get(std::string key)
{
    Node* node = search(key);
    if(node)
    {
        return node->value_;
    }
    return std::string();
}

// put操作：将数据写入server，当存在相同的key时进行覆盖
// 首先查找是否存在key，存在则覆盖值，并将页标记为dirty；不存在，将数据写入到put_page中
// 最后都要将操作页连接到头部
// 注：put_page一直接受put操作的值，直到满页后，put_page才指向新的一页，当页被弹出时，写入磁盘
void HashCache::put(std::string key, std::string value)
{
    Node* node = NULL;
    node = search(key);
    if(node)
    {
        // 查找到key，进行覆盖写
        if(value != node->value_)
        {
            node->value_ = value;
            node->page_->dirty_ = true;
        }
        return;
    }
    else
    {
        // server不存在当前节点，写入put_page
        static int data_index = 0;   
        if(data_index == PAGE_SIZE || put_page_existed_ == false)
        {
            // put_page写满时，将put_page中的key，压入映射表
            push_key_to_keymap(put_page_);
            data_index = 0;
        }
        if(data_index == 0 || put_page_existed_ == false)
        {
            put_page_ = get_new_page();
            put_page_existed_ = true;
            put_page_->file_num_ = ++new_file_index_; // put_page对应文件序号
            put_page_->dirty_ = true;
            for(int i = 0; i<PAGE_SIZE; i++)
            {
                put_page_->data_[i].key_ = "";
                put_page_->data_[i].value_ = "";
            }
            attach(put_page_);
        }
        put_page_->data_[data_index].key_ = key;
        put_page_->data_[data_index].value_ = value;
        hash_map_[key] = put_page_->data_+data_index;
        data_index++;
        detach(put_page_);
        attach(put_page_);
    }
}

int HashCache::load_file_to_page(int file_num, Page* &page, std::unordered_map<std::string, Node*> &hash_map)
{
    std::ifstream input;
    std::string file_name = std::to_string(file_num) + ".dat";
    input.open(file_name, std::ios::in);
    if(!input || page == NULL)
    {
        // 不存在文件
        return -1;
    }
    file_accessed[file_num] = true;
    page->file_num_ = file_num;
    Node* data = page->data_;
    for(int i = 0; i<PAGE_SIZE; i++)
    {
        if(!(input >> data[i].key_ >> data[i].value_))
        {
            data[i].key_ = "";
            data[i].value_ = "";
        }
        hash_map[data[i].key_] = data+i;
    }
    input.close();
    return 0;
}

int HashCache::save_page_to_file(Page* &page)
{
    page->dirty_ = false;
    if(page == NULL)
    {
        return -1;
    }
    std::ofstream output;
    std::string file_name = std::to_string(page->file_num_)+".dat";
    output.open(file_name, std::ios::out);
    Node* data = page->data_;
    for(int i = 0; i<PAGE_SIZE; i++)
    {
        output << data[i].key_ << " " << data[i].value_ << std::endl;
    }
    output.close();
    return 0;
}

int HashCache::load_new_file_index(const char* save_file_index)
{
    std::ifstream input;
    input.open(save_file_index, std::ios::in);
    if(!input)
    {
        return 0;
    }
    int index;
    input >> index;
    input.close();
    return index;
}

void HashCache::save_new_file_index(const char* save_file_index)
{
    std::ofstream output;
    output.open(save_file_index, std::ios::out);
    if(!(output << new_file_index_))
    {
        std::cout << "保存存储文件序号文件失败：write " << save_file_index << std::endl;
        exit(-1); 
    }
    output.close();
}

void HashCache::erase_page_of_cache(Page* &page, std::unordered_map<std::string, Node*> &hash_map)
{
    file_accessed[page->file_num_] = false; // 该文件已经不在hash_map中
    for(int i = 0; i<PAGE_SIZE; i++)
    {
        hash_map.erase(page->data_[i].key_);
    }
}

void HashCache::save_cache()
{
    Page* page = head_->next;
    while(page != tail_)
    {
        if(page->dirty_ == true)
        {
            save_page_to_file(page);
        }
        page = page->next;
    }
    save_new_file_index();
    push_key_to_keymap(put_page_);
}

void HashCache::detach(Page* page)
{
    page->prev->next = page->next;
    page->next->prev = page->prev;
}

void HashCache::attach(Page* page)
{
    page->prev = head_;
    page->next = head_->next;
    head_->next->prev = page;
    head_->next = page;
}


Page* HashCache::get_new_page()
{
    Page* page;
    if(free_entries_.empty() == true)
    {
        page = tail_->prev;
        if(page == put_page_)
        {
            put_page_existed_ = false;
            push_key_to_keymap(page);
        }
        if(page->dirty_ == true)
        {
            save_page_to_file(page);
        }
        erase_page_of_cache(page, hash_map_);
        detach(page);
        free_entries_.push_back(page);
    }
    page = free_entries_.back();
    free_entries_.pop_back();
    return page;
}

void HashCache::push_key_to_keymap(Page* &page, const char* key_filename)
{
    if(page == NULL)
    {
        return;
    }
    std::ofstream output;
    output.open(key_filename, std::ios::app);
    Node* data = page->data_;
    int file_num = page->file_num_;
    for(int i = 0; i<PAGE_SIZE; i++)
    {
        key_file_map_[data[i].key_] = file_num;
        output << data[i].key_ << " " << file_num << std::endl;
    }
    output.close();
}

void HashCache::load_key_file_map(const char* key_filename)
{
    std::ifstream input;
    input.open(key_filename, std::ios::in);
    if(!input) 
    {
        return;
    }
    std::string key;
    int file_num;
    while(input >> key >> file_num)
    {
        key_file_map_[key] = file_num;
    }
    input.close();
}