#include "tools.h"

void split(const string& s, vector<string>& sv, const char flag)
{
    sv.clear();
    istringstream iss(s);
    string temp;
    while(getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
    return;
}

unsigned int Hash(const string &str)
{
    unsigned int h = 0;
    for (int i = 0; i < str.size(); i++)
    {
        h = 31 * h + (str[i] - 'a');
    }
    return h;
}