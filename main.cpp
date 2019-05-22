#include "main.h"

#define IDLE_FPS 5
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS_COUNTER_SIZE 16
#define TARGET_FPS 40
#define FPS_CHECK_INTERVAL 15


int main(int argc, char** argv)
{
    auto textures = load_textures();

    sf::View hud(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));
    sf::View game(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));

    // used to resize all views when screen resized
    std::vector<sf::View*> views;
    views.push_back(&hud);
    views.push_back(&game);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Minesweeper++");

    window.setFramerateLimit(TARGET_FPS);

    // event buffer
    sf::Event event;

    // mono font
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
        return -1;
    }

    // enable FPS counter in debug mode
    #ifndef NDEBUG
        sf::Text fps_counter;
        fps_counter.setFont(font);
        fps_counter.setCharacterSize(FPS_COUNTER_SIZE);
    #endif

    sf::Clock frame_clock;
    sf::Clock lifetime_clock;

    Grid grid(40, 40, 80, &textures);

    long current_frame = 0;

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
                    resize_window(views, event.size.width, event.size.height);
                    break;

                case sf::Event::MouseMoved:
                    manage_move(&window, sf::Vector2i(event.mouseMove.x, event.mouseMove.y), &grid, &game);
                    break;

                case sf::Event::MouseButtonReleased:
                    manage_click(event.mouseButton.button, &grid);
                    break;

                case sf::Event::MouseWheelScrolled:
                    zoom_view(&game, event.mouseWheelScroll.delta);
                    break;
            }
        }

        current_frame ++;

        window.clear(sf::Color::Black);

        // switch to draw to game view
        window.setView(game);

        grid.draw_to(&window);

        // switch to draw to HUD (static components)
        window.setView(hud);
        #ifndef NDEBUG
            show_fps(&window, &frame_clock, &fps_counter);
        #endif

        frame_clock.restart();

        if (lifetime_clock.getElapsedTime().asSeconds() > FPS_CHECK_INTERVAL)
        {
            std::cout << "Average " << FPS_CHECK_INTERVAL << " second FPS: " << current_frame / lifetime_clock.getElapsedTime().asSeconds() << std::endl;

            lifetime_clock.restart();
            current_frame = 0;
        }

        // if the window isnt focused, tell the program to slow down a little
        if (!window.hasFocus())
        {
            usleep(((1.0 / IDLE_FPS) * 1000 - frame_clock.getElapsedTime().asMilliseconds()) * 1000);
        }

        window.display();
    }
}


std::unordered_map<std::string, sf::Texture*> load_textures()
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
void* show_fps(sf::RenderWindow *window, sf::Clock *clock, sf::Text *location)
{
    int fps = 1 / ( clock->getElapsedTime().asMicroseconds() / 1000000.0 );

    location->setString(std::to_string(fps));

    window->draw(*location);

    return 0;
}
#endif

void* resize_window(std::vector<sf::View*> views, int width, int height)
{
    for (auto view : views)
    {
        sf::FloatRect visibleArea(0.f, 0.f, width, height);
        view->reset(visibleArea);
    }
}

void* manage_move(sf::RenderWindow* window, sf::Vector2i pos, Grid* grid, sf::View* game_view)
{
    sf::Vector2f world_pos = window->mapPixelToCoords(pos, *game_view);

    grid->set_hovered(world_pos.x, world_pos.y);
}

void* manage_click(sf::Mouse::Button button, Grid* grid)
{
    if (button == sf::Mouse::Left)
    {
        grid->open_click();
    }
}

void* zoom_view(sf::View* view, int direction)
{
    if (direction > 0)
    {
        view->zoom(1.2);
    }
    else
    {
        view->zoom(0.8);
    }
}