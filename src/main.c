#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>

typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} Input;

typedef struct
{
    double x, y;
    double dirX, dirY;
    double planeX, planeY;
} Player;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
} Game;

const int TARGET_FPS = 120;
const double FRAME_TIME = (1000/TARGET_FPS);

const int WIDTH = 1280;
const int HEIGHT = 720;

const int worldMap[15][15] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,2,0,4,4,4,0,0,1},
    {1,0,0,0,0,0,0,2,0,0,0,4,0,0,1},
    {1,0,0,0,0,0,0,2,0,4,4,4,0,0,1},
    {1,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
    {1,2,0,0,2,2,2,2,2,2,2,2,2,0,1},
    {1,0,0,0,0,0,0,2,0,0,0,0,0,0,1},
    {1,0,5,5,5,0,0,2,0,6,6,0,2,0,1},
    {1,0,5,0,5,0,0,2,0,6,0,0,2,0,1},
    {1,0,5,5,5,0,0,2,0,6,6,0,2,0,1},
    {1,0,0,0,0,0,0,2,0,0,0,0,2,0,1},
    {1,0,2,2,2,2,2,2,2,2,2,0,2,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

void init(Game *game);

void quit(Game *game);

void setFPS(Game *game, int *fpsFrames, Uint32 *fpsLastTime);

void handleInput(Input *input, bool *running);

void updatePlayer(Player *p, Input *in, double moveSpeed, double rotSpeed);

void renderWorld(SDL_Renderer *renderer, Player *p);

int main()
{
    Game game;
    Input input = {0};
    Player player = {
        .x = 3.5, .y = 3.5,
        .dirX = -1, .dirY = 0,
        .planeX = 0, .planeY = 0.66
    };

    init(&game);

    game.running = true;
    Uint32 frameStart;
    Uint32 fpsLastTime = 0;
    int fpsFrames = 0;

    while(game.running){
        setFPS(&game, &fpsFrames, &fpsLastTime);

        frameStart = SDL_GetTicks();
        double frameTime = 1/(double)TARGET_FPS;

        handleInput(&input, &game.running);

        updatePlayer(&player, &input, 5 * frameTime, 3 * frameTime);

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        renderWorld(game.renderer, &player);
        
        SDL_RenderPresent(game.renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < FRAME_TIME)
            SDL_Delay(FRAME_TIME - frameTime);
    }

    quit(&game);

    return 0;
}

void setFPS(Game *game, int *fpsFrames, Uint32 *fpsLastTime){
    (*fpsFrames)++;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - *fpsLastTime >= 1000){
        char title[64];
        snprintf(title, sizeof(title), "Raycaster | FPS: %d", *fpsFrames);
        SDL_SetWindowTitle(game->window, title);
        *fpsFrames = 0;
        *fpsLastTime = currentTime;
    }
}

void quit(Game *game){
	if(game->renderer) SDL_DestroyRenderer(game->renderer);

	if(game->window) SDL_DestroyWindow(game->window);

    printf("Quiting SDL.\n");
   
    SDL_Quit();
    
    printf("Quiting....\n");
	
}

void init(Game *game){
    printf("Initializing SDL.\n");
    
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)){ 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(1);
    }
    
    printf("SDL initialized.\n");

	printf("Create Window.\n");
	game->window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if(!game->window){
		printf("Could not create window: %s.\n", SDL_GetError());
		exit(1);
	}

	printf("Window Created\n");

	game->renderer = SDL_CreateRenderer(game->window, -1, 0);

	if(!game->renderer){
		printf("Could not get renderer: %s.\n", SDL_GetError());
		exit(1);
	}
}

void handleInput(Input *input, bool *running){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                *running = false;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_UP) input->up = true;
                if(event.key.keysym.sym == SDLK_DOWN) input->down = true;
                if(event.key.keysym.sym == SDLK_LEFT) input->left = true;
                if(event.key.keysym.sym == SDLK_RIGHT) input->right = true;
                break;
            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_UP) input->up = false;
                if(event.key.keysym.sym == SDLK_DOWN) input->down = false;
                if(event.key.keysym.sym == SDLK_LEFT) input->left = false;
                if(event.key.keysym.sym == SDLK_RIGHT) input->right = false;
                break;
        }
    }
}

void updatePlayer(Player *p, Input *in, double moveSpeed, double rotSpeed){
    if(in->up) {
        if(worldMap[(int)(p->x + p->dirX * moveSpeed)][(int)p->y] == 0)
            p->x += p->dirX * moveSpeed;
        if(worldMap[(int)p->x][(int)(p->y + p->dirY * moveSpeed)] == 0)
            p->y += p->dirY * moveSpeed;
    }
    if(in->down) {
        if(worldMap[(int)(p->x + p->dirX * moveSpeed)][(int)p->y] == 0)
            p->x -= p->dirX * moveSpeed;
        if(worldMap[(int)p->x][(int)(p->y + p->dirY * moveSpeed)] == 0)
            p->y -= p->dirY * moveSpeed;
    }

    if(in->left || in->right){
        double sign = in->left ? 1 : -1;
        double oldDirX = p->dirX;
        p->dirX = p->dirX * cos(sign * rotSpeed) - p->dirY * sin(sign * rotSpeed);
        p->dirY = oldDirX  * sin(sign * rotSpeed) + p->dirY * cos(sign * rotSpeed);

        double oldPlaneX = p->planeX;
        p->planeX = p->planeX * cos(sign * rotSpeed) - p->planeY * sin(sign * rotSpeed);
        p->planeY = oldPlaneX  * sin(sign * rotSpeed) + p->planeY * cos(sign * rotSpeed);
    }
}

void renderWorld(SDL_Renderer *renderer, Player *p){
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);

    SDL_RenderFillRect(renderer, NULL);
    
    SDL_SetRenderDrawColor(renderer, 235, 235, 205, 255);
    SDL_Rect fillRect = {0, 0.5*HEIGHT, WIDTH, 0.5*HEIGHT};
    SDL_RenderFillRect(renderer, &fillRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    for(int x = 0; x < WIDTH; x++){
        double cameraX = 2 * x / (double)WIDTH - 1;
        double rayDirX = p->dirX + p->planeX * cameraX;
        double rayDirY = p->dirY + p->planeY * cameraX;
        
        int mapX = (int)p->x;
        int mapY = (int)p->y;

        double sideDistX;
        double sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0/rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0/rayDirY);
        double perpWallDist;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if(rayDirX < 0){
            stepX = -1;
            sideDistX = (p->x - mapX) * deltaDistX;
        }else{
            stepX = 1;
            sideDistX = (mapX + 1.0 - p->x) * deltaDistX;
        }
        if(rayDirY < 0){
            stepY = -1;
            sideDistY = (p->y - mapY) * deltaDistY;
        }else{
            stepY = 1;
            sideDistY = (mapY + 1.0 - p->y) * deltaDistY;
        }

        while(hit==0)
        {
            if(sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if(worldMap[mapX][mapY] > 0) hit = 1;
        }

        int r;
        int g;
        int b;
        
        switch(worldMap[mapX][mapY]){
            case 1:
                r = 100;
                g = 100;
                b = 100;
                break;
            case 2:
                r = 255;
                g = 255;
                b = 0;
                break;
            case 3:
                r = 255;
                g = 0;
                b = 255;
                break;
            case 4:
                r = 255;
                g = 0;
                b = 0;
                break;
            case 5:
                r = 0;
                g = 255;
                b = 255;
                break;
            case 6:
                r = 0;
                g = 255;
                b = 0;
                break;
            case 7:
                r = 0;
                g = 0;
                b = 255;
                break;
            default:
                r = 0;
                g = 0;
                b = 0;
                break;
        }

        if(side==0) perpWallDist = (sideDistX - deltaDistX);
        else        perpWallDist = (sideDistY - deltaDistY);

        int lineHeight = (int)(HEIGHT/perpWallDist);

        int drawStart = -lineHeight / 2 + HEIGHT / 2;
        if(drawStart < 0)drawStart = 0;
        int drawEnd = lineHeight / 2 + HEIGHT / 2;
        if(drawEnd >= HEIGHT)drawEnd = HEIGHT - 1;

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
    }
}
