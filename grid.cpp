#include "SFML/Graphics.hpp"

#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <unordered_map>

#define SPRITE_SIZE 32.0

bool reverse_compare(int i, int j)
{
    return i > j;
}


class Cell {
public:
    bool mine = false;
    bool open = false;
    bool hovered = false;
    int surrounding = 0;

    Cell(auto textures)
    {
        sf::Sprite sprite;

        sf::Texture* tex = &(*textures)["tile"];
        sprite.setTexture(*tex);

        sf::Vector2u scale = tex->getSize();

        sprite.setScale(SPRITE_SIZE / scale.x, SPRITE_SIZE / scale.y);

        this->sprite = sprite;
    }

    bool open_cell(auto textures)
    {
        this->open = true;

        if (this->mine)
        {
            sf::Texture* tex = &(*textures)["mine"];

            this->sprite.setTexture(*tex);
            return false;
        }
        else
        {
            this->sprite.setTexture((*textures)[std::to_string(this->surrounding)]);
            return true;
        }
    }

    sf::Sprite* get_sprite()
    {
        if (this->hovered && !this->open)
        {
            this->sprite.setColor(sf::Color(255, 180, 180));
        }
        else
        {
            this->sprite.setColor(sf::Color(255, 255, 255));
        }

        return &this->sprite;
    }

private:
    sf::Sprite sprite;
    sf::Texture* overlay;
};


class Grid {
public:
    Grid(int width, int height, int mines, auto textures)
    {
        srand( time(NULL) );

        this->textures = textures;
        this->total_cells = width * height;

        sf::Texture* tex = &(*textures)["tile"];

        this->bg_sprite.setTexture(*tex);
        this->bg_sprite.setColor(sf::Color(50, 50, 50));
        sf::Vector2u scale = tex->getSize();

        this->bg_sprite.setScale(SPRITE_SIZE / scale.x, SPRITE_SIZE / scale.y);
        
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

            sprite->setPosition(col * SPRITE_SIZE, row * SPRITE_SIZE);

            if (cell.open)
            {
                this->bg_sprite.setPosition(col * SPRITE_SIZE, row * SPRITE_SIZE);

                window->draw(this->bg_sprite);
            }
            window->draw(*sprite);
        }
    }

    void* set_hovered(int x, int y)
    {
        this->grid[this->hovered].hovered = false;
        this->hovered = -1;

        for (int i = 0; i < this->total_cells; i++)
        {
            int col = (i % this->width) * SPRITE_SIZE;
            int row = (i / this->width) * SPRITE_SIZE;

            if (col < x && (col + SPRITE_SIZE) > x && row < y && (row + SPRITE_SIZE) > y)
            {
                this->grid[i].hovered = true;
                this->hovered = i;

                break;
            }
        }
    }

    void* open_cell()
    {
        if (this->hovered >= 0)
        {
            this->grid[this->hovered].open_cell(this->textures);
        }
    }

private:
    int width;
    int height;
    int mines;
    int total_cells;
    int hovered = 0;

    sf::Sprite bg_sprite;

    std::unordered_map<std::string, sf::Texture>* textures;

    std::vector<Cell> grid;
};