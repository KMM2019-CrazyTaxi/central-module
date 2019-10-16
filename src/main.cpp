#include <iostream>
#include <thread>
#include <functional>

#include "QPULib.h"
#include "io_handler.hpp"

void gcd(Ptr<Int> p, Ptr<Int> q, Ptr<Int> r) {
    Int a = *p;
    Int b = *q;
    While (any(a != b))
        Where (a > b)
        a = a-b;
        End
        Where (a < b)
        b = b-a;
        End
    End
    *r = a;
}

int main () {
      
    // Construct kernel
    auto k = compile(gcd);

    // Allocate and initialise arrays shared between ARM and GPU
    SharedArray<int> a(16), b(16), r(16);
    srand(0);
    for (int i = 0; i < 16; i++) {
        a[i] = 100 + (rand() % 100);
        b[i] = 100 + (rand() % 100);
    }

    // Invoke the kernel and display the result
    k(&a, &b, &r);
    for (int i = 0; i < 16; i++)
    printf("gcd(%i, %i) = %i\n", a[i], b[i], r[i]);
  

    io_data data;
    std::thread io_thread(io_thread_main, std::ref(data));
    
    while (true) {

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    io_thread.join();

    return 0;
}