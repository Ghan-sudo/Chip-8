#include <stdio.h>
#include <SDL/SDL.h>

#define NUMKEYS 16
#define WRES 64
#define HRES 32
#define DEBUBIO 1

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 1280 

#define SCALE SCREEN_HEIGHT/HRES

uint8_t gfx[WRES][HRES] = {0} ;

uint8_t key = 0 ;
uint8_t quit = 0 , draw = 1 ;
typedef struct
{
	SDL_Renderer *renderer ;
	SDL_Window *window ;
} Disp ;

Disp app ;

void InitSDL(void)
{
	int renderFlags , windowFlags ;
	renderFlags = SDL_RENDERER_ACCELERATED ;
	windowFlags = 0 ;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Could not Initialize window %s \n" , SDL_GetError()) ;
		exit(1) ;
	}

	app.window = SDL_CreateWindow("CHIP-8" , SDL_WINDOWPOS_UNDEFINED ,  SDL_WINDOWPOS_UNDEFINED , SCREEN_WIDTH , SCREEN_HEIGHT , windowFlags) ;

	if(!app.window)
	{
		printf("Failed To open window w: %d H: %d Because :%s \n" , SCREEN_WIDTH , SCREEN_HEIGHT , SDL_GetError) ;
		exit(1) ;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY , "linear") ;

	app.renderer = SDL_CreateRenderer(app.window , -1 , renderFlags) ;

	if(!app.renderer)
	{
		printf("Failed to render because : %s \n" , SDL_GetError) ;
		exit(1) ;
	}
}

void cleanup(void)
{
	SDL_DestroyRenderer(app.renderer) ;
	SDL_DestroyWindow(app.window) ;
	SDL_Quit() ;
}

void Inp(SDL_Event event)
{

	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
						case SDLK_1: key = 0x1; break;
						case SDLK_2: key = 0x2; break;
						case SDLK_3: key = 0x3; break;
						case SDLK_4: key = 0xC; break;
						case SDLK_q: key = 0x4; break;
						case SDLK_w: key = 0x5; break;
						case SDLK_e: key = 0x6; break;
						case SDLK_r: key = 0xD; break;
						case SDLK_a: key = 0x7; break;
						case SDLK_s: key = 0x8; break;
						case SDLK_d: key = 0x9; break;
						case SDLK_f: key = 0xE; break;
						case SDLK_z: key = 0xA; break;
						case SDLK_x: key = 0x0; break;
						case SDLK_c: key = 0xB; break;
						case SDLK_v: key = 0xF; break;
						case SDLK_ESCAPE: exit(1); break;
				}
			} else if (event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym) {
						case SDLK_1: key = 0; break;
						case SDLK_2: key = 0; break;
						case SDLK_3: key = 0; break;
						case SDLK_4: key = 0; break;
						case SDLK_q: key = 0; break;
						case SDLK_w: key = 0; break;
						case SDLK_e: key = 0; break;
						case SDLK_r: key = 0; break;
						case SDLK_a: key = 0; break;
						case SDLK_s: key = 0; break;
						case SDLK_d: key = 0; break;
						case SDLK_f: key = 0; break;
						case SDLK_z: key = 0; break;
						case SDLK_x: key = 0; break;
						case SDLK_c: key = 0; break;
						case SDLK_v: key = 0; break;
				}
			} else if (event.type == SDL_QUIT) {
				quit = 1;
			}
	}
}

void Draw(uint8_t gfx[WRES][HRES])
{
	//SDL_SetRenderDrawColor(app.renderer , 0 , 0 , 0 , 0) ;
	//SDL_RenderClear(app.renderer) ;

	for(int i = 0 ; i < WRES ; i++)
	{
		for(int j = 0 ; j < HRES ; j++)
		{
			if(gfx[i][j])
			{
				SDL_SetRenderDrawColor(app.renderer , 255 , 255 , 255 , 255) ;
				for(int x = 0 ; x < SCALE ; x++)
					for(int y = 0 ; y < SCALE ; y++)
					{
						//printf("I : %d J : %d  X : %d Y : %d \n" , i , j , x , y) ;
						SDL_RenderDrawPoint(app.renderer , x + (i*SCALE) ,y + (j * SCALE)) ;
					}
			}
			else
			{
			SDL_SetRenderDrawColor(app.renderer , 0 , 0 , 0 , 0) ;
			for(int x = 0 ; x < SCALE ; x++)
				for(int y = 0 ; y < SCALE ; y++)
					SDL_RenderDrawPoint(app.renderer , x + (i*SCALE) , y + (j*SCALE)) ;
			}
		}
	}
	SDL_RenderPresent(app.renderer) ;
}

#ifdef DEBUGIO
void main()
{
	atexit(cleanup) ;

	InitSDL() ;
	uint8_t gfx[WRES][HRES] = 
	{
		{1 ,1 ,1} ,
		{1} , {} , {} , {1}
	}	;

	uint8_t ogfx[WRES][HRES] = 
	{
		{1 , 0 , 1} ,
		{1} , {} , {} , {0}
	} ;

	while(1)
	{
		Draw(gfx) ;
		Draw(ogfx) ;

		Inp() ;

		SDL_Delay(0) ;
	}
	return 0 ;
}
#endif
