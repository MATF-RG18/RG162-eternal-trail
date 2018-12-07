#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>

static int window_width, window_height;

static void on_keyboard(unsigned char key,int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);


int main(int argc, char *argv[])
{
	//Inicijalizujemo stanje
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	//pravmo prozor
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	glutCreateWindow("Eternal Solitude...");

	//pozivi callback funkcija
	glutKeyboardFunc(on_keyboard);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);


	//boja za ciscenje prozora
	glClearColor(0.5, 0.5, 0.5, 0);

	//glavna petlja
	glutMainLoop();

	return 0;
}

//pozva se kad se promeni razmera prozora
static void on_reshape(int width, int height){
	window_width = width;
	window_height = height;
}

//na osnovu pritisnutog dugmeta radimo nesto
static void on_keyboard(unsigned char key,int x, int y){

	switch(key){
		case 27:
			exit(1);
	}


}
static void on_display(void){


	//cistimo buffer boja
	glClear(GL_COLOR_BUFFER_BIT);

	//
	glViewport(0,0,window_width,window_height);

	//postaljamo projekciju
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,window_width/window_height,1,1500);

	//postavljamo kameru
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3, 3, 3,
			  0, 0, 0,
			  0, 1, 0);


	//crtamo koordinate
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
	glEnd();

	//pravimo naseg igraca

	glPushMatrix(); //stavljamo staru matricu stanja na stek
	glTranslatef(0,1,0);
	glutWireSphere(1, 20, 20);
	glPopMatrix(); //vracamo staru matricu sa steka

	glColor3f(1, 0, 0);
	glutWireCube(1);


	//zamenjujemo sadrzaje buffera
	glutSwapBuffers();


}
