#include "num.cc"
using namespace std;
using namespace sjtu;
//#define int lont long


signed main() {
    int2048 x,y;
    freopen("test.in","r",stdin);
    cin >> x >> y;
    freopen("CON","r",stdin);
    // cout << "pass\n";
    // cout << x * y << '\n';
    cout << x % y;
    return 0;
}