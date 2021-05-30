#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 768
#define SCREEN_HEIGHT 1024
#define SCREEN_BPP 32
#define BOX_WIDTH 192
#define BOX_HEIGHT 256

SDL_Surface *screen=NULL;
SDL_Surface *image=NULL;

bool quit=false;

SDL_Event event;

Uint32 *c=NULL;

int d=-1;
int x=0,y=0;
int xi=0,yi=0;
int px=0,py=0;

bool upKeyPressed=false;
bool downKeyPressed=false;
bool leftKeyPressed=false;
bool rightKeyPressed=false;

void lock(SDL_Surface *surface) {
  if(SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
  }
}

void unlock(SDL_Surface *surface) {
  if( SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
  }
}

Uint32 get_pixel32( SDL_Surface* surface, int x, int y ) {
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Get the requested pixel
    return pixels[ ( y * surface->w ) + x ];
}

void put_pixel32( SDL_Surface* surface, int x, int y, Uint32 pixel ) {
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip ) {
	//Holds offsets
	SDL_Rect offset;

	//Get offsets
	offset.x = x;
	offset.y = y;

	//Blit
	SDL_BlitSurface( source, clip, destination, &offset );
}

bool validxy(int x,int y) {
	return (x>=0 && x<=SCREEN_WIDTH-1) && (y>=0 && y<=SCREEN_HEIGHT-1);
}

void Draw_Rect( SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel) {

	lock(surface);

	for(int i=0;i<w;i++) {
		put_pixel32(surface,x+i,y,pixel);
		put_pixel32(surface,x+i,y+h,pixel);
	}

	for(int j=0;j<h;j++) {
		put_pixel32(surface,x,y+j,pixel);
		put_pixel32(surface,x+w,y+j,pixel);
	}

	unlock(surface);

}

void Fill_Rect( SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel) {
	SDL_Rect rect={x,y,w,h};
	SDL_FillRect(surface,&rect,pixel);
}

void scan(SDL_Surface *surface,int x,int y,int w,int h) {
	lock(surface);
	for(int j=0;j<h;j++) {
		for(int i=0;i<w;i++) {
			c[i+j*w]=get_pixel32(surface,x+i,y+j);
		}
	}
	unlock(surface);
}

void print(SDL_Surface *surface,int x,int y,int w,int h) {
	lock(surface);
	for(int j=0;j<h;j++) {
		for(int i=0;i<w;i++) {
			put_pixel32(screen,x+i,y+j,c[i+j*w]);
		}
	}
	unlock(surface);
}

void erase(SDL_Surface *surface,int x,int y,int w,int h,Uint32 pixel) {
	Fill_Rect(screen,x,y,w,h,pixel);
}

void grid(SDL_Surface *surface,int w,int h,Uint32 pixel) {
	for(int j=0;j<SCREEN_HEIGHT/h;j++) {
		for(int i=0;i<SCREEN_WIDTH/w;i++) {
			Draw_Rect(surface,i*w,j*h,w-1,h-1,pixel);
		}
	}
}



int main(int argc,char **argv) {

	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);

	screen=SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_FULLSCREEN);

	Uint32 black=SDL_MapRGB(screen->format,0x00,0x00,0x00);

	c=calloc(BOX_WIDTH*BOX_HEIGHT,sizeof(Uint32));

	image=SDL_LoadBMP("image.bmp");

	apply_surface(0,0,image,screen,NULL);

	erase(screen,0,0,BOX_WIDTH,BOX_HEIGHT,black);

	grid(screen,BOX_WIDTH,BOX_HEIGHT,black);

	SDL_Flip(screen);

	while(!quit) {

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					quit=true;
				break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit=true;
						break;
						case SDLK_UP: upKeyPressed=true; break;
						case SDLK_DOWN: downKeyPressed=true; break;
						case SDLK_LEFT: leftKeyPressed=true; break;
						case SDLK_RIGHT: rightKeyPressed=true; break;
						default:
						break;
					}
				break;
				default:
				break;
			}
		}

		d=-1;
		px=x;
		py=y;

		if(upKeyPressed) {
			if(y-BOX_HEIGHT>=0) d=0;
		} else if(downKeyPressed) {
			if(y+BOX_HEIGHT<SCREEN_HEIGHT) d=1;
		} else if(leftKeyPressed) {
			if(x-BOX_WIDTH>=0) d=2;
		} else if(rightKeyPressed) {
			if(x+BOX_WIDTH<SCREEN_WIDTH) d=3;
		}

		switch(d) {
			case 0:
				y-=BOX_HEIGHT;
				scan(screen,x,y,BOX_WIDTH,BOX_HEIGHT);
				erase(screen,x,y,BOX_WIDTH,BOX_HEIGHT,black);
				print(screen,px,py,BOX_WIDTH,BOX_HEIGHT);
			break;
			case 1:
				y+=BOX_HEIGHT;
				scan(screen,x,y,BOX_WIDTH,BOX_HEIGHT);
				erase(screen,x,y,BOX_WIDTH,BOX_HEIGHT,black);
				print(screen,px,py,BOX_WIDTH,BOX_HEIGHT);
			break;
			case 2:
				x-=BOX_WIDTH;
				scan(screen,x,y,BOX_WIDTH,BOX_HEIGHT);
				erase(screen,x,y,BOX_WIDTH,BOX_HEIGHT,black);
				print(screen,px,py,BOX_WIDTH,BOX_HEIGHT);
			break;
			case 3:
				x+=BOX_WIDTH;
				scan(screen,x,y,BOX_WIDTH,BOX_HEIGHT);
				erase(screen,x,y,BOX_WIDTH,BOX_HEIGHT,black);
				print(screen,px,py,BOX_WIDTH,BOX_HEIGHT);
			break;
		}

		upKeyPressed=false;
		downKeyPressed=false;
		leftKeyPressed=false;
		rightKeyPressed=false;

		SDL_Flip(screen);
	}

	SDL_FreeSurface(image);

	SDL_Quit();

	return 0;
}

