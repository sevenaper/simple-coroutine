#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include "fiber.h"
void run_in_fiber()
{
    std::cout << "run in fiber begin" << std::endl;
    Fiber::YieldToHold();
    std::cout << "run in fiber end" << std::endl;
    Fiber::YieldToHold();
}
void test_fiber()
{
    std::cout << "main begin -1" << std::endl;
    Fiber::GetThis();
    Fiber::ptr fiber(new Fiber(run_in_fiber()));
    fiber->swapIn();
    std::cout << "main after swapIn" << std::endl;
    fiber->swapIn();
    std::cout << "main after end" << std::endl;
    fiber->swapIn();
    std::cout << "main after end2" << std::endl;
}
int main()
{
    std::vector<std::shared_ptr<std::thread>> thrs;
    for (int i = 0; i < 3; ++i)
    {
        thrs.push_back(std::shared_ptr(new std::thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto i : thrs)
    {
        i->join();
    }
}
