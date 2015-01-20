#ifndef WORKER_HPP__
#define WORKER_HPP__

class worker
{
public:
    worker() {}
    virtual ~worker() {}
    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif // WORKER_HPP__


