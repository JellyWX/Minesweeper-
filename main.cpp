#include "main.h"

#define IDLE_FPS 5
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS_COUNTER_SIZE 16
#define TARGET_FPS 40


int main(int argc, char** argv)
{
    auto textures = load_textures();

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Minesweeper++");

    // event buffer
    sf::Event event;

    // mono font
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
    {
        std::cout << "Error loading font";
        return -1;
    }

    // enable FPS counter in debug mode
    #ifndef NDEBUG
        sf::Text fps_counter;
        fps_counter.setFont(font);
        fps_counter.setCharacterSize(FPS_COUNTER_SIZE);
    #endif

    sf::Clock clock;

    Grid grid(10, 14, 10, &textures);

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
                    resize_window(&window, event.size.width, event.size.height);
                    break;

                case sf::Event::MouseMoved:
                    manage_move(event.mouseMove.x, event.mouseMove.y, &grid);
                    break;

                case sf::Event::MouseButtonReleased:
                    manage_click(event.mouseButton.button, &grid);
                    break;
            }
        }

        window.clear(sf::Color::Black);

        grid.draw(&window);

        #ifndef NDEBUG
            show_fps(&window, &clock, &fps_counter);
        #endif

        clock.restart();

        // if the window isnt focused, tell the program to slow down a little
        // otherwise, cap framerate at the target FPS
        if (!window.hasFocus())
        {
            usleep(((1.0 / IDLE_FPS) * 1000 - clock.getElapsedTime().asMilliseconds()) * 1000);
        }
        else
        {
            usleep(((1.0 / TARGET_FPS) * 1000 - clock.getElapsedTime().asMilliseconds()) * 1000);
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

void* resize_window(sf::RenderWindow *window, int width, int height)
{
    sf::FloatRect visibleArea(0.f, 0.f, width, height);
    window->setView(sf::View(visibleArea));
}

void* manage_move(int x, int y, Grid* grid)
{
    grid->set_hovered(x, y);
}

void* manage_click(sf::Mouse::Button button, Grid* grid)
{
    if (button == sf::Mouse::Left)
    {
        grid->open_click();
    }
}