#ifdef QPU

#include "qpu_cursor.hpp"

void Cursor::init(Ptr<Float> p) {
    gather(p);
    current = 0;
    cursor = p+16;
}

// Receive the first vector and fetch the second.
// (prime the software pipeline)
void Cursor::prime() {
    receive(next);
    gather(cursor);
}

// Receive the next vector and fetch another.
void Cursor::advance() {
    cursor = cursor+16;
    prev = current;
    gather(cursor);
    current = next;
    receive(next);
}

// Receive final vector and don't fetch any more.
void Cursor::finish() {
    receive(next);
}

// Shift the current vector left one element
void Cursor::shiftLeft(Float& result) {
    result = rotate(current, 15);
    Float nextRot = rotate(next, 15);
    Where (index() == 15)
        result = nextRot;
    End
}

// Shift the current vector right one element
void Cursor::shiftRight(Float& result) {
    result = rotate(current, 1);
    Float prevRot = rotate(prev, 1);
    Where (index() == 0)
        result = prevRot;
    End
}

#endif
