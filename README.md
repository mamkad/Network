# Классы для работы с сетью

## Сокет

Класс-обёртка над сокетом. Реализация: [ссылка](https://github.com/mamkad/Network/blob/main/Socket)

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
	Socket server("127.0.0.1", 6666, SERVER);
	server.bind();
	server.listen(2);
	
	buff_t buff;

	for(;;)
	{
		try
		{
			int fd = server.accept();

			Socket::recv(fd, buff);

			buff[0] = 'A';
			buff[1] = 'B';
			buff[2] = '\0';

			Socket::send(fd, buff);

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
	Socket client("127.0.0.1", 6666, CLIENT);

	buff_t buff;

	b[0] = 'B';
	b[1] = 'A';
	b[2] = '\0';

	client.connect();

	Socket::send(client.fd(), buff);
	Socket::recv(client.fd(), buff);

	return 0;
}
```
