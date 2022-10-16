/*
    Класс-обёртка над сокетом. 
    2022-10-08
*/
#pragma once

#include "Address.hpp"

#include <array>
#include <utility>

using std::array;
using std::pair;

namespace Network
{
    // макс длина буфера
    enum { BUFFSIZE = 256 }; 
                
    // тип буфера                                    
    using Buff = array<char, BUFFSIZE>; 

    // сокет
    class Socket final
    {
    private:  
        int         fd_;         // дескриптор
        SocketType  socketType_; // тип сокета
        int         backlog_;    // max длина очереди на подключение

    private:
        void reset( ); // обнуление параметров
       
    public:
        Socket( );
        Socket( SocketType const&, Address const& ); 
        Socket( Socket const& ) = delete;                
        Socket( Socket&& );                                                                   
        Socket& operator= ( Socket const& ) = delete; 
        Socket& operator= ( Socket&& )      = delete;
        ~Socket( ); 

    public:
        void create( SocketType const&, Address const& ); // создание сокета (создание fd и инициализация структуры sockaddr_in)
        void close( ); // закрыть сокет

        pair<int, Address> accept( ); // принятие входящих подключений (для серверов)
        void connect( Address const& );   // подключение (для клиентов)

    public:
        int fd( )                 const noexcept; // получить файловый дескриптор
        SocketMode mode( )        const noexcept; // получить текущий режим работы сокета
        SocketType const& type( ) const noexcept; // получить текущий тип сокета

    public:     
        bool isActive( ) const noexcept; // открыт ли текущий сокет

    public:
        static ssize_t receive( int, Buff& );    // получить данные по fd 
        static ssize_t send( int, Buff const& ); // послать  данные по fd 
    };
}