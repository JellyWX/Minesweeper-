#include "SFML/Graphics.hpp"

#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unistd.h>

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
    bool flagged = false;
    int surrounding = 0;

    Cell(auto textures)
    {
        sf::Sprite sprite;

        sf::Texture* tex = (*textures)["tile"];
        sprite.setTexture(*tex);

        sf::Vector2u scale = tex->getSize();

        sprite.setScale(SPRITE_SIZE / scale.x, SPRITE_SIZE / scale.y);

        this->sprite = sprite;
    }

    bool open_cell(auto textures)
    {
        if (this->open)
        {
            return true;
        }

        this->open = true;

        if (this->mine)
        {
            sf::Texture* tex = (*textures)["mine"];

            this->sprite.setTexture(*tex);
            return false;
        }
        else
        {
            this->sprite.setTexture(*(*textures)[std::to_string(this->surrounding)]);
            return true;
        }
    }

    void* flag_cell(auto textures)
    {
        this->flagged = !this->flagged;

        if (this->flagged)
        {
            sf::Texture* tex = (*textures)["mark"];

            this->sprite.setTexture(*tex);
        }
        else
        {
            sf::Texture* tex = (*textures)["tile"];

            this->sprite.setTexture(*tex);
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
};


class Grid {
public:
    Grid(int width, int height, int mines, auto textures)
    {
        srand( time(NULL) );

        this->textures = textures;
        this->total_cells = width * height;

        this->width = width;
        this->height = height;
        this->mines = mines;

        sf::Texture* tex = (*textures)["bg_tile"];
        sf::Vector2u scale = tex->getSize();

        #ifndef NDEBUG
        std::cout << "Detected background cell scale: " << scale.x << " " << scale.y << std::endl;
        #endif

        if (this->total_cells < mines - 9)
        {
            exit(-1);
        }

        this->bg_sprite.setSize(sf::Vector2f(scale.x * this->width, scale.y * this->height));
        this->bg_sprite.setPosition(0, 0);

        this->bg_sprite.setTexture(tex);
        this->bg_sprite.setTextureRect(sf::IntRect(0, 0, scale.x * this->width, scale.y * this->height));

        this->bg_sprite.setScale(SPRITE_SIZE / (double)scale.x, SPRITE_SIZE / (double)scale.y);


        this->make_cells(textures);
    }

    void* make_cells(auto textures)
    {
        for (int i = 0; i < this->total_cells; i++)
        {
            Cell cell(textures);

            this->grid.push_back(cell);
        }
    }

    void* add_mines()
    {
        std::vector<int> mine_positions;
        for (int m = 0; m < this->mines; m++)
        {
            int mine = rand() % this->total_cells;

            unsigned short col = mine % this->width;
            unsigned short row = mine / this->width;

            bool valid = true;

            for (int c = -1; c < 2; c++)
            {
                for (int r = -1; r < 2; r++)
                {
                    if (col + c < 0 || row + r < 0 || col + c >= this->width || row + r >= this->height)
                    {
                        continue;
                    }
                    else
                    {
                        if (mine + c + this->width * r == this->hovered)
                        {
                            valid = false;
                            break;
                        }
                    }
                }
            }

            if (!valid || this->grid[mine].mine)
            {
                m --;
                continue;
            }
            else
            {
                this->grid[mine].mine = true;
            }
        }

        this->count_mines();
    }

    void* count_mines()
    {
        for (int i = 0; i < this->total_cells; i++)
        {
            Cell cell = this->grid[i];

            if (cell.mine)
            {
                unsigned short col = i % this->width;
                unsigned short row = i / this->width;

                for (int r = -1; r < 2; r++)
                {
                    for (int c = -1; c < 2; c++)
                    {
                        if (col + c < 0 || row + r < 0 || col + c >= this->width || row + r >= this->height)
                        {
                            continue;
                        }
                        else
                        {
                            Cell* surrounding = &this->grid[i + c + this->width * r];
                            surrounding->surrounding++;
                        }
                    }
                }
            }
        }
    }

    void* draw_to(sf::RenderWindow *window)
    {
        window->draw(this->bg_sprite);

        for (int i = 0; i < this->total_cells; i++)
        {
            unsigned short col = i % this->width;
            unsigned short row = i / this->width;

            Cell cell = this->grid[i];
            sf::Sprite* sprite = cell.get_sprite();

            sprite->setPosition(col * SPRITE_SIZE, row * SPRITE_SIZE);

            window->draw(*sprite);
        }
    }

    bool set_hovered(int x, int y)
    {
        this->grid[this->hovered].hovered = false;
        int past_hovered = this->hovered;
        this->hovered = -1;

        if (!(x >= SPRITE_SIZE * this->width || y >= SPRITE_SIZE * this->height || x < 0 || y < 0))
        {
            unsigned short col = x / SPRITE_SIZE;
            unsigned short row = y / SPRITE_SIZE;

            this->hovered = row * this->width + col;
            this->grid[this->hovered].hovered = true;
        }

        if (this->hovered == past_hovered)
        {
            return false;
        }
        else 
        {
            return true;
        }
    }

    bool open_cell(int i)
    {
        Cell* c = &this->grid[i];

        if (!c->flagged)
        {
            c->open_cell(this->textures);

            int col = i % this->width;
            int row = i / this->width;

            if (c->surrounding == 0)
            {
                for (int r = -1; r < 2; r++)
                {
                    for (int c = -1; c < 2; c++)
                    {
                        if (col + c < 0 || row + r < 0 || col + c >= this->width || row + r >= this->height || (c == 0 && r == 0))
                        {
                            continue;
                        }
                        else
                        {
                            Cell* surround = &this->grid[i + c + this->width * r];

                            if (!surround->open)
                            {
                                this->open_cell(i + c + this->width * r);
                            }
                        }
                    }
                }
            }

            return c->mine;
        }
    }

    bool open_click()
    {
        if (this->hovered >= 0)
        {
            if (!this->initialized)
            {
                this->add_mines();
                this->initialized = true;
            }

            return this->open_cell(this->hovered);
        }
    }

    void* flag_click()
    {
        if (this->hovered >= 0)
        {
            Cell* cell = &this->grid[this->hovered];

            if (!cell->open)
            {
                cell->flag_cell(this->textures);
            }
        }
    }

private:
    int width;
    int height;
    int mines;
    int total_cells;
    int hovered = 0;
    bool initialized = false;

    sf::RectangleShape bg_sprite;

    std::unordered_map<std::string, sf::Texture*>* textures;

    std::vector<Cell> grid;
};