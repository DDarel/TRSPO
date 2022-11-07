#include <iostream>
#include <thread>

void FirstFunc() {
    int sum = 0;
    for (int i = 0; i < 10000000; i++) {
        sum++;
    }
    std::cout << "Sum first: " << sum << " " << std::endl;
}

void SecondFunc() {
    int sum1 = 0;
    for (int i = 0; i < 1000; i++) {
        sum1++;
    }
    std::cout << "Sum  second: " << sum1 << " " << std::endl;
}

int main()
{
    std::thread local(FirstFunc);
    std::thread local2(SecondFunc);

    local.join();
    local2.join();
}
