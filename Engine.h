#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Particle.h"

using namespace sf;
using namespace std;

class Engine
{
private:
    RenderWindow       m_Window;
    vector<Particle>   m_particles;

    
    Color m_bgColor;
    float m_bgPhase;

    void input();
    void update(float dtAsSeconds);
    void draw();

public:
    Engine();
    void run();
};
