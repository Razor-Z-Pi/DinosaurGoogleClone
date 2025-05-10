#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 300;
const int GROUND = 250;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -10.0f;

class Dino {
public:
    sf::RectangleShape shape;
    float velocity;
    bool isJumping;

    Dino() {
        shape.setSize(sf::Vector2f(40, 60));
        shape.setPosition(100, GROUND - 60);
        shape.setFillColor(sf::Color::Black);
        velocity = 0;
        isJumping = false;
    }

    void jump() {
        if (!isJumping) {
            velocity = JUMP_FORCE;
            isJumping = true;
        }
    }

    void update() {
        // Применяем гравитацию
        velocity += GRAVITY;
        shape.move(0, velocity);

        // Проверяем, чтобы динозавр не упал ниже земли
        if (shape.getPosition().y > GROUND - 60) {
            shape.setPosition(shape.getPosition().x, GROUND - 60);
            velocity = 0;
            isJumping = false;
        }
    }
};

class Cactus {
public:
    sf::RectangleShape shape;
    float speed;

    Cactus(float x) {
        int height = rand() % 30 + 30;
        shape.setSize(sf::Vector2f(20, height));
        shape.setPosition(x, GROUND - height);
        shape.setFillColor(sf::Color::Black);
        speed = 5.0f;
    }

    void update() {
        shape.move(-speed, 0);
    }

    bool isOffScreen() const {
        return shape.getPosition().x + shape.getSize().x < 0;
    }
};

int main() {
    srand(time(nullptr));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Google Dino Game");
    window.setFramerateLimit(60);

    Dino dino;
    std::vector<Cactus> cacti;
    sf::Clock cactusClock;
    sf::Clock gameClock;
    float cactusSpawnTime = 1.5f;
    bool gameOver = false;
    int score = 0;

    // Линия земли
    sf::RectangleShape ground(sf::Vector2f(WIDTH, 2));
    ground.setPosition(0, GROUND);
    ground.setFillColor(sf::Color::Black);

    // Шрифт для счета
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(WIDTH - 100, 20);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Up) {
                    if (gameOver) {
                        // Рестарт игры
                        gameOver = false;
                        dino = Dino();
                        cacti.clear();
                        score = 0;
                        cactusClock.restart();
                        gameClock.restart();
                    }
                    else {
                        dino.jump();
                    }
                }
            }
        }

        if (!gameOver) {
            // Обновление динозавра
            dino.update();

            // Спавн кактусов
            if (cactusClock.getElapsedTime().asSeconds() > cactusSpawnTime) {
                cacti.emplace_back(WIDTH);
                cactusClock.restart();
                cactusSpawnTime = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.5f));
            }

            // Обновление кактусов
            for (auto it = cacti.begin(); it != cacti.end();) {
                it->update();
                if (it->isOffScreen()) {
                    it = cacti.erase(it);
                    score += 1;
                }
                else {
                    // Проверка столкновений
                    if (dino.shape.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                        gameOver = true;
                    }
                    ++it;
                }
            }

            // Увеличение скорости со временем
            float gameTime = gameClock.getElapsedTime().asSeconds();
            for (auto& cactus : cacti) {
                cactus.speed = 5.0f + gameTime / 10.0f;
            }
        }

        // Отрисовка
        window.clear(sf::Color::White);
        window.draw(ground);
        window.draw(dino.shape);
        for (const auto& cactus : cacti) {
            window.draw(cactus.shape);
        }

        // Отображение счета
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        if (gameOver) {
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over! Press SPACE to restart");
            gameOverText.setCharacterSize(24);
            gameOverText.setFillColor(sf::Color::Black);
            gameOverText.setPosition(WIDTH / 2 - 200, HEIGHT / 2 - 30);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}