
#include <cassert>

#pragma once

class ProgressBar {

public:
    ProgressBar(const unsigned n, const unsigned step_) {
        assert(100 % step_ == 0);
        assert(step_ <= 100);
        
        step = step_;
        bigStep = 10;
        instanceSize = n;
        current = 0;
        finished = false;
        
        update();
    }

    ProgressBar(const unsigned n, const unsigned step_, const unsigned bigStep_) {
        assert(100 % step_ == 0);
        assert(step_ <= 100);
        
        step = step_;
        bigStep = bigStep_;
        instanceSize = n;
        current = 0;
        finished = false;
        
        update();
    }

    void advance() {
        current += 1;
        update();
    }

private:
    void update() {
        if (finished)
            return;
        
        if (current == 0) {
            std::cout << "0%" << std::flush;
            lastStep = 0;
            lastBigStep = 0;
            return;
        }

        unsigned percent = (current * 100) / instanceSize;

        unsigned bigPercent = percent / bigStep;
        if (bigPercent > lastBigStep) {
            std::cout << (bigPercent * bigStep) << "%" << std::flush;
            lastBigStep = percent / bigStep;
            lastStep = percent / step;
        } else if (percent / step > lastStep) {
            std::cout << "." << std::flush;
            lastStep = percent / step;
        }

        if (percent >= 100) {
            std::cout << "\n";
            finished = true;
        }
            

    }

    unsigned instanceSize;
    unsigned step; // How much percent does one dot represent (1, 5, 10, 15, 20....)
    unsigned bigStep; // Big steps (5%, 10%..)

    unsigned lastStep;
    unsigned lastBigStep;
    unsigned current;

    bool finished;
};