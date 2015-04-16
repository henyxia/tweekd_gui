#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>

#define AWESOME 7

Display *dis;
int screen;
Window win;
GC gc;
int width=320;
int height=240;
int i=0;

void init_x();
void close_x();
void redraw();

int main ()
{
	XEvent event;		/* the XEvent declaration !!! */
	KeySym key;		/* a dealie-bob to handle KeyPress Events */	
	char text[255];		/* a char buffer for KeyPress Events */
	XColor col;
	int initOffsetBMP;

	init_x();

	printf("Patte de canard\n");
	Colormap cmap = DefaultColormap(dis, DefaultScreen(dis));
	printf("?\n");

	FILE* home = NULL;
	char buffer[4];
	home = fopen("home.bmp", "r");
	if(home == NULL)
		return 0;

	fseek(home, 0x0A, SEEK_SET);
	buffer[0] = getc(home);
	buffer[1] = getc(home);
	buffer[2] = getc(home);
	buffer[3] = getc(home);
	printf("Pixel array offset %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
	initOffsetBMP = buffer[0] + (buffer[1] >> 8) + (buffer[2] >> 16) + (buffer[3] >> 24);

	fseek(home, 0x12, SEEK_SET);
        buffer[0] = getc(home);
        buffer[1] = getc(home);
        buffer[2] = getc(home);
        buffer[3] = getc(home);
        printf("Width %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);

	//fseek(home, 0x12, SEEK_SET);
        buffer[0] = getc(home);
        buffer[1] = getc(home);
        buffer[2] = getc(home);
        buffer[3] = getc(home);
        printf("Height %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);

	fseek(home, 0x1C, SEEK_SET);
        buffer[0] = getc(home);
        buffer[1] = getc(home);
        printf("Bits Per Pixel %02x %02x\n", buffer[0], buffer[1]);

	if(buffer[1] != 0 || buffer[0] != 0x18)
	{
		printf("Could not read BMP\n");
		return 1;
	}


	fseek(home, initOffsetBMP, SEEK_SET);

	while(1)
	{
		XNextEvent(dis, &event);
	
		if (event.type==Expose && event.xexpose.count==0)
			redraw();

		if (event.type==KeyPress&& XLookupString(&event.xkey,text,255,&key,0)==1)
		{
			if (text[0]=='q')
				close_x();
			printf("You pressed the %c key!\n",text[0]);
		}
		if (event.type==ButtonPress)
		{
			//int x=event.xbutton.x,
			    //y=event.xbutton.y;

			//strcpy(text,"X is FUN!");
			//XSetForeground(dis,gc,rand()%event.xbutton.x%255);
			//XDrawString(dis,win,gc,x,y, text, strlen(text));

		        buffer[0] = getc(home);
		        buffer[1] = getc(home);
		        buffer[2] = getc(home);

			int j;

			for(j=0; j<AWESOME; j++)
			{
				col.red = buffer[2];
				col.green = buffer[1];
				col.blue = buffer[0];





				if(XAllocColor(dis, cmap, &col) == 0)
					printf("Failed to allocate color\n");
				if(XSetForeground(dis, gc, col.pixel) == 0)
					printf("Failed to set foreground\n");
				if(XDrawPoint(dis,win,gc,i%width,i/height) == 0)
					printf("Failed to draw point\n");
			        buffer[0] = getc(home);
			        buffer[1] = getc(home);
			        buffer[2] = getc(home);
				printf("X %03d Y %03d R %02x G %02x B %02x\n", i%width, i/height, buffer[0], buffer[1], buffer[2]);
				i++;
			}
		}
	}
	printf("End of display\n");
	return 0;
}

void init_x() {
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis,screen),
	white=WhitePixel(dis, screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,width,height, 0,black, white);
	XSetStandardProperties(dis,win,"Howdy","Hi",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);        
	XSetBackground(dis,gc,white);
	XSetForeground(dis,gc,black);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};
