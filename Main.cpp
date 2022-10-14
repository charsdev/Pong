#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./constants.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle1, paddle2;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;
        if (event.key.keysym.sym == SDLK_UP)
            paddle1.vel_y = -400;
        if (event.key.keysym.sym == SDLK_DOWN)
            paddle1.vel_y = +400;

        if (event.key.keysym.sym == SDLK_w)
            paddle2.vel_y = -400;
        if (event.key.keysym.sym == SDLK_s)
            paddle2.vel_y = +400;

        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP)
            paddle1.vel_y = 0;
        if (event.key.keysym.sym == SDLK_DOWN)
            paddle1.vel_y = 0;
        if (event.key.keysym.sym == SDLK_w)
            paddle2.vel_y = 0;
        if (event.key.keysym.sym == SDLK_s)
            paddle2.vel_y = 0;

        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // Initialize values for the the ball object
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    // Initialize the values for the paddle object
    paddle1.width = 20;
    paddle1.height = 100;
    paddle1.y = (WINDOW_HEIGHT / 2) - paddle1.height / 2;
    paddle1.x = BOX_WIDTH;
    paddle1.vel_x = 0;
    paddle1.vel_y = 0;

    // Initialize the values for the paddle object
    paddle2.width = 20;
    paddle2.height = 100;
    paddle2.y = (WINDOW_HEIGHT / 2) - paddle2.height / 2;
    paddle2.x = WINDOW_WIDTH - 20 - BOX_WIDTH ;
    paddle2.vel_x = 0;
    paddle2.vel_y = 0;
}

void update(void) {
    // Calculate how much we have to wait until we reach the target frame time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame
    last_frame_time = SDL_GetTicks();

    // update ball and paddle position
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    paddle1.x += paddle1.vel_x * delta_time;
    paddle1.y += paddle1.vel_y * delta_time;

    paddle2.x += paddle2.vel_x * delta_time;
    paddle2.y += paddle2.vel_y * delta_time;

    //// Check for ball collision with the walls
    if (ball.y <= 0 || ball.y + ball.height >= WINDOW_HEIGHT)
        ball.vel_y = -ball.vel_y;
    
    if (ball.x < 0)
        ball.vel_x = -ball.vel_x;

    // Check for ball collision with the paddle 1
    if (ball.y + ball.height >= paddle1.y && ball.y + ball.height >= paddle1.y && ball.y <= paddle1.y + paddle1.height)
        ball.vel_x = -ball.vel_x;

    // Check for ball collision with the paddle 2
    if (ball.y + ball.height >= paddle2.y && ball.y + ball.height >= paddle2.y && ball.y <= paddle2.y + paddle2.height)
        ball.vel_x = -ball.vel_x;


    // Prevent paddle from moving outside the boundaries of the window
    if (paddle1.y <= 0)
        paddle1.y = 0;
    if (paddle1.y >= WINDOW_HEIGHT - paddle1.height)
        paddle1.y = WINDOW_HEIGHT - paddle1.height;

    if (paddle2.y <= 0)
        paddle2.y = 0;
    if (paddle2.y >= WINDOW_HEIGHT - paddle2.height)
        paddle2.y = WINDOW_HEIGHT - paddle2.height;

    // Check for game over
    std::cout << ball.x + ball.width << std::endl;
    if (ball.x + ball.width > WINDOW_WIDTH ) {
        ball.y = WINDOW_HEIGHT / 2;
        ball.x = WINDOW_WIDTH / 2;
        ball.vel_x = -ball.vel_x;
        ball.vel_y = -ball.vel_y;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect1 = {
        (int)paddle1.x,
        (int)paddle1.y,
        (int)paddle1.width,
        (int)paddle1.height
    };

    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect2 = {
        (int)paddle2.x,
        (int)paddle2.y,
        (int)paddle2.width,
        (int)paddle2.height
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect1);
    SDL_RenderFillRect(renderer, &paddle_rect2);

    SDL_Rect mRect = { WINDOW_WIDTH / 2 - BOX_WIDTH / 2, BOX_WIDTH, BOX_WIDTH, WINDOW_HEIGHT };

    for (float y = static_cast<float>(mRect.y); y < mRect.h; y += (1.93f * mRect.w)) {
        SDL_Rect rect { mRect.x, static_cast<int>(y), mRect.w, mRect.w };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[]) {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
