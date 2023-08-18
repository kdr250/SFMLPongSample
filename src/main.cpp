#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>
#include <vector>

using Block = std::unique_ptr<sf::RectangleShape>;
using Blocks = std::vector<Block>;

enum class GameState {
    PLAY,
    GAME_OVER,
    CLEAR
};

void handleInput(float& paddleSpeed, sf::Keyboard::Key key, const GameState& gameState)
{
    if (gameState != GameState::PLAY)
        return;

    switch (key)
    {
    case sf::Keyboard::A:
    case sf::Keyboard::Left:
        paddleSpeed = -10.0f;
        break;
    case sf::Keyboard::D:
    case sf::Keyboard::Right:
        paddleSpeed = 10.0f;
        break;
    default:
        paddleSpeed = 0.0f;
        break;
    }
}

void movePaddle(sf::RectangleShape& paddle, const float& paddleSpeed, sf::RenderWindow& window, GameState& gameState)
{
    if (gameState != GameState::PLAY)
        return;

    sf::Vector2f position = paddle.getPosition();
    if (paddleSpeed < 0.0f)
    {
        position.x = position.x + paddleSpeed > 0.0f ? position.x + paddleSpeed : 0;
    }
    else if (paddleSpeed > 0.0f)
    {
        position.x = position.x + paddleSpeed < window.getSize().x - paddle.getSize().x
            ? position.x + paddleSpeed
            : window.getSize().x - paddle.getSize().x;
    }
    paddle.setPosition(position);
}

void moveBall(sf::CircleShape& ball,
              sf::Vector2f& ballVelocity,
              GameState& gameState,
              const sf::RenderWindow& window,
              const sf::RectangleShape& paddle)
{
    sf::Vector2f position = ball.getPosition();
    position.x += ballVelocity.x;
    position.y += ballVelocity.y;
    if (position.x <= 0.0f || position.x >= window.getSize().x)
    {
        ballVelocity.x *= -1;
    }
    if (position.y <= 0.0f)
    {
        ballVelocity.y *= -1;
    }
    if (position.y >= window.getSize().y)
    {
        gameState = GameState::GAME_OVER;
    }
    if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
    {
        position.y -= ball.getRadius() * 2.0f;
        ballVelocity.y *= -1;
    }
    ball.setPosition(position);
}

void isCollide(const sf::CircleShape& ball, Blocks& blocks, sf::Vector2f& ballVelocity, GameState& gameState)
{
    for (auto itr = blocks.begin(); itr != blocks.end();)
    {
        Block& block = *itr;
        if (ball.getGlobalBounds().intersects(block->getGlobalBounds()))
        {
            sf::Vector2f blockPosition = block->getPosition();
            sf::Vector2f blockSize = block->getSize();
            sf::Vector2f ballPosition = ball.getPosition();

            if ((blockPosition.x <= ballPosition.x) && (ballPosition.x <= blockPosition.x + blockSize.x))
            {
                ballVelocity.y *= -1.3f;
            }
            else
            {
                ballVelocity.x *= -1.3f;
            }

            itr = blocks.erase(itr);
        }
        else
        {
            itr++;
        }
    }
    if (blocks.empty())
    {
        gameState = GameState::CLEAR;
    }
}

void update(sf::CircleShape& ball,
              sf::Vector2f& ballVelocity,
              Blocks& blocks,
              GameState& gameState,
              sf::RenderWindow& window,
              sf::RectangleShape& paddle,
              const float& paddleSpeed)
{
    movePaddle(paddle, paddleSpeed, window, gameState);
    moveBall(ball, ballVelocity, gameState, window, paddle);
    isCollide(ball, blocks, ballVelocity, gameState);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Sample");
    window.setFramerateLimit(60);

    sf::RectangleShape paddle(sf::Vector2f(100.0f, 10.0f));
    paddle.setFillColor(sf::Color::Blue);
    paddle.setPosition(400, 700);
    float paddleSpeed = 0.0f;

    sf::CircleShape ball(10.0f);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition(300, 500);
    sf::Vector2f ballVelocity(5.0f, 5.0f);

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

    sf::Font font;
    font.loadFromFile("resources/font/Roboto-Bold.ttf");
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Yellow);
    text.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    GameState gameState = GameState::PLAY;

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
                    handleInput(paddleSpeed, event.key.code, gameState);
                }
            }
            else
            {
                paddleSpeed = 0.0f;
            }
        }

        if (gameState == GameState::PLAY)
        {
            update(ball, ballVelocity, blocks, gameState, window, paddle, paddleSpeed);
        }

        window.clear();
        window.draw(paddle);
        window.draw(ball);

        for (auto& block : blocks)
        {
            window.draw(*block);
        }

        if (gameState == GameState::CLEAR)
        {
            text.setString("Clear!");
            text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
            window.draw(text);
        }
        else if (gameState == GameState::GAME_OVER)
        {
            text.setString("Game Over");
            text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
            window.draw(text);
        }

        window.display();
    }

    return 0;
}
