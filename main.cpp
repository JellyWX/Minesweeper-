#include "main.h"

#define SLEEP_TIME 50000
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS_COUNTER_SIZE 16
#define TARGET_FPS 120


int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Test");

    // event buffer
    sf::Event event;

    // mono font
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
    {
        std::cout << "Error loading font";
        return -1;
    }

    sf::Text fps_counter;
    fps_counter.setFont(font);
    fps_counter.setCharacterSize(FPS_COUNTER_SIZE);

    sf::Clock clock;

    bool focused = true;

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

                case sf::Event::LostFocus:
                    focused = false;
                    break;

                case sf::Event::GainedFocus:
                    focused = true;
                    break;
            }
        }

        // if the window isnt focused, tell the program to slow down a little
        if (!focused)
        {
            usleep(SLEEP_TIME);
        }
        else
        {
            window.clear(sf::Color::Black);

            usleep(((1.0 / TARGET_FPS) * 1000 - clock.getElapsedTime().asMilliseconds()) * 1000);

            show_fps(&window, &clock, &fps_counter);
        }

        window.display();
    }
}

void* show_fps(sf::RenderWindow *window, sf::Clock *clock, sf::Text *location)
{
    int fps = 1 / ( clock->getElapsedTime().asMicroseconds() / 1000000.0 );

    location->setString(std::to_string(fps));

    clock->restart();

    window->draw(*location);

    return 0;
}

void* resize_window(sf::RenderWindow *window, int width, int height)
{
    sf::FloatRect visibleArea(0.f, 0.f, width, height);
    window->setView(sf::View(visibleArea));
}