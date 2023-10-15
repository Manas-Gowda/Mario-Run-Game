#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2
#define STATUS_STATE_WIN 3
#define STATUS_MENU -1

#define NUM_STARS 100
#define NUM_LEDGES 120


typedef struct {
    float x,y;
    float dx,dy; //change in the man's position
    short life;
    char *name;

    int onLedge, isdead;
    int animFrame, facingLeft, slowingDown;
}Man;

typedef struct{
    int x,y, baseX, baseY, mode;
    float phase;
}Star;

typedef struct{
    int x,y,w,h;
}Ledge;

typedef struct {
    //player
    Man man;

    float scrollx;

    Star stars[NUM_STARS];
    Ledge ledges[NUM_LEDGES];

    //images
    SDL_Texture *star;
    SDL_Texture *manFrames[2];
    SDL_Texture *brick[2];
    SDL_Texture *fire, *bg;
    SDL_Texture *bg2, *img1, *playImg, *nameImg;
    SDL_Rect playBtn;

    //text
    SDL_Texture *label, *progress;
    int labelW,labelH;

    //fonts
    TTF_Font *font, *font2;

    int time, deathCountdown;
    int statusState;

    //sounds
    int musicChannel;
    Mix_Chunk *bgMusic, *jumpSound, *landSound, *dieSound;

    SDL_Renderer *renderer;
}GameState;

#endif // MAIN_H_INCLUDED
