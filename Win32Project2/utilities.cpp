#include "utilities.h"
#include <iostream>

float determinant(sf::Vector2f v1, sf::Vector2f v2)
{
	return v1.x * v2.y - v2.x * v1.y;
}

float dotProduct(sf::Vector2f v1, sf::Vector2f v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float magnitude(sf::Vector2f v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

void updateMapMatrix(std::vector<double> &mapMatrix, sf::Vector2f topLeft, sf::Vector2f bottomRight, int value)
{
	for (int i = topLeft.y / 10; i < bottomRight.y / 10; ++i)
	{
		for (int j = topLeft.x / 10; j < bottomRight.x / 10; ++j)
		{
			mapMatrix[i * 128 + j] = value;
		}
	}
}

sf::Vector2f rotateVector2f(sf::Vector2f v, double angle)
{
	angle = angle * 3.141592 / 180;
	return sf::Vector2f(v.x * std::cos(angle) + v.y * (-std::sin(angle)), v.x * std::sin(angle) + v.y * std::cos(angle));
}

void snapshotWriteByte(UdpSnapshot *msg, long long value, int size)
{
	if (msg->maxSize - msg->currentSize < 1)
	{
		msg->overflow = true;
		return;
	}
	if (size == 1)
	{
		msg->data[msg->currentSize] = value;
		msg->currentSize += 1;
	}
	else if (size == 2)
	{
		*reinterpret_cast<short*>(&msg->data[msg->currentSize]) = value;
		msg->currentSize += 2;
	}
	else if (size == 4)
	{
		*reinterpret_cast<int*>(&msg->data[msg->currentSize]) = value;
		msg->currentSize += 4;
	}
	else if (size == 8)
	{
		*reinterpret_cast<long long*>(&msg->data[msg->currentSize]) = value;
		msg->currentSize += 8;
	}
}

long long snapshotReadByte(UdpSnapshot *msg, int size)
{
	int value;
	if (size == 1)
	{
		value = msg->data[msg->readByte];
		msg->readByte += size;
	}
	else if (size == 2)
	{
		value = *reinterpret_cast<short*>(&msg->data[msg->readByte]);
		msg->readByte += size;
	}
	else if (size == 4)
	{
		value = *reinterpret_cast<int*>(&msg->data[msg->readByte]);
		msg->readByte += size;
	}
	else if (size == 8)
	{
		value = *reinterpret_cast<long long*>(&msg->data[msg->readByte]);
		msg->readByte += size;
	}
	return value;
}

void snapshotInit(UdpSnapshot *msg, char *data, int length)
{
	memset(msg, 0, sizeof(*msg));
	msg->data = data;
	msg->maxSize = length;
}

void snapshotWriteChar(UdpSnapshot *msg, char value)
{
	snapshotWriteByte(msg, value, 1);
}

void snapshotWriteShort(UdpSnapshot *msg, short value)
{
	snapshotWriteByte(msg, value, 2);
}

void snapshotWriteInt(UdpSnapshot *msg, int value)
{
	snapshotWriteByte(msg, value, 4);
}

void snapshotWriteFloat(UdpSnapshot *msg, float value)
{
	union {
		float f;
		int i;
	} dat;
	dat.f = value;
	snapshotWriteByte(msg, dat.i, 4);
}

void snapshotWriteLongLong(UdpSnapshot *msg, long long value)
{
	snapshotWriteByte(msg, value, 8);
}

void snapshotWriteDouble(UdpSnapshot *msg, double value)
{
	union {
		long long l;
		double d;
	} dat;
	dat.d = value;
	snapshotWriteByte(msg, dat.l, 8);
}

void snapshotWriteCString(UdpSnapshot *msg, const char *c)
{
	if (!c)
		return;
	size_t length = std::strlen(c);
	for (size_t i = 0; i <= length; ++i) //write including '\0'
	{
		snapshotWriteByte(msg, c[i], 1);
	}
}

char snapshotReadChar(UdpSnapshot *msg)
{
	return snapshotReadByte(msg, 1);
}

short snapshotReadShort(UdpSnapshot *msg)
{
	return snapshotReadByte(msg, 2);
}

int snapshotReadInt(UdpSnapshot *msg)
{
	return snapshotReadByte(msg, 4);
}

float snapshotReadFloat(UdpSnapshot *msg)
{
	union {
		float f;
		int l;
	} dat;
	dat.l = snapshotReadByte(msg, 4);
	return dat.f;
}

long long snapshotReadLongLong(UdpSnapshot *msg)
{
	return snapshotReadByte(msg, 8);
}

double snapshotReadDouble(UdpSnapshot *msg)
{
	union {
		long long l;
		double d;
	} dat;
	dat.l = snapshotReadByte(msg, 8);
	return dat.d;
}

const char* snapshotReadCString(UdpSnapshot *msg)
{
	std::string str;
	char c = static_cast<char>(snapshotReadByte(msg, 1));
	while (c != '\0')
	{
		str += c;
		c = static_cast<char>(snapshotReadByte(msg, 1));
	}
	return str.c_str();
}