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

    Cell()
    {
        sf::Sprite sprite;

        this->sprite = sprite;
    }

    sf::Drawable* draw()
    {
        if (!this->open)
        {

            return &this->sprite;
        }
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

        this->make_cells();
        this->count_mines();
    }

    void* make_cells()
    {
        std::vector<int> mine_positions;
        for (int m = 0; m < this->mines; m++)
        {
            mine_positions.push_back(rand() % (this->total_cells - m));
        }

        std::sort(mine_positions.begin(), mine_positions.end(), reverse_compare);

        for (int i = 0; i < total_cells; i++)
        {
            Cell cell;
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
            
        }
    }

private:
    int width;
    int height;
    int mines;
    int total_cells;

    std::vector<Cell> grid;
};