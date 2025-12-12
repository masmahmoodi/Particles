#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Particle.h"

using namespace sf;
using namespace std;

class Engine
{
private:
    RenderWindow m_Window;
    vector<Particle> m_particles;

    float m_bgTime = 0.0f; 
    void input();
    void update(float dtAsSeconds);
    void draw();

public:
    Engine();
    void run();
};
