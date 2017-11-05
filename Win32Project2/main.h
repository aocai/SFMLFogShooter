#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "utilities.h"
#include "Player.h"
#include "Cirno.h"
#include "Aya.h"
#include "Suika.h"
#include "Sakuya.h"

float windowWidth;
float windowHeight;

void startScreen(RenderWindow&);
void fadeScreen(RenderWindow&, float);
void client(std::string);
void sendSocketMessageToServer(std::string);
void sendSocketMessageToClients(std::string);
void sendPlayerInputToServer(PlayerFrameInput&);
void processInitPacketQueue();
void processPlayerPacketQueue(Player&);
void GameOverScreen(RenderWindow&, std::string);
void createPlayArea(RenderWindow&, Player&, Player&);
void waitRoom(RenderWindow&);
void PlayerGameplay(RenderWindow&);
void GameLobby(RenderWindow&, std::string);
void CreateRoom(RenderWindow&);
bool ConnectingRoom(RenderWindow&, std::string);
void JoinRoom(RenderWindow&);
void AIGameplay(RenderWindow&);