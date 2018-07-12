/*******************************************************************************
 * (c) Copyright 2018 Microsemi SoC Products Group. All rights reserved.
 * v1.0 2017/12/13 anton.krug@microsemi.com
 * Using UART at 115200 baudrate with MiV Core running at 50MHz
 * Requires a 110x40 characters big terminal window
 *
 * Please refer README.md in the root folder of this project for more details.
 * Reference: https://rosettacode.org/wiki/Mandelbrot_set
 */

#include <stdio.h>
#include <float.h>
#include <math.h>

#include "tests/test-utils.h"

#define WIDTH 110
#define HEIGHT 40

#ifndef ITERATIONS
#define ITERATIONS 1 // How many times repeat same the same the frame
#endif
const unsigned int max_iterations = ITERATIONS; // Use by gdb

#ifndef ANIMATION_SPEED
#define ANIMATION_SPEED 0.02f // How large steps are done between the frames
#endif

// https://stackoverflow.com/questions/37538/how-do-i-determine-the-size-of-my-array-in-c
#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

struct MandelbrotView {
  float lookAtX;
  float lookAtY;
  float width;
  float height;
  float gamma;
};

// Coordinate system is similar to Christian Stigen Larsen's approach (Y is upside down as well)
// http://tilde.club/~david/m/
struct MandelbrotView sets[] = {
  {-0.5f,          0.0f,        2.5f,          3.0f,            1.0f},
  {-1.2059523f,   -0.34705183f, 0.36000158f,   0.17550077f,     2.0f},
  {-0.55f,        -0.64f,       0.15f,         0.115f,          4.0f},
  { 0.5f,          0.0f,        1.0f,          2.0f,            1.0f},
  {-0.0068464412f,-0.80686056f, 0.0160606767f, 0.00782957993f, 12.0f}
};



const unsigned char shades[] = { '.', ',', ':', '-', '=', '+', '*', '#', '%', '@'};

const char * microsemiLogo =
  "\n                               .''+'''''';'++++###'\n\
                       ,';:::::::;;;;;;;;''''''''+''++''';'''#:\n\
                 `+';';;;;;;::::::;;;;;;;'''''''''''''`:@+++'';;;;;#\n\
             `;'''''''';;;;;:::::::;:;#':;''''';'+#''''''#  @'+++;;;;;:#\n\
          ,'''''''''''';;;;;::;::::,::';          +@;++,'''+'   @'+;;;;;;;#\n\
        ''''''''''''''''';;;;;;:+;:`                    @@.''''@   #'':';;;;:\n\
      ''''''''''''''+::;;;''++++;                          @@''''@    ;':,;;'+#\n\
     ':, `::;+'''''''''';#@@;                                 ;+'''@    ,:+#+:\n\
    @';;.,;##+.  ;'''                                           '#+''\n\
                 :';;#                                           @+;''@\n\
                 ;#';;+                                            +#`.,    ```.,,,.\n\
       :''''     `:;;;;+                                       .:;,`.........,..,.,,.\n\
   ';;;;;;:,:     ';#;;;;                                `......`````..........,,..`@\n\
   '`';;;;;;;'     :;@';;;'                            `.........``.````........,,, @\n\
   ';'';;;;;;;'     +;,:;;;;:+                        ..............`.``.......... @\n\
    .;',';;;;;;;'    `;'@;;;;;;;':                 ;...................``````..., +@\n\
      ';:.';;;;;;;'    +;;@;;;;;;;;:;:::;'++##:..,,,,,,.................``.``.` @+@\n\
        ;;;';;;;;;;;'    '''#;;;;;;;;;::::::::,,,,,,,,,,,...................` +''\n\
          ';;':,';;;;;;'   #'+'#;;;;;;:::::::::,,,,,,,,,,,,............... @@;'@\n\
             ;;;',,.';;;;;:' #'++#,,':;;;:::::::,,,,,,,,,,,,,,,.,.....`+'',;:\n\
                ';;;;+,..,'';;;;::##'#.;'::;::::::,,,,,,,,,,,,,,..@#@.`.,,`\n\
                    +:;;;;;'+;;;'###+';:::,,.......,,;+@@@@+#@+.`.....`\n\
                         ,+;;''''''''''''''+++++++++++++''';;:,::\n\
                                  `;+###+'''''++###@@#+,\n\
     ___                   ___         ___         ___         ___         ___         ___\n\
    /\\__\\        ___      /\\  \\       /\\  \\       /\\  \\       /\\  \\       /\\  \\       /\\__\\        ___\n\
   /  |  |      /\\  \\    /  \\  \\     /  \\  \\     /  \\  \\     /  \\  \\     /  \\  \\     /  |  |      /\\  \\\n\
  / | |  |      \\ \\  \\  / /\\ \\  \\   / /\\ \\  \\   / /\\ \\  \\   / /\\ \\  \\   / /\\ \\  \\   / | |  |      \\ \\  \\ \n\
 / /| |__|__    /  \\__\\/ /  \\ \\  \\ /  \\~\\ \\  \\ / /  \\ \\  \\ _\\ \\~\\ \\  \\ /  \\~\\ \\  \\ / /| |__|__    /  \\__\\\n\
/ / |    \\__\\__/ /\\/__/ /__/ \\ \\__/ /\\ \\ \\ \\__/ /__/ \\ \\__/\\ \\ \\ \\ \\__/ /\\ \\ \\ \\__/ / |    \\__\\__/ /\\/__/\n\
\\/__/~~/ /  /\\/ /  /  \\ \\  \\  \\/__\\/_|  \\/ /  \\ \\  \\ / /  \\ \\ \\ \\ \\/__\\ \\~\\ \\ \\/__\\/__/~~/ /  /\\/ /  /\n\
      / /  /\\  /__/    \\ \\  \\        | |  /  / \\ \\  / /  / \\ \\ \\ \\__\\  \\ \\ \\ \\__\\       / /  /\\  /__/\n\
     / /  /  \\ \\__\\     \\ \\  \\       | |\\/__/   \\ \\/ /  /   \\ \\/ /  /   \\ \\ \\/__/      / /  /  \\ \\__\\\n\
    / /  /    \\/__/      \\ \\__\\      | |  |      \\  /  /     \\  /  /     \\ \\__\\       / /  /    \\/__/\n\
    \\/__/                 \\/__/       \\|__|       \\/__/       \\/__/       \\/__/       \\/__/\n\
                                                                   Power Matters (tm)\n\
         -=- IEEE 754 Single floating point mandelbrot fractal demonstration -=-\n";


void mandelbrot(float lookAtX, float lookAtY, float width, float height, float gamma) {
  // Calculate boundaries of the fractal
  const float xmin    = lookAtX - (width  / 2);
  const float ymin    = lookAtY - (height / 2);
  const float stepX   = width  / WIDTH;
  const float stepY   = height / HEIGHT;
  const int   maxIter = (float)NELEMS(shades) * gamma;  // Max iterations will affect the "exposure"

  printf("Center=%e,%e width=%e, height=%e\n", lookAtX, lookAtY, width, height);

  for (int cursorY = 2; cursorY < HEIGHT; cursorY++) {
    // skip few lines to allow margins for other text
    const float y = ymin + cursorY * stepY;

    for (int cursorX = 0; cursorX < WIDTH; cursorX++) {
      const float x = xmin + cursorX * stepX;
      float u  = 0.0f;
      float v  = 0.0f;
      float u2 = 0.0f; // u squared
      float v2 = 0.0f; // v squared
      int iter;        // iterations executed

      for (iter = 0 ; iter < maxIter && ( u2+v2 < 4.0f); iter++) {
        v  = 2 * (u*v) + y;
        u  = (u2) - (v2) +x;
        u2 = u * u;
        v2 = v * v;
      }

      // print nothing if iterated too much, or normalize the result and shade accordingly
      printf("%c", (iter >= maxIter) ? ' ' : shades[(int)(iter/gamma)]);
      testAddToChecksumFloat(iter/gamma);
    }
    printf("\n");
  }
}


float rescale(float old, float new, float percentage) {
  // make sure even with overflowed percentage it will compute corectly
  return ((new - old) * fminf(1.0f, fmaxf(0.0f, percentage))) + old;
}


void screenClear() {
#ifdef SERIAL_TERMINAL_ANIMATION
  printf("\033[2J"); // http://www.termsys.demon.co.uk/vtansi.htm
#endif
}


void screenCursorToTopLeft() {
#ifdef SERIAL_TERMINAL_ANIMATION
  printf("\033[0;0H");
#endif
}


int main(int argc, char **argv) {
  screenClear();
  printf(microsemiLogo);
  screenClear();

  // Render following mandelbrot series
  for (int i = 0 ; i < (NELEMS(sets) - 1); i++) {
    for (float percentage = 0.0f; percentage <= 1.3f; percentage += ANIMATION_SPEED) {
      // display motion between the sets:
      // 0.0f to 1.0f will be transitions
      // 1.0f to 1.3f will render same frame (timing without using timer)
      for (int iterate = 0; iterate < ITERATIONS; iterate++) {
        // depending on the #define one image can be repeated multiple times
        const int   iNext   = (i +1) % NELEMS(sets);
        const float lookAtX = rescale(sets[i].lookAtX, sets[iNext].lookAtX, percentage);
        const float lookAtY = rescale(sets[i].lookAtY, sets[iNext].lookAtY, percentage);
        const float width   = rescale(sets[i].width,   sets[iNext].width,   percentage);
        const float height  = rescale(sets[i].height,  sets[iNext].height,  percentage);
        const float gamma   = rescale(sets[i].gamma,   sets[iNext].gamma,   percentage);

        mandelbrot(lookAtX, lookAtY, width, height, gamma);
        screenCursorToTopLeft();
      }
    }
  }
  screenClear();
  printf(microsemiLogo);
  
  testVerify();

#ifndef EXIT_FROM_THE_INFINITE_LOOP
  while(1);
#endif
  
  return 0;
}
