#include "../ConsistentHash.h"
#include <iostream>
#include <string>
using namespace std;

int main()
{
    for(int i = 0; i<100; i++){
        string key = "key"+i;
        cout<<Hash(key)%10<<endl;
    }
}