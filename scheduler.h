#pragma once
#include <memory>
#include <vector>
#include <list>
#include <iostream>

#include "fiber.h"
#include "thread.h"
class Scheduler
{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();
    const std::string& getName()const { return m_name; }

};
