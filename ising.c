#include <ncurses.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define DELAY 35000

float float_rand(){
  float num = rand() / (float) RAND_MAX;
  return num;
}

double energy(char * s, int w, int h, float * B, float * J){
  double energy = 0;
  int n_ind[4][2];
  /*Spin correlation contribution to energy*/
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      /*Define neighbour indecies*/
      n_ind[0][0] = i-1;
      n_ind[0][1] = j;
      n_ind[1][0] = i+1;
      n_ind[1][1] = j;
      n_ind[2][0] = i;
      n_ind[2][1] = j+1;
      n_ind[3][0] = i;
      n_ind[3][1] = j-1;
      /*Filter out valid indecies and calcualte corresponding energy*/
      for (int k = 0; k < 4; k++) {
        if(n_ind[k][0] < 0 || n_ind[k][1] < 0){continue;}
        if(n_ind[k][0] >= w || n_ind[k][1] >= h){continue;}
        else {
          energy -= *J * *(s + n_ind[k][0] * h + n_ind[k][1]) * *(s + h * i + j);
        }
      }
    }
  }

  /*Magnetic contribution to energy*/
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      energy += *B * *(s + h * i + j);
    }
  }
  return energy;
}

int main(int argc, char *argv[]) {
  // INIT CURSES SCREEN
  initscr();
  noecho();
  curs_set(FALSE);

  // SET MODEL PARAMETERS ACCORDING TO ARGUMENTS
  float T = 2.5;
  float B = 0.0;
  const float J = 1;

  if (argc >= 2) { sscanf(argv[1], "%f", &T); }
  if (argc == 3) { sscanf(argv[2], "%f", &B); }

  // SET MODEL SIZE FROM TERMINAL SCREEN
  int x = 0;
  int y = 0;
  getmaxyx(stdscr, y, x);
  char spins[x][y];

  // SET ALL SPINS UP (+1)
  for (int i = 0; i < x; i++) {
    for (int j = 0; j <  y; j++) {
      spins[i][j] = 1;
    }
  }

  const float beta = 1/T;
  double Eo = 0;
  double En = 0;

  double prob;
  int rand_x;
  int rand_y;

  while (1) {
    // CLEAR CURSES SCREEN
    clear();

    // UPDATE CURSES SCREEN
    // Spins
    for (int i = 0; i < x; i++) {
      for (int j = 0; j <  y; j++) {
        if (spins[i][j] == 1){ mvprintw(j, i, "+"); }
        else { mvprintw(j, i, "o"); }
      }
    }
    // Info panel
    if (y > 3 && x > 6) {
      mvprintw(y-3, x-15, " B = %.1f\n", B);
      mvprintw(y-2, x-15, " T = %.1f\n", T);
      mvprintw(y-1, x-15, " E = %.1f\n", energy(&spins, x, y, &B, &J));
    }
    refresh();

    /*Calculate updated spins*/
    for (int z = 0; z <= x * y; z++) {
      rand_x = round(x * float_rand());
      rand_y = round(y * float_rand());
      Eo = energy(&spins, x, y, &B, &J);
      spins[rand_x][rand_y] *= -1;
      En = energy(&spins, x, y, &B, &J);
      /* Always accepts the new config if energy is lower */
      if (En <= Eo) { continue; }
      /* If energy is higher, accept at a probability of exp(DELTA E / BETA)*/
      else {
        prob = exp((Eo - En) * beta);
        if (prob <= float_rand()) { spins[rand_x][rand_y] *= -1; }
      }
    }
    usleep(DELAY);
  }

  endwin();
  return 0;
}
