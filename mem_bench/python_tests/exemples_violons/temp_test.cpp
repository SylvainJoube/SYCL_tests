#include <iostream>
#include <vector>

struct st {
    int pouet = 4;
    std::vector<int> v;
};

int main() {
    st a, b;
    a.v.reserve(88);
    for (int i = 0; i < 88; ++i) a.v.push_back(i);
    b.v.reserve(2);
    for (int i = 0; i < 2; ++i) b.v.push_back(i);
    auto v = int(6) + double(8.7);
    std::cout << v << " - a(" << sizeof(a.v.data()) << ")  b(" << sizeof(b.v.data()) << ")\n";
}