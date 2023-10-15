#include <stdio.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "status.c"

#define GRAVITY 0.35f


void initStars(GameState *game){
    for (int i=0; i<NUM_STARS;i++){
        game->stars[i].baseX = 320+rand()%38400;
        game->stars[i].baseY = rand()%(480-52);
        game->stars[i].mode = rand()%2;
        //to make the stars oscillate at different phases
        game->stars[i].phase = 2*3.14*(rand()%360)/360.0f;    //gives random float values from 0 to 2pi
        //game->stars[i].phase = rand()%(2*3.1416f);
    }
}

void loadGame(GameState *game){
    game->man.x = 100;
    game->man.y = 240-40;
    game->man.dy = 0;
    game->man.dx = 0;
    game->man.onLedge = 0;
    game->man.animFrame = 0;
    game->man.facingLeft = 1;
    game->man.slowingDown = 0;
    game->man.life = 3;
    game->man.isdead = 0;
    game->time = 0;
    game->statusState = STATUS_MENU;
    game->deathCountdown = -1;

    SDL_Surface *surface = NULL;

    //load images and create rendering textures from them
    surface = IMG_Load("star1.png");
    if(surface == NULL){
        printf("cannot find star.png!\n");
        SDL_Quit();
        exit(1);
    }

    game->star = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("man_lta.png");
    if(surface == NULL){
        printf("cannot find man_lta.png!\n");
        SDL_Quit();
        exit(1);
    }

    game->manFrames[0] = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("man_ltb.png");
    if(surface == NULL){
        printf("cannot find man_ltb.png!\n");
        SDL_Quit();
        exit(1);
    }

    game->manFrames[1] = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("brick1.png");
    game->brick[0] = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("brick2.png");
    game->brick[1] = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("fire.png");
    game->fire = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("bg1.png");
    game->bg = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("bg2.jpg");
    game->bg2 = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("icon2(2).png");
    game->img1 = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("play-button.png");
    game->playImg = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("MiniMario_text.png");
    game->nameImg = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);

    //load fonts
    game->font = TTF_OpenFont("Crazy-Pixel.ttf",48);
    if(!game->font){
        printf("cannot find font file!\n");
        SDL_Quit();
        exit(1);
    }
    game->label = NULL;
    init_status_lives(game);

    game->font2 = TTF_OpenFont("Crazy-Pixel.ttf",20);

    //load music
    game->bgMusic = Mix_LoadWAV("music.wav");
    if(game->bgMusic !=  NULL)
        {Mix_VolumeChunk(game->bgMusic, 35); printf("hi\n");  }  //volume -> 0 to 128

    game->dieSound = Mix_LoadWAV("die.wav");
    Mix_VolumeChunk(game->dieSound, 30);
    game->jumpSound = Mix_LoadWAV("jump.wav");
    game->landSound = Mix_LoadWAV("land.wav");

//    for (int i=0;i<NUM_STARS;i++){
//        game->stars[i].x = 320+rand()%38400; //random number b/w 320 to 38399
//        game->stars[i].y = rand()%480;    //random number b/w 0 to 479
//    }

    initStars(game);

    //init ledges
    for (int i=0; i<NUM_LEDGES-20; i++){
        game->ledges[i].w = 256;
        game->ledges[i].h = 64;
        game->ledges[i].x = i*(256+128);
        if(i==0)
            game->ledges[i].y = 400;
        else
            game->ledges[i].y = 415-rand()%210;
    }
    for (int i=NUM_LEDGES-20; i<NUM_LEDGES; i++){
        game->ledges[i].w = 256;
        game->ledges[i].h = 64;
        game->ledges[i].x = 640+rand()%38400;
        if(i%2==0)
            game->ledges[i].y = 200;
        else
            game->ledges[i].y = 416;
    }
//    game->ledges[99].x = 350;
//    game->ledges[99].y = 190;
//
//    game->ledges[98].x = 350;
//    game->ledges[98].y = 350;
}

void process(GameState *game){
    //add time
    game->time++;

    if(game->statusState == STATUS_MENU){
        game->playBtn.x = 320-78-sin(game->time*0.1)*2;
        game->playBtn.y = 320-sin(game->time*0.1)*2;
        game->playBtn.w = 155+sin(game->time*0.1)*4;
        game->playBtn.h = 60+sin(game->time*0.1)*4;
    }

//    int startTime;
    if(game->statusState == STATUS_STATE_LIVES){

    //if(game->time > 120){
        //game->statusState = STATUS_STATE_GAME;
        //play the music and get the music channel (for stopping it)
        game->musicChannel = Mix_PlayChannel(1, game->bgMusic, -1);// -1 --> play on any available sound channel; -1 --> loop forever
        // printf("hello");
    }

    if(game->statusState == STATUS_STATE_GAME){
        if(!game->man.isdead){
            //man movement
            Man *man = &game->man;
            man->y += man->dy;
            man->x += man->dx;

            //change the animation frame only when the man is moving left or right and when he is on the brick and he is not slowing down
            if(man->dx != 0 && man->onLedge && !man->slowingDown){
                if(game->time %8 == 0){    //change the frame every 8fps (once in 8 iterations of the main event loop)
                    if(man->animFrame == 0) man->animFrame =1;
                    else man->animFrame =0;
                }
            }

            if(man->x > 38400){
                init_game_win(game);
                game->statusState = STATUS_STATE_WIN;
            }

            man->dy += GRAVITY;

            //Star movement
            for (int i=0;i<NUM_STARS;i++){
                game->stars[i].x = game->stars[i].baseX;
                game->stars[i].y = game->stars[i].baseY;
                if(game->stars[i].mode == 0)
                    game->stars[i].x = game->stars[i].baseX+sin(game->stars[i].phase+game->time*0.06f)*75;
                else
                    game->stars[i].y = game->stars[i].baseY+cos(game->stars[i].phase+game->time*0.06f)*75;
            }
        //printf("%lf -- %lf -- %lf --> %lf\n",(game->time*0.06f), game->stars[0].phase, game->stars[0].phase+game->time*0.06f, sin(game->time*0.06f)*75);
        }
        else if(game->man.isdead && game->deathCountdown < 0)
            game->deathCountdown = 120;
        else if(game->deathCountdown >= 0){
            game->deathCountdown--;
            if(game->deathCountdown<0){
                game->man.life--;
                if(game->man.life>0){
                    init_status_lives(game);
                    game->statusState = STATUS_STATE_LIVES;

                    //reset
                    game->musicChannel = Mix_PlayChannel(1, game->bgMusic, -1);
                    game->time = 0;
                    game->man.isdead = 0;
                    game->man.x = 100;
                    game->man.y = 240-40;
                    game->man.dy = 0;
                    game->man.dx = 0;
                    game->man.onLedge = 0;
                }
                else{   //all lives over
                    init_game_over(game);
                    game->statusState = STATUS_STATE_GAMEOVER;
                    game->time = 0;
                }
            }
        }
    }
    game->scrollx = -game->man.x+320/2;
    if (game->scrollx > 0)
        game->scrollx = 0;
    if (game->scrollx < -38400+640)
        game->scrollx = -38400+640;
}

//useful utility function to see if two rectangles are colliding at all
int collide(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
    return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}

void collisionDetect(GameState *game){

    for (int i=0;i<NUM_STARS;i++){    //checking collision with any stars
        if(collide(game->man.x, game->man.y, game->stars[i].x, game->stars[i].y, 48, 48,54,52)){
            if(!game->man.isdead){
                game->man.isdead = 1;
                Mix_HaltChannel(game->musicChannel);
                Mix_PlayChannel(3,game->dieSound,0);
            }
            break;
        }
    }

    if (game->man.y > 480)
        game->man.isdead = 1;

    //check for collision with any ledges (brick blocks)
    for (int i=0;i<NUM_LEDGES;i++){
        float mw = 38, mh = 49;
        float mx = game->man.x, my = game->man.y;
        float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;


        if (mx+mw/2 > bx && mx+mw/2 < bx+bw){
            //are we bumping our head?
            if(my < by+bh && my > by && game->man.dy < 0){
                game->man.y = by+bh; //correct y
                my = by+bh;
                game->man.dy = 0;   //bumped our head, stop any jump velocity
                //game->man.onLedge = 1;
            }
        }
        if (mx+mw > bx && mx < bx+bw){
            //are we landing on the brick
            if (my+mh > by && my < by && game->man.dy > 0){
                game->man.y = by-mh; //correct y
                my = by-mh;
                game->man.dy = 0;   //landed on this ledge, stop any jump velocity
                if (!game->man.onLedge){
                    Mix_PlayChannel(2,game->landSound,0);
                    game->man.onLedge = 1;
                }
            }
        }

        if (my+mh > by && my < by+bh){
            //rubbing against the right edge
            if (mx < bx+bw && mx+mw > bx+bw && game->man.dx < 0){
                //correct x
                game->man.x = bx+bw;
                mx = bx+bw;

                game->man.dx = 0;
            }
            //rubbing against the left edge
            else if (mx+mw > bx && mx < bx && game->man.dx > 0){
                game->man.x = bx-mw; //correct x
                mx = bx-mw;

                game->man.dx = 0;
            }
        }
    }
}

int processeEvents(SDL_Window *window,GameState *game){
    SDL_Event event;
    int done =0;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_WINDOWEVENT_CLOSE:{
                if (window){
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done=1;
                }
            }break;
            case SDL_KEYDOWN:{
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        done=1;
                        break;
//                    case SDLK_RIGHT:
//                        man->x +=10;
//                        break;
//                    case SDLK_LEFT:
//                        man->x -=10;
//                        break;
                    case SDLK_UP:
                        //printf("%g\n",game->man.dy);
                        if(game->man.onLedge){    //if man not in air
                            //printf("up\n");
                            game->man.dy = -7.5;
                            game->man.onLedge = 0;
                            Mix_PlayChannel(0, game->jumpSound, 0);     //do not loop
                        }break;
                }
            }break;

            case SDL_MOUSEBUTTONDOWN:
                if(game->statusState == STATUS_MENU &&
                   event.button.button == SDL_BUTTON_LEFT &&
                   event.button.x >= game->playBtn.x &&
                   event.button.x <= (game->playBtn.x + game->playBtn.w) &&
                   event.button.y >= game->playBtn.y &&
                   event.button.y <= (game->playBtn.y + game->playBtn.h)){

                        game->statusState = STATUS_STATE_LIVES;
                        Mix_PlayChannel(0, game->landSound, 0);
                   }
                break;

            case SDL_QUIT:    //quit out of the game
                done = 1;
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);    //returns a list
    //more jumping when the up arrow is not released
    if (state[SDL_SCANCODE_UP]){
        game->man.dy -= 0.2f;}

    //walking
    if (state[SDL_SCANCODE_LEFT]){
        game->man.dx -= 0.5;    //man speeds up slowly
        if(game->man.dx < -6) game->man.dx = -6;
        game->man.facingLeft = 1;
        game->man.slowingDown = 0;
    }
    else if (state[SDL_SCANCODE_RIGHT]){
        game->man.dx += 0.5;
        if(game->man.dx > 6) game->man.dx = 6;
        game->man.facingLeft = 0;    //flip the animation frame if man is walking right
        game->man.slowingDown = 0;
    }
//    if (state[SDL_SCANCODE_UP])
//        game->man.y -= 5;
//    if (state[SDL_SCANCODE_DOWN])
//        game->man.y += 5;
    else{
        game->man.animFrame = 0;    //makes the man stop at the right animation frame
        game->man.slowingDown = 1;  //not to change the animation frame when man is slowing down
        game->man.dx *= 0.75f;      //friction to slow down and stop man
        if (fabsf(game->man.dx) < 0.01f) game->man.dx = 0;
    }

    if(game->statusState == STATUS_STATE_GAMEOVER || game->statusState == STATUS_STATE_WIN){
        SDL_Delay(2000);
        done = 1;
    }
    return done;
}

void doRender(SDL_Renderer *renderer, GameState *game){

    if(game->statusState == STATUS_MENU)
        draw_game_menu(game);

    else if(game->statusState == STATUS_STATE_LIVES){
        draw_status_lives(game);
        SDL_RenderPresent(renderer);
        SDL_Delay(1500);
        game->statusState = STATUS_STATE_GAME;
    }

    else if (game->statusState == STATUS_STATE_GAMEOVER || game->statusState == STATUS_STATE_WIN)
        draw_game_result(game);

    else if(game->statusState == STATUS_STATE_GAME)
    {
        SDL_SetRenderDrawColor(renderer,85,155,59,176);   //set drawing color to green //r,g,b,alpha //rgba(115,171,59,253)
        SDL_RenderClear(renderer);      //clear the screen (to green)

        SDL_Rect brect = {(game->scrollx*(951-640))/(38400-640),0,951,480};
        SDL_RenderCopy(renderer,game->bg,NULL,&brect);
        //printf("%lf -- %lf --> %lf\n",game->man.x, game->scrollx, (game->scrollx*(951-640))/(38400-640));


        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_Rect prect = {120,15,400,20};
        SDL_RenderDrawRect(renderer, &prect);
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        SDL_Rect progressRect = {121, 16, (game->scrollx/38400)*-400, 18};
        SDL_RenderFillRect(renderer, &progressRect);
        game_progress(game);

        for (int i=0;i<NUM_LEDGES-20;i++){    //draw the ledges
            SDL_Rect ledgeRect = {game->scrollx+game->ledges[i].x,game->ledges[i].y,game->ledges[i].w,game->ledges[i].h};
            SDL_RenderCopy(renderer,game->brick[0],NULL,&ledgeRect);     //draws sdl texture which we pass onto the screen
        }
        for (int i=NUM_LEDGES-20; i<NUM_LEDGES;i++){    //draw the ledges
            SDL_Rect ledgeRect = {game->scrollx+game->ledges[i].x,game->ledges[i].y,game->ledges[i].w,game->ledges[i].h};
            SDL_RenderCopy(renderer,game->brick[1],NULL,&ledgeRect);     //draws sdl texture which we pass onto the screen
        }

        SDL_Rect rect = {game->scrollx+game->man.x,game->man.y,38,49};    //draw a rectangle at man's position //x,y,width,height
        SDL_RenderCopyEx(renderer, game->manFrames[game->man.animFrame], NULL,&rect,0,NULL,(game->man.facingLeft == 0));

        if (game->man.isdead){      //draw fire
            SDL_Rect rect = {game->scrollx+game->man.x+20-38/2,game->man.y-24-83/2,38,83};
            SDL_RenderCopyEx(renderer, game->fire, NULL,&rect,0,NULL,(game->time%20<10));
        }

        for (int i=0;i<NUM_STARS;i++){
            SDL_Rect starRect = {game->scrollx+game->stars[i].x,game->stars[i].y,54,52};    //draw the star image
            SDL_RenderCopy(renderer,game->star,NULL,&starRect);     //draws sdl texture which we pass onto the screen
        }
    }

    SDL_RenderPresent(renderer);    //we are done drawing, "present" or show to the screen what we've drawn
}

int main(int argc, char *argv[]){

    GameState gameState;
    SDL_Window *window;
    SDL_Renderer *renderer;
    srand(time(NULL));  //initializes system to use random numbers which vary with time

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);    //initialize SDL
    window = SDL_CreateWindow("Mini Mario",
                              SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,  //initial x and y position
                              640,480,
                              0);   //flags

    SDL_Surface *icon = IMG_Load("icon2.png");
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);//these flags are used for hardware acceleration and present is synchronized with the refresh rate
    gameState.renderer = renderer;
    TTF_Init(); //initialize font system

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);

    loadGame(&gameState);
    int done=0;

    while(!done){   //event loop

        done = processeEvents(window,&gameState); //check for events
        process(&gameState);
        collisionDetect(&gameState);

        doRender(renderer,&gameState);     //render display

        //SDL_Delay(10); //don't burn up the cpu
    }

    //shutdown game and unload all memory
    SDL_DestroyTexture(gameState.star);
    SDL_DestroyTexture(gameState.manFrames[0]);
    SDL_DestroyTexture(gameState.manFrames[1]);
    SDL_DestroyTexture(gameState.brick[0]);
    SDL_DestroyTexture(gameState.brick[1]);
    SDL_DestroyTexture(gameState.bg);
    SDL_DestroyTexture(gameState.bg2);
    SDL_DestroyTexture(gameState.img1);
    SDL_DestroyTexture(gameState.nameImg);
    SDL_DestroyTexture(gameState.playImg);
    if(gameState.label != NULL)
        SDL_DestroyTexture(gameState.label);
    TTF_CloseFont(gameState.font);
    if(gameState.progress != NULL)
        SDL_DestroyTexture(gameState.progress);
    TTF_CloseFont(gameState.font2);

    Mix_FreeChunk(gameState.bgMusic);
    Mix_FreeChunk(gameState.dieSound);
    Mix_FreeChunk(gameState.jumpSound);
    Mix_FreeChunk(gameState.landSound);

    //SDL_Delay(2000); //msec
    //close and destroy window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    TTF_Quit();
    SDL_Quit();     //clean up

    return 0;
}
