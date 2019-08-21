
#include "et.h"


#define TIMER0 0
#define TIMER1 1


static int window_width, window_height;

static void on_keyboard(unsigned char key,int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_arrow_key_press(int key, int x, int y);
static void on_arrow_key_released(int key, int x, int y);


//****PARAMETRI ANIMACIJE*****//

int animation_ongoing = 0;

int RightKeyPressed = 0;
int LeftKeyPressed = 0;

float ro = sqrt(17);
float sigma = 0;
float fi = 0;
int faktor = 0;


float x_trans = 0;


/*promenljive koje sluzae za trnaslaciju dve ploce staze
 *azuriraju se u on_timer f-ji pre iscratavanje novog frejma
 *koriste se u f-ji draw_path() (koja se nalazi u fajlu ET2_f.c) kao
 *parametri u f-ji translacije kako bi se napravila animacija kretanja staze
 */
float path_trans_1 = 0;
float path_trans_2 = -1;

extern int pattern_1;
extern int pattern_2;

/*INVERZ MATRICE POGLEDA V^(-1)
*koristi se u kao deo mehanizma detekcije kolizija
*za detalje pogledati fajl collision.c
*/
gsl_matrix_view i; 
//******************//

//definicije ovih globalnih parametara 
//pogledati u fajlovima: et_draw.c i collision.c
extern int darkness_timer;
extern int score;
extern int game_over_flag;
extern int ghost_timer;


//******************//



/*------------------------MAIN----------------------------------*/
int main(int argc, char *argv[])
{
	//Inicijalizujemo stanje
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	//pravmo prozor
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,800);
	glutCreateWindow("Eternal Solitude...");

	//pozivi callback funkcija
	glutKeyboardFunc(on_keyboard);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutSpecialFunc(on_arrow_key_press);
 	glutSpecialUpFunc(on_arrow_key_released);

	//boja za ciscenje prozora
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);


	//odabir pocetnih sablona prepreka
	int rand_num;
	srand(time(NULL));
	rand_num = rand();
	pattern_1 = rand_num % 3 + 1;
	
	srand(time(NULL));
	rand_num = rand();
	pattern_2 = rand_num % 3 + 1;

	//ucitavanje high_scora iz datoteke
	load_high_score();

	//glavna petlja
	glutMainLoop();

	return 0;
}

/*--------------------------------------------------------------*/
//na osnovu pritisnutog dugmeta radimo nesto

//UPUTSTVO ZA TASTERE (detaljnija objasnjenjna u f-ji on_display):
//case: b - pokretanje igre
//case: n - pauziranje igre
//case: left_arrow_key, right_arrow_key - pomeranje igraca levo-desno
//case: w,s,a,d - rotacija kamere po sferi
//case: r - ponistavanje kretnji kamere
//case: 27 - izlazak iz programa

static void on_keyboard(unsigned char key,int x, int y){

	switch(key){
		
		case 27:
			exit(1);

		//pokretanje animacije
		case 'b':
			if(!animation_ongoing){
				animation_ongoing = 1;
				glutTimerFunc(50, on_timer, TIMER0);
			}
			break;

		//zaustavljanje animacije
		case 'p':
			if(animation_ongoing){
				animation_ongoing = 0;
				glutTimerFunc(50, on_timer, TIMER0);
			}
			break;

		case 'c':	

			restart_game();	
			glutTimerFunc(50, on_timer, TIMER0);
			break;

		case 'a':
				
				//sferne koor.
				sigma = (sigma + 0.1);
				if(sigma > 180 || sigma < -180)
					sigma = 0;				
				
				faktor = 1;

				if(!animation_ongoing)
					glutTimerFunc(50, on_timer, TIMER1);
			break;

		case 'd':
				
				//sferne koor.
				sigma = (sigma - 0.1);
				if(sigma > 180 || sigma < -180)
					sigma = 0;

				faktor = 1;

				if(!animation_ongoing)
					glutTimerFunc(50, on_timer, TIMER1);

			break;

		case 'w':
				
				//sferne koor.
				fi = (fi - 0.1);
				if(fi > 90 || fi < -90)
					fi = 0;

				faktor = 1;

				if(!animation_ongoing)
					glutTimerFunc(50, on_timer, TIMER1);


			break;

		case 's':

				//sferne koor.
				fi = (fi + 0.1);
				if(fi > 90 || fi < -90)
					fi = 0;

				faktor = 1;


				if(!animation_ongoing)
					glutTimerFunc(50, on_timer, TIMER1);

			break;


		case 'r':
			faktor = 0;

			if(!animation_ongoing)
				glutTimerFunc(50, on_timer, TIMER1);

			break;

	}
}

static void on_arrow_key_press(int key, int x, int y){

	switch(key){

		case GLUT_KEY_LEFT:

			LeftKeyPressed = 1;

			if(!RightKeyPressed){
				
				if(animation_ongoing){
					x_trans -= 0.1;
					if(x_trans <= -2)
						x_trans = -2;						
				}
			}

			break;		

		case GLUT_KEY_RIGHT:

			RightKeyPressed = 1;

			if(!LeftKeyPressed){

				if(animation_ongoing){
				x_trans += 0.1;
				if(x_trans >= 2)
					x_trans = 2;
				}

			}


			break;
	}


}

static void on_arrow_key_released(int key, int x, int y){

	switch(key){

		case GLUT_KEY_LEFT:
			LeftKeyPressed = 0;
			break;

		case GLUT_KEY_RIGHT:
			RightKeyPressed = 0;
			break;
	}

} 

/*--------------------------------------------------------------*/
static void on_timer(int value){
    

    //TIMER0 - tajmer za glavnu animaciju
	//TIMER1 - tajmer za kretanje kamere po zamisljenoj sferi


    /* Azuriranje parametara */
    
    /*ukoluko je valuse == 1 onda callback dolazi od
     *kretanja kamere po sferi pa u tom slucaju ne pomeramo
     *stazu vec samo kameru (tj ovaj if se preskace)
     *time izbegavamo da se staza krece dok pomeramo
     *kameru po zamisljenj sferi, drugim recima ne menjamo 
     *vrednosti path_trans_1 i path_trans_2 pa staza
     *osaje u istom polozaju kada je pauzira igra,
     *odnosno ne ubrazava se kretanje staze kada je igra u toku
     */

	/*ovde se takodje azuriraju tajmeri za ghost_mode i darknes_mode
	 *dakle, svaki put pre iscrtavanja novog frejma
	 */

    if(value != TIMER1){
    	
		path_trans_1 += 0.002;
		path_trans_2 += 0.002;

		darkness_timer -= 0.002;
		if(darkness_timer <= 0)
			darkness_timer = 0;

		ghost_timer -= 0.002;
		if(ghost_timer <= 0)
			ghost_timer = 0;
	}	



    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (animation_ongoing)
   		glutTimerFunc(1, on_timer, TIMER0);
    	
}
/*--------------------------------------------------------------*/
//pozva se kad se promeni razmera prozora

static void on_reshape(int width, int height){
	window_width = width;
	window_height = height;
}
/*--------------------------------------------------------------*/
static void on_display(void){


	//cistimo buffere
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	set_lights(); //pozivamo f-je za postavljanje osvetljenja i materijala

	//
	glViewport(0, 0, window_width, window_height);

	//postaljamo projekciju
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, window_width/(float)window_height, 1, 1500);
	

	
	//POSTAVLJAMO KAMERU
	//POCETNE KOORDINATE KAMERE: 0 1 5

	/*kameru pomeramo po zamisljenoj sferi
	 *ro, sigma i fi su parametri koji opisuju sferu
	 *ro je rastojanje tacke od koodinatnog pocetka (poluprecnik sfere)
	 *sigma i fi su odgovarajuci uglovi
	 *sigma uzima vrednosti od -pi do pi
	 *fi uzima vrednosti od -pi/2 do pi/2
	 *kretnja kamere se postizu prtiskom na tastere 'w','s','a','d'
	 *faktor je parametar koji sluzi za ponistavanje kretnji po sferi
	 *i vracanje kameru u poctni polozaj, taj efekat se postize
	 *mnozenjem sfernih koordinata nulom, faktor uzima vrednosti 0 ili 1
	 *ponistavanje kretnji kamere se desava pritiskom na taster 'r'
	 */

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0 +  faktor*(ro*cos(sigma)*sin(fi)), //x 
			  1 +  faktor*(ro*sin(sigma)*sin(fi)), //y 
			  4 +  faktor*(ro*cos(fi)),            //z 
			  0, 0, 0,					           //loook-at
			  0, 1, 0);                            //up

	
	

	//PAMTIMO INVERZ MATRICE V (MATRICE POGLEDA) NA SAMOM POCETKU
	//KAKO BI GA PONISTILI U MODELVIEW (MV) MATRICI MNOZENJEM MATRICA
	// V^(-1) * VM = M 
	//VIDETI DETALJE U collision.c
	save_V_inverse();

	print_score(score);
	
	draw_lives();
	
	//draw_axes(); //pozivamo f-ju za iscrtavanje koordinatnih osa


	/* x_trans je parametar koji se menja pritiskom 
	 * tastera '<-' ili '->'. Sluzi za pomeranje igraca levo-desno.
	 * Prosledjuje se f-ji za iscrtavanja igraca kako bi se uz
	 * pomoc translacije za zadatu vrednost parametra igrac iscrtao na
	 * odgovarajucoj poziciji (videti f-ju draw_player, fajl ET2_f.c)
	 */
	draw_player(x_trans); //pozivamo f-ju za iscrtavanje igraca

	draw_path(); //pozivamo f-ju za iscrtavanje staze

		
	if(game_over_flag){
		animation_ongoing = 0;
		game_over(score);		
	}
	

	//zamenjujemo sadrzaje buffera
	glutSwapBuffers();
}

