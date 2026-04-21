#include "src.hpp"
#include <iostream>
using namespace std;
int main(){
    pylist ls;
    ls.append(1);
    cout << ls[0] << endl;
    ls.append(2);
    cout << ls[1] << endl;
    ls.pop();
    ls.append(3);
    cout << ls[1] << endl; // expect 3
    pylist ls2 = ls;
    ls.append(4);
    cout << ls2[2] << endl; // expect 4
    ls.pop();
    ls2.append(5);
    cout << ls[2] << endl; // expect 5
    pylist ls3; ls3.append(6);
    ls[0] = ls3;
    cout << ls2[0][0] << endl; // expect 6
    ls2.append(ls3);
    ls2[3].append(7);
    ls[1] = pylist();
    cout << ls[0][1] << endl; // expect 7
    ls2[2] = ls[1];
    ls2[1].append(8);
    cout << ls2[2][0] << endl; // expect 8
    ls[0] = pylist();
    ls2 = ls;
    ls3 = ls3;
    ls2 = ls3;
    ls3[0] = ls3;
    ls2.append(3*3);
    cout << ls[3][0][0][0][0][0][0][0][0][0][0][0][0][2] << endl; // 9
    cout << 2 * ls[3][0][2] + (ls2[1] << 1 >> 1) - (1 ^ ls2[2] ^ 1) - (ls2[1] / 1) - (ls[1][0] % 8) + 1 << endl; // 10
    cout << ls << endl;
    cout << ls.pop() << endl;
    cout << 0 + ls[2].pop() << endl;
    cout << ls << endl;
}
