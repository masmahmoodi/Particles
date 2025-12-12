#include "Particle.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

static Color fromHSV(float h, float s, float v, Uint8 a = 255)
{
    while (h < 0.f)   h += 360.f;
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

    Uint8 R = static_cast<Uint8>((r + m) * 255.f);
    Uint8 G = static_cast<Uint8>((g + m) * 255.f);
    Uint8 B = static_cast<Uint8>((b + m) * 255.f);

    return Color(R, G, B, a);
}

Particle::Particle(RenderTarget& target,
                   int numPoints,
                   Vector2i mouseClickPosition)
    : m_ttl(TTL),
      m_numPoints(numPoints),
      m_radiansPerSec(0.0f),
      m_vx(0.0f),
      m_vy(0.0f),
      m_color1(Color::White),
      m_A(2, numPoints),
      m_groundY(0.0f),
      m_vertexColors(numPoints)
{
    Vector2u winSize = target.getSize();
    m_cartesianPlane.setCenter(0.f, 0.f);
    m_cartesianPlane.setSize(static_cast<float>(winSize.x),
                             -static_cast<float>(winSize.y));

    m_centerCoordinate =
        target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    Vector2i bottomPixel(0, static_cast<int>(winSize.y));
    Vector2f bottomCoord =
        target.mapPixelToCoords(bottomPixel, m_cartesianPlane);
    m_groundY = bottomCoord.y;

    float frac = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    m_radiansPerSec = frac * static_cast<float>(M_PI);

    m_vx = 100.f + static_cast<float>(rand() % 401);
    if (rand() % 2) m_vx *= -1.f;

    m_vy = 100.f + static_cast<float>(rand() % 401);

    float baseHue = static_cast<float>(rand() % 360);

    double theta  = (static_cast<double>(rand()) / RAND_MAX) * 2.0 * M_PI;
    double dTheta = 2.0 * M_PI / (numPoints);

    for (int j = 0; j < numPoints; ++j)
    {
        double baseRads = 35.0 + (static_cast<double>(rand()) / RAND_MAX) * 30.0;
        double radius;

        if (j % 3 == 0)
            radius = baseRads * 2.0;
        else if (j % 3 == 1)
            radius = baseRads * 1.2;
        else
            radius = baseRads * 0.6;

        double dx = radius * cos(theta);
        double dy = radius * sin(theta);

        m_A(0, j) = m_centerCoordinate.x + static_cast<float>(dx);
        m_A(1, j) = m_centerCoordinate.y + static_cast<float>(dy);

        float t = static_cast<float>(j) / static_cast<float>(numPoints);
        float hue = baseHue + 80.f * t;
        Color c = fromHSV(hue, 0.95f, 1.0f, 240);

        m_vertexColors[j] = c;

        theta += dTheta;
    }

    m_color1 = Color(255, 255, 255);
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    VertexArray glowFan(TrianglesFan, m_numPoints + 1);

    Vector2f centerCart = m_centerCoordinate;
    Vector2i centerPix =
        target.mapCoordsToPixel(centerCart, m_cartesianPlane);

    glowFan[0].position = Vector2f(static_cast<float>(centerPix.x),
                                   static_cast<float>(centerPix.y));
    glowFan[0].color = Color(255, 255, 255, 50);

    for (int j = 0; j < m_numPoints; ++j)
    {
        Vector2f cartPoint(static_cast<float>(m_A(0, j)),
                           static_cast<float>(m_A(1, j)));

        Vector2f dir = cartPoint - centerCart;
        Vector2f glowCart = centerCart + dir * 1.4f;

        Vector2i glowPix =
            target.mapCoordsToPixel(glowCart, m_cartesianPlane);

        Color c = m_vertexColors[j];
        glowFan[j + 1].position =
            Vector2f(static_cast<float>(glowPix.x),
                     static_cast<float>(glowPix.y));
        glowFan[j + 1].color = Color(c.r, c.g, c.b, 80);
    }

    VertexArray fan(TrianglesFan, m_numPoints + 1);

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
        fan[j + 1].color = m_vertexColors[j];
    }

    VertexArray outline(LinesStrip, m_numPoints + 1);
    for (int j = 0; j < m_numPoints; ++j)
    {
        Vector2f cartPoint(static_cast<float>(m_A(0, j)),
                           static_cast<float>(m_A(1, j)));
        Vector2i pix =
            target.mapCoordsToPixel(cartPoint, m_cartesianPlane);

        outline[j].position =
            Vector2f(static_cast<float>(pix.x), static_cast<float>(pix.y));
        outline[j].color = Color(255, 255, 255, 230);
    }
    outline[m_numPoints] = outline[0];

    target.draw(glowFan, states);
    target.draw(fan, states);
    target.draw(outline, states);
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

    if (m_centerCoordinate.y < m_groundY)
    {
        float penetration = m_groundY - m_centerCoordinate.y;
        translate(0.0, penetration);

        m_vy = -m_vy * 0.6f;
    }
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
