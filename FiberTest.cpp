#include <iostream>
#include "Fiber.h"
void run_in_fiber()
{
    std::cout << "run in fiber begin" << std::endl;
    Fiber::YieldToHold();
    std::cout << "run in fiber end" << std::endl;
    Fiber::YieldToHold();
}
int main()
{
    Fiber::GetThis();
    Fiber::ptr fiber(new Fiber(run_in_fiber));
    fiber->swapIn();
    std::cout <<"main after swapIn" << std::endl;
    fiber->swapIn();
    std::cout <<"main after end" << std::endl;
}