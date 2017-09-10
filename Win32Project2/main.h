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