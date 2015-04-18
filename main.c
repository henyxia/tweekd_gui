#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>		/* getenv(), etc. */
#include <unistd.h>		/* sleep(), etc.  */

Window create_simple_window(Display* display, int width, int height, int x, int y)
{
  int screen_num = DefaultScreen(display);
  int win_border_width = 2;
  Window win;

  win = XCreateSimpleWindow(display, RootWindow(display, screen_num), x, y, width, height, win_border_width, BlackPixel(display, screen_num), WhitePixel(display, screen_num));

  XMapWindow(display, win);
  XFlush(display);

  return win;
}

GC create_gc(Display* display, Window win, int reverse_video)
{
  GC gc;
  unsigned long valuemask = 0;		/* which values in 'values' to  */
					/* check when creating the GC.  */
  XGCValues values;			/* initial values for the GC.   */
  unsigned int line_width = 2;		/* line width for the GC.       */
  int line_style = LineSolid;		/* style for lines drawing and  */
  int cap_style = CapButt;		/* style of the line's edje and */
  int join_style = JoinBevel;		/*  joined lines.		*/
  int screen_num = DefaultScreen(display);

  gc = XCreateGC(display, win, valuemask, &values);
  if (gc < 0) {
	fprintf(stderr, "XCreateGC: \n");
  }

  if (reverse_video)
  {
    XSetForeground(display, gc, WhitePixel(display, screen_num));
    XSetBackground(display, gc, BlackPixel(display, screen_num));
  }
  else
  {
    XSetForeground(display, gc, BlackPixel(display, screen_num));
    XSetBackground(display, gc, WhitePixel(display, screen_num));
  }

  XSetLineAttributes(display, gc, line_width, line_style, cap_style, join_style);
  XSetFillStyle(display, gc, FillSolid);

  return gc;
}

int main(int argc, char* argv[])
{
  Display* display;		/* pointer to X Display structure.           */
  int screen_num;		/* number of screen to place the window on.  */
  Window win;			/* pointer to the newly created window.      */
  unsigned int display_width,
               display_height;	/* height and width of the X display.        */
  unsigned int width, height;	/* height and width for the new window.      */
  char *display_name = getenv("DISPLAY");  /* address of the X display.      */
  GC gc;			/* GC (graphics context) used for drawing    */
				/*  in our window.			     */
  Colormap screen_colormap;     /* color map to use for allocating colors.   */
  XColor red, brown, blue, yellow, green;
				/* used for allocation of the given color    */
				/* map entries.                              */
  Status rc;			/* return status of various Xlib functions.  */
	int initOffsetBMP;
	XColor col;

  display = XOpenDisplay(display_name);
  if (display == NULL) {
    fprintf(stderr, "%s: cannot connect to X server '%s'\n",
            argv[0], display_name);
    exit(1);
  }

  screen_num = DefaultScreen(display);
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);

  width = display_width;
  height = display_height;
  printf("window width - '%d'; height - '%d'\n", width, height);

  win = create_simple_window(display, width, height, 0, 0);

  gc = create_gc(display, win, 0);
  XSync(display, False);
  screen_colormap = DefaultColormap(display, DefaultScreen(display));

  rc = XAllocNamedColor(display, screen_colormap, "red", &red, &red);
  if (rc == 0) {
    fprintf(stderr, "XAllocNamedColor - failed to allocated 'red' color.\n");
    exit(1);
  }
  rc = XAllocNamedColor(display, screen_colormap, "brown", &brown, &brown);
  if (rc == 0) {
    fprintf(stderr, "XAllocNamedColor - failed to allocated 'brown' color.\n");
    exit(1);
  }
  rc = XAllocNamedColor(display, screen_colormap, "blue", &blue, &blue);
  if (rc == 0) {
    fprintf(stderr, "XAllocNamedColor - failed to allocated 'blue' color.\n");
    exit(1);
  }
  rc = XAllocNamedColor(display, screen_colormap, "yellow", &yellow, &yellow);
  if (rc == 0) {
    fprintf(stderr, "XAllocNamedColor - failed to allocated 'yellow' color.\n");
    exit(1);
  }
  rc = XAllocNamedColor(display, screen_colormap, "green", &green, &green);
  if (rc == 0) {
    fprintf(stderr, "XAllocNamedColor - failed to allocated 'green' color.\n");
    exit(1);
  }
  
  /* draw one pixel near each corner of the window */
  /* draw the pixels in a red color. */
  XSetForeground(display, gc, red.pixel);
  XDrawPoint(display, win, gc, 5, 5);
  XDrawPoint(display, win, gc, 5, height-5);
  XDrawPoint(display, win, gc, width-5, 5);
  XDrawPoint(display, win, gc, width-5, height-5);

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

	col.flags= DoRed | DoGreen | DoBlue;
	
	int i;
	for(i=0; i<76800; i++)
	{
		col.red = buffer[2] * 257;
		col.green = buffer[1] * 257;
		col.blue = buffer[0] * 257;

		if(XAllocColor(display, screen_colormap, &col) == 0)
			printf("Failed to allocate color\n");
		if(XSetForeground(display, gc, col.pixel) == 0)
			printf("Failed to set foreground\n");
		if(XDrawPoint(display,win,gc,i%width,height-1-(i/width)) == 0)
			printf("Failed to draw point\n");
		buffer[0] = getc(home);
		buffer[1] = getc(home);
		buffer[2] = getc(home);
		//printf("X %03d Y %03d R %02x G %02x B %02x\n", i%width, i/width, buffer[0], buffer[1], buffer[2]);
	}





  /* draw two intersecting lines, one horizontal and one vertical, */
  /* which intersect at point "50,100".                            */
  /* draw the line in a brown color. */
  //XSetForeground(display, gc, brown.pixel);
  //XDrawLine(display, win, gc, 50, 0, 50, 200);
  //XDrawLine(display, win, gc, 0, 100, 200, 100);

  /* flush all pending requests to the X server. */
  XFlush(display);

  /* make a delay for a short period. */
  sleep(4);

  /* close the connection to the X server. */
  XCloseDisplay(display);
  return(0);
}
