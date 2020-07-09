#pragma once
#include <memory>
#include "fiber.h"
#include "mutex.h"
class Scheduler
{
    public:
    typedef std::shared_ptr<Scheduler> ptr;
};
