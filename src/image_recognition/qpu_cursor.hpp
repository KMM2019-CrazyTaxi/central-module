#pragma once

#include "QPULib.h"

class Cursor {

public:

    // Initialise to cursor to a given pointer
    // and fetch the first vector.
    void init(Ptr<Float> p);

    // Receive the first vector and fetch the second.
    // (prime the software pipeline)
    void prime();

    // Receive the next vector and fetch another.
    void advance();

    // Receive final vector and don't fetch any more.
    void finish();

    // Shift the current vector left one element
    void shiftLeft(Float& result);

    // Shift the current vector right one element
    void shiftRight(Float& result);

    Ptr<Float> cursor;
    Float prev, current, next;

};
