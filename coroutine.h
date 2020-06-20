/*
 * @Author: your name
 * @Date: 2020-06-20 21:51:50
 * @LastEditTime: 2020-06-20 21:51:51
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /liuxin/coroutine/coroutine.h
 */ 
#pragma once
#include <memory>
#include <functional>
#include <ucontext.h>

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum state
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
     * 执行函数 栈大小 是否在主协程上执行
     **/
    Fiber(std::function<void()> cb, size_t stack_size, bool use_main = false);
    
};