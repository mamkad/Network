#include "Socket.hpp"

#include <cstring>
#include <exception>
#include <string>
#include <utility>

using std::exception;
using std::logic_error;
using std::invalid_argument;
using std::string;

#include <iostream>
using std::cout;

namespace Network
{
    // обнуление параметров
    void Socket::reset()
    {
        fd_ = -1;
        memset(&address_, '\0', sizeof(address_));
        socket_mode_ = NOTASSIGN;
    }  

    // обменять данные с другим сокетом
    void Socket::swap(Socket& otherSocket)
    {
        std::swap(fd_,          otherSocket.fd_);
        std::swap(address_,     otherSocket.address_);
        std::swap(socket_mode_, otherSocket.socket_mode_);
    }                    

    // функция с проверкой однотипных условий
    void Socket::check(char const* funcName, char const* mode, socket_mode_t needMode)
    {
        if (!active())
        {
            throw logic_error("Нельзя использовать функцию" + string(funcName) + ", пока не открыт сокет");
        }

        if (socket_mode_ != needMode)
        {
            throw logic_error("Функция " + string(funcName) + " должна использоваться только для сокета в режиме: " + string(mode));
        }
    }

    // проверить корректность ip-адреса
    bool Socket::checkIp(string const& ip)
    {
        if (ip.empty())
        {
            return false;
        }

        return true;
    }

    // инициализация структуры адреса
    void Socket::addressInit(string const& ip, in_port_t port)
    {
        address_.sin_family = socket_type_.domain;
        address_.sin_port = htons(port);
        address_.sin_addr.s_addr = inet_addr(ip.data());
    }                                   
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    Socket::Socket()
    {
        reset();
        setType({.domain = AF_INET, .type = SOCK_STREAM, .protocol = 0});
    }

    Socket::Socket(string const& ip, in_port_t port, socket_mode_t mode)
    {
        reset();
        setType({.domain = AF_INET, .type = SOCK_STREAM, .protocol = 0});
        create(ip, port, mode);
        cout << "Socket()\n";
    }

    Socket::Socket(Socket&& otherSocket) 
    {
        cout << "Socket(Socket&& otherSocket)\n";
        reset();
        swap(otherSocket);
    }
  
    Socket::~Socket()                 
    {
        cout << "~Socket()\n";
        close();
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
     // задать тип сокета
    void Socket::setType(socket_type_t const& socket_type)
    {
        if(active())
        {
            throw logic_error("Нельзя задать новый тип сокета, пока не закрыт текущий сокет");
        }

        socket_type_ = socket_type;
    }              


    // инициализации класса сокета (создание fd и инициализация структуры sockaddr_in)
    void Socket::create(string const& ip, in_port_t port, socket_mode_t mode)
    {   
        if(active())
        {
            throw logic_error("Нельзя создать новый сокет, пока не закрыт текущий");
        }

        if (!checkIp(ip))
        {
            throw logic_error("Некорректно задан ip адрес: " + ip);
        }

        if ( (fd_ = ::socket(socket_type_.domain, socket_type_.type, socket_type_.protocol)) < 0 )
        {
            throw logic_error("Не удалось создать сокет: " + string(strerror(errno)));
        }

        addressInit(ip, port);
        
        socket_mode_ = mode;
    }    

    // закрыть сокет
    void Socket::close()
    {
        if (!active())
        {
            return;
        }

        if ( ::close(fd_) < 0 )
        {
            reset();
            throw logic_error("Ошибка при закрытии сокета: "+ string(strerror(errno)));
        }

        reset();
        cout << "\tclose()\n";
    }

    // cвязывание адреса с локальным адресом протокола (для серверов)
    void Socket::bind()    
    {
        check("bind()", "SERVER", SERVER);

        if ( ::bind(fd_, (struct sockaddr*)&address_, sizeof(address_)) < 0 )
        {
            throw logic_error("Не удалось связать адрес с локальным адресом протокола: "+ string(strerror(errno)));
        }
    }

    // перевод сокета в состояние LISTEN (для серверов)
    void Socket::listen(int backlog)                 
    {
        check("listen()", "SERVER", SERVER);

        if ( ::listen(fd_, backlog) < 0 )
        {
            throw logic_error("Не удалось перевести сокет в состояние LISTEN: " + string(strerror(errno)));
        }
    }

    // принятие входящих подключений (для серверов)
    int Socket::accept()  
    {
        check("accept()", "SERVER", SERVER);

        int clientFd;
        socklen_t addressSize = sizeof(address_);

        if ( (clientFd = ::accept(fd_, (struct sockaddr*)&address_, &addressSize)) < 0 )
        {
            throw logic_error("Не удалось получить fd следующего установленного соединения: " + string(strerror(errno)));
        }

        return clientFd;
    }	

    // подключение
    void Socket::connect() 
    {
        check("connect()", "CLIENT", CLIENT);

        if ( ::connect(fd_, (struct sockaddr*)&address_, sizeof(address_)) < 0 )
        {
            throw logic_error("Не удалось установить соединение : " + string(strerror(errno)));
        }
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // получить дескриптор
    int Socket::fd() const noexcept
    {
        return fd_;
    }

    // получить структуру адреса
    sockaddr_in const& Socket::address() const noexcept 
    {
        return address_;
    }

    // получить режим работы сокета
    socket_mode_t Socket::mode() const noexcept
    {
        return socket_mode_;
    }

    socket_type_t const& Socket::type()    const noexcept // получить тип сокета
    {
        return socket_type_;
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // активен ли сокет
    bool Socket::active() const noexcept
    {
        return (fd_ >= 0);
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // получить сообщение 
    ssize_t Socket::recv(int fd, buff_t& buff)       
    {
        ssize_t len = 0;

        if ( (len = ::recv(fd, buff.data(), buff.size(), 0)) && len < 0 )
        {
            throw logic_error("Ошибка при получении сообщения: " + string(strerror(errno)));
        }

        return len;
    }

    // послать сообщение
    ssize_t Socket::send(int fd, buff_t const& buff)   
    {
        ssize_t len = 0;

        if ( (len = ::send(fd, buff.data(), buff.size(), 0)) && len < 0 )
        {
            throw logic_error("Ошибка при отправке сообщения: " + string(strerror(errno)));
        }

        return len;
    }  
}
