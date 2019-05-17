#include "SFML/Graphics.hpp"

#include "grid.cpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>


int main(int argc, char** argv);

void* show_fps(sf::RenderWindow *window, sf::Clock *clock, sf::Text *location);

void* resize_window(sf::RenderWindow *window, int width, int height);