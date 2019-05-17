#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>


bool reverse_compare(int i, int j)
{
    return i > j;
}

struct Cell {
    bool mine = false;
    bool open = false;
    int surrounding = 0;
};


class Grid {
public:
    Grid(int width, int height, int mines)
    {
        srand( time(NULL) );

        if (width * height < mines)
        {
            exit(-1);
        }

        this->width = width;
        this->height = height;
        this->mines = mines;

        this->place_mines();
    }

    void* place_mines()
    {
        int total_cells = width * height;

        std::vector<int> mine_positions;
        for (int m = 0; m < this->mines; m++)
        {
            mine_positions.push_back(rand() % (total_cells - m));
        }

        std::sort(mine_positions.begin(), mine_positions.end(), reverse_compare);

        for (int i = 0; i < total_cells; i++)
        {
            Cell cell;
            if (mine_positions.back() <= i && (!mine_positions.empty()))
            {
                std::cout << mine_positions.back() << std::endl;
                cell.mine = true;
                mine_positions.pop_back();
            }
            this->grid.push_back(cell);
        }

        int c = 0;

        for (int i = 0; i < total_cells; i++)
        {
            if (grid[i].mine)
            {
                c ++;
            }
        }

        std::cout << c << " mines places out of " << this->mines << std::endl;
    }

private:
    int width;
    int height;
    int mines;

    std::vector<Cell> grid;
};