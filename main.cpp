#include "main.h"

#define IDLE_FPS 5
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS_COUNTER_SIZE 16
#define TARGET_FPS 40
#define FPS_CHECK_INTERVAL 15
#define ZOOM 0.15
#define LONG_HOLD_TIME 250
#define MOVE_THRESHOLD 24


Minesweeper::Minesweeper(unsigned short width, unsigned short height, unsigned short mines) :
    window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Minesweeper++"),
    hud(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT)),
    game(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT))
{
    auto textures = this->load_textures();

    // used to resize all views when screen resized
    this->views.push_back(&hud);
    this->views.push_back(&game);

    this->window.setFramerateLimit(TARGET_FPS);

    // event buffer
    sf::Event event;

    // mono font
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
        exit(-1);
    }

    // enable FPS counter in debug mode
    #ifndef NDEBUG
    sf::Text fps_counter;
    fps_counter.setFont(font);
    fps_counter.setCharacterSize(FPS_COUNTER_SIZE);
    #endif

    Grid* grid = new Grid(width, height, mines, &textures);

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::EventType::Closed:
                   window.close();
                   break;

                case sf::Event::Resized:
                    resize_window(event.size.width, event.size.height);
                    break;

                case sf::Event::MouseMoved:
                    manage_move(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), grid, &game);
                    break;

                case sf::Event::MouseButtonPressed:
                    {
                        this->mouse_down_time.restart();
                        this->mouse_down = true;
                        this->mouse_down_position = sf::Mouse::getPosition(window);
                        this->old_mouse_position = sf::Mouse::getPosition(window);
                        this->mouse_held_moved = false;
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    manage_click(event.mouseButton.button, grid);
                    break;

                case sf::Event::MouseWheelScrolled:
                    zoom_view(event.mouseWheelScroll.delta);
                    break;
            }
        }

        current_frame ++;

        window.clear(sf::Color::Black);

        // switch to draw to game view
        window.setView(game);

        grid->draw_to(&window);

        // switch to draw to HUD (static components)
        window.setView(hud);
        #ifndef NDEBUG
        show_fps(&frame_clock, &fps_counter);
        #endif

        frame_clock.restart();

        check_average_fps();

        // if the window isnt focused, tell the program to slow down a little
        if (!window.hasFocus())
        {
            usleep(((1.0 / IDLE_FPS) * 1000 - frame_clock.getElapsedTime().asMilliseconds()) * 1000);
        }

        window.display();
    }
}

std::unordered_map<std::string, sf::Texture*> Minesweeper::load_textures()
{
    std::unordered_map<std::string, sf::Texture*> map;

    struct dirent *drnt;

    DIR *dr;
    dr = opendir("../images/");
    drnt = readdir(dr);

    while (drnt != 0) {
        std::string name = drnt->d_name;

        // name longer than 4 and ending in .png
        if (name.length() > 4 && (name.substr(name.length() - 4) == ".png"))
        {
            sf::Texture* t = new sf::Texture;
            if (t->loadFromFile("../images/" + name))
            {
                #ifndef NDEBUG
                    std::cout << "Loaded file " << name << " successfully" << std::endl;
                #endif

                t->setRepeated(true);
                map[name.substr(0, name.length() - 4)] = t;
            }
        }
        drnt = readdir(dr);
    }

    return map;
}

#ifndef NDEBUG
void* Minesweeper::show_fps(sf::Clock *clock, sf::Text *location)
{
    int fps = 1 / ( clock->getElapsedTime().asMicroseconds() / 1000000.0 );

    location->setString(std::to_string(fps));

    this->window.draw(*location);

    return 0;
}
#endif

void* Minesweeper::check_average_fps()
{
    if (this->lifetime_clock.getElapsedTime().asSeconds() > FPS_CHECK_INTERVAL)
    {
        std::cout << "Average " << FPS_CHECK_INTERVAL << " second FPS: " << this->current_frame / this->lifetime_clock.getElapsedTime().asSeconds() << std::endl;

        this->lifetime_clock.restart();
        this->current_frame = 0;
    }
}

void* Minesweeper::resize_window(int width, int height)
{
    for (auto view : this->views)
    {
        sf::FloatRect visibleArea(0.f, 0.f, width, height);
        view->reset(visibleArea);
    }
}

void* Minesweeper::manage_move(sf::Vector2i pos, Grid* grid, sf::View* game_view)
{

    if (!this->mouse_held_moved)
    {
        sf::Vector2i movement_abs = this->mouse_down_position - pos;
    
        if (magnitude(movement_abs) > MOVE_THRESHOLD)
        {
            this->mouse_held_moved = true;
        }
    }
    else if (this->mouse_down)
    {
        sf::Vector2i movement = old_mouse_position - pos;

        game_view->move(movement.x, movement.y);
        this->old_mouse_position = pos;
    }

    sf::Vector2f world_pos = this->window.mapPixelToCoords(pos, *game_view);

    grid->set_hovered(world_pos.x, world_pos.y);
}

void* Minesweeper::manage_click(sf::Mouse::Button button, Grid* grid)
{
    if (!this->mouse_held_moved)
    {
        if (mouse_down_time.getElapsedTime().asMilliseconds() < LONG_HOLD_TIME && button == sf::Mouse::Left)
        {
            grid->open_click();
        }
        else
        {
            grid->flag_click();
        }
    }

    this->mouse_down = false;
    this->mouse_down_time.restart();
}

void* Minesweeper::zoom_view(int direction)
{
    this->game.zoom(1 + ZOOM * direction);
}

int main(int argc, char** argv)
{
    Minesweeper* game = new Minesweeper(20, 20, 100);
}

double magnitude(sf::Vector2i v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}