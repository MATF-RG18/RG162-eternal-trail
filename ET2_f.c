#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>

#include "ET2.h"

//** GLOABLNI NIZVOVI ZA MATERIJALE **//
//** vrednosti ovih nizova se postavljaju pre iscrtavanja objekata **//
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
		if(path_trans_1 >= ghost_border)
			path_trans_1 = -1.04+path_trans_2;

		if(path_trans_2 >= ghost_border)
			path_trans_2 = -1.04+path_trans_1;
		

		glPushMatrix();

			//1. PLOCA STAZE
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5 + path_trans_1);
			glutSolidCube(1);

			//kockica na sredini
			glScalef(1.0/4, 1.0/0.125, 1.0/100);
			glTranslatef(0, 0.5, 0);
			glutSolidCube(1);			
					

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

		glPopMatrix();
		
		

	glPopMatrix();
}