#ifndef real_time_database_locked_hpp
#define real_time_database_locked_hpp

#include "real_time_database_naive.hpp"

class RealTimeDatabaseLocked
    : public RealTimeDatabase
{
private:
    struct entry{
      std::string name;
      std::vector<char> data;
    };

    std::vector<entry> entries;

public:
    std::mutex mtx;

    std::string GetName() const override
    { return "locked"; }

    bool IsThreadSafe() const override
    { return true; }

    intptr_t Register(const char *name, size_t size) override
    {
      for(auto &e : entries){
          if(e.name==name){
              assert(e.data.size() == size);
              return 0;
          }
      }

      mtx.lock();
      entries.push_back({ name, std::vector<char>(size, 0) });
      mtx.unlock();

      return 0;
    }

    void Read(const char *name, intptr_t token, size_t size, void *data) override
    {
      for(auto &e : entries){
          if(e.name==name){
              mtx.lock();
              assert(e.data.size()==size);
              memcpy(data, &e.data[0], size);
              mtx.unlock();
              return;
          }
      }

      memset(data, 0, size);
    }

    void Write(const char *name, intptr_t token, size_t size, const void *data) override
    {


      for(auto &e : entries){
          if(e.name==name){
              mtx.lock();
              assert(e.data.size()==size);
              memcpy(&e.data[0], data, size);
              mtx.unlock();
              return;
          }
      }

      mtx.lock();
      entries.push_back({ name, std::vector<char>(size, 0) });
      memcpy(&entries.back().data[0], data, size);
      mtx.unlock();

    }
};

#endif
