#pragma once
#include <SFML/Graphics.hpp>

//using namespace sf;

struct UdpSnapshot
{
	bool overflow;
	char *data;
	int maxSize;
	int currentSize;
	int readByte;
};

struct ProjectileSnapshot
{
	short projectileID;
	short projectileType;
	float posX;
	float posY;
	float targetX;
	float targetY;
	long long mask;
};

struct PlayerSnapshot
{
	short playerID;
	float posX;
	float posY;
	float hp;
	std::vector<ProjectileSnapshot> proj;
};

struct GameSnapshot
{
	int snapshotID;
	std::vector<PlayerSnapshot> p;
};

float determinant(sf::Vector2f v1, sf::Vector2f v2);

float dotProduct(sf::Vector2f v1, sf::Vector2f v2);

float magnitude(sf::Vector2f v);

void updateMapMatrix(std::vector<double> &mapMatrix, sf::Vector2f topLeft, sf::Vector2f bottomRight, int value);

sf::Vector2f rotateVector2f(sf::Vector2f position, sf::Vector2f origin, double angle);

void snapshotWriteByte(UdpSnapshot *msg, long long value, int size);

long long snapshotReadByte(UdpSnapshot *msg, int size);

void snapshotInit(UdpSnapshot *msg, char *data, int length);;

void snapshotWriteChar(UdpSnapshot *msg, char value);

void snapshotWriteShort(UdpSnapshot *msg, short value);

void snapshotWriteInt(UdpSnapshot *msg, int value);

void snapshotWriteFloat(UdpSnapshot *msg, float value);

void snapshotWriteLongLong(UdpSnapshot *msg, long long value);

void snapshotWriteDouble(UdpSnapshot *msg, double value);

void snapshotWriteCString(UdpSnapshot *msg, const char *c);

char snapshotReadChar(UdpSnapshot *msg);

short snapshotReadShort(UdpSnapshot *msg);

int snapshotReadInt(UdpSnapshot *msg);

float snapshotReadFloat(UdpSnapshot *msg);

long long snapshotReadLongLong(UdpSnapshot *msg);

double snapshotReadDouble(UdpSnapshot *msg);

const char* snapshotReadCString(UdpSnapshot *msg);