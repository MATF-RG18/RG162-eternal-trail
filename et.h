#ifndef _ET_H_
#define _ET_H_

#include <GL/glut.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>



typedef struct obstacle{

	/*leva i desna granica prepreke po x 
	 *zajedno definisu sirinu prepreke po x osi
	 *kako bi se utvrdilo da li je igrac izbegao prepreku 
	 *ili je doslo do kolizije.
	 *z koordinata predstavlja z koordinatu centra prednje pljosni prepreke
	 *sluzi da bi se utvrdilo da li je prepreka dovoljno prisla
	 *igracu.
	 *ukoliko su oba ova uslova ispunjena doslo je do kolizije i igra se prekida
	 */ 

	double z;     //z koordinata prepreke
	double x_l;	  //leva granica prepreke po x
	double x_r;   //desna granica prepreke po x

} OBSTACLE;


void draw_axes();
void set_lights();
void set_materials(char* material_name);
void draw_player(float x_trans);
void draw_path();

void   save_V_inverse();
double get_z_front();
double get_z_back();
double get_x_left();
double get_x_right();
void   check_collision();

void darkness();
void print_score();
void draw_lives();
void game_over(int score_f);

void load_high_score();
void save_new_high_score();
void restart_game();



#endif
