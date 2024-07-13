#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#include <cmath>
#include <iostream>
#include <vector>

const int WINDOW_WIDTH = 1700;
const int WINDOW_HEIGHT = 900;
const int BALL_SIZE = 190;
const float BALL_SPEED = 2.5f;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

enum Color { RED, GREEN };

struct Ball {
    float x, y;
    float vx, vy;
    Color color;

    Ball(float posX, float posY, float velX, float velY, Color col)
        : x(posX), y(posY), vx(velX), vy(velY), color(col) {}
};

void handleBallCollision(Ball &ball) {
    // Check for collision with window borders
    if (ball.x <= 0 || ball.x + BALL_SIZE >= WINDOW_WIDTH) {
        ball.vx = -ball.vx;
    }
    if (ball.y <= 0 || ball.y + BALL_SIZE >= WINDOW_HEIGHT) {
        ball.vy = -ball.vy;
    }
}

bool checkCollision(const Ball &a, const Ball &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < BALL_SIZE;
}

void removeCollidingBalls(std::vector<Ball> &balls) {
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (checkCollision(balls[i], balls[j]) &&
                balls[i].color != balls[j].color) {
                balls.erase(balls.begin() + j);
                balls.erase(balls.begin() + i);
                return;
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Shooting Ball Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    std::vector<Ball> balls;
    Color nextColor = RED;

    Uint32 frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Shoot ball with alternating color
                balls.push_back(
                    Ball(mouseX, mouseY, BALL_SPEED, BALL_SPEED, nextColor));
                nextColor = (nextColor == RED) ? GREEN : RED;
            }
        }

        // Update balls
        for (auto &ball : balls) {
            ball.x += ball.vx;
            ball.y += ball.vy;

            handleBallCollision(ball);
        }

        // Remove colliding balls of different colors
        removeCollidingBalls(balls);

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
        SDL_RenderClear(renderer);

        for (const auto &ball : balls) {
            if (ball.color == RED) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red ball
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green ball
            }
            SDL_Rect rect = {static_cast<int>(ball.x), static_cast<int>(ball.y),
                             BALL_SIZE, BALL_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
