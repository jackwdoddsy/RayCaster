#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>

struct controller
{
    int isLeftPressed;
    int isRightPressed;
    int isUpPressed;
    int isDownPressed;
};

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

const int TARGET_FPS = 60;
const int FRAME_TIME = (1000/TARGET_FPS);

const int WIDTH = 1280;
const int HEIGHT = 720;

const int worldMap[9][9]=
{
    {1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1},
};

int init();

int quit();

int setFPS(int *fpsFrames, Uint32 *fpsLastTime);

int main(int argc, char *argv[])
{
    init();

    //Game variables
    double posX = 7, posY = 7;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66; 
	SDL_Event event;
	int running = 1;
    int isLeftPressed = false;
    int isRightPressed = false;
    int isUpPressed = false;
    int isDownPressed = false;
    Uint32 frameStart;
    Uint32 frameTime;
    Uint32 fpsLastTime = 0;
    int fpsFrames = 0;

	while(running)
    {
        setFPS(&fpsFrames, &fpsLastTime);

        frameStart = SDL_GetTicks();
        double frametime = 1.0/60.0;
        double moveSpeed = 5 * frametime;
        double rotSpeed = 3 * frametime;

        double oldDirX;
        double oldPlaneX;

        while(SDL_PollEvent(&event))
        {

			switch(event.type)
            {
				case SDL_QUIT:
					running = 0;
                    printf("Quit has been pressed\n");
					break;
				case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym){
                        case SDLK_UP:
                            isUpPressed = true;
                            break;
                        case SDLK_DOWN:
                            isDownPressed = true;
                            break;
                            case SDLK_LEFT:
                            isLeftPressed = true;                           
                            break;
                        case SDLK_RIGHT:
                            isRightPressed = true;                   
                            break;
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                     switch(event.key.keysym.sym){
                        case SDLK_UP:
                            isUpPressed = false;
                            break;
                        case SDLK_DOWN:
                            isDownPressed = false;
                            break;
                        case SDLK_LEFT:
                            isLeftPressed = false;                           
                            break;
                        case SDLK_RIGHT:
                            isRightPressed = false;                   
                            break;
                    }
                    break;
                }
				case SDL_MOUSEMOTION:
				{
					SDL_MouseMotionEvent *motion = &event.motion;
					//printf( "Mouse Moved\n""  State %d\n""  X:    %d\n""  Y:    %d\n""  xrel: %d\n""  yrel: %d\n", motion->state, motion->x, motion->y, motion->xrel, motion->yrel);
					break;
				}
			}
		}

        if(isUpPressed){
            if(worldMap[(int)(posX + dirX * moveSpeed)][(int)posY] == 0)
                posX += dirX * moveSpeed;
            if(worldMap[(int)posX][(int)(posY + dirY * moveSpeed)] == 0)
                posY += dirY * moveSpeed;
        }
        if(isDownPressed){
            if(worldMap[(int)(posX + dirX * moveSpeed)][(int)posY] == 0)
                posX -= dirX * moveSpeed;
            if(worldMap[(int)posX][(int)(posY - dirY * moveSpeed)] == 0)
                posY -= dirY * moveSpeed;
        }
        if(isLeftPressed){
            oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);

            oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
        if(isRightPressed){
            oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            
            oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }

    	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        for(int w = 0; w < WIDTH; w++){
            double cameraX = 2 * w / (double)WIDTH - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)posX;
            int mapY = (int)posY;

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
                sideDistX = (posX - mapX) * deltaDistX;
            }else{
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if(rayDirY < 0){
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }else{
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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

            if(side==0) perpWallDist = (sideDistX - deltaDistX);
            else        perpWallDist = (sideDistY - deltaDistY);

            int lineHeight = (int)(HEIGHT/perpWallDist);

            int drawStart = -lineHeight / 2 + HEIGHT / 2;
            if(drawStart < 0)drawStart = 0;
            int drawEnd = lineHeight / 2 + HEIGHT / 2;
            if(drawEnd >= HEIGHT)drawEnd = HEIGHT - 1;

            SDL_RenderDrawLine(renderer, w, drawStart, w, drawEnd);
        }

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < FRAME_TIME)
            SDL_Delay(FRAME_TIME - frameTime);
	}
    quit();
    exit(0);
}

int setFPS(int *fpsFrames, Uint32 *fpsLastTime){
    (*fpsFrames)++;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - *fpsLastTime >= 1000){
        char title[64];
        snprintf(title, sizeof(title), "Raycaster | FPS: %d", *fpsFrames);
        SDL_SetWindowTitle(window, title);
        *fpsFrames = 0;
        *fpsLastTime = currentTime;
    }
}

int quit(){
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

    printf("Quiting SDL.\n");
   
    SDL_Quit();
    
    printf("Quiting....\n");
	
}

int init(){
    printf("Initializing SDL.\n");
    
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)){ 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(1);
    }
    
    printf("SDL initialized.\n");

	printf("Create Window.\n");
	window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if(!window){
		printf("Could not create window: %s.\n", SDL_GetError());
		exit(1);
	}

	printf("Window Created\n");

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(!renderer){
		printf("Could not get renderer: %s.\n", SDL_GetError());
		exit(1);
	}
}
