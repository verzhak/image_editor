
#include "bitmap/display.h"

c_display::c_display(shared_ptr<c_bitmap> t_bmp, string t_disp_name) :
	bmp(t_bmp), is_full(false), disp_name(t_disp_name)
{
	pipe(pp);
	pid = fork();

	if(pid == -1)
		throw string("Ошибка при попытке отобразить изображение");
	else if(! pid)
		exit(thread_main());

	// ############################################################################ 
	// Ждем запуска дочернего процесса
	
	char notused;
	read(pp[0], & notused, 1);

	// ############################################################################ 

	refresh();
}
		
c_display::~c_display()
{
	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);

	close(pp[0]);
	close(pp[1]);
}

void c_display::refresh()
{
	const char * disp_name_c_str = disp_name.c_str();
	unsigned disp_name_len = strlen(disp_name_c_str); // Гарантировано точно возвращает размер в байтах строки

	write(pp[1], & disp_name_len, sizeof(disp_name_len));
	write(pp[1], disp_name_c_str, disp_name_len);

	kill(pid, SIGUSR1);
}

void c_display::toggle_full()
{
	kill(pid, SIGUSR2);

	is_full = ! is_full;
}

// ############################################################################ 

int * temp_pp;
string temp_disp_name;

#define EVENT_REFRESH SDL_USEREVENT
#define EVENT_TERMINATE (SDL_USEREVENT + 1)
#define EVENT_TOGGLE_FULL (SDL_USEREVENT + 2)

void SIGUSR1_handler(int notused)
{
	// refresh
	
	unsigned buf_len;
	shared_ptr<char> buf;
	char * p_buf;
	
	read(temp_pp[0], & buf_len, sizeof(buf_len));
	buf.reset(new char[buf_len + 1]);
	p_buf = buf.get();

	if(buf)
	{
		read(temp_pp[0], p_buf, buf_len);
		p_buf[buf_len] = '\0';
		temp_disp_name = p_buf;
	}

	SDL_Event rk;

	rk.type = EVENT_REFRESH;
	SDL_PushEvent(& rk);
}

void SIGUSR2_handler(int notused)
{
	// toggle full

	SDL_Event rk;

	rk.type = EVENT_TOGGLE_FULL;
	SDL_PushEvent(& rk);
}

void SIGTERM_handler(int notused)
{
	// terminate

	SDL_Event rk;

	rk.type = EVENT_TERMINATE;
	SDL_PushEvent(& rk);
}

int c_display::thread_main()
{
	SDL_Init(SDL_INIT_VIDEO);

	bool is_run = true;
	unsigned v, u, ul_y, ul_x, lr_y, lr_x, aoi_height, aoi_width, height = bmp->height, width = bmp->width;

	SDL_Surface * surf = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_PixelFormat * fmt = surf->format;
	SDL_Event event;
	SDL_Rect rect;

	Uint32 r_shift = fmt->Rshift, g_shift = fmt->Gshift, b_shift = fmt->Bshift, * pixels = (Uint32 *) surf->pixels;

	SDL_WM_SetCaption(disp_name.c_str(), NULL);

	signal(SIGUSR1, & SIGUSR1_handler);
	signal(SIGUSR2, & SIGUSR2_handler);
	signal(SIGTERM, & SIGTERM_handler);

	temp_pp = pp;
	write(pp[1], & is_run, 1);

	while(is_run)
	{
		SDL_WaitEvent(& event);

		if(event.type == EVENT_REFRESH)
		{
			disp_name = temp_disp_name;
			SDL_WM_SetCaption(disp_name.c_str(), NULL);

			SDL_LockSurface(surf);

			ul_y = bmp->aoi().ul_y();
			ul_x = bmp->aoi().ul_x();
			lr_y = bmp->aoi().lr_y();
			lr_x = bmp->aoi().lr_x();
			aoi_height = bmp->aoi().height();
			aoi_width = bmp->aoi().width();

			rect.y = (ul_y < BORDER_WIDTH) ? 0 : (ul_y - BORDER_WIDTH);
			rect.x = (ul_x < BORDER_WIDTH) ? 0 : (ul_x - BORDER_WIDTH);
			rect.h = aoi_height + BORDER_WIDTH_2;
			rect.w = aoi_width + BORDER_WIDTH_2;
			
			if(rect.h > height)
				rect.h = height;

			if(rect.w > width)
				rect.w = width;

			SDL_FillRect(surf, NULL, SDL_MapRGB(fmt, 0, 0, 0));
			SDL_FillRect(surf, & rect, BORDER_COLOR);

			if(is_full)
			{
				for(v = ul_y; v < lr_y; v++)
					for(u = ul_x; u < lr_x; u++)
						pixels[v * width + u] = (uint32_t) bmp->pixel(v, u).get_full_u8(R) << r_shift |
							(uint32_t) bmp->pixel(v, u).get_full_u8(G) << g_shift | (uint32_t) bmp->pixel(v, u).get_full_u8(B) << b_shift;
			}
			else
			{
				for(v = ul_y; v < lr_y; v++)
					for(u = ul_x; u < lr_x; u++)
						pixels[v * width + u] = (uint32_t) bmp->pixel(v, u).get_u8(R) << r_shift |
							(uint32_t) bmp->pixel(v, u).get_u8(G) << g_shift | (uint32_t) bmp->pixel(v, u).get_u8(B) << b_shift;
			}

			SDL_UnlockSurface(surf);
			SDL_Flip(surf);
		}
		else if(event.type == EVENT_TOGGLE_FULL)
			is_full = ! is_full;
		else if(event.type == EVENT_TERMINATE)
			is_run = false;
	}

	SDL_Quit();

	return 0;
}

