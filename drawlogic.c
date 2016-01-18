#include "drawlogic.h"
#include "gamelogic.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

struct DrawConfig {
    SDL_Renderer* renderer;
    SDL_Window* window;

    SDL_Texture* bird_texture;
    SDL_Texture* background_texture;
    SDL_Texture* pipe_texture;
    SDL_Texture* pipe_top_texture;

    TTF_Font* game_font;

    int window_width;
    int window_height;

};

DrawConfig *create_DrawConfig(){
    DrawConfig* config = malloc(sizeof(DrawConfig));

    config->window_width  = 500;
    config->window_height = 500;
    

    /*
     * Initialize SDL and related libraries.
     */
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();


    /*
     * create and initialize SDL resources.
     */
    SDL_CreateWindowAndRenderer(config->window_width,config->window_height,0,&(config->window),&(config->renderer));

    /*
     * Set the windows title
     */
    SDL_SetWindowTitle(config->window,"Flappy Bird");

    /*
     * Load image files using SDL_image.
     */
    config->bird_texture = IMG_LoadTexture(config->renderer,"resources/images/bird.png");
    config->background_texture = IMG_LoadTexture(config->renderer,"resources/images/background.png");
    config->pipe_texture = IMG_LoadTexture(config->renderer,"resources/images/pipe.png");
    config->pipe_top_texture = IMG_LoadTexture(config->renderer,"resources/images/pipe-top.png");

    /*
     * Load font using TTF_Font
     */
    config->game_font = TTF_OpenFont("resources/fonts/VeraMono.ttf",24);

    return config;
}

void destroy_DrawConfig(DrawConfig* config){
    /*
     * Destroy SDL resources and quit SDL + related libraries.
     */
    SDL_DestroyWindow(config->window);
    SDL_DestroyRenderer(config->renderer);
    SDL_DestroyTexture(config->bird_texture);
    SDL_DestroyTexture(config->background_texture);
    SDL_DestroyTexture(config->pipe_texture);
    SDL_DestroyTexture(config->pipe_top_texture);
    TTF_CloseFont(config->game_font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    free(config);
}

void render_background(GameData* data, DrawConfig* config){
    SDL_Rect source_rect = {
        0,
        0,
        config->window_width,
        config->window_height
    };

    SDL_RenderCopy(config->renderer,config->background_texture,&source_rect,NULL);
}

void render_all_pipes(GameData* data, DrawConfig* config){
    SDL_SetRenderDrawColor(config->renderer,0x00,0x00,0xFF,0xFF); //blue
    for(int i=0; i < get_num_pipes(data); i++){
        Pipe pipe = get_pipe(data,i);
        render_pipe(data,config,&pipe);
    }
}

void render_pipe(GameData* data, DrawConfig* config, Pipe* pipe){

    /*
      * Query pipe textures for their height and width;
     */
    int pipe_top_texture_h, pipe_top_texture_w;
    SDL_QueryTexture(config->pipe_top_texture, NULL, NULL, &pipe_top_texture_w, &pipe_top_texture_h);

    int pipe_texture_h, pipe_texture_w;
    SDL_QueryTexture(config->pipe_texture, NULL, NULL, &pipe_texture_w, &pipe_texture_h);

    /*
     * Render upper pipe
     */
    int i;
    for(i = 0; i < scale_y_to_userspace(data, config, pipe->top_end) - pipe_top_texture_h; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(data,config,pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect);                              
    }
    SDL_Rect top_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data,config,pipe->top_end) - pipe_top_texture_h,
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    SDL_RenderCopyEx(config->renderer,config->pipe_top_texture,NULL,&top_cap_rect,0,NULL,SDL_FLIP_VERTICAL);


    /*
     * Render lower pipe
     */
    SDL_Rect low_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data, config, pipe->bottom_start),
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    SDL_RenderCopy(config->renderer,config->pipe_top_texture,NULL,&low_cap_rect);
    for(i = scale_y_to_userspace(data, config, pipe->bottom_start) + pipe_top_texture_h; i < config->window_height ; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(data,config,pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect);
    }
}

void render_score(GameData* data, DrawConfig* config){
    SDL_Color color={0,0,0};

    char score_str[9];
    sprintf(score_str, "%d", get_score(data));

    SDL_Surface *text_surface;
    text_surface = TTF_RenderText_Solid(config->game_font,score_str,color);

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(config->renderer,text_surface);

    int text_width,text_height;
    SDL_QueryTexture(text_texture,NULL,NULL,&text_width,&text_height);

    SDL_Rect dest_rect = {
        (config->window_width - text_width) / 2,
        config->window_height/10,
        text_width,
        text_height
    };

    SDL_RenderCopy(config->renderer,text_texture,NULL,&dest_rect);

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}


void render_bird(GameData* data, DrawConfig* config){

    /*
     * Query Bird image for it's width and heignt
     */
    int bird_w,bird_h;
    SDL_QueryTexture(config->bird_texture, NULL, NULL, &bird_w, &bird_h);

    /*
     * Specifiy where in the window the bird will be rendered.
     */
    SDL_Rect dest_rect = {
        scale_x_to_userspace(data,config,get_bird_x(data)) - (bird_w / 2),
        scale_y_to_userspace(data,config,get_bird_y(data)) - (bird_h / 2),
        bird_w,
        bird_h
    };

    SDL_RenderCopy(config->renderer,config->bird_texture,NULL,&dest_rect);
}

void render_gameover_message(GameData* data, DrawConfig* config){
    SDL_Color color={0,0,0};

    char score_str[35];
    sprintf(score_str, "You lost with a score of %d", get_score(data));


    SDL_Surface *score_surface;
    score_surface = TTF_RenderText_Solid(config->game_font,score_str,color);

    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(config->renderer,score_surface);

    int score_width,score_height;
    SDL_QueryTexture(score_texture,NULL,NULL,&score_width,&score_height);

    SDL_Rect score_rect = {
        (config->window_width - score_width) / 2,
        config->window_height/4,
        score_width,
        score_height
    };

    SDL_RenderCopy(config->renderer,score_texture,NULL,&score_rect);

    SDL_DestroyTexture(score_texture);
    SDL_FreeSurface(score_surface);


    SDL_Surface *again_surface;
    again_surface = TTF_RenderText_Solid(config->game_font,"Press Space to Play Again",color);

    SDL_Texture *again_texture = SDL_CreateTextureFromSurface(config->renderer,again_surface);

    int again_width,again_height;
    SDL_QueryTexture(again_texture,NULL,NULL,&again_width,&again_height);

    SDL_Rect again_rect = {
        (config->window_width - again_width) / 2,
        config->window_height/2,
        again_width,
        again_height
    };

    SDL_RenderCopy(config->renderer,again_texture,NULL,&again_rect);

    SDL_DestroyTexture(again_texture);
    SDL_FreeSurface(again_surface);
}

int scale_x_to_userspace(GameData* data, DrawConfig* config, int x){
    double x_scale_factor = config->window_width / get_stage_width(data);
    return (x * x_scale_factor);
}

int scale_y_to_userspace(GameData* data, DrawConfig* config, int y){
    double y_scale_factor = config->window_height / get_stage_height(data);
    return (y * y_scale_factor);
}


void render_game(GameData* data, DrawConfig* config){

    /*
     * Clear the window prior to any rendering
     */
    SDL_SetRenderDrawColor(config->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(config->renderer);

    /*
     * Draw all componenets of the came
     */
    render_background(data,config);
    render_all_pipes(data,config);
    render_score(data,config);
    render_bird(data,config);

    /*
     * Render GameOver message if the user has lost
     */
    if(is_gameover(data)){
        render_gameover_message(data,config);
    }

    /*
     * Update the users view to reflect changes 
     */
    SDL_RenderPresent(config->renderer);
}
