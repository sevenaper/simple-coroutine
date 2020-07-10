#include "fiber.h"
#include <ucontext.h>
#include <atomic>
#define DEFAULT_STATCK_SIZE 1024 * 1024
class MallocStackAllocator
{
public:
    static void *Alloc(size_t size)
    {
        return malloc(size);
    }

    static void Dealloc(void *vp, size_t size)
    {
        return free(vp);
    }
};

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

static thread_local Fiber *t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

using StackAllocator = MallocStackAllocator;
Fiber::Fiber()
{
    m_state = EXEC;
    SetThis(this);
    getcontext(&m_ctx);
    ++s_fiber_count;
}
Fiber::Fiber(std::function<void()> cb, size_t stack_size)
    : m_id(++s_fiber_id), m_state(INIT), m_cb(cb)
{
    ++s_fiber_count;
    m_stacksize = stack_size ? stack_size : DEFAULT_STATCK_SIZE;

    m_stack = StackAllocator::Alloc(m_stacksize);
    getcontext(&m_ctx);
    m_ctx.uc_link = nullptr;

    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if (m_stack)
    {
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }
    else
    {
        Fiber *cur = t_fiber;
        if (cur == this)
        {
            SetThis(nullptr);
        }
    }
}
//重置协程函数，并且重置状态 INIT TERM
void Fiber::reset(std::function<void()> cb)
{
    m_cb = cb;
    getcontext(&m_ctx);
    m_ctx.uc_link = nullptr;

    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}
//切换到当前协程执行
void Fiber::swapIn()
{
    SetThis(this);
    m_state = EXEC;
    if (swapcontext((&(*t_threadFiber))->m_ctx, &m_ctx))
    {
    }
}

//切换到后台执行
void Fiber::swapOut()
{
    SetThis((*t_threadFiber).get());
    if (swapcontext(&m_ctx, (*t_threadFiber)->m_ctx))
    {
    }
}

void Fiber::SetThis(Fiber *f)
{
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis()
{
    if (t_fiber)
        return t_fiber->shared_from_this();
    Fiber::ptr main_fiber(new Fiber);
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

//协程切换到后台，并且设置成Ready状态
void Fiber::YieldToReady()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

//协程切换到后台，并且设置成Hold状态
void Fiber::YieldToHold()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

//总协程数
uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}

void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch (const std::exception &e)
    {
        cur->m_state = EXCEPT;
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
}

uint64_t Fiber::GetFiberId()
{
    if (t_fiber)
    {
        return t_fiber->getId();
    }
    return 0;
}
