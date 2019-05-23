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
    Minesweeper();

private:
    sf::RenderWindow window;

    sf::View hud;
    sf::View game;

    unsigned int current_frame = 0;

    sf::Clock frame_clock; // for managing FPS
    sf::Clock lifetime_clock; // for checking FPS

    std::unordered_map<std::string, sf::Texture*> load_textures();

    #ifndef NDEBUG
    void* show_fps(sf::Clock *clock, sf::Text *location);
    #endif

    void* check_average_fps();

    void* resize_window(std::vector<sf::View*> views, int width, int height);

    void* manage_move(sf::RenderWindow* window, sf::Vector2i pos, Grid* grid, sf::View* game_view);

    void* manage_click(sf::Mouse::Button button, Grid* grid);

    void* zoom_view(sf::View *view, int direction);
};