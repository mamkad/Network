/*
    Класс-обёртка над адресом. 
    2022-10-16
*/
#pragma once

#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <exception>

using std::string;

namespace Network
{
    // режимы работы сокета 
    enum SocketMode 
    { 
        SERVER    = 0, 
        CLIENT    = 1,
        NOTASSIGN = 2 
    }; 

    // тип сокета
    struct SocketType 
    { 
        int domain_; 
        int type_; 
        int protocol_; 
        SocketMode socketMode_; 
    }; 

    // адрес
    class Address
    {
    private:
        string      ip_;         // ip
        in_port_t   port_;       // port
        int         family_;     // семейство адресов
        sockaddr_in address_;    // структура адреса
        socklen_t   addressSize_;

    private:
        void reset();

    public:
        Address( );
        Address( int, string const&, in_port_t );
        Address( Address const& );
        Address( Address&& );

        Address& operator= ( Address const& );
        Address& operator= ( Address&& );

        ~Address();

    public:
        void create( int, string const&, in_port_t );

    public:
        void clear( );
        bool isEmpty() const noexcept;

    public:
        string const& ip()           const noexcept;
        in_port_t  port()            const noexcept;
        int family()                 const noexcept;
        sockaddr_in const& address() const noexcept;
        sockaddr* sAddress()         const noexcept;
        //socklen_t& size()            noexcept;
    };
}
