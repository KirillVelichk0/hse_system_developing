#ifndef BIDIRECTIONALPIPE_H
#define BIDIRECTIONALPIPE_H

#include <memory>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

template <bool isParent>
class PipeEnd;

class BidirectionalPipe
{
private:
    template <bool isParent>
    friend class PipeEnd;

    struct Pipe{
        int m_fds[2];
        Pipe(){
            m_fds[0] = -1;
            m_fds[1] = -1;
        }
        ~Pipe(){
            for(int i = 0; i < 2; i++){
                if(m_fds[i] != -1){
                    close((m_fds[i]));
                    m_fds[i] = -1;
                }
            }
        }
        Pipe(Pipe&& another){
            for(int i = 0; i<2; i++){
                std::swap(m_fds[i], another.m_fds[i]);
            }
        }
        Pipe& operator=(Pipe&& another){
            for(int i = 0; i<2; i++){
                std::swap(m_fds[i], another.m_fds[i]);
            }
            return *this;
        }
        int& first(){
            return m_fds[0];
        }
        int& second(){
            return m_fds[1];
        }

        Pipe(const Pipe&)=delete;
        Pipe& operator=(const Pipe&) = delete;
    };

private:
    Pipe m_parentToChild;
    Pipe m_childToParent;

public:
    BidirectionalPipe();
    ~BidirectionalPipe() = default;

    BidirectionalPipe(BidirectionalPipe&&)=default;
    BidirectionalPipe& operator=(BidirectionalPipe&&) = default;


public:
    BidirectionalPipe(const BidirectionalPipe&)=delete;
    BidirectionalPipe& operator=(const BidirectionalPipe&) = delete;
};

template <bool isParent>
class PipeEnd{
private:
    std::int32_t m_writeFd = -1;
    std::int32_t m_readFd = -1;
public:
    PipeEnd(BidirectionalPipe&& pipe){
        if constexpr(isParent){
            std::swap(m_writeFd, pipe.m_parentToChild.second());
            std::swap(m_readFd, pipe.m_childToParent.first());
        } else{
            std::swap(m_writeFd, pipe.m_childToParent.second());
            std::swap(m_readFd, pipe.m_parentToChild.first());
        }
    }
    ~PipeEnd(){
        if(m_readFd != -1){
            close(m_readFd);
        }
        if(m_writeFd != -1){
            close(m_writeFd);
        }
    }

    PipeEnd(PipeEnd&& another){
        std::swap(m_readFd, another.m_readFd);
        std::swap(m_writeFd, another.m_writeFd);
    };
    PipeEnd& operator=(PipeEnd&& another){
        std::swap(m_readFd, another.m_readFd);
        std::swap(m_writeFd, another.m_writeFd);
        return *this;
    };

    template <class BufferType>
    std::uint32_t ReadToBuffer(BufferType& buffer, std::size_t lenToRead){
        if(buffer.size() < lenToRead){
            throw std::invalid_argument("lenToRead less then buffer size");
        }

        auto bytesReaden = read(m_readFd, (char*)buffer.data(), lenToRead * sizeof(buffer[0]));
        if(bytesReaden == -1){
            if(errno == EAGAIN){
                return 0;
            }
            throw std::runtime_error("Cant read to buffer");
        }
        return bytesReaden;
    }

    template <class BufferType>
    void WriteFromBuffer(BufferType& buffer, std::size_t lenToWrite){
        constexpr char endSymb = '\0';
        if(buffer.size() < lenToWrite){
            throw std::invalid_argument("lenToWrite less then buffer size");
        }
        int sended = 0;
        while(sended < lenToWrite){
            auto bytesWriten = write(m_writeFd, (char*)buffer.data() + sended, lenToWrite * sizeof(buffer[0]) - sended);
            if(bytesWriten < 0){
                throw std::runtime_error("Cant write data to pipe");
            }
            sended += bytesWriten;
        }
        auto bytesWriten = write(m_writeFd, (char*)&endSymb, 1);
        if(bytesWriten < 0){
            throw std::runtime_error("Cant write end symbol to pipe");
        }
    }

public:
    PipeEnd(const PipeEnd&) = delete;
    PipeEnd& operator=(const PipeEnd&)=delete;
};

using ParentEndPipe = PipeEnd<true>;
using ChildEndPipe = PipeEnd<false>;




#endif // BIDIRECTIONALPIPE_H
