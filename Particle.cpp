#include "Particle.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

Particle::Particle(RenderTarget& target,
                   int numPoints,
                   Vector2i mouseClickPosition)
    : m_ttl(TTL),
      m_numPoints(numPoints),
      m_radiansPerSec(0.0f),
      m_vx(0.0f),
      m_vy(0.0f),
      m_color1(Color::White),
      m_color2(Color::Red),
      m_A(2, numPoints)
{
    Vector2u winSize = target.getSize();
    m_cartesianPlane.setCenter(0.f, 0.f);
    m_cartesianPlane.setSize(static_cast<float>(winSize.x),
                             -static_cast<float>(winSize.y));

    m_centerCoordinate =
        target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    float frac = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    m_radiansPerSec = frac * static_cast<float>(M_PI);

    m_vx = 100.f + static_cast<float>(rand() % 401);
    if (rand() % 2) m_vx *= -1.f;
    m_vy = 100.f + static_cast<float>(rand() % 401);

    m_color1 = Color::White;
    m_color2 = Color(static_cast<Uint8>(rand() % 256),
                     static_cast<Uint8>(rand() % 256),
                     static_cast<Uint8>(rand() % 256));

    double theta  = (static_cast<double>(rand()) / RAND_MAX) * (M_PI / 2.0);
    double dTheta = 2.0 * M_PI / (numPoints - 1);

    for (int j = 0; j < numPoints; ++j)
    {
        double radius = 20.0 + (static_cast<double>(rand()) / RAND_MAX) * 60.0;
        double dx = radius * cos(theta);
        double dy = radius * sin(theta);

        m_A(0, j) = m_centerCoordinate.x + static_cast<float>(dx);
        m_A(1, j) = m_centerCoordinate.y + static_cast<float>(dy);

        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    VertexArray fan(TrianglesFan, m_numPoints + 1);

    Vector2i centerPix =
        target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane);
    fan[0].position = Vector2f(static_cast<float>(centerPix.x),
                               static_cast<float>(centerPix.y));
    fan[0].color = m_color1;

    for (int j = 0; j < m_numPoints; ++j)
    {
        Vector2f cartPoint(static_cast<float>(m_A(0, j)),
                           static_cast<float>(m_A(1, j)));

        Vector2i pix =
            target.mapCoordsToPixel(cartPoint, m_cartesianPlane);

        fan[j + 1].position =
            Vector2f(static_cast<float>(pix.x), static_cast<float>(pix.y));
        fan[j + 1].color = m_color2;
    }

    target.draw(fan, states);
}

void Particle::update(float dt)
{
    m_ttl -= dt;

    rotate(dt * m_radiansPerSec);

    scale(SCALE);

    float dx = m_vx * dt;

    m_vy -= G * dt;
    float dy = m_vy * dt;

    translate(dx, dy);
}

void Particle::rotate(double theta)
{
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    RotationMatrix R(theta);
    m_A = R * m_A;

    translate(temp.x, temp.y);
}

void Particle::scale(double c)
{
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    ScalingMatrix S(c);
    m_A = S * m_A;

    translate(temp.x, temp.y);
}

void Particle::translate(double xShift, double yShift)
{
    TranslationMatrix T(xShift, yShift, m_A.getCols());
    m_A = T + m_A;

    m_centerCoordinate.x += static_cast<float>(xShift);
    m_centerCoordinate.y += static_cast<float>(yShift);
}

bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: ("
             << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")"
             << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..."
         << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) ||
            !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                 << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                 << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) ||
            !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                 << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                 << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) ||
            !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                 << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                 << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}
