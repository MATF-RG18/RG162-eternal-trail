#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>


#include "ET2.h"

static int window_width, window_height;

static void on_keyboard(unsigned char key,int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);


//****PARAMETRI*****//

static int timer_active = 0;


float ro = 1.5*sqrt(10);
float sigma = 0;
float fi = 0;
int faktor = 0;

//
float x_trans = 0;



//******************//


/*------------------------MAIN----------------------------------*/
int main(int argc, char *argv[])
{
	//Inicijalizujemo stanje
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	//pravmo prozor
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,800);
	glutCreateWindow("Eternal Solitude...");

	//pozivi callback funkcija
	glutKeyboardFunc(on_keyboard);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);

	//boja za ciscenje prozora
	glClearColor(0, 0, 0, 0);

	//glavna petlja
	glutMainLoop();

	return 0;
}

/*--------------------------------------------------------------*/
//na osnovu pritisnutog dugmeta radimo nesto

//UPUTSTVO ZA TASTERE (detaljnija objasnjenjna u f-ji on_display): 
//case: w,s,a,d - rotacija kamere po sferi
//case: j,l - pomeranje igraca levo-desno
//case: r - ponistavanje kretni kamere
static void on_keyboard(unsigned char key,int x, int y){

	switch(key){
		
		case 27:
			exit(1);


		case 'a':						
			if(!timer_active){
				
				//sferne koor.
				sigma = (sigma + 0.1);
				if(sigma > 180 || sigma < -180)
					sigma = 0;				
				
				faktor = 1;

				
				glutTimerFunc(50, on_timer, 0);
				//timer_active = 1;
			}

			//if(timer_active)
				//timer_active = 0;

			break;

		case 'd':
			if(!timer_active){
				
				//sferne koor.
				sigma = (sigma - 0.1);
				if(sigma > 180 || sigma < -180)
					sigma = 0;

				faktor = 1;

				glutTimerFunc(50, on_timer, 0);
				//timer_active = 1;
			}

			//if(timer_active)
				//timer_active = 0;

			break;

		case 'w':			
			if(!timer_active){
				
				//sferne koor.
				fi = (fi - 0.1);
				if(fi > 90 || fi < -90)
					fi = 0;

				faktor = 1;

				glutTimerFunc(50, on_timer, 0);
				//timer_active = 1;
			}

			//if(timer_active)
				//timer_active = 0;

			break;

		case 's':			
			if(!timer_active){

				//sferne koor.
				fi = (fi + 0.1);
				if(fi > 90 || fi < -90)
					fi = 0;

				faktor = 1;

				glutTimerFunc(50, on_timer, 0);
				//timer_active = 1;
			}

			//if(timer_active)
				//timer_active = 0;

			break;

		case 'r':
			faktor = 0;
			glutTimerFunc(50, on_timer, 0);
			//timer_active = 0;
			break;

		case 'l':

			x_trans += 0.1;

			if(x_trans >= 2)
				x_trans = 2;

			glutTimerFunc(50, on_timer, 0);	

			break;
		
		case 'j':

			x_trans -= 0.1;

			if(x_trans <= -2)
				x_trans = -2;	
			
			glutTimerFunc(50, on_timer, 0);

			break;

	}
}
/*--------------------------------------------------------------*/
static void on_timer(int value){
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;

    /* Azuriranje parametara */
    

    


    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (timer_active)
        glutTimerFunc(50, on_timer, 0);
}
/*--------------------------------------------------------------*/
//pozva se kad se promeni razmera prozora

static void on_reshape(int width, int height){
	window_width = width;
	window_height = height;
}
/*--------------------------------------------------------------*/
static void on_display(void){


	//cistimo buffer boja
	glClear(GL_COLOR_BUFFER_BIT);

	//
	glViewport(0,0,window_width,window_height);

	//postaljamo projekciju
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,window_width/window_height,1,1500);


	glPushMatrix();
	
	//POSTAVLJAMO KAMERU
	//POCETNE KOORDINATE KAMERE: 0 1 5

	//kameru pomeramo po zamisljenoj sferi
	//ro, sigma i fi su parametri koji opisuju sferu
	//ro je rastojanje tacke od koodinatnog pocetka (poluprecnik sfere)
	//sigma i fi su odgovarajuci uglovi
	//sigma uzima vrednosti od -pi do pi
	//fi uzima vrednosti od -pi/2 do
	//kretnja kamere se postizu prtiskom na tastere 'w','s','a','d'
	/* faktor je parametar koji sluzi za ponistavanje kretnji po sferi
	 * i vracanje kameru u poctni polozaj, taj efekat se postize
	 * mnozenjem sfernih koordinata nulom, faktor uzima vrednosti 0 ili 1
	 * ponistavanje kretnji kamere se desava pritiskom na taster 'r'
	 */

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0 +  faktor*(ro*cos(sigma)*sin(fi)), //x
			  1 +  faktor*(ro*cos(fi)),            //y (z iz sfernih koor)
			  4 +  faktor*(ro*sin(sigma)*sin(fi)), //z (y iz sfernih koord)
			  0, 0, 0,					           //loook-at
			  0, 1, 0);                            //up

	glPopMatrix();
	
	//pozivamo f-je za postavljanje osvetljenja i materijala
	set_lights();
	set_maetials();
	
	//pozivamo f-ju za iscrtavanje koordinatnih osa
	draw_axes();

	//pozivamo f-ju za iscrtavanje staze
	draw_path();
	
	//pravimo naseg igraca
	/* x_trans je parametar koji se menja pritiskom 
	 * tastera 'j' ili 'l'. Sluzi za pomeranje igraca levo-desno.
	 * Prosledjuje se f-ji za iscrtavanja igraca kako bi se uz
	 * pomoc translacije za zadatu vrednost parametra igrac iscrtao na
	 * odgovarajucoj poziciji (videti f-ju draw_player)
	 */
	draw_player(x_trans);


	//zamenjujemo sadrzaje buffera
	glutSwapBuffers();
}
