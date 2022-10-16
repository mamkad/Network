#include "Address.hpp"

#include <utility>
#include <cstring>
#include <exception>
#include <stdexcept>

#include <iostream>
using std::cout;

using std::swap;
using std::move;
using std::invalid_argument;

namespace Network
{
	void Address::reset( )
	{
		ip_ .clear( );
		ip_.shrink_to_fit( );
		port_ = 0;
		family_ = 0;
		memset( &address_, '\0', sizeof( address_ ) );
		addressSize_ = 0;
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	Address::Address( )
	{
		cout << "Address( )\n";
		reset( );
	}

	Address::Address( int family, string const& ip, in_port_t port)
	{
		reset( );
		create( family, ip, port );
	}

	Address::Address( Address const& otherAddress )
	{
		reset( );
		create( otherAddress.family_, otherAddress.ip_, otherAddress.port_ );
	}

	Address::Address( Address&& otherAddress )
	{
		reset( );

		ip_ = move( otherAddress.ip_ );

		swap( port_,   otherAddress.port_ );
		swap( family_, otherAddress.family_ );
		swap( address_,otherAddress.address_ );
	}

	Address& Address::operator= ( Address const& otherAddress )
	{
		if ( this != &otherAddress )
		{
			reset( );
			create( otherAddress.family_, otherAddress.ip_, otherAddress.port_ );
		}

		return *this;
	}

	Address& Address::operator= ( Address&& otherAddress )
	{
		if ( this != &otherAddress )
		{
			reset( );

			ip_ = move( otherAddress.ip_ );

			swap( port_,   otherAddress.port_ );
			swap( family_, otherAddress.family_ );
			swap( address_,otherAddress.address_ );
		}

		return *this;
	}

	Address::~Address( )
	{
		reset( );
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	void Address::create( int family, string const& ip, in_port_t port )
	{
		if ( !isEmpty( ) )
		{
			clear( );
		}

		// check ip

		ip_   = ip;
		port_ = port;
		family_ = family;

		address_.sin_family = family_;
	    address_.sin_port = htons( port_ );
	    address_.sin_addr.s_addr = inet_addr( ip_.data() );
	  
	    //addressSize_ = sizeof( address_ );
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	void Address::clear( )
	{
		reset( );
	}

	bool Address::isEmpty( ) const noexcept
	{
		return ip_.empty( );
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------

	string const& Address::ip( ) const noexcept
	{
		return ip_;
	}

	in_port_t Address::port( ) const noexcept
	{
		return port_;
	}

	int Address::family( ) const noexcept
	{
		return family_;
	}

	sockaddr_in const& Address::address( ) const noexcept
	{
		return address_;
	}

	sockaddr* Address::sAddress() const noexcept
	{
		return (sockaddr*)&address_;
	}
	/*
	socklen_t& Address::size() noexcept
	{
		return addressSize_;
	}*/
}