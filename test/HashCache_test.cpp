#include "../HashCache.h"
#include <iostream>
using namespace std;

int main()
{
    HashCache cache;
    /*for(int i = 0; i<5; i++) {
        string keyi = "key"+to_string(i);
        string valuei = "value"+to_string(i);
        cache.put(keyi, valuei);
    }*/
   // cache.save_cache();
    for(int i = 0; i<5; i++)
    {
        string keyi = "key"+to_string(i);
        cout<<i<<": "<<cache.get(keyi)<<endl;
    }
    //cout<<cache.get("key3")<<endl;
    return 0;
}