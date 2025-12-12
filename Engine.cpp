#include "Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

Engine::Engine()
{
    m_Window.create(sf::VideoMode::getDesktopMode(),
                    "Particles",
                    sf::Style::Close);
    m_Window.setVerticalSyncEnabled(true);
    srand(static_cast<unsigned int>(time(nullptr)));
}

void Engine::run()
{
    using namespace sf;

    cout << "Starting Particle unit tests..." << endl;

    Particle p(m_Window, 4,
               { static_cast<int>(m_Window.getSize().x) / 2,
                 static_cast<int>(m_Window.getSize().y) / 2 });
    p.unitTests();

    cout << "Unit tests complete.  Starting engine..." << endl;

    Clock clock;

    while (m_Window.isOpen())
    {
        Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}

void Engine::input()
{
    using namespace sf;

    Event event;
    while (m_Window.pollEvent(event))
    {
        if (event.type == Event::Closed)
            m_Window.close();

        if (event.type == Event::KeyPressed &&
            event.key.code == Keyboard::Escape)
            m_Window.close();

        if (event.type == Event::MouseButtonPressed &&
            event.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

            for (int i = 0; i < 5; ++i)
            {
                int numPoints = 25 + rand() % 26;
                m_particles.emplace_back(m_Window, numPoints, mousePos);
            }
        }
    }
}

void Engine::update(float dtAsSeconds)
{
    auto it = m_particles.begin();
    while (it != m_particles.end())
    {
        if (it->getTTL() <= 0.0f)
        {
            it = m_particles.erase(it);
        }
        else
        {
            it->update(dtAsSeconds);
            ++it;
        }
    }
}

void Engine::draw()
{
    m_Window.clear();

    for (auto& p : m_particles)
        m_Window.draw(p);

    m_Window.display();
}
