#pragma once
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <winsock2.h>
#include <memory>
#include <queue>
#include "Player.h"
#include "MyTcpSocket.h"
#include "utilities.h"
#include "Dispatcher.h"

class ReuseableListener : public sf::TcpListener
{
public:
	ReuseableListener()
	{
		create();
		reuse();
	}
	void reuse()
	{
		char reuse = 1;
		setsockopt(getHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
};

class Server
{
private:
	std::string ip;
	int serverSnapshotID = 0;
	const unsigned short PORT = 5000;
	unsigned short playerCount = 0;
	unsigned short maxPlayerCount = 4;
	std::vector<std::unique_ptr<MyTcpSocket>> sockets;
	sf::SocketSelector clientSocketSelector;
	std::queue <std::string> packetQueue;
	std::vector<sf::RectangleShape> customWalls;
	std::vector<Player> players = std::vector<Player>(4);
	bool serverRunning = true;
	bool gameRunning = false;
	bool tryListen = true;
	bool tryReceiving = true;
	unsigned int roomReadyMask = 0;
	std::vector<bool> playersConnected = std::vector<bool>(maxPlayerCount, false);
	std::vector<bool> playersReadied = std::vector<bool>(maxPlayerCount, false);

	void connectionListen();
	void processTcpPackets();
	void sendSocketMessageToClients(const std::string&);
	void sendSocketMessage(const std::unique_ptr<MyTcpSocket>&, const std::string&);
	void receiveMessageFromClients();
	bool allPlayersReady();
	ProjectileSnapshot constructProjectileSnapshot(const std::unique_ptr<Projectile> &proj) const;
	PlayerSnapshot constructPlayerSnapshot(const Player&) const;
	GameSnapshot constructSnapshot();
public:
	void stop();
	void startServer();


	Server() = default;
	virtual ~Server() = default;
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server(Server &&e) = delete;
	Server& operator=(Server&&) = delete;
};