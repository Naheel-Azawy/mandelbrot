#define w       12
#define h       12
#define maxiter 25

typedef struct {
  double r, i;
} complex;

double cR;
double cI;

double xmin, xmax, ymin, ymax;
double dx, dy;

void put(unsigned int px, unsigned int py, unsigned int iter);

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

void run(int use_julia) {
  xmin = -1.5;
  xmax = 1.5;
  ymin = -1.5;
  ymax = 1.5;

  dx = (xmax - xmin) / w;
  dy = (ymax - ymin) / h;

  cR = -0.776758703673039;
  cI = 0.135566095570315;

  if (use_julia) {
    julia();
  } else {
    mandelbrot();
  }
}

#ifdef ARDUINO

#include <KeyChainino.h>

unsigned long timer1_count_;
bool game_started = true;

ISR(TIMER1_OVF_vect) {
  // disable Timer1 interrupts
  TIMSK1 &= ~(1 << TOIE1);
  TCNT1 = timer1_count_;  // load counter for next interrupt
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
}

ISR(INT1_vect) { // BUTTON_A INTERRUPT
  // do nothing
}

ISR(INT0_vect) { // BUTTON_B INTERRUPT
  // do nothing
}

void put(unsigned int px, unsigned int py, unsigned int iter) {
  KC.pixel(px, py, iter >= maxiter ? 1 : 0);
}

void setup() {
  // initialize and clear display
  KC.init();
  KC.clear();
  KC.display();

  // configure buttons pins
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

  // initialize Timer 1
  timer1_count_ = 64900;
  noInterrupts();          // disable interrupts
  TCCR1A = 0;              // normal operation
  TCCR1B = 0;              // clear prescaler
  TCNT1 = timer1_count_;   // load counter
  TCCR1B |= (1 << CS10);   // prescaler = 1024
  TIMSK1 |= (1 << TOIE1);  // enable timer overflow
  interrupts();            // enable all interrupts

  KC.goSleep();
}

void show(int use_julia) {
  KC.clear();
  run(use_julia);
  KC.display();
  delay(5000);
}

void loop() {
  if (game_started) {
    show(false);
    show(true);
    game_started = false;
  } else {
    KC.goSleep(); // sleep to reduce power
    game_started = true;
  }
}

#else

#include <stdio.h>

char mat[w][h];

void put(unsigned int px, unsigned int py, unsigned int iter) {
  mat[px][py] = iter >= maxiter ? 1 : 0;
}

void show(int use_julia) {
  run(use_julia);
  for (int j = 0; j < w; ++j) {
    for (int i = 0; i < w; ++i) {
      printf("%s", mat[i][j] ? "##" : "  ");
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {
  show(0);
  show(1);
  return 0;
}

#endif
