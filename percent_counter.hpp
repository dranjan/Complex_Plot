#ifndef PERCENT_COUNTER_HPP
#define PERCENT_COUNTER_HPP

#include <iostream>
#include <iomanip>

class percent_counter {
protected:
    int limit;
    int current;
    int percent_complete;

public:
    percent_counter(unsigned int l) 
        : limit(l), current(0), percent_complete(0)
    { }

    void start() {
        current = 0;
        percent_complete = 0;

        std::cout << "  0% complete.";
    }

    void count(unsigned int inc = 1) {
        current += inc;
        int new_percent = 100*current/limit;
        if (new_percent > percent_complete) {
            percent_complete = new_percent;

            std::cout << "\r" << std::setw(3) << percent_complete
                      << "% complete.";
            std::cout.flush();

            if (new_percent == 100) std::cout << std::endl;
        }
    }
};

#endif
