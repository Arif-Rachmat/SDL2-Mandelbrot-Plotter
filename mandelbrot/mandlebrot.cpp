#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <string>

std::mutex trdLock;

short WIDTH = 800, HEIGHT = 800;
unsigned short max_iteration = 100;
unsigned int dtime, now;
unsigned __int8 step = 255, p = 0;
float zoomulp = 2, zoomval = 1, zoomMax = 150000000000000;

char text[50];

long double xmin = -2.5, xmax = 1.5;									// Rendering area/coordinate, this define the range of
long double ymin = -2.0, ymax = 2.0;									// the rendered area/coordinate on the window.

long double xcoor = 0.281717921930775, ycoor = 0.5771052841488505;		// Locked coordinate, this is the coordinate where the camera will zoom in.

bool running = true;
bool lctrl = false, lshift = false;
bool ui = true, autoZoom = false;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
TTF_Font* font;

//color palette
unsigned __int8 palette[4][5][3] = {
	{
	{0,7,100},
	{32,107,203},
	{237,255,255},
	{255,170,0},
	{0,2,0}
	},
	{
	{254,0,0},
	{254,134,35},
	{185,254,0},
	{0,228,23},
	{4,1,249},
	},
	{
	{167, 125, 167},
	{0, 32, 41},
	{47, 183, 7},
	{95, 184, 211},
	{0, 0, 211}
	},
	{
	{1, 1, 199},
	{251, 0, 130},
	{0, 247, 125},
	{137, 0, 253},
	{248, 130, 1}
	}
};


//pre-define function
long double map(long double var, long double in_min, long double in_max, long double out_min, long double out_max);
void zoom(float z);
void zoom(float z, long double xPosition, long double yPosition);
void textRender(char buffer[], SDL_Renderer* renderer, TTF_Font* font, int x, int y, int w, int h);
void eventPoll();
void iterate(unsigned short x, unsigned short xo);
void DeInit();
void sendHelp();



//███    ███  █████  ██ ███    ██
//████  ████ ██   ██ ██ ████   ██
//██ ████ ██ ███████ ██ ██ ██  ██
//██  ██  ██ ██   ██ ██ ██  ██ ██
//██      ██ ██   ██ ██ ██   ████

int main(int argc, char* argv[]) {
	
	//Argument handling
	if (argc > 1) {		// Check if there is a given argument
		for (int i = 1; i < argc; i++){
			char* er;
			errno = 0;

			//Print Helppppppppppppppp
			if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "-help") {
				sendHelp();
				return 0;
			}

			// Colorscheme/palette argument
			else if (std::string(argv[i]) == "-p") {
				i++;
				if (!argv[i])std::cout << "-p [palette] argument not found\nSee mandelbrot.exe -h for more info\n";
				p = strtol(argv[i], &er, 10);
				if ( errno != 0 || *er != '\0') {													//Check if the given argument is an integer
					std::cout << "Invalid argument. -p must have an int type argument\nSee mandelbrot.exe -h for more info\n";
					return 0;
				}
				else if (p > (sizeof palette/sizeof palette[0] - 1)) {								//Check if the value is out of bound of the available color palette.
					std::cout << "value out of bound\nSee 'mandelbrot.exe -h' for more info\n";
					return 0;
				}
			}

			//Resolution argument
			else if (std::string(argv[i]) == "-r") {
				i++;
				if (!argv[i])std::cout << "-r [Width] [Height] argument not found\nSee mandelbrot.exe -h for more info\n";
				WIDTH = strtol(argv[i], &er, 10);
				if (errno != 0 || *er != '\0') {
					std::cout << "Invalid argument. -r must have an int type argument\nSee mandelbrot.exe -h for more info\n";
					return 0;
				}
				i++;
				if (!argv[i])std::cout << "-r [Height] argument not found\nSee mandelbrot.exe -h for more info\n";
				HEIGHT = strtol(argv[i], &er, 10);
				if (errno != 0 || *er != '\0') {
					std::cout << "Invalid argument. -r must have an int type argument\nSee mandelbrot.exe -h for more info\n";
					return 0;
				}
				xmin = double(WIDTH) / double(HEIGHT) * -2.5;
				xmax = double(WIDTH) / double(HEIGHT) * 1.5;
			}

			//First Zooming coordinate argument
			else if (std::string(argv[i]) == "-c") {
				i++;
				if (!argv[i])std::cout << "-c [X coordinate] [Y coordinate] argument not found\nSee mandelbrot.exe -h for more info\n";
				xcoor = strtold(argv[i], &er);
				if (errno != 0 || *er != '\0') {
					std::cout << "Invalid argument. -c must have a numeric type argument\nSee mandelbrot.exe -h for more info\n";
					return 0;
				}
				i++;
				if (!argv[i])std::cout << "-c [Y coordinate] argument not found\nSee mandelbrot.exe -h for more info\n";
				ycoor = strtold(argv[i], &er) * -1;
				if (errno != 0 || *er != '\0') {
					std::cout << "Invalid argument. -c must have a numeric type argument\nSee mandelbrot.exe -h for more info\n";
					return 0;
				}
			}
			
			// Check if the given argument match any of the available option
			else{
				std::cout << "Invalid argument\nSee 'mandelbrot.exe -h' for more info\n";
				return 0;
			}
		}
	}
	FreeConsole();
	
	// Initialize
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_EVENTS);
	SDL_Init(SDL_INIT_TIMER);

	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_RENDERER_ACCELERATED, &window, &renderer);
	SDL_SetWindowTitle(window, "Mandelbrot | Arif R | 2022");

	// Open windows system font, need changes for other system support.
	if (!(font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 72)));

	//Program main loop.
	while (running) {

		if (ui)
		{
			//Iteration text render.
			sprintf_s(text, "Max Iteration : %d/%d  |  Step : %d", max_iteration, 65535, step);
			textRender(text, renderer, font, 0, 0, strlen(text) * 9, 25);

			//zoom text render.
			sprintf_s(text, "Zoom(%0.2f) : x%0.2f", zoomulp, zoomval);
			textRender(text, renderer, font, 0, 30, strlen(text) * 9, 25);

			//Timer text render.
			now = SDL_GetTicks();
			sprintf_s(text, "Time : %dms", now - dtime);
			textRender(text, renderer, font, 0, 60, strlen(text) * 9, 25);
			dtime = now;

			//Position info render.
			sprintf_s(text, "X:%f  |  Y:%f", map(WIDTH / 2, 0, WIDTH, xmin, xmax), map(HEIGHT / 2, 0, HEIGHT, ymin, ymax) * -1);
			textRender(text, renderer, font, 0, 90, strlen(text) * 9, 25);
		}

		// Main Render.
		SDL_RenderPresent(renderer);

		// start iterating thread.
		std::thread t1(iterate, 0, WIDTH * 0.25);
		std::thread t2(iterate, WIDTH * 0.25, WIDTH * 0.5);
		std::thread t3(iterate, WIDTH * 0.5, WIDTH * 0.75);
		std::thread t4(iterate, WIDTH * 0.75, WIDTH);

		// poll event while Waiting until all thread finish.
		eventPoll();
		t1.join();
		eventPoll();
		t2.join();
		eventPoll();
		t3.join();
		eventPoll();
		t4.join();

		// Autozoom at locked coordinate.
		if (autoZoom) {
			if (zoomMax / zoomval < zoomulp) {			// if the next zoom multiplier surpass the zoom value
				zoom(zoomMax / zoomval, xcoor, ycoor);	// threshold, then zoom to the maximum zoom value and
				autoZoom = false;						// disable autozoom
			}
			else zoom(zoomulp, xcoor, ycoor);
		}
	}
	DeInit();
	return 0;
}



//███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██
//██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██
//█████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██
//██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██
//██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████

//value mapping function, based on arduion IDE map function.
long double map(long double var, long double in_min, long double in_max, long double out_min, long double out_max) {
	return(var - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Cursor location based Zoom Function.
void zoom(float z) {

	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);

	long double posX = xmin + (xmax - xmin) * mousex / WIDTH;		//calculate cursor position on the window for zooming.
	long double posY = ymin + (ymax - ymin) * mousey / HEIGHT;

	xcoor = posX;
	ycoor = posY;

	long double xTemp = posX - (xmax - xmin) / 2 / z;
	xmax = posX + (xmax - xmin) / 2 / z;
	xmin = xTemp;

	long double yTemp = posY - (ymax - ymin) / 2 / z;
	ymax = posY + (ymax - ymin) / 2 / z;
	ymin = yTemp;

	zoomval *= z;
}

//Coordinate based Zoom Function
void zoom(float z, long double xPosition, long double yPosition) {

	long double xTemp = xPosition - (xmax - xmin) / 2 / z;
	xmax = xPosition + (xmax - xmin) / 2 / z;
	xmin = xTemp;

	long double yTemp = yPosition - (ymax - ymin) / 2 / z;
	ymax = yPosition + (ymax - ymin) / 2 / z;
	ymin = yTemp;

	zoomval *= z;
}

//Text rendering Function, always call before main rendering function.
void textRender(char buffer[], SDL_Renderer* renderer, TTF_Font* font, int x, int y, int w, int h) {
	SDL_Rect Message_rect;
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, buffer, { 255,255,255 });
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Message_rect.x = x;
	Message_rect.y = y;
	Message_rect.w = w;
	Message_rect.h = h;

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

//Event handling function
void eventPoll() {
	//input and event handling
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:	//Exit handler
			running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)zoom(zoomulp);		//zoom in.
			if (event.button.button == SDL_BUTTON_RIGHT)zoom(1 / zoomulp);	//zoom out.
			break;

		case SDL_MOUSEWHEEL:
			if (!lctrl && !lshift) {							// scrollwheel only.
				if (event.wheel.y > 0) max_iteration = (65535 - max_iteration >= step) ? max_iteration + step : 65535;	// iteration stepping and 
				if (event.wheel.y < 0) max_iteration = (max_iteration > step) ? max_iteration - step : 1;				// check for overflowing stepping.
			}
			else if (lctrl && !lshift) {						// scrollwheel + left ctrl.
				if (event.wheel.y > 0 && step < 255)step++;	// increase step for iteration stepping.
				if (event.wheel.y < 0 && step > 1)step--;	// decrease step for iteration stepping.
			}
			else if (!lctrl && lshift) {											// scrollwheel + left shift.
				if (event.wheel.y > 0 && zoomulp < 5)zoomulp *= 1.01;				// increase zoom multiplier.
				if (event.wheel.y < 0 && zoomulp > 1)zoomulp /= 1.01;				// decrease zoom multiplier.
			}
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_LSHIFT:	lshift = true;				break;	// left shift hold check.
			case SDLK_LCTRL:	lctrl = true;				break;	// left ctrl hold check.
			case 'u':	ui = (ui) ? false : true;			break;	// toggle UI/info.
			case 'z': autoZoom = (autoZoom) ? false : true;	break;	// toggle Auto Zoom.
			case 'e': zoom(zoomulp, xcoor, ycoor);		break;	// Locked zoom in.
			case 'q': zoom(1 / zoomulp, xcoor, ycoor);	break;	// Locked zoom out.
			}
			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LCTRL) lctrl = false;		// left ctrl release check.
			if (event.key.keysym.sym == SDLK_LSHIFT) lshift = false;	// left shift release check.
			break;
		}
	}
}

//Iteration Function, used with multithreading
void iterate(unsigned short x, unsigned short xo) {
	for (; x < xo; x++) {
		for (unsigned short y = 0; y < HEIGHT; y++)
		{
			long double x0 = map(x, 0, WIDTH, xmin, xmax);
			long double y0 = map(y, 0, HEIGHT, ymin, ymax);

			long double a = 0;
			long double b = 0;

			unsigned short n = 0;

			// Iteration calculation for each pixels.
			while (a * a + b * b <= 2 * 2 && n < max_iteration) {
				long double aTemp = a * a - b * b + x0;
				b = 2 * a * b + y0;
				a = aTemp;
				n++;
			}


			trdLock.lock();		// Lock thread to make sure the pixel coloring process uninterupt.

			//Coloring based on color Pallet.
			if (n == max_iteration)SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);			//if the iteration is equal to the max iteration, color it black.
			else SDL_SetRenderDrawColor(renderer,
				map(n % 51, 0, 50, palette[p][n / 51 % 5][0], palette[p][(n / 51 % 5 + 1 > 4) ? 0 : n / 51 % 5 + 1][0]),		// Coloring based on a linear interpolated
				map(n % 51, 0, 50, palette[p][n / 51 % 5][1], palette[p][(n / 51 % 5 + 1 > 4) ? 0 : n / 51 % 5 + 1][1]),		// value between point on the color
				map(n % 51, 0, 50, palette[p][n / 51 % 5][2], palette[p][(n / 51 % 5 + 1 > 4) ? 0 : n / 51 % 5 + 1][2]),		// pallete using the current iteration.
				255);
			SDL_RenderDrawPoint(renderer, x, y);	//Plotting.
			trdLock.unlock();
		}
	}
}

//Log help info
void sendHelp() {
	std::cout << "\n\nMandelbrot Set Plotter / Explorer made by Arif Rachmat using SDL2 library" << std::endl;
	std::cout << "Usage : mandelbrot.exe [argument]" << std::endl;
	std::cout << "\n	-r <width> <heigth>" << std::endl;
	std::cout << "		set the resolution of the program" << std::endl;
	std::cout << "\n	-c <x coordinate> <y coordinate>" << std::endl;
	std::cout << "		set the initial zoom coordinate" << std::endl;
	std::cout << "\n	-p <palette index>" << std::endl;
	std::cout << "		set the colortheme / palette.available color palette index :" << std::endl;
	std::cout << "\nVisit the github page for more infoand documentation :" << std::endl;
	std::cout << "https ://github.com/Arif-Rachmat/SDL2-Mandelbrot-Plotter\n" << std::endl;
}

//Deinitializing function, *call before returning the main function
void DeInit() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}