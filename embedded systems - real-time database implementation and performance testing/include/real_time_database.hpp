#ifndef real_time_database_hpp
#define real_time_database_hpp

#include <mutex>
#include <thread>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

/*! An instance of this class is a key-value store intended to
    support a real-time computer system.
    
    Each real-time entity has a name, which is just a unique
    string of characters.
    Associated with each entity is it's current value, which is
    stored as bytes.
    The size of an entry is not allowed to change, so all function calls
    using the same name must use the same size.
    A value that has not yet been written is all zero bytes.

    The `Register` function can be used to register a value with a
    given name and size. The return value of `Register` is a token
    which can be passed to `Read` or `Write` to improve performance.
    There is no requirement to call `Register`, and a zero token passed
    to `Read` or `Write` means that there is no token. Database
    implementations are not required to optimised for tokens. Database
    users may decide whether or not to call `Register`, but if they
    do they must ensure that they always pass the token that matches
    the name.
*/
class RealTimeDatabase
{
public:
    virtual ~RealTimeDatabase()
    {}

    /*! Returns the name of the database implementation. */
    virtual std::string GetName() const=0;

    /*! Return true if and only if the database is thread-safe
        A thread safe database allows any method to be called concurrently 
        with any other method at the same time from different threads.
    */
    virtual bool IsThreadSafe() const=0;

    /*! This method declares a real-time entity with the given name, and declares
        it will be a value of size bytes.
        The return value is a token that can be used in later calls to `Read` and `Write
        to improve performance. The token can only be used on the same instance
        that created it.
    */
    virtual intptr_t Register(const char *name, size_t size) = 0;

    /*! Read the real-time-entity called `name`, and store in the `size`-byte buffer pointed to by `data`.
        Token must either be 0, or the result of calling Register on name.
    */
    virtual void Read(const char *name, intptr_t token, size_t size, void *data) = 0;

    /*! Over-write the real-time-entity called `name` with the value in the `size`-byte buffer pointed to by `data`.
        Token must either be 0, or the result of calling Register on name.
    */
    virtual void Write(const char *name, intptr_t token, size_t size, const void *data) = 0;

    ///////////////////////////////////
    // Various convenience functions

    intptr_t Register(const std::string &name, size_t size)
    { return Register(name.c_str(), size); }

    void Read(const std::string &name, intptr_t token, size_t size, void *data)
    { Read(name.c_str(), token, size, data); }

    void Write(const std::string &name, intptr_t token, size_t size, const void *data)
    { Write(name.c_str(), token, size, data); }

    void Read(const std::string &name, size_t size, void *data)
    { Read(name.c_str(), 0, size, data); }

    void Write(const std::string &name, size_t size, const void *data)
    { Write(name.c_str(), 0, size, data); }

    void Read(const char *name, size_t size, void *data)
    { Read(name, 0, size, data); }

    void Write(const char *name, size_t size, const void *data)
    { Write(name, 0, size, data); }
};

#endif
