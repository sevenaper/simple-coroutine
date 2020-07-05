#pragma once
#include <memory>
#include "Fiber.h"
#include "mutex.h"
class Scheduler
{
    public:
    typedef std::shared_ptr<Scheduler> ptr;
};