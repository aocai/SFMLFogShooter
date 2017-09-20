#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>

void Server::connectionListen()
{
	std::cout << "connectionListen thread start here" << std::endl;
	static ReuseableListener listener;
	std::cout << "accepting socket on port " << PORT << std::endl;
	listener.close();
	listener.listen(PORT);

	sf::SocketSelector selector;
	selector.add(listener);
	while (tryListen)
	{
		if (selector.wait(sf::milliseconds(2000)))
		{
			if (selector.isReady(listener))
			{
				auto socket = std::make_unique<MyTcpSocket>();
				if (listener.accept(*socket) == sf::Socket::Done)
				{
					std::cout << "socket accepted on ip " << socket->getRemoteAddress().toString() << std::endl;
					int i = 0;
					for (; i < playersConnected.size() && playersConnected[i]; ++i);
					if (i >= maxPlayerCount)
					{
						sendSocketMessage(socket, "ROOM_FULL");
						socket->disconnect();
						std::cout << "socket declined, room full " << std::endl;
						continue;
					}
					playersConnected[i] = true;
					sendSocketMessageToClients("PLAYER_CONNECTED;" + std::to_string(i + 1));

					//send info about all currently connected players to newly connected player
					for (const auto &s : sockets)
					{
						sendSocketMessage(socket, "PLAYER_CONNECTED;" + std::to_string(s->ClientID));
					}

					++playerCount;
					socket->ClientID = i + 1;
					sockets.emplace_back(std::move(socket));
					clientSocketSelector.add(*sockets.back());
					sendSocketMessage(sockets.back(), "CONNECTED_AS;" + std::to_string(sockets.back()->ClientID));
				}
			}
		}
	}
	std::cout << "connectionListen thread ends here" << std::endl;
}

void Server::processServerPackets()
{
	while (!packetQueue.empty())
	{
		std::string s = packetQueue.front();
		packetQueue.pop();
		std::cout << s << std::endl;
		std::string code = "";
		auto it = s.begin();
		for (; it != s.end() && *it != ';'; ++it)
		{
			code += *it;
		}
		if (code == "PLAYER_DISCONNECTED")
		{
			++it;
			int playerNum = static_cast<int>(*it - '0');
			std::cout << "server received packet PLAYER_DISCONNECTED!" << std::endl;
			if (!playersConnected[playerNum - 1])
			{
				std::cout << "error, player already disconnected!" << std::endl;
			}
			else
			{
				playersConnected[playerNum - 1] = false;
				playersReadied[playerNum - 1] = false;
				for (int i = 0; i < sockets.size(); ++i)
				{
					if (sockets[i]->ClientID == playerNum)
					{
						sockets[i]->disconnect();
						sockets.erase(sockets.begin() + i);
						break;
					}
				}
				--playerCount;
				sendSocketMessageToClients("PLAYER_DISCONNECTED;" + std::to_string(playerNum));
				std::cout << "player " << playerNum << " has disconnected!" << std::endl;
			}
		}
		else if (code == "PLAYER_READY")
		{
			++it;
			int playerNum = static_cast<int>(*it - '0');
			std::cout << "server received packet PLAYER_READY!" << std::endl;
			if (playersReadied[playerNum - 1])
			{
				std::cout << "error, player already readied!" << std::endl;
			}
			else
			{
				playersReadied[playerNum - 1] = true;
				sendSocketMessageToClients("PLAYER_READY;" + std::to_string(playerNum));
				std::cout << "player " << playerNum << " has readied up!" << std::endl;
			}
		}
		else if (code == "PLAYER_UNREADY")
		{
			++it;
			int playerNum = static_cast<int>(*it - '0');
			std::cout << "server received packet PLAYER_UNREADY!" << std::endl;
			if (!playersReadied[playerNum - 1])
			{
				std::cout << "error, player already not ready!" << std::endl;
			}
			else
			{
				playersReadied[playerNum - 1] = false;
				sendSocketMessageToClients("PLAYER_UNREADY;" + std::to_string(playerNum));
				std::cout << "player " << playerNum << " has unreadied!" << std::endl;
			}
		}
		else if (code == "CREATE_WALL")
		{
			std::cout << "server received packet CREATE_WALL!" << std::endl;
			++it;

			std::string upperLeftX;
			std::string upperLeftY;
			std::string sizeX;
			std::string sizeY;
			for (; it != s.end() && *it != ','; ++it)
			{
				upperLeftX += *it;
			}
			++it;
			for (; it != s.end() && *it != ';'; ++it)
			{
				upperLeftY += *it;
			}
			++it;
			for (; it != s.end() && *it != ','; ++it)
			{
				sizeX += *it;
			}
			++it;
			for (; it != s.end() && *it != ';'; ++it)
			{
				sizeY += *it;
			}
			RectangleShape r{ Vector2f{ static_cast<float>(atof(sizeX.c_str())), static_cast<float>(atof(sizeY.c_str())) } };
			r.setPosition(Vector2f{ static_cast<float>(atof(upperLeftX.c_str())), static_cast<float>(atof(upperLeftY.c_str())) });

			if ([&]()->bool {
				for (const auto &p : players)
				{
					if (p.getBounds().intersects(r.getGlobalBounds()))
						return false;
				}
				return true;
			}())
			{
				customWalls.emplace_back(r);
				sendSocketMessageToClients(s);
			}
		}
		else if (code == "DELETE_WALL")
		{
			std::cout << "server received packet DELETE_WALL!" << std::endl;
			++it;

			int wallIndex = 0;
			while (it != s.end())
			{
				wallIndex = wallIndex * 10 + static_cast<int>(*it - '0');
				++it;
			}
			customWalls.erase(customWalls.begin() + wallIndex);
			sendSocketMessageToClients(s);
		}
		else if (code == "ROOM_READY")
		{
			std::cout << "server received packet ROOM_READY!" << std::endl;
			++it;

			int playerNumber = static_cast<int>(*it - '0');
			roomReadyMask |= (1 << (playerNumber - 1));
			if (roomReadyMask == (1 << playerCount) - 1)
			{
				sendSocketMessageToClients("ROOM_READY");
			}
		}
	}
}

void Server::sendSocketMessageToClients(const std::string &message)
{
	sf::Packet packet;
	packet << message;
	static int counter2 = 0;

	for (const auto &socket : sockets)
	{
		while (socket->send(packet) == sf::Socket::Partial);
	}
	++counter2;
	std::cout << "sent " << counter2 << " packets to all clients" << std::endl;
}

void Server::sendSocketMessage(const std::unique_ptr<MyTcpSocket> &socket, const std::string &message)
{
	sf::Packet packet;
	packet << message;

	while (socket->send(packet) == sf::Socket::Partial);
	std::cout << "sent packets to client " << socket->ClientID << ":" << message << std::endl;
}

void Server::receiveMessageFromClients()
{
	std::cout << "receiveMessageFromClients thread start here" << std::endl;
	while (tryReceiving)
	{
		if (clientSocketSelector.wait(sf::milliseconds(2000)))
		{
			for (const auto &socket : sockets)
			{
				if (clientSocketSelector.isReady(*socket))
				{
					sf::Packet packet;
					auto status = socket->receive(packet);
					if (status == sf::Socket::Done)
					{
						std::string s;
						packet >> s;
						packetQueue.push(s);
					}
				}
			}
		}
	}
	std::cout << "receivemsg thread ends here" << std::endl;
}

void Server::stop()
{
	isRunning = false;
}

bool Server::allPlayersReady()
{
	for (int i = 0; i < maxPlayerCount; ++i)
	{
		if (playersConnected[i] ^ playersReadied[i])
		{
			return false;
		}
	}
	return true;
}

ProjectileSnapshot Server::constructProjectileSnapshot(const std::unique_ptr<Projectile> &proj) const
{
	ProjectileSnapshot snapshot;
	snapshot.projectileID = proj->getID();
	snapshot.projectileType = proj->getTypeID();
	snapshot.posX = proj->getSprite()->getPosition().x;
	snapshot.posY = proj->getSprite()->getPosition().y;
	if (snapshot.projectileType == 0)
	{
		StraightProjectile *sp = dynamic_cast<StraightProjectile*>(proj.get());
		snapshot.targetX = sp->getTarget().x;
		snapshot.targetY = sp->getTarget().y;
	}
	else if (snapshot.projectileType == 1)
	{
		SpiralProjectile *sp = dynamic_cast<SpiralProjectile*>(proj.get());
		if (sp)
		{
			snapshot.mask = sp->getNotValidMask();
			std::cout << "serverside notvalidmask = " << snapshot.mask << std::endl;
		}
	}
	else if (snapshot.projectileType == 2)
	{
		ExpandProjectile *ep = dynamic_cast<ExpandProjectile*>(proj.get());
		snapshot.targetX = ep->getTarget().x;
		snapshot.targetY = ep->getTarget().y;
	}
	return snapshot;
}

PlayerSnapshot Server::constructPlayerSnapshot(const Player &player) const
{
	PlayerSnapshot snapshot;
	snapshot.playerID = player.getID();
	snapshot.posX = player.getPosition().x;
	snapshot.posY = player.getPosition().y;
	snapshot.hp = player.getCurrentHP();
	for (const auto &playerProj : *player.getProjectiles())
	{
		snapshot.proj.emplace_back(constructProjectileSnapshot(playerProj));
	}

	return snapshot;
}

GameSnapshot Server::constructSnapshot()
{
	GameSnapshot snapshot;
	snapshot.snapshotID = serverSnapshotID;
	++serverSnapshotID;
	for (const auto &p : players)
	{
		snapshot.p.emplace_back(constructPlayerSnapshot(p));
	}
	return snapshot;
}

//write a snapshot of server game state into a UdpSnapshot
void GamesnapToUdpSnap(GameSnapshot &gamesnap, UdpSnapshot &snap)
{
	snapshotWriteInt(&snap, gamesnap.snapshotID);
	snapshotWriteShort(&snap, static_cast<short>(gamesnap.p.size()));
	for (const auto &p : gamesnap.p)
	{
		snapshotWriteShort(&snap, p.playerID);
		snapshotWriteFloat(&snap, p.posX);
		snapshotWriteFloat(&snap, p.posY);
		snapshotWriteFloat(&snap, p.hp);
		snapshotWriteShort(&snap, static_cast<short>(p.proj.size()));
		for (const auto &proj : p.proj)
		{
			snapshotWriteShort(&snap, proj.projectileID);
			snapshotWriteShort(&snap, proj.projectileType);
			snapshotWriteFloat(&snap, proj.posX);
			snapshotWriteFloat(&snap, proj.posY);
			//snapshotWriteShort(&snap, std::strlen(proj.data));
			//snapshotWriteCString(&snap, proj.data);
			if (proj.projectileType == 0 || proj.projectileType == 2)
			{
				snapshotWriteFloat(&snap, proj.targetX);
				snapshotWriteFloat(&snap, proj.targetY);
			}
			else if (proj.projectileType == 1)
			{
				std::cout << "server attempting to write mask = " << proj.mask << std::endl;
				snapshotWriteLongLong(&snap, proj.mask);
			}
		}
	}
}

void Server::startServer()
{
	std::thread connectionListenThread(&Server::connectionListen, this);
	std::thread receiveThread(&Server::receiveMessageFromClients, this);

	size_t udpPacketSize = 508;
	char *buffer1 = static_cast<char*>(malloc(udpPacketSize));
	UdpSnapshot snap;
	UdpSnapshot receivedSnap;
	sf::UdpSocket UdpSendSocket;
	UdpSendSocket.setBlocking(false);
	UdpSendSocket.bind(sf::Socket::AnyPort);

	char *buffer2 = static_cast<char*>(malloc(udpPacketSize));
	sf::UdpSocket UdpReceiveSocket;
	UdpReceiveSocket.setBlocking(false);
	UdpReceiveSocket.bind(5001);

	//while in game lobby
	while (isRunning)
	{
		processServerPackets();
		if (playerCount > 0 &&
			[&]()->bool {
			for (int i = 0; i < maxPlayerCount; ++i)
			{
				if (playersConnected[i] ^ playersReadied[i])
					return false;
			}
			return true;
		}())
		{
			sendSocketMessageToClients("ALL_PLAYERS_READY");
			std::cout << "All players readied up!" << std::endl;
			break;
		}
	}

	for (const auto &socket : sockets)
	{
		players.emplace_back(Vector2f(32, 40), Vector2f(socket->ClientID * 200.f, 720 / 2.f), socket->ClientID);
	}

	sf::Clock clock;
	sf::Time time;
	int frameCounter = 0;

	while (isRunning)
	{
		time = clock.getElapsedTime();

		processServerPackets(); //process administrative packets sent via tcp

		std::size_t received;
		sf::IpAddress sender;
		unsigned short port;
		while (true)
		{
			std::size_t received;
			sf::IpAddress sender;
			unsigned short port;
			UdpReceiveSocket.receive(buffer2, udpPacketSize, received, sender, port);

			if (received <= 0)
				break;

			//std::cout << "Received " << received << " bytes from " << sender.toString() << " on port " << port << std::endl;

			snapshotInit(&receivedSnap, buffer2, udpPacketSize);

			int playernum = static_cast<int>(snapshotReadChar(&receivedSnap));
			std::cout << "playernum = " << playernum << std::endl;
			Vector2f deltaPos{ snapshotReadFloat(&receivedSnap), snapshotReadFloat(&receivedSnap) };
			std::cout << "deltaPos = (" << deltaPos.x << ", " << deltaPos.y << ")" << std::endl;
			int skillUsedMask = snapshotReadInt(&receivedSnap);
			std::cout << "skillUsedMask = " << skillUsedMask << std::endl;

			players[playernum - 1].move(deltaPos, customWalls);

			if (skillUsedMask & 5) //if straight or expand shot
			{
				Vector2f target;
				target.x = snapshotReadFloat(&receivedSnap);
				target.y = snapshotReadFloat(&receivedSnap);
				if (skillUsedMask & 1)
				{
					players[playernum - 1].rangeAttack(target);
					//Projectile::incrementIDCount();
				}
				if (skillUsedMask & 4)
				{
					players[playernum - 1].shootExpand(target);
					Projectile::incrementIDCount();
				}
			}
			if (skillUsedMask & 2)
			{
				players[playernum - 1].shootSpiral();
				Projectile::incrementIDCount();
			}
		}

		//processUserInputPackets(); //process player updates and simulate

		//do projectile collision detection
		for (int i = 0; i < players.size(); ++i)
		{
			for (int j = 0; j < players.size(); ++j)
			{
				if (i == j)
					continue;
				float dmg = players[i].calcProjCollision(players[j].getBounds());
				players[j].takeDamage(dmg);
			}
			players[i].updateProjectile();
		}

		if (frameCounter >= 3)
		{
			snapshotInit(&snap, buffer1, udpPacketSize);
			//send snapshot every 3 simulation frames
			GameSnapshot gamesnap = constructSnapshot();
			GamesnapToUdpSnap(gamesnap, snap);
			for (const auto &socket : sockets)
			{
				UdpSendSocket.send(snap.data, snap.currentSize, socket->getRemoteAddress(), 5002);
				UdpSendSocket.send(snap.data, snap.currentSize, socket->getRemoteAddress(), 5003); //for testing, currently all sockets receive n times data
			}
			frameCounter = 0;
		}
		++frameCounter;

		//sleep to cap at 60 simulations per second
		std::cout << "sleeping for " << (sf::microseconds(1000000 / 60) - (clock.getElapsedTime() - time)).asMicroseconds() << "microseconds" << std::endl;
		sleep(sf::microseconds(1000000 / 60) - (clock.getElapsedTime() - time));
	}

	tryListen = false;
	tryReceiving = false;
	connectionListenThread.join();
	receiveThread.join();
}