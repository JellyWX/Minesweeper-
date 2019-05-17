#include "main.h"


int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML Test");

    sf::Event event;

    sf::Font font;

    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
    {
        std::cout << "Error loading font";
        return -1;
    }

    sf::Text text;

    text.setFont(font);

    text.setCharacterSize(8);

    sf::Clock clock;

    std::string clock_timer;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::EventType::Closed)
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        show_fps(&window, &clock, &text);

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