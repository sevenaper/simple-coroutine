#include "Fiber.h"
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
static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr;

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

void Fiber::reset(std::function<void()> cb)
{
    m_cb = cb;
    getcontext(&m_ctx);
    m_ctx.uc_link = nullptr;

    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_cb, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void swapIn()
{
    SetThis(this);
    m_state = EXEC;
    if (swapcontext((&(*t_threadFiber))->m_ctx, &m_ctx))
    {
    }
}
void swapOut() {}

static void Fiber::SetThis(Fiber *f) {}
static Fiber::ptr GetThis() {}
static void YieldToReady() {}
static void YieldToHold() {}
static uint64_t TotalFibers() {}
static MainFunc() {}