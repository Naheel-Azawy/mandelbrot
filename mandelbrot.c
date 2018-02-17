//#!/bin/execute -c "-lm -Wall" julia7885
// TODO: https://www.cs.colorado.edu/~srirams/courses/csci2824-spr14/gmpTutorial.html
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <windows.h> // terminal_size, Sleep
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h> // nanosleep
#else
#include <unistd.h> // usleep
#endif
#ifndef WIN32
#include <sys/ioctl.h> // terminal_size
#endif

void sleep_ms(int milliseconds) {
#ifdef WIN32
	Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
#else
	usleep(milliseconds * 1000);
#endif
}

typedef struct {
	unsigned int w, h;
} size;

size get_terminal_size() {
	size s;
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	s.w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	s.h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize max;
	ioctl(0, TIOCGWINSZ , &max);
	s.w = max.ws_col;
	s.h = max.ws_row;
#endif
	return s;
}

typedef struct {
	double r, i;
} complex;

const unsigned char black[] = {0, 0, 0, 0, 0, 0};
unsigned char color[6];
char *ascii_chars;
unsigned int ascii_chars_len;
FILE* f;
bool to_stdout;

double xmin, xmax, ymin, ymax;
double dx, dy;

char *stdout_buffer = NULL;
size_t stdout_buffer_count = 0;
size_t stdout_buffer_size = 0;

char*			output;
bool			autores;
unsigned int	w;
unsigned int	h;
unsigned int	maxiter;
double			zoom;
double			pR;
double			pI;
bool			use_julia;
double			cR;
double			cI;

void put(unsigned int px, unsigned int py, unsigned int iter) {
	if (to_stdout) {
		if (iter >= maxiter) {
			//putchar(' ');
			stdout_buffer[stdout_buffer_count++] = ' ';
		} else {
			//putchar(ascii_chars[iter % ascii_chars_len]);
			stdout_buffer[stdout_buffer_count++] = ascii_chars[iter % ascii_chars_len];
		}
		if (to_stdout && px == w-1) {
			//putchar('\n');
			stdout_buffer[stdout_buffer_count++] = '\n';
		}
	} else {
		if (iter >= maxiter) {
			fwrite(black, 6, 1, f);
		} else {
			color[0] = iter >> 8;
			color[1] = iter & 255;
			color[2] = iter >> 8;
			color[3] = iter & 255;
			color[4] = iter >> 8;
			color[5] = iter & 255;
			fwrite(color, 6, 1, f);
		}
	}
}

void mandelbrot() {
	complex c;
	complex z;
	complex z2;
	unsigned int px, py;
	unsigned int iter;
	for (py = 0; py < h; py++) {
		c.i = ymax - py * dy;
		for (px = 0; px < w; px++) {
			c.r = xmin + px * dx;
			z.r = z2.r = z.i = z2.i = 0;
			for (iter = 1; iter < maxiter && (z2.r + z2.i < 4); iter++) {				
				z.i = 2 * z.r * z.i + c.i;
				z.r = z2.r - z2.i + c.r;
				z2.r = z.r * z.r;
				z2.i = z.i * z.i;
			}
			put(px, py, iter);
		}
	}
}

void julia() {
	complex c = { .r=cR, .i=cI };
	complex z;
	complex z2;
	unsigned int px, py;
	unsigned int iter;
	for (py = 0; py < h; py++) {
		for (px = 0; px < w; px++) {
			z.r = xmin + px * dx;
			z.i = ymax - py * dy;
			z2.r = z.r * z.r;
			z2.i = z.i * z.i;
			for (iter = 1; iter < maxiter && (z2.r + z2.i < 4); iter++) {				
				z.i = 2 * z.r * z.i + c.i;
				z.r = z2.r - z2.i + c.r;
				z2.r = z.r * z.r;
				z2.i = z.i * z.i;
			}
			put(px, py, iter);
		}
	}
}

void set_values(
	char*			_output,
	bool			_autores,
	unsigned int	_w,
	unsigned int	_h,
	unsigned int	_maxiter,
	double			_zoom,
	double			_pR,
	double			_pI,
	bool			_use_julia,
	double			_cR,
	double			_cI
) {
	output		= _output;
	autores		= _autores;
	w			= _w;
	h			= _h;
	maxiter		= _maxiter;
	zoom		= _zoom;
	pR			= _pR;
	pI			= _pI;
	use_julia	= _use_julia;
	cR			= _cR;
	cI			= _cI;
	if (autores) {
		size s = get_terminal_size();
		w = s.w;
		h = s.h - 3;
	} else {
		w = w;
		h = h;
	}

	double x, y;
	double d;
	double z = exp(zoom);
	if (w >= h) {
		xmin = pR-2/z;
		xmax = pR+2/z;
		x = xmax - xmin;
		d = ((x * h) / w) / 2;
		ymin = pI-d;
		ymax = pI+d;
		y = ymax - ymin;
	} else {
		ymin = pI-2/z;
		ymax = pI+2/z;
		y = ymax - ymin;
		d = ((y * w) / h) / 2;
		xmin = pR-d;
		xmax = pR+d;
		x = xmax - xmin;
	}

	to_stdout = strcmp(output, "stdout") == 0;
	if (to_stdout) {
		ymin *= 2;
		ymax *= 2;
		y = ymax - ymin;
		stdout_buffer_count = 0;
		size_t new_size = (w + 1) * h;
		if (new_size != stdout_buffer_size) {
			stdout_buffer_size = new_size;
			stdout_buffer = (char*) malloc(stdout_buffer_size + 1);
		}
	} else {
		f = fopen(output, "wb");
		fprintf(f, "P6\n%d\n%d\n%d\n", w, h, maxiter < 256 ? 256 : maxiter);
	}
	
	dx = x / w;
	dy = y / h;
}

void print_vals() {
	printf("%s %d %d %d %f %f %f", output, w, h, maxiter, zoom, pR, pI);
	if (use_julia)
		printf(" %f %f", cR, cI);
	printf("\n");
}

void end() {
	if (to_stdout) {
		stdout_buffer[stdout_buffer_count] = '\0';
		puts(stdout_buffer);
	} else {
		fclose(f);
	}
}

// c = 0.7885e^(ia) ; a = 0..2 pi
void julia7885(bool autores, unsigned int w, unsigned int h, double step, int delay) {
	const double pi = 3.14159265358979323844;
	const double r  = 0.7885;
	for (;;) for (double a = 0; a <= 2; a += step) {
		set_values(
			"stdout", autores, w, h,
			100, 0, 0, 0, true,
			(r * cos(a * pi)),
			(r * sin(a * pi))
		);
		julia();
		end();
		print_vals();
		sleep_ms(delay);
	}
}

void brotzoom(bool autores, unsigned int w, unsigned int h, double step, int delay) {
	for (double z = 0;; z += step) {
		set_values(
			"stdout", autores, w, h,
			2000*z, z, -1.5, 0,
			false, 0, 0
		);
		mandelbrot();
		end();
		print_vals();
		sleep_ms(delay);
	}
}

void err() {
	printf("Usage:   mandelbrot <out.ppm | stdout> <w> <h> <maxiter> <zoom> <pR> <pI> [<cR> <cI>]\n");
	printf("Usage:   mandelbrot julia7885 [<w> <h> <step> <delay>]\n");
	printf("Usage:   mandelbrot brotzoom [<w> <h> <step> <delay>]\n");
	printf("Example: mandelbrot pic.ppm 5760 3240 1200 10.3 0.270925 0.004747\n");
	printf("Example: mandelbrot pic.ppm 5760 3240 1500 0 0 0 -0.8 0.156\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	ascii_chars = "AB:!+%$@#=";
	ascii_chars_len = strlen(ascii_chars);
	if (argc == 8 || argc == 10) {
		bool use_julia = (argc >= 10);
		if (use_julia) {
			cR = atof(argv[8]);
			cI = atof(argv[9]);
		}
		int w = atoi(argv[2]);
		int h = atoi(argv[3]);
		set_values(
			argv[1], (w == -1 || h == -1), w, h, atoi(argv[4]),
			atof(argv[5]), atof(argv[6]), atof(argv[7]), use_julia, cR, cI
		);
		if (use_julia) {
			julia();
		} else {
			mandelbrot();
		}
		end();
	} else if (argc >= 2 && strcmp(argv[1], "julia7885") == 0) {
		if (argc == 2) {
			julia7885(true, -1, -1, 0.001, 30);
		} else if (argc == 6) {
			int w = atoi(argv[2]);
			int h = atoi(argv[3]);
			julia7885((w == -1 || h == -1), w, h, atof(argv[4]), atoi(argv[5]));
		} else {
			err();
		}
	} else if (argc >= 2 && strcmp(argv[1], "brotzoom") == 0) {
		if (argc == 2) {
			brotzoom(true, -1, -1, 0.01, 30);
		} else if (argc == 6) {
			int w = atoi(argv[2]);
			int h = atoi(argv[3]);
			brotzoom((w == -1 || h == -1), w, h, atof(argv[4]), atoi(argv[5]));
		} else {
			err();
		}
	} else {
		err();
	}
	return 0;
}




