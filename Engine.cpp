#include "Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

static sf::Color fromHSV(float h, float s, float v, sf::Uint8 a = 255)
{
    while (h < 0.f) h += 360.f;
    while (h >= 360.f) h -= 360.f;

    float c = v * s;
    float x = c * (1.f - fabsf(fmodf(h / 60.f, 2.f) - 1.f));
    float m = v - c;

    float r = 0.f, g = 0.f, b = 0.f;

    if (h < 60.f)       { r = c; g = x; b = 0.f; }
    else if (h < 120.f) { r = x; g = c; b = 0.f; }
    else if (h < 180.f) { r = 0.f; g = c; b = x; }
    else if (h < 240.f) { r = 0.f; g = x; b = c; }
    else if (h < 300.f) { r = x; g = 0.f; b = c; }
    else                { r = c; g = 0.f; b = x; }

    sf::Uint8 R = static_cast<sf::Uint8>((r + m) * 255.f);
    sf::Uint8 G = static_cast<sf::Uint8>((g + m) * 255.f);
    sf::Uint8 B = static_cast<sf::Uint8>((b + m) * 255.f);

    return sf::Color(R, G, B, a);
}

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
    m_bgTime += dtAsSeconds; 

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
   

   
    float hueTop    = fmodf(t * 35.f, 360.f);
    float hueBottom = fmodf(hueTop + 140.f, 360.f);

    
    sf::Color top    = fromHSV(hueTop,    0.55f, 0.22f, 255);
    sf::Color bottom = fromHSV(hueBottom, 0.60f, 0.10f, 255);

    sf::VertexArray bg(sf::Quads, 4);
    bg[0].position = {0.f, 0.f};
    bg[1].position = {static_cast<float>(m_Window.getSize().x), 0.f};
    bg[2].position = {static_cast<float>(m_Window.getSize().x),
                      static_cast<float>(m_Window.getSize().y)};
    bg[3].position = {0.f, static_cast<float>(m_Window.getSize().y)};

    bg[0].color = top;
    bg[1].color = top;
    bg[2].color = bottom;
    bg[3].color = bottom;

    m_Window.clear();
    m_Window.draw(bg);

    for (auto& p : m_particles)
        m_Window.draw(p);

    m_Window.display();
}
