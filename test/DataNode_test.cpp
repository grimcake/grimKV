#include "../DataNode.h"
#include <iostream>
using namespace std;

int main()
{
    DataNode datanode("127.0.0.1", 8888, 0);
    datanode.start();
    return 0;
}