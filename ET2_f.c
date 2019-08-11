#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

#include "ET2.h"

//** GLOABLNI NIZVOVI ZA MATERIJALE **//
//** postavljamo vrednosti ovih nizova pre iscrtavanja objekata **//
GLfloat ambient_coeffs[4];
GLfloat diffuse_coeffs[4];
GLfloat specular_coeffs[4];
GLfloat shininess;
/***************************/


extern float path_trans_1;
extern float path_trans_2;

/*granicnik za transalciju ploca staze (videti f-ju draw_path())
 *vrednost granicnika je zadata eksperimentalno, tako da igrac
 *ne primecuje pomeranje ploca
 */
float ghost_border = 1.5;

//redni brojevi sablona prepreka
int pattern_1 = 1;
int pattern_2 = 1;

/************KOLIZIJE*****************/
extern float x_trans;
extern int animation_ongoing;


float i_X_l; 
float i_X_d;
float i_z_front_tolerance = -0.6;
float i_z_back_tolerance  =  0.0;



extern gsl_matrix_view i;



/*koriste se u f-ji  save_V_invert()
 *IZ NEKOG NEPOZNATOG RAZLOGA MORAJU BITI DEFINISANO GLOBALNO
 *kada su definisani globalno ujedno se i njihovi el.
 *inicijalizuju na 0
 */
double rez[16];
double inv[16];

/************************************/




/*********************************************************************************/
void draw_axes(){

	glPushMatrix();

    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(1000, 0, 0);

        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1000, 0);

        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1000);
    glEnd();

    glPopMatrix();
}

/*********************************************************************************/
void set_lights(){


    
    //NOTE: ugasiti ako se program sporo izvrsava !!!
   	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  	//NOTE:
    //ako hocemo da si unutrasnjost bude obasjana
    //moramo dodati i odgovarajuce materija za unutrasnjost (za back-face)
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


	//glShadeModel(GL_SMOOTH);

	GLfloat light_position[] = { 1, 1, 1, 0 };
    
    GLfloat light_ambient[]  = { 0.3, 0.3, 0.3, 1 }; //ono sto se dobija po sobi od predmete
    GLfloat light_diffuse[]  = { 0.7, 0.7, 0.7, 1 }; //ono sto direktno dolazi iz izvora svetlosti
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 }; //odlsja, beli krug, ima ga metal


	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    



}

/*********************************************************************************/
void draw_player(float x_trans){

	ambient_coeffs[0]  = 0.1745;   ambient_coeffs[1]  = 0.01175;  ambient_coeffs[2]  = 0.01175; ambient_coeffs[3]  = 1;
	diffuse_coeffs[0]  = 0.61424;  diffuse_coeffs[1]  = 0.04136;  diffuse_coeffs[2]  = 0.04136; diffuse_coeffs[3]  = 1;
	specular_coeffs[0] = 0.727811; specular_coeffs[1] = 0.626959; specular_coeffs[2] = 0.626959; specular_coeffs[3] = 1;
	shininess = 0.6*128;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);	


	glPushMatrix(); 
		
		
		//svetlo koje emituje objekat
		//GLfloat material_emission[] = {0.3, 0.2, 0.2, 0.0};
		//glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);


		glTranslatef(x_trans, 0.3, 0);
		//glScalef(0.4, 0.4, 0.4);
		glutSolidSphere(0.3, 100, 100);


		//IZRACUNAVANJE POLOZAJA IGRACA KAKO BI SE 
		//DETEKTOVALA KOLIZIJA SA PREKAMA
		
		double sfrea_mv[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, sfrea_mv);
		gsl_matrix_view s = gsl_matrix_view_array(sfrea_mv, 4, 4);

		double c[] = { 	0.00, 0.00, 0.00, 0.00,
					0.00, 0.00, 0.00, 0.00,
					0.00, 0.00, 0.00, 0.00,
					0.00, 0.00, 0.00, 0.00
				};


		gsl_matrix_view C = gsl_matrix_view_array(c, 4, 4);

		gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &s.matrix, &i.matrix,
					0.0, &C.matrix
				);

		i_X_d =  gsl_matrix_get(&C.matrix, 3, 0) + 0.3*gsl_matrix_get(&C.matrix, 0, 0);
		i_X_l =  gsl_matrix_get(&C.matrix, 3, 0) - 0.3*gsl_matrix_get(&C.matrix, 0, 0);



	glPopMatrix(); 
}

/*********************************************************************************/
void draw_path(){
		

	glPushMatrix();	


		
	    /*ukoliko neka od plca staze prodje zadatu granicu
	     *nadovezuje se na kraj ove druge 
	     *i time se postize beskonacno generisanje staze
	     *u kodu, to se postize resetovanjem vrednosti path_trans_1
	     *i path_trans_2 na pocetnu vredost za polcu br. 2 (odnosno na
	     *pocetnu vredost za promenljivu path_trans_2 definisanu i fajlu ET2.C)
	     *kako bi se u narednom frejmu ploca koja je dostigla granicu nadovezala 
	     *na kraj druge ploce
	     */

		/*u ovom trenutnu se izvlaci i slucajan broj koji
		 *predstavlja broj redni broj sablona prepreka na ploci
		 */

		/*takodje se odlucuje i o tome da li ce
		 *misteriozna sfera biti iscrtana na sablonima	(spooky...) 
		 */

		if(path_trans_1 >= ghost_border){

			path_trans_1 = -1+path_trans_2;			
			
			int rand_num_1;

			rand_num_1 = rand();
			pattern_1 = rand_num_1 % 3 + 1;

		}

		if(path_trans_2 >= ghost_border){

			path_trans_2 = -1+path_trans_1;
			//int rand_num_2 = rand();
			//pattern_2 = rand_num_2 % 3 + 1;
		}







		/*****KRECEMO SA ISCRTAVANJEM STAZE*********/

		glPushMatrix();	

			ambient_coeffs[0]  = 0.2125;    ambient_coeffs[1]  = 0.1275;   ambient_coeffs[2]  = 0.054; ambient_coeffs[3]  = 1;
			diffuse_coeffs[0]  = 0.714;     diffuse_coeffs[1]  = 0.4284;     diffuse_coeffs[2]  = 0.18144; diffuse_coeffs[3]  = 1;
			specular_coeffs[0] = 0.393548; specular_coeffs[1] = 0.271906; specular_coeffs[2] = 0.166721; specular_coeffs[3] = 1;
			shininess = 0.2*128;
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);


			//1. PLOCA STAZES
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5 + path_trans_1);
			glutSolidCube(1);


			//SABLON BR. 1 ZA PRVU PLOCU
			if(pattern_1 == 1){

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					ambient_coeffs[0]  = 0.25;    ambient_coeffs[1]  = 0.20725;   ambient_coeffs[2]  = 0.20725; ambient_coeffs[3]  = 1;
					diffuse_coeffs[0]  = 1.0;     diffuse_coeffs[1]  = 0.829;     diffuse_coeffs[2]  = 0.829; diffuse_coeffs[3]  = 1;
					specular_coeffs[0] = 0.296648; specular_coeffs[1] = 0.296648; specular_coeffs[2] = 0.296648; specular_coeffs[3] = 1;
					shininess = 0.088*128;
					glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
					glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
					glMaterialf(GL_FRONT, GL_SHININESS, shininess);

						

					glTranslatef(0, 0.5, 0);
					glutSolidCube(1);
					check_collision();

					glPushMatrix();

						glPushMatrix();
							glScalef(0.8, 0.8, 0.8);
							glTranslatef(0.5, 0.2, -20);
							glutSolidCube(1);
							check_collision();
						glPopMatrix();


						glScalef(0.8, 4.5, 1);
						glTranslatef(1.8, 0.4, -40);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-3.6, 0, 0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.8, 0.2, 0);
						glScalef(5.2, 0.2, 0.2);
						glutSolidCube(1);


					glPopMatrix();

					
					glPushMatrix();

						glPushMatrix();
							glScalef(1.4, 1, 0.5);
							glTranslatef(-0.4, 0.0, 55);
							glutSolidCube(1);
							check_collision();
						glPopMatrix();

						glScalef(1.3, 1.5, 0.3);
						glTranslatef(0.9, 0.2, 150);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-1.8, 0, 0);
						glutSolidCube(1);
						check_collision();						

					glPopMatrix();

				glPopMatrix();

			}		

			//SABLON BR. 2 ZA PRVU PLOCU
			if(pattern_1 == 2){

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					ambient_coeffs[0]  = 0.135;    ambient_coeffs[1]  = 0.2225;   ambient_coeffs[2]  = 0.1575; ambient_coeffs[3]  = 1;
					diffuse_coeffs[0]  = 0.54;     diffuse_coeffs[1]  = 0.89;     diffuse_coeffs[2]  = 0.63; diffuse_coeffs[3]  = 1;
					specular_coeffs[0] = 0.316228; specular_coeffs[1] = 0.316228; specular_coeffs[2] = 0.316228; specular_coeffs[3] = 1;
					shininess = 0.1*128;
					glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
					glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
					glMaterialf(GL_FRONT, GL_SHININESS, shininess);


					
					glScalef(1.5, 1.3, 0.8);
					glTranslatef(0, 0.5, 0);
					glutSolidCube(1);
					check_collision();

					glPushMatrix();
						glScalef(1/1.8, 1/1.3, 1/0.8);
						glScalef(1.2, 2.5, 1);
						glTranslatef(-1.5, 0.25, 40);						
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.0, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(0.6, 1.3, 1);
						glTranslatef(1.8, 0.1, 0);
						glutSolidCube(1);
						check_collision();


						glTranslatef(-3.5, 0.45, 0);
						glScalef(6.0, 0.1, 1);
						glutSolidCube(1);						
					glPopMatrix();


					glPushMatrix();
						glScalef(1/1.8, 1/1.3, 1/0.8);
						glScalef(1.8, 0.8, 0.3);
						glTranslatef(-0.5, 0, 70);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

					glPushMatrix();
						glScalef(1/1.8, 1/1.3, 1/0.8);
						glTranslatef(-1.0, -0.1, -10);
						glScalef(1.9, 0.9, 0.5);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.2, 0, -25);						
						glutSolidCube(1);
						check_collision();

						glTranslatef(-1.3, 0, -25);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

				glPopMatrix();

			}


			//SABLON BR. 3 ZA PRVU PLOCU
			if(pattern_1 == 3){

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					ambient_coeffs[0]  = 0.4; ambient_coeffs[1]  = 0.4; ambient_coeffs[2]  = 0.9; ambient_coeffs[3]  = 1;
					diffuse_coeffs[0]  = 0.4; diffuse_coeffs[1]  = 0.4; diffuse_coeffs[2]  = 0.9; diffuse_coeffs[3]  = 1;
					specular_coeffs[0] = 0.5; specular_coeffs[1] = 0.5; specular_coeffs[2] = 0.5; specular_coeffs[3] = 1;
					shininess = 0.1*100;
					glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
					glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
					glMaterialf(GL_FRONT, GL_SHININESS, shininess);


					glPushMatrix();
						glScalef(0.7, 1.9, 0.7);
						glTranslatef(0, 0.5, 0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.1, 0, 0);						
						glutSolidCube(1);
						check_collision();

						glScalef(2.7, 1, 1);
						glTranslatef(-1.3, 0, -20);
						glutSolidCube(1);
						check_collision();
					glPopMatrix();

					
					glPushMatrix();
						glScalef(0.8, 3.3, 0.6);
						glTranslatef(-1.5, 0.5, 70);
						glutSolidCube(1);
						check_collision();

						glTranslatef(3,0,0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-1.5, 0, -20);
						glutSolidCube(1);
						check_collision();

						glTranslatef(0, 0.5, 0);
						glScalef(4.5, 0.2, 0.2);
						glutSolidCube(1);					
					glPopMatrix();


					glPushMatrix();

						glScalef(0.5, 2.3, 1);
						glTranslatef(0, 0.5, -40);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-3.2, 0, 0); 
						glutSolidCube(1);
						check_collision();

						glTranslatef(6.4, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(7.4, 0.2, 1);
						glTranslatef(-0.433, 2.2, 0);
						glutSolidCube(1);


					glPopMatrix();

				glPopMatrix();

			}


		glPopMatrix();
		



		glPushMatrix();


			ambient_coeffs[0]  = 0.2125;    ambient_coeffs[1]  = 0.1275;   ambient_coeffs[2]  = 0.054; ambient_coeffs[3]  = 1;
			diffuse_coeffs[0]  = 0.714;     diffuse_coeffs[1]  = 0.4284;     diffuse_coeffs[2]  = 0.18144; diffuse_coeffs[3]  = 1;
			specular_coeffs[0] = 0.393548; specular_coeffs[1] = 0.271906; specular_coeffs[2] = 0.166721; specular_coeffs[3] = 1;
			shininess = 0.2*128;
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
				


			//2. PLOCA STAZE
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5+path_trans_2);
			glutSolidCube(1);



			//SABLON BR. 1 ZA DRUGU PLOCU
			if(pattern_2 == 1){	

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					ambient_coeffs[0]  = 0.25;    ambient_coeffs[1]  = 0.20725;   ambient_coeffs[2]  = 0.20725; ambient_coeffs[3]  = 1;
					diffuse_coeffs[0]  = 1.0;     diffuse_coeffs[1]  = 0.829;     diffuse_coeffs[2]  = 0.829; diffuse_coeffs[3]  = 1;
					specular_coeffs[0] = 0.296648; specular_coeffs[1] = 0.296648; specular_coeffs[2] = 0.296648; specular_coeffs[3] = 1;
					shininess = 0.088*128;
					glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
					glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
					glMaterialf(GL_FRONT, GL_SHININESS, shininess);


					//kockica na sredini
					
					glTranslatef(0, 0.5, 0);
					glutSolidCube(1);
					check_collision();


					glPushMatrix();
						glScalef(2.55, 1.5, 0.3);
						glTranslatef(-0.25, 0.15, -100);
						glutSolidCube(1);
						check_collision();
					glPopMatrix();


					glPushMatrix();
						glScalef(0.8, 2.8, 0.9);
						glTranslatef(-1.5, 0.3, 50);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.3, 0, 0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(0.9, 0, -15);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.3, 0, 0);
						glutSolidCube(1);
						check_collision();
					glPopMatrix();


					glPushMatrix();

						glScalef(1.8, 1, 0.2);
						glTranslatef(0.6, 0, 110);
						glutSolidCube(1);
						check_collision();

						glScalef(1/1.9, 1, 1);
						glScalef(1.3, 1, 1);
						glTranslatef(-1.9, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(1.5, 1, 1);
						glTranslatef(0.1, 0, -60);
						glutSolidCube(1);
						check_collision();

						glScalef(0.7, 1, 1);
						glTranslatef(1.8, 0, 0);
						glutSolidCube(1);
						check_collision();


					glPopMatrix();


					glPushMatrix();

						glScalef(0.4, 4.0, 0.8);
						glTranslatef(-2.0, 0, -15);
						glutSolidCube(1);
						check_collision();

						glTranslatef(4.0, 0, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

					glPushMatrix();

						glScalef(2.0, 1.5, 1);
						glTranslatef(0.5, 0.2, -45);
						glutSolidCube(1);	
						check_collision();

						glScalef(0.2, 2.0, 0.7);
						glTranslatef(0, 0.3, 1);
						glutSolidCube(1);
						check_collision();

						glScalef(10.5, 0.1, 1);
						glTranslatef(-0.4, 2.0, 0);
						glutSolidCube(1);

					glPopMatrix();

				glPopMatrix();
			}		

			//SABLON BR. 2 ZA DRUGU PLOCU
			if(pattern_1 == 2){

			}

			//SABLON BR. 3 ZA DRUGU PLOCU
			if(pattern_1 == 3){

			}


		glPopMatrix();


	glPopMatrix();
}



/*********************************************************************************/
void save_V_inverse(){


	glGetDoublev(GL_MODELVIEW_MATRIX, rez);

	gsl_matrix_view m = gsl_matrix_view_array(rez, 4, 4);

	i = gsl_matrix_view_array(inv, 4, 4);

	gsl_permutation* p = gsl_permutation_alloc(4);
	int s;


	gsl_linalg_LU_decomp(&m.matrix, p, &s);
	gsl_linalg_LU_invert(&m.matrix, p, &i.matrix);

}

double get_z(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);


	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };


	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &mv.matrix, &i.matrix,
					0.0, &C.matrix
				);
	


	return gsl_matrix_get(&C.matrix, 3, 2) + 1*gsl_matrix_get(&C.matrix, 2, 2)/2.0;

}

double get_x_right(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };

	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &mv.matrix, &i.matrix,
					0.0, &C.matrix
				);


	return gsl_matrix_get(&C.matrix, 3, 0) + 1.0*gsl_matrix_get(&C.matrix, 0, 0)/2.0;


}

double get_x_left(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };

	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &mv.matrix, &i.matrix,
					0.0, &C.matrix
				);


	return gsl_matrix_get(&C.matrix, 3, 0) - 1.0*gsl_matrix_get(&C.matrix, 0, 0)/2.0;


}

void check_collision(){

	OBSTACLE obs;
	obs.z   = get_z();
	obs.x_l = get_x_left();
	obs.x_r = get_x_right();


		if( (obs.z >= i_z_front_tolerance && obs.z < i_z_back_tolerance) && 
								!(i_X_d < obs.x_l || i_X_l > obs.x_r) )
		{
			animation_ongoing = 0;
			//printf("SUDAR!\n");
					
		}	
	

}
