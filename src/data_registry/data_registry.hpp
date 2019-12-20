#ifndef CM_DATA_REGISTRY_H
#define CM_DATA_REGISTRY_H

#include <string>
#include <mutex>
#include <unordered_map>

#include "registry_entries.hpp"

/**
 * A single entry in the data_registry
 */
struct registry_entry {

    // Id associated with the entry (necessary?)
    int id;

    // Lock associated with the entry
    std::mutex lock;

    // Data associated with the entry
    void *data;
};

/**
 * Global data registry that contains important parameters that several
 * threads need access to. 
 */
class data_registry {

public:

    /**
     * Deleted copy constructor
     */
    data_registry(const data_registry&) = delete;
    
    /**
     * Deleted copy assignment operator
     */
    data_registry& operator=(const data_registry&) = delete;

    /**
     * Acquire a reference to the singleton instance of this class
     * 
     * @return Reference to the singleton instance
     */
    static data_registry& get_instance();

    /**
     * Acquires the data associated with the given id.
     * 
     * @param id The id associated with the data
     * @return void pointer to the data 
     * 
     * NOTE: This function locks the mutex associated with the data,
     * make sure the release it with release_data(id)
     */
    void* acquire_data(int id);

    /**
     * Releases the mutex of the data associated with the given id
     * 
     * @param id ID of the data to be released
     */
    void release_data(int id);

private:

    /**
     * Default constructor that initialises the data registry
     * with the default entries. There is an entry for each type
     * in registry_entries.hpp
     */
    data_registry();

    /**
     * Destructor that frees the dynamic memory for each entry.
     * This dynamic memory is where the data is stored.
     */
    ~data_registry();
    
    // Map that maps the ids to the entries 
    std::unordered_map<int, registry_entry> registry;

};


#endif