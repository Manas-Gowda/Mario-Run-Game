#include<stdio.h>
#include"main.h"

void draw_game_menu(GameState *game)
{
    SDL_Rect bg2rect = {0,0,640,480};
    SDL_RenderCopy(game->renderer, game->bg2, NULL,&bg2rect);

    SDL_Rect namerect = {320-124,8,249,179};
    SDL_RenderCopy(game->renderer, game->nameImg, NULL,&namerect);

    SDL_Rect img1rect = {10,160,280,251};
    SDL_RenderCopy(game->renderer, game->img1, NULL,&img1rect);

    //SDL_Rect playrect = {320-78,320,155,60};
    SDL_RenderCopy(game->renderer, game->playImg, NULL, &game->playBtn);
}

void init_status_lives(GameState *game)
{
    char str[10] = "";
    sprintf(str,"x %u",game->man.life);

    SDL_Color white = {255,255,255,255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(game->font,str,white);
    game->labelW = textSurface->w;
    game->labelH = textSurface->h;

    game->label = SDL_CreateTextureFromSurface(game->renderer,textSurface);
    SDL_FreeSurface(textSurface);
}

void draw_status_lives(GameState *game)
{
    SDL_SetRenderDrawColor(game->renderer,0,0,0,255);
    SDL_RenderClear(game->renderer);

    SDL_Rect rect = {320-70,240-24,48,48};
    SDL_RenderCopyEx(game->renderer, game->manFrames[0], NULL,&rect,0,NULL,0);


    SDL_SetRenderDrawColor(game->renderer,255,255,255,255);
    SDL_Rect textrect = {320-20,240-game->labelH/2,game->labelW,game->labelH};
    SDL_RenderCopy(game->renderer,game->label,NULL,&textrect);

    //SDL_Delay(1000);
}

void init_game_over(GameState *game)
{
    char str[10] = "Game Over";

    SDL_Color white = {255,255,255,255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(game->font,str,white);
    game->labelW = textSurface->w;
    game->labelH = textSurface->h;

    game->label = SDL_CreateTextureFromSurface(game->renderer,textSurface);
    SDL_FreeSurface(textSurface);
}

void draw_game_result(GameState *game)
{
    SDL_SetRenderDrawColor(game->renderer,0,0,0,255);
    SDL_RenderClear(game->renderer);

    SDL_SetRenderDrawColor(game->renderer,255,255,255,255);
    SDL_Rect textrect = {320-game->labelW/2,240-game->labelH/2,game->labelW,game->labelH};
    SDL_RenderCopy(game->renderer,game->label,NULL,&textrect);
}

void init_game_win(GameState *game)
{
    char str[10] = "You Win!";

    SDL_Color white = {255,255,255,255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(game->font,str,white);
    game->labelW = textSurface->w;
    game->labelH = textSurface->h;

    game->label = SDL_CreateTextureFromSurface(game->renderer,textSurface);
    SDL_FreeSurface(textSurface);
}

void game_progress(GameState *game)
{
    char str[10] = "";
    sprintf(str,"%d %%",(int)(game->man.x*100)/38400);

    SDL_Color white = {255,255,255,255};
    SDL_Surface *textSurf = TTF_RenderText_Blended(game->font2,str,white);
    game->labelW = textSurf->w;
    game->labelH = textSurf->h;
    game->progress = SDL_CreateTextureFromSurface(game->renderer,textSurf);
    SDL_FreeSurface(textSurf);

    SDL_SetRenderDrawColor(game->renderer,255,255,255,255);
    SDL_Rect textrect = {320-game->labelW/2,24-game->labelH/2,game->labelW,game->labelH};
    SDL_RenderCopy(game->renderer,game->progress,NULL,&textrect);
}
