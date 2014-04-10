#ifndef MEMBUF_H
#define MEMBUF_H

#include <streambuf>

class membuf : public std::streambuf
{
public:

    membuf(char *base, std::size_t size) {
        this->setp(base, base + size);
        this->setg(base, base, base + size);
    }
    std::size_t written() const { return this->pptr() - this->pbase(); }
    std::size_t read() const    { return this->gptr() - this->eback(); }
};

#endif // MEMBUF_H
