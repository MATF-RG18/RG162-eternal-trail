#ifndef _ET_H_
#define _ET_H_

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>



typedef struct plane{

	float begining;
	float end;

} PLANE;


void draw_axes();
void set_lights();
void set_maetials();
void draw_player(float x_trans);
void draw_path();

void   save_V_inverse();
double get_z();
double get_x_left();
double get_x_right();



#endif