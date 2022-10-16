# Классы для работы с сетью

## Сокет

Класс-обёртка над сокетом. Реализация: [сокет](https://github.com/mamkad/Network/blob/main/Socket)

Пример использования:

***Сервер***

```c++
#include "Socket.hpp"

#include <iostream>
#include <exception>

using std::cout;
using std::exception;

using namespace Network;

int main()
{
	Socket server( {
			.domain_ = AF_INET, 
			.type_ = SOCK_STREAM, 
			.protocol_ = 0,
			.socketMode_ = SERVER}, 
			{ AF_INET, "127.0.0.1", 6666 } );
	
	Buff buff;

	for(;;)
	{
		try
		{
			auto conn = server.accept( );

			Socket::receive( conn.first, buff );

			cout << buff.data( ) << '\n';

			buff[0] = 'A';
			buff[1] = 'B';
			buff[2] = '\0';

			Socket::send( conn.first, buff );
			::close( conn. first );
			break;
		}
		catch(exception const& e)
		{
			cout << e.what() << '\n';
		}
	}
	
	return 0;
}
```
***Клиент***
```c++
#include "Socket.hpp"

#include <iostream>
#include <exception>

using std::cout;
using std::exception;

using namespace Network;

int main()
{
	try 
	{
		Socket client( {
			.domain_ = AF_INET, 
			.type_ = SOCK_STREAM, 
			.protocol_ = 0,
			.socketMode_ = CLIENT }, 
			 {} );

		Buff buff;

		buff[0] = 'B';
		buff[1] = 'A';
		buff[2] = '\0';

		client.connect( { AF_INET, "127.0.0.1", 6666 } );

		Socket::send( client.fd(), buff );
		Socket::receive( client.fd(), buff );

		cout << buff.data( ) << '\n';

		client.close( );

	}
	catch( exception const& e )
	{
		cout << e.what() << '\n';
	}

	return 0;
}
```