#include <GL/glut.h>
#include <GL/glu.h>

void draw_axes(){

	//glBegin(GL_LINES);
	//	//x osa
	//	glColor3f(1, 0, 0);
	//	glVertex3f(0, 0, 0);
	//	glVertex3f(10, 0, 0);
	//	//y osa
	//	glColor3f(0, 1, 0);
	//	glVertex3f(0, 0, 0);
	//	glVertex3f(0, 10, 0);
	//	//z osa
	//	glColor3f(0, 0, 1);
	//	glVertex3f(0, 0, 0);
	//	glVertex3f(0, 0, 10);
	//glEnd();


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

}


void set_lights(){

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	GLfloat light_position[] = { 100, 100, 100, 1 };
    
    GLfloat light_diffuse[]  = { 0.7, 0.3, 0.5, 1 };
    GLfloat light_specular[] = { 0.5, 0.5, 0.5, 1 };
    GLfloat light_ambient[]  = { 0.1, 0.1, 0.1, 1 };
    
    //GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    //GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
	//GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1 };



    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);


	//GLfloat light_position[] =  { 10, 10, 10, 0};
	//GLfloat light_diffuse[]  =  { 0., 0.7, 0.4, 1};
	//GLfloat light_specular[] =  { 0.9, 0.9, 0.9, 1};
    //GLfloat light_ambient[]  =  { 0.1, 0.2, 0.1, 1};
    

    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}


void set_maetials(){


    //GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };
    //GLfloat specular_coeffs[] = { 0.2, 0.2, 0.2, 1 };
	//GLfloat ambient_coeffs[] = { 1.0, 0.8, 0.4, 1 };
    //GLfloat shininess = 2;
    
    GLfloat ambient_coeffs[] = { 1.0, 0.1, 0.1, 1 };
	GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };    
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };    
    GLfloat shininess = 20;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);


	//GLfloat diffuse_coeffs[]  = {0.5, 0.4, 0.8, 1};
	//GLfloat specular_coeffs[] = {0.7, 0.7, 0.7, 1};
	//GLfloat ambient_coeffs[]  = {0.1, 0.1, 0.1, 1};
	//GLfloat shininess[] = {2};

	//glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	//glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}



void draw_player(float x_trans){
	glPushMatrix(); 
		glColor3f(0.2, 0.3, 0.8);
		glTranslatef(x_trans, 0, 0);
		glScalef(0.4, 0.4, 0.4);
		glutSolidSphere(1, 20, 20);
	glPopMatrix(); 
}

void draw_path(){
	glPushMatrix();
		glScalef(5, 0.2, 10);
		glColor3f(0.2, 0.3, 0.2);
		glutWireCube(1);
	glPopMatrix();
}