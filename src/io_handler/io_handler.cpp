#include <iostream>
#include <thread>
#include <cstdlib>

#include "io_handler.hpp"
#include "defs.hpp"

#ifdef WIRING_PI
    #include <WiringPi.h>
#endif

int io_thread_main(io_data& data) {

    std::srand(std::time(0));

    while (true) {

        data.distance = update_distance();
        data.acceleration = update_acceleration();

        std::cout << "IO execution" << std::endl;
        std::cout << "d(IO): " << data.distance << std::endl;
        std::cout << "a(IO): " << data.acceleration << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(210));
    }

    return 0;
}

static float update_distance() {

#ifdef WIRING_PI
    // pi implementation
#else
    return (float)std::rand()/(float)(RAND_MAX/1);
#endif

}

static float update_acceleration() {

#ifdef WIRING_PI
    // pi implementation
#else
    return (float)std::rand()/(float)(RAND_MAX/10);
#endif
}

