#include<unordered_map>
using namespace std;



int main()
{
    unordered_map<int ,int *> map;
    map[100] = new int(1);

    map.erase(100);
    auto o = map[100];    
}