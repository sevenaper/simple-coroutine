#pragma once
#include <memory>
#include <functional>
#include <ucontext.h>

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State
    {
        INIT,   //初始化
        HOLD,   //暂停
        EXEC,   //执行
        TERM,   //结束
        READY,  //准备
        EXECEPT //异常
    };

private:
    Fiber();

public:
    /**
     * cb执行函数   stack_size栈大小 
     **/
    Fiber(std::function<void()> cb, size_t stack_size);
    ~Fiber();

    void reset(std::function<void()> cb); //重置写成函数，并重置状态 INIT,TERM
    void swapIn();                        //切换到当前协程执行
    void swapOut();                       //切换到后台执行

public:
    static void SetThis(Fiber *f); //设置当前协程
    static Fiber::ptr GetThis();   //返回当前协程
    static void YieldToReady();    //协程切换到后台，并且设置为ready状态
    static void YieldToHold();     //协程切换到后台，并且设置为hold状态
    static uint64_t TotalFibers(); //协程总数
    static MainFunc();

private:
    uint64_t m_id = 0;        //协程id
    uint32_t m_stacksize = 0; //栈大小
    State m_state = INIT;     //协程状态

    ucontext_t m_ctx;
    void *m_stack = nullptr;
    std::function<void()> m_cb;
};