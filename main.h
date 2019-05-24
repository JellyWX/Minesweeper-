#pragma once

#include "SFML/Graphics.hpp"

#include "grid.cpp"

#include <unordered_map>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <dirent.h>


int main(int argc, char** argv);


class Minesweeper
{
public:
    Minesweeper(unsigned short width, unsigned short height, unsigned short mines);

private:
    sf::RenderWindow window;

    sf::View hud;
    sf::View game;

    std::vector<sf::View*> views;

    unsigned int current_frame = 0;

    sf::Clock frame_clock; // for managing FPS
    sf::Clock lifetime_clock; // for checking FPS

    sf::Clock mouse_down_time;
    bool mouse_down = false;
    bool mouse_held_moved = false;

    std::unordered_map<std::string, sf::Texture*> load_textures();

    #ifndef NDEBUG
    void* show_fps(sf::Clock *clock, sf::Text *location);
    #endif

    void* check_average_fps();

    void* resize_window(int width, int height);

    void* manage_move(sf::Vector2i pos, Grid* grid, sf::View* game_view);

    void* manage_click(sf::Mouse::Button button, Grid* grid);

    void* zoom_view(int direction);
};