#include "Socket.hpp"

#include <cstring>
#include <exception>
#include <string>

using std::exception;
using std::logic_error;
using std::invalid_argument;
using std::swap;

#include <iostream>
using std::cout;

namespace Network
{
    // обнуление параметров
    void Socket::reset()
    {
        fd_ = -1;
        socketType_ = { .domain_ = -1, .type_ = -1, .protocol_ = -1, .socketMode_ = NOTASSIGN };
        backlog_ = 0;
    }                           
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    Socket::Socket()
    {
        reset( );
    }

    Socket::Socket( SocketType const& socketType, Address const& address )
    {
        reset( );
        create( socketType, address );
    }

    Socket::Socket( Socket&& otherSocket ) 
    {
        reset( );

        swap( fd_,         otherSocket.fd_ );
        swap( socketType_, otherSocket.socketType_ );
        swap( backlog_,    otherSocket.backlog_ );
    }
  
    Socket::~Socket()                 
    {
        close( );
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // создать сокет
    void Socket::create( SocketType const& socketType, Address const& address ) 
    {
        // для создание нового сокета предыдущий должен быть закрыт
        if( isActive( ) )
        {
            throw logic_error( "Нельзя создать новый сокет, пока не закрыт текущий" );
        }

         // сохраняем информацию о типе сокета
        socketType_ = socketType;

        if ( socketType_.socketMode_ == SERVER && address.isEmpty( ) )
        {
            throw logic_error( "При создании сокета в режиме SERVER адрес должен быть указан явно" );
        }

        // создание сокета
        if ( ( fd_ = ::socket( socketType_.domain_, socketType_.type_, socketType_.protocol_ ) ) < 0 )
        {
            throw logic_error( "Не удалось создать сокет: " + string( strerror( errno ) ) );
        }

        // проверяем режим сокета
        if ( socketType_.socketMode_ == SERVER )
        {
            // привязка сокета к локальному адресу протокола
            socklen_t serverAddressSize = sizeof( address.address( ) );

            if ( ::bind( fd_, address.sAddress( ), serverAddressSize ) < 0 )
            {
                close( );
                throw logic_error( "Не удалось связать адрес с локальным адресом протокола: "+ string( strerror( errno ) ) );
            }

            // перевод сокета в режим ожидания запросов (состояние LISTEN (для серверов))
            if ( ::listen( fd_, backlog_ ) < 0 )
            {
                close( );
                throw logic_error( "Не удалось перевести сокет в состояние LISTEN: " + string( strerror( errno ) ) );
            }
        }
        else if ( socketType_.socketMode_ != CLIENT )
        {
            close( );
            throw logic_error( "При создании сокета нужно явно указывать его тип: либо CLIENT либо SERVER" );
        }
    }            

    // закрыть сокет
    void Socket::close( )
    {
        if ( !isActive( ) )
        {
            return;
        }

        if ( ::close( fd_ ) < 0 )
        {
            reset( );
            throw logic_error( "Ошибка при закрытии сокета: "+ string( strerror( errno ) ) );
        }

        reset( );
    }

    // принятие входящих подключений (для серверов)
    pair<int, Address> Socket::accept()  
    {
        if ( !isActive( ) )
        {
            throw logic_error( "Сокет не открыт" );
        }

        if ( socketType_.socketMode_ != SERVER )
        {
            throw logic_error( "Функция должна вызываться только для сокета в режиме: SERVER" );
        }

        int       clientFd;
        Address   clientAddress;
        socklen_t clientAddressSize = sizeof( clientAddress.address( ) );
       
        if ( ( clientFd = ::accept( fd_, clientAddress.sAddress( ), &clientAddressSize ) ) < 0 )
        {
            throw logic_error("Не удалось получить fd следующего установленного соединения: " + string(strerror(errno)));
        }

        return { clientFd, clientAddress };
    }   

    // подключение
    void Socket::connect( Address const& address ) 
    {
        if ( !isActive() )
        {
            return;
        }

        if ( socketType_.socketMode_ != CLIENT )
        {
            throw logic_error( "Функция должна вызываться только для сокета в режиме: SERVER" );
        }

        socklen_t serverAddressSize = sizeof( address.address( ) );

        if ( ::connect( fd_, address.sAddress( ), serverAddressSize ) < 0 )
        {
            throw logic_error( "Не удалось установить соединение : " + string( strerror( errno ) ) );
        }
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // получить дескриптор
    int Socket::fd( ) const noexcept
    {
        return fd_;
    }

    // получить режим работы сокета
    SocketMode Socket::mode( ) const noexcept
    {
        return socketType_.socketMode_;
    }

    SocketType const& Socket::type( ) const noexcept // получить тип сокета
    {
        return socketType_;
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // активен ли сокет
    bool Socket::isActive( ) const noexcept
    {
        return ( fd_ >= 0 );
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // получить сообщение 
    ssize_t Socket::receive( int fd, Buff& buff )       
    {
        ssize_t len = 0;

        if ( ( len = ::recv( fd, buff.data( ), buff.size( ), 0 ) ) && len < 0 )
        {
            throw logic_error( "Ошибка при получении сообщения: " + string( strerror( errno ) ) );
        }

        return len;
    }

    // послать сообщение
    ssize_t Socket::send( int fd, Buff const& buff )   
    {
        ssize_t len = 0;

        if ( ( len = ::send( fd, buff.data( ), buff.size( ), 0 ) ) && len < 0 )
        {
            throw logic_error( "Ошибка при отправке сообщения: " + string( strerror( errno ) ) );
        }

        return len;
    }  
}
