#include "SFML/Graphics.hpp"

#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>


bool reverse_compare(int i, int j)
{
    return i > j;
}


class Cell {
public:
    bool mine = false;
    bool open = false;
    int surrounding = 0;

    Cell(auto textures)
    {
        sf::Sprite sprite;

        sf::Texture* tex = &(*textures)["tile"];
        sprite.setTexture(*tex);

        sf::Vector2u scale = tex->getSize();

        sprite.setScale(32.f / scale.x, 32.f / scale.y);

        this->sprite = sprite;
    }

    sf::Sprite* get_sprite()
    {
        return &this->sprite;
    }

private:
    sf::Sprite sprite;
};


class Grid {
public:
    Grid(int width, int height, int mines, auto textures)
    {
        srand( time(NULL) );

        this->total_cells = width * height;

        if (this->total_cells < mines - 9)
        {
            exit(-1);
        }

        this->width = width;
        this->height = height;
        this->mines = mines;

        this->make_cells(textures);
        this->count_mines();
    }

    void* make_cells(auto textures)
    {
        std::vector<int> mine_positions;
        for (int m = 0; m < this->mines; m++)
        {
            mine_positions.push_back(rand() % (this->total_cells - m));
        }

        std::sort(mine_positions.begin(), mine_positions.end(), reverse_compare);

        for (int i = 0; i < total_cells; i++)
        {
            // textures is already a pointer so just copy it
            Cell cell(textures);
            if (mine_positions.back() <= i && (!mine_positions.empty()))
            {
                cell.mine = true;
                mine_positions.pop_back();
            }
            this->grid.push_back(cell);
        }
    }

    void* count_mines()
    {
        // TODO
    }

    void* draw(sf::RenderWindow *window)
    {
        for (int i = 0; i < this->total_cells; i++)
        {
            int col = i % this->width;
            int row = i / this->width;

            Cell cell = this->grid[i];
            sf::Sprite* sprite = cell.get_sprite();

            sprite->setPosition(col * 32, row * 32);

            window->draw(*sprite);
        }
    }

private:
    int width;
    int height;
    int mines;
    int total_cells;

    std::vector<Cell> grid;
};