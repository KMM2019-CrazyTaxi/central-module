#ifndef CM_DOUBLE_BUFFER_H
#define CM_DOUBLE_BUFFER_H

#define DEFAULT_DOUBLE_BUFFER_SIZE 1024

template<typename T>
class double_buffer {

public:

    double_buffer();
    ~double_buffer();

    T* read_buffer;
    T* write_buffer;

    

};


#endif