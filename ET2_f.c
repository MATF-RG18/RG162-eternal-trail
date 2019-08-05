#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>

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

float k_z; 
float k_X_l; 
float k_X_d;

extern gsl_matrix_view i;



/*koriste se u f-ji  save_V_invert()
 *IZ NEKOG NEPOZNATOG RAZLOGA MORAJU BITI DEFINISANO GLOBALNO
 *kada su definisani globalno ujeno se i njihovi el.
 *inicijalizuju na 0
 */
double rez[16];
double inv[16];

/*****************************/


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


void set_lights(){

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    //NOTE: ugasiti ako se program sporo izvrsava !!!
   	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  	//NOTE:
    //ako hocemo da si unutrasnjost bude obasjana
    //moramo dodati i odgovarajuce materija za unutrasnjost (za back-face)
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


	glShadeModel(GL_SMOOTH);

	GLfloat light_position[] = { 1, 1, 1, 0 };
    
    GLfloat light_ambient[]  = { 0.7, 0.7, 0.7, 1 }; //ono sto se dobija po sobi od predmete
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1 }; //ono sto direktno dolazi iz izvora svetlosti
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1 }; //odlsja, beli krug, ima ga metal

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);



}


void draw_player(float x_trans){
	
	glPushMatrix(); 
		
		ambient_coeffs[0]  = 0.7; ambient_coeffs[1]  = 0.1; ambient_coeffs[2]  = 0.3; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.7; diffuse_coeffs[1]  = 0.1; diffuse_coeffs[2]  = 0.3; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.5; specular_coeffs[1] = 0.5; specular_coeffs[2] = 0.5; specular_coeffs[3] = 1;
		shininess = 128;
	    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		
		//svetlo koje emituje objekat
		//GLfloat material_emission[] = {0.3, 0.2, 0.2, 0.0};
		//glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);

		


		glTranslatef(x_trans, 0.3, 0);
		//glScalef(0.4, 0.4, 0.4);
		glutSolidSphere(0.3, 100, 100);




		printf("*************************\n");
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

		/*
		for(int k = 0; k<4; k++){

			for(int j = 0; j<4; j++)
				printf("%lf ", gsl_matrix_get(&C.matrix, k, j) );
			printf("\n");
		}
		
		printf("\n");
		*/


	glPopMatrix(); 
}

void draw_path(){
	
	glPushMatrix();	

		ambient_coeffs[0]  = 0.4; ambient_coeffs[1]  = 0.4; ambient_coeffs[2]  = 0.9; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.4; diffuse_coeffs[1]  = 0.4; diffuse_coeffs[2]  = 0.9; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.5; specular_coeffs[1] = 0.5; specular_coeffs[2] = 0.5; specular_coeffs[3] = 1;
		shininess = 10;
	    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		



	    /*ukoliko neka od plca staze prodje zadatu granicu
	     *nadovezuje se na kraj ove druge 
	     *i time se postize beskonacno generisanje staze
	     *u kodu, to se postize resetovanjem vrednosti path_trans_1
	     *i path_trans_2 na pocetnu vredost za polcu br. 2 (odnosno na
	     *pocetnu vredost za promenljivu path_trans_2 definisanu i fajlu ET2.C)
	     *kako bi se u narednom frejmu ploca koja je dostigla granicu nadovezala 
	     *na kraj druge ploce
	     */
	    //TODO: 
	    //random izvlacenje narednog sablona za ravan ide u ovom delu
		if(path_trans_1 >= ghost_border)
			path_trans_1 = -1+path_trans_2;

		if(path_trans_2 >= ghost_border)
			path_trans_2 = -1+path_trans_1;


		
	    //NE DIRATI REDOSLED OVIH TRANSOFRAMCIJA!!!
	    //mora prvo da ide skaliranje pa translacija
	    //kako bi uvek translirari za isti vektor,
	    //bez obzira na skaliranje
		

		//UNDO: 100 -> 10 u glScalef
		glPushMatrix();

			//1. PLOCA STAZES
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5 + path_trans_1);
			glutSolidCube(1);


			

			//kockica na sredini
			glScalef(1.0/4, 1.0/0.125, 1.0/100);
			glTranslatef(0, 0.5, 0);
			glutSolidCube(1);			

			
			/************************/

			k_z = get_z();
			
			k_X_l = get_x_left();
			k_X_d =	get_x_right();

			printf("%f\n", k_z );
			printf("%f %f\n", k_X_l, k_X_d);

			printf("%f %f\n", i_X_l, i_X_d );


			/************************/

			if(k_X_l <= i_X_l && k_X_d >= i_X_d)
				printf("true ");
			else
				printf("false ");

			if(k_z >= i_z_front_tolerance && k_z < i_z_back_tolerance)
				printf("true\n");
			else
				printf("false\n");
			

			if( (k_z >= i_z_front_tolerance && k_z < i_z_back_tolerance) && 
									(k_X_l <= i_X_l && k_X_d >= i_X_d) )
			{
				animation_ongoing = 0;
				printf("SUDAR!\n");
				
			}

			/******************************/


			if(pattern_1 == 1){

				glPushMatrix();
					glScalef(0.6, 4.5, 1);
					glTranslatef(1.8, 0.4, -40);
					glutSolidCube(1);
					glTranslatef(-3.6, 0, 0);
					glutSolidCube(1);
				glPopMatrix();

				
				glPushMatrix();
					glScalef(1.3, 1.5, 0.3);
					glTranslatef(0.9, 0.2, 150);
					glutSolidCube(1);
					glTranslatef(-1.8, 0, 0);
					glutSolidCube(1);
				glPopMatrix();

			}		

			if(pattern_1 == 2){

			}

			if(pattern_1 == 3){

			}


		glPopMatrix();
		



		glPushMatrix();

			//2. PLOCA STAZE
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5+path_trans_2);
			glutSolidCube(1);

			//kockica na sredini
			glScalef(1.0/4, 1.0/0.125, 1.0/100);
			glTranslatef(0, 0.5, 0);
			glutSolidCube(1);



			if(pattern_1 == 1){	

				glPushMatrix();
					glScalef(2.55, 1.5, 0.3);
					glTranslatef(-0.25, 0.15, -100);
					glutSolidCube(1);
				glPopMatrix();


				glPushMatrix();
					glScalef(0.8, 2.8, 0.9);
					glTranslatef(-1.5, 0.2, 50);
					glutSolidCube(1);
					glTranslatef(1.3, 0, 0);
					glutSolidCube(1);
				glPopMatrix();

			}		

			if(pattern_1 == 2){

			}

			if(pattern_1 == 3){

			}


		glPopMatrix();
		

		//NOTE: 
		/* NAKON IZVEDENIH TRANSFORAMCIJA
		 * CEO PROSTOR JE TRNASFORMISAN
		 * T.D. TO UTICE I NA SVE OSTALE TRANSFORMACIJE
		 *
         */

		




		//ako hocemo razmak izmedju ravni mrdnemo se za
		//vise od 1 npr za 1.1 so ce napraviti razmak
		//sirine 1 izmedju 1. ravni i 2. ravni		
		
		
		//printf("p1: %f  p2: %f\n", p1.begining, p2.begining);
		

	glPopMatrix();
}

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
	/*
	for(int i = 0; i<4; i++){

		for(int j = 0; j<4; j++)
			printf("%lf ", gsl_matrix_get(&C.matrix, i, j) );
		printf("\n");
	}
	*/


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