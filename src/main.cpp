#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <memory>

using Block = std::unique_ptr<sf::RectangleShape>;
using Blocks = std::vector<Block>;

void movePaddle(sf::RectangleShape& paddle, sf::Keyboard::Key key)
{
    float speed = 80.0f;
    sf::Vector2f position = paddle.getPosition();
    switch (key)
    {
    case sf::Keyboard::A:
        position.x -= speed;
        break;
    case sf::Keyboard::D:
        position.x += speed;
        break;
    case sf::Keyboard::W:
        position.y -= speed;
        break;
    case sf::Keyboard::S:
        position.y += speed;
        break;
    default:
        break;
    }
    paddle.setPosition(position);
}

void moveBall(sf::CircleShape& ball, sf::Vector2f& velocity, const sf::RenderWindow& window, const sf::RectangleShape& paddle)
{
    sf::Vector2f position = ball.getPosition();
    position.x += velocity.x;
    position.y += velocity.y;
    if (position.x <= 0.0f || position.x >= window.getSize().x)
    {
        velocity.x *= -1;
    }
    if (position.y <= 0.0f || position.y >= window.getSize().y)
    {
        velocity.y *= -1;
    }
    if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
    {
        position.y -= ball.getRadius();
        velocity.y *= -1;
    }
    ball.setPosition(position);
}

void isCollide(const sf::CircleShape& ball, Blocks& blocks)
{
    for (auto itr = blocks.begin(); itr != blocks.end();)
    {
        Block& block = *itr;
        if (ball.getGlobalBounds().intersects(block->getGlobalBounds()))
        {
            itr = blocks.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Sample");
    window.setFramerateLimit(60);

    sf::RectangleShape paddle(sf::Vector2f(100.0f, 10.0f));
    paddle.setFillColor(sf::Color::Blue);
    paddle.setPosition(400, 500);

    sf::CircleShape ball(10.0f);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition(300, 300);
    sf::Vector2f velocity(-5.0f, -5.0f);

    Blocks blocks;
    for (int i = 0; i < 8; i++)
    {
        float x = (i % 4) * 250.0f + 100.0f;
        float y = (i / 4) * 150.0f + 100.0f;
        Block block = std::make_unique<sf::RectangleShape>(sf::Vector2f(200.0f, 100.0f));
        block->setFillColor(sf::Color::Red);
        block->setPosition(sf::Vector2f(x, y));
        blocks.emplace_back(std::move(block));
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else
                {
                    movePaddle(paddle, event.key.code);
                }
            }
        }

        moveBall(ball, velocity, window, paddle);

        isCollide(ball, blocks);

        window.clear();
        window.draw(paddle);
        window.draw(ball);

        for (auto& block : blocks)
        {
            window.draw(*block);
        }

        window.display();
    }

    return 0;
}