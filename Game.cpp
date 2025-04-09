#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace sf;

struct SnakeSegment { int x, y; } s[100];
struct Food { int x, y; } f;
struct Obstacle { int x, y; } obs[2];

int num = 1;
int dir = 2, score = 0;
bool isGameOver = false;

const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 16;
const std::string SCORE_FILE = "scores.txt";

void generateObstacles() {
    for (int i = 0; i < 2; ++i) {
        obs[i].x = rand() % GRID_WIDTH;
        obs[i].y = rand() % GRID_HEIGHT;
    }
}

void updateSnake() {
    for (int i = num; i > 0; i--) {
        s[i].x = s[i - 1].x;
        s[i].y = s[i - 1].y;
    }

    if (dir == 0) s[0].y += 1;
    if (dir == 1) s[0].y -= 1;
    if (dir == 2) s[0].x += 1;
    if (dir == 3) s[0].x -= 1;

    // Wrap around
    if (s[0].x < 0) s[0].x = GRID_WIDTH - 1;
    if (s[0].x >= GRID_WIDTH) s[0].x = 0;
    if (s[0].y < 0) s[0].y = GRID_HEIGHT - 1;
    if (s[0].y >= GRID_HEIGHT) s[0].y = 0;

    // Check obstacle collision AFTER wrapping
    for (int i = 0; i < 2; ++i) {
        if (s[0].x == obs[i].x && s[0].y == obs[i].y) {
            isGameOver = true;
            return;
        }
    }

    // Check self collision
    for (int i = 1; i < num; i++) {
        if (s[0].x == s[i].x && s[0].y == s[i].y) {
            isGameOver = true;
            return;
        }
    }

    // Eat food
    if (s[0].x == f.x && s[0].y == f.y) {
        num++;
        score++;

        // Reposition food safely
        bool onSnakeOrObstacle;
        do {
            onSnakeOrObstacle = false;
            f.x = rand() % GRID_WIDTH;
            f.y = rand() % GRID_HEIGHT;
            for (int i = 0; i < num; i++) {
                if (s[i].x == f.x && s[i].y == f.y) {
                    onSnakeOrObstacle = true;
                    break;
                }
            }
            for (int i = 0; i < 2; ++i) {
                if (obs[i].x == f.x && obs[i].y == f.y) {
                    onSnakeOrObstacle = true;
                    break;
                }
            }
        } while (onSnakeOrObstacle);

        // Generate new obstacles
        generateObstacles();
    }
}


std::vector<int> loadHighScores() {
    std::vector<int> scores;
    std::ifstream file(SCORE_FILE);
    int val;
    while (file >> val) {
        scores.push_back(val);
    }
    return scores;
}

void saveHighScores(std::vector<int>& scores) {
    std::ofstream file(SCORE_FILE);
    for (int i = 0; i < std::min(5, (int)scores.size()); i++) {
        file << scores[i] << std::endl;
    }
}

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake Game");

    Texture tSnake, tFood, tObstacle;
    if (!tSnake.loadFromFile("images/blue.png") ||
        !tFood.loadFromFile("images/red.png") ||
        !tObstacle.loadFromFile("images/black.png")) {
        std::cerr << "Failed to load textures!" << std::endl;
        return -1;
    }

    Font font;
    if (!font.loadFromFile("Font/PlayfairDisplay-VariableFont_wght.ttf")) {
        std::cerr << "Could not load font!" << std::endl;
        return -1;
    }

    Sprite snakeSprite(tSnake);
    Sprite foodSprite(tFood);
    Sprite spriteObstacle(tObstacle);

    Text gameOverText("GAME OVER!", font, 40);
    gameOverText.setFillColor(Color::Black);
    gameOverText.setPosition(GRID_WIDTH * CELL_SIZE / 2 - 100, 20);

    Text scoreText("", font, 20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(5, 5);

    Text highScoresText("", font, 20);
    highScoresText.setFillColor(Color::Black);
    highScoresText.setPosition(GRID_WIDTH * CELL_SIZE / 2 - 100, 70);

    Clock clock;
    float timer = 0, delay = 0.2f;
    int lastDir = 2;

    f.x = 5; f.y = 5;
    s[0].x = GRID_WIDTH / 2;
    s[0].y = GRID_HEIGHT / 2;

    generateObstacles();

    std::vector<int> highScores = loadHighScores();

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
            if (e.type == Event::Closed)
                window.close();

        if (!isGameOver) {
            if (Keyboard::isKeyPressed(Keyboard::Up) && lastDir != 0) dir = 1;
            if (Keyboard::isKeyPressed(Keyboard::Down) && lastDir != 1) dir = 0;
            if (Keyboard::isKeyPressed(Keyboard::Right) && lastDir != 3) dir = 2;
            if (Keyboard::isKeyPressed(Keyboard::Left) && lastDir != 2) dir = 3;

            if (timer > delay) {
                timer = 0;
                updateSnake();
                lastDir = dir;
            }
        } else {
            if (std::find(highScores.begin(), highScores.end(), score) == highScores.end()) {
                highScores.push_back(score);
                std::sort(highScores.begin(), highScores.end(), std::greater<int>());
                saveHighScores(highScores);
            }
        }

        window.clear(isGameOver ? Color::White : Color(120, 200, 100));

        if (!isGameOver) {
            for (int i = 0; i < 2; ++i) {
                spriteObstacle.setPosition(obs[i].x * CELL_SIZE, obs[i].y * CELL_SIZE);
                window.draw(spriteObstacle);
            }

            for (int i = 0; i < num; i++) {
                snakeSprite.setPosition(s[i].x * CELL_SIZE, s[i].y * CELL_SIZE);
                window.draw(snakeSprite);
            }

            foodSprite.setPosition(f.x * CELL_SIZE, f.y * CELL_SIZE);
            window.draw(foodSprite);

            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
        } else {
            window.draw(gameOverText);
            std::string hs = "Score: " + std::to_string(score) + "\nTop 5 Scores:\n";
            for (int i = 0; i < std::min(5, (int)highScores.size()); i++) {
                hs += std::to_string(i + 1) + ". " + std::to_string(highScores[i]) + "\n";
            }
            highScoresText.setString(hs);
            window.draw(highScoresText);
        }

        window.display();
    }

    return 0;
}
