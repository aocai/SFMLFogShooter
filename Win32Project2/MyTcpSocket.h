#pragma once
#include <SFML/Network.hpp>

class MyTcpSocket : public sf::TcpSocket
{
public:
	unsigned short ClientID;
};