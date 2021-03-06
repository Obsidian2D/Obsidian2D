#include <chrono>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "Player.h"
#include "SoundManager.h"
#include "Debug.h"
#include "World.h"


#define MS_PER_UPDATE 16

static unsigned long long getCurrentTime()
{
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

void run()
{
    auto window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Obsidian2D", sf::Style::Default, sf::ContextSettings(24,8,16));
    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    //view.zoom(1.25f);
    window->setView(view);

    World protoWorld("assets/maps/0001.json");
    Player player;
    player.position.setPosition(protoWorld.getFirstTilePosition());
    // Declare and load a font
    sf::Font font;
    if(!font.loadFromFile("assets/fonts/MavenPro-Regular.ttf")) {
        std::cout << "font failed" << std::endl;
    }
    // Create a text
    sf::Text text("Little Bobby Tables", font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2);

    unsigned long long previous = getCurrentTime();
    unsigned long long lag = 0;

    bool running = true;

    sf::Clock deltaClock;
    while (running)
    {
        unsigned long long current = getCurrentTime();
        unsigned long long elapsed = current - previous;
        previous = current;
        lag += elapsed;

        sf::Event event;
        while (window->pollEvent(event))
        {
            Debug::processEvent(event);

            if (event.type == sf::Event::Closed) {
                window->close();
                running = false;
            } else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F12) {
                Debug::startStop(*window);
            }

            player.input(event);
        }

        while (lag >= MS_PER_UPDATE)
        {
            lag -= MS_PER_UPDATE;

            player.update(elapsed);
            view.setCenter(player.position.getPosition().x, player.position.getPosition().y);
            text.setPosition(player.position.getPosition().x+10, player.position.getPosition().y);
            window->setView(view);
        }

        sf::Vector2f mouseWorldPos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

        vec2f mouse;
        mouse.x = mouseWorldPos.x;
        mouse.y = mouseWorldPos.y;

        protoWorld.update(current, mouse);
        Debug::update(*window, deltaClock.restart());
        Debug::test();

        window->clear(sf::Color::Color(44,62,80));

        protoWorld.draw(window);

        window->draw(player.getSprite());

        // auto defaultView = window->getDefaultView();
        // window->setView(defaultView);
         window->draw(text);
        // window->setView(view);
        Debug::render(*window);

        window->display();
    }

    Debug::shutdown();
}
