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
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_Init failed: %s\n",SDL_GetError());
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"IMG_Init for IMG_INIT_PNG failed %s\n",IMG_GetError());
    }  
    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"TTF_Init faled %s\n",TTF_GetError());
    }

    /*
     * create and initialize SDL resources.
     */
    if(SDL_CreateWindowAndRenderer(config->window_width,config->window_height,0,&(config->window),&(config->renderer))){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Could not create window and renderer: %s\n",SDL_GetError());
    }

    /*
     * Set the windows title
     */
    SDL_SetWindowTitle(config->window,"Flappy Bird");
    
    /*
     * Load image files using SDL_image.
     */
    char* bird_path =  "resources/images/bird.png";
    config->bird_texture = IMG_LoadTexture(config->renderer,bird_path);
    if(config->bird_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",bird_path,IMG_GetError());
    } 

    char* background_path = "resources/images/background.png";
    config->background_texture = IMG_LoadTexture(config->renderer,background_path);
    if(config->background_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",background_path,IMG_GetError());
    }
    
    char* pipe_path = "resources/images/pipe.png";
    config->pipe_texture = IMG_LoadTexture(config->renderer,pipe_path);
    if(config->pipe_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipe_path,IMG_GetError());
    }
    
    char* pipetop_path = "resources/images/pipe-top.png";
    config->pipe_top_texture = IMG_LoadTexture(config->renderer,pipetop_path);
    if(config->pipe_top_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipetop_path,IMG_GetError());
    }

    /*
     * Load font using TTF_Font
     */
    char* font_path = "resources/fonts/VeraMono.ttf"; 
    config->game_font = TTF_OpenFont(font_path,24);
    if(config->game_font == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Font at \"%s\" failed to load: %s\n",font_path,TTF_GetError());
    }
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

    if(SDL_RenderCopy(config->renderer,config->background_texture,&source_rect,NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering background texture: %s\n",SDL_GetError());
    }
}

void render_all_pipes(GameData* data, DrawConfig* config){
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
    if(SDL_QueryTexture(config->pipe_top_texture, NULL, NULL, &pipe_top_texture_w, &pipe_top_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe top texture: %s\n",SDL_GetError());
    }

    int pipe_texture_h, pipe_texture_w;
    if(SDL_QueryTexture(config->pipe_texture, NULL, NULL, &pipe_texture_w, &pipe_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe texture: %s\n",SDL_GetError());
    }

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
        
        if(SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect) != 0){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
    SDL_Rect top_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data,config,pipe->top_end) - pipe_top_texture_h,
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopyEx(config->renderer,config->pipe_top_texture,NULL,&top_cap_rect,0,NULL,SDL_FLIP_VERTICAL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }

    /*
     * Render lower pipe
     */
    SDL_Rect low_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data, config, pipe->bottom_start),
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopy(config->renderer,config->pipe_top_texture,NULL,&low_cap_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }
    for(i = scale_y_to_userspace(data, config, pipe->bottom_start) + pipe_top_texture_h; i < config->window_height ; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(data,config,pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        if(SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect)){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
}

void render_score(GameData* data, DrawConfig* config){
    SDL_Color color={0,0,0};

    char score_str[9];
    sprintf(score_str, "%d", get_score(data));

    SDL_Surface *text_surface;
    text_surface = TTF_RenderText_Solid(config->game_font,score_str,color);
    if(text_surface == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error rendering text \"%s\": %s\n",score_str,TTF_GetError());
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(config->renderer,text_surface);
    if(text_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error creating texture for text \"%s\":%s\n",score_str,SDL_GetError());
    }

    int text_width,text_height;
    if(SDL_QueryTexture(text_texture,NULL,NULL,&text_width,&text_height)  != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of text texture: %s\n",SDL_GetError());
    } 

    SDL_Rect dest_rect = {
        (config->window_width - text_width) / 2,
        config->window_height/10,
        text_width,
        text_height
    };

    if(SDL_RenderCopy(config->renderer,text_texture,NULL,&dest_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering text texture: %s\n",SDL_GetError());
    }

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}


void render_bird(GameData* data, DrawConfig* config){

    /*
     * Query Bird image for it's width and heignt
     */
    int bird_w,bird_h;
    if(SDL_QueryTexture(config->bird_texture, NULL, NULL, &bird_w, &bird_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of bird texture: %s\n",SDL_GetError());
    }

    /*
     * Specifiy where in the window the bird will be rendered.
     */
    SDL_Rect dest_rect = {
        scale_x_to_userspace(data,config,get_bird_x(data)) - (bird_w / 2),
        scale_y_to_userspace(data,config,get_bird_y(data)) - (bird_h / 2),
        bird_w,
        bird_h
    };

    if(SDL_RenderCopy(config->renderer,config->bird_texture,NULL,&dest_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering bird texture: %s\n",SDL_GetError());
    }
}

void render_gameover_message(GameData* data, DrawConfig* config){
    SDL_Color color={0,0,0};
    
    /*
     * Create texture for and render a gameover message containing the final
     * score.
     */

    char score_str[35];
    sprintf(score_str, "You lost with a score of %d", get_score(data));

    SDL_Surface *score_surface;
    score_surface = TTF_RenderText_Solid(config->game_font,score_str,color);
    if(score_surface == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error rendering text \"%s\": %s\n",score_str,TTF_GetError());
    }
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(config->renderer,score_surface);
    if(score_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error creating texture for text \"%s\": %s\n",score_str,SDL_GetError());
    }

    int score_width,score_height;
    if(SDL_QueryTexture(score_texture,NULL,NULL,&score_width,&score_height) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of texture for text \"%s\": %s\n",score_str,SDL_GetError());
    }

    SDL_Rect score_rect = {
        (config->window_width - score_width) / 2,
        config->window_height/4,
	score_width,
        score_height
    };

    if(SDL_RenderCopy(config->renderer,score_texture,NULL,&score_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error rendering surface for text \"%s\": %s\n",score_str,SDL_GetError());
    }

    SDL_DestroyTexture(score_texture);
    SDL_FreeSurface(score_surface);

    /*
     * Create texture for and rendure gamover message prompting the player
     * to play again.
     */
    char* again_str = "Press Space to Play Again";

    SDL_Surface *again_surface;
    again_surface = TTF_RenderText_Solid(config->game_font,again_str,color);
    if(again_surface == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error rendering text \"%s\": %s\n",again_str,TTF_GetError());
    }

    SDL_Texture *again_texture = SDL_CreateTextureFromSurface(config->renderer,again_surface);
    if(again_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error creating texture for text \"%s\": %s\n",again_str,SDL_GetError());
    }
   
    int again_width,again_height;
    if(SDL_QueryTexture(again_texture,NULL,NULL,&again_width,&again_height) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of texture for text \"%s\": %s\n",again_str,SDL_GetError());
    }

    SDL_Rect again_rect = {
        (config->window_width - again_width) / 2,
        config->window_height/2,
        again_width,
        again_height
    };

    if(SDL_RenderCopy(config->renderer,again_texture,NULL,&again_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error rendering surface for text \"%s\": %s\n",again_str,SDL_GetError());
    }

    SDL_DestroyTexture(again_texture);
    SDL_FreeSurface(again_surface);
}

void render_clear(DrawConfig* config){
    if(SDL_SetRenderDrawColor(config->renderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error setting render color to white (FF,FF,FF,FF): %s\n",SDL_GetError());
    }
    if(SDL_RenderClear(config->renderer) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error clearing renderer: %s\n",SDL_GetError());
    }
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
    render_clear(config); 

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
