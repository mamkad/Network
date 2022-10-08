#pragma once

#include <unistd.h>
#include <arpa/inet.h>
#include <array>
#include <string>

using std::array;
using std::string;

namespace Network
{
    // тип сокета
    struct socket_type_t
    {
        int domain;
        int type;
        int protocol;
    };

    // режимы работы сокета
    enum socket_mode_t {SERVER = 0, CLIENT = 1, NOTASSIGN = 2}; 

    // макс длина буфера
    enum {MSGSIZE = 256};                                       

    using buff_t = array<char, MSGSIZE>; // тип буфера

    /*
        Класс-обёртка над сокетом. 
        2022-10-08
     */

    class Socket final
    {
        // данные класса
    private:  
        int           fd_;             // дескриптор
        sockaddr_in   address_;        // структура адреса
        socket_mode_t socket_mode_;    // режим работы сокета
        socket_type_t socket_type_;    // тип сокета

        // флаги для контроля корректного порядка вызовов bind, listen, accept (для серверов)
    private:
        bool isBindCalled_;   // Была ли вызвана bind
        bool isListenCalled_; // Была ли вызвана listen
        
        // служебные функции. Внутренний интерфейс
    private:
        void        reset      ();                                        // обнуление параметров
        void        swap       (Socket& otherSocket);                     // обменять данные с другим сокетом
        void        check      (char const*, char const*, socket_mode_t); // функция с проверкой однотипных условий
        static bool checkIp    (string const&);                           // проверить корректность ip-адреса
        void        addressInit(string const&, in_port_t);                // инициализация структуры адреса 

    public:
        Socket();
        Socket(string const&, in_port_t, socket_mode_t); // конструктор, принимает ip, порт и размер сообщения
        Socket(Socket const&) = delete;                  // запрещаем копировать
        Socket(Socket&&);                               
        ~Socket();                                     

        // запрещаем присваивать
    public:
        Socket& operator= (Socket const&) = delete; 
        Socket& operator= (Socket&&)      = delete;

        // основной интерфейс
    public:
        void setType(socket_type_t const&);                   // задать тип сокета
        void create(string const&, in_port_t, socket_mode_t); // инициализации класса сокета (создание fd и инициализация структуры sockaddr_in)
        void close();                                         // закрыть сокет

        void bind();      // cвязывание адреса с локальным адресом протокола (для серверов)
        void listen(int); // перевод сокета в состояние LISTEN               (для серверов)
        int  accept();    // принятие входящих подключений                   (для серверов)
        void connect();   // подключение

        // seter-ы
    public:
        int                  fd()      const noexcept; // получить дескриптор
        sockaddr_in const&   address() const noexcept; // получить структуру адреса
        socket_mode_t        mode()    const noexcept; // получить режим работы сокета
        socket_type_t const& type()    const noexcept; // получить тип сокета

    public:     
        bool active() const noexcept; // активен ли сокет

    public:
        static ssize_t recv(int, buff_t&);       // получить сообщение 
        static ssize_t send(int, buff_t const&); // послать сообщение
    };
}