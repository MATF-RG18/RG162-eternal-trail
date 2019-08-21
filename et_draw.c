
#include "et.h"



extern float path_trans_1;
extern float path_trans_2;

/*granicnik za transalciju ploca staze (videti f-ju draw_path())
 *vrednost granicnika je zadata eksperimentalno, tako da igrac
 *ne primecuje pomeranje ploca
 */
float ghost_border = 1.5;

/*redni brojevi sablona prepreka za svaku plocu posebno:
 *u zavisnosti od izvucenog broja bira se jedan od sablona
 *koji ce biti iscrtan na ploci. svaka ploca ima po tri moguca
 *sablona za izbor, sto je ukupno 6 razlicitih zablona prepreka
 *inicijalno, u main f-ji, biraju se sabloni, zatim tokom animacije
 *svaki put kada ploce prodju ghost_border, i pri restartovanju igre
 *u f-ji restart_game() (game_state.c)
 */
int pattern_1;
int pattern_2;

/************KOLIZIJE*****************/
/*i_X_l i i_X_r su promenljive koje prate levu tj desnu granicu 
 *igraca, odnosno "bokove" lopte, koriste se pri proveri kolizija
 *igraca sa prerpekama da bi utvrdilo da li je igrac udario u prepreku
 *ili nije, azuriraju se pri svakom iscrtavanju igraca u f-ji draw_player
 */
extern float x_trans; //parametar za translaciju igraca
float i_X_l; 
float i_X_r;
extern gsl_matrix_view i;
/************************************/


/*************DODATNE STVARI**********/

/*********DARKNESS_MODE PARAMETRI***************/
//DOKLE GOD TRAJE OVAJ MOD STAZA SE ISCRTAVA CRNOM BOJOM 
//STO OTEZAVA IZBEGAVANJE PREPREKA
//AKTIVIRA SE KOLIZIJOM SA CRNOM SFEROM

/*promenljiva za promenu materijala staze na crnu boju
 *sluzi kao indikator trajanja darknes_moda, podize se 
 *prilikom sudara sa crnim sferama i omogucava 
 *iscrtavanje staze u crnoj boji promenom materijla
 *tako sto ze zove fja darkness() ukoliko je ova zastavica podignuta
 *pogledati fju draw_path gde se ova zastavica koristi
 *fju check_collision() (collision.c) gde se ova zastavica postavlja
 */
int black_flag = 0;


/*promenljiva koja signalizira da se provereava kolizija sa
 *sferom. postavlja se iz razloga sto se racunanje koordinata sfera
 *(radi provere kolizije sa igracem), za nijansu razlikuje od
 *racunnanj istih tih koordinata za ostale prepreke na stazi.
 *uzima vrednosti 0, 1, 2 ili 3:
 *0 - ne proverava se kolizija sa sferom
 *1 - sudar sa crnom sferon koji aktivira darknes_mode
 *2 - sudar sa zlatnom sferom koji donosi novi zivot
 *3 - sudar sa plavom sferom koji ativira ghost_mode
 */
int sphere_collision = 0; 

/*promenljiva koja sluzi kao zabrana crtanja sfere na stazi 
 *u narednim frejmovima nakon kolizije igraca sa njom
 *kako bi se postigao efekat "skupljanja" sfere
 *navedene su dve promenljive za zabranu crtanja na svkoj od ploca staze
 */
int draw_dark_orb_1 = 0;
int draw_dark_orb_2 = 0;

/*tajmer za duzinu trajanja darkness-moda
 *azirira se u on_timer f-ji
 *(videti on_timer f-ju i nacin na koji se tajmer azurura)
 */
int darkness_timer = 0;

/*********LIVES PARAMETRI***************/
//VODE EVIDENCIJU O PREOSTALIM ZIVOTIMA I O SKUPLJANJU
//NOVIH ZIVOTA (KOLIZIJA SA ZUTOM SFEROM = +1 ZIVOT)

/*inicijalno, igrac ima 3 zivota koja se smanjuju pri svakom sudaru
 *sa preprekom, o preostalim zivotima vodi racuna 
 *globalna promenljiva lives_remain
 *draw_life_orb_2 i draw_life_orb_2 sluzi istoj svrsi kao i 
 *draw_dark_orb_1 i draw_dark_orb_2, tu su kako bi se stvorio
 *efekat "skupljanja" zivota tako sto brane isrtavanje zlatnih sfera
 *u narednim frejmovima nakom kolizije
 */
int lives_remain = 3;
int draw_life_orb_1 = 0;
int draw_life_orb_2 = 0;


/*********GHOST_MODE PARAMETRI***************/
//DOKLE GOD TRAJE OVAJ MOD IGRAC MOZE DA PROLAZI KROZ PREPREKE
//AKTIVIRA SE KOLIZIJOM SA PLAVOM SFEROM

/*ove promenljive su anologne promenljivama 
 *za darknes_mode
 */
int draw_ghost_orb_1 = 0;
int draw_ghost_orb_2 = 0;
int ghost_mode_flag = 0;
int ghost_timer = 0;

/************************************/






/*********************************************************************************/
//FJA ZA STAMPANJE SKORA U GORNJEM DESNOM UGLU EKRANA
void print_score(int score_a){

	glPushMatrix();

	
	set_materials("white");


	char string_score[5];
	char score_text[] = "SCORE: ";

	int j = score_a%10;	
	score_a = score_a/10;
	int d = score_a%10;
	score_a = score_a/10;
	int s = score_a%10;
	score_a = score_a/10;
	int h = score_a%10;

	string_score[0] = '0' + h;
	string_score[1] = '0' + s;
	string_score[2] = '0' + d;
	string_score[3] = '0' + j;
	string_score[4] = '\0';
	

	glColor3f(1.0, 0.0, 0.0);
	glRasterPos3f(1.5, 2.0, 0);
	char* c;
	for(c = score_text; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	for(c = string_score; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

	glPopMatrix();
}
/*********************************************************************************/
//FJA ZA ISCRTAVNJE PREOSTALIH ZIVOTA U GORNJEM LEVOM UGLU EKRANA
//PUNOM SFEROM SE ISRTAVAJU PREOSTALI ZIVOTI DOK ZICANE SFERE 
//PREDSTAVLJAJU IZGUBLJENE ZIVOTE
void draw_lives(){

	glPushMatrix();

		
		set_materials("gold");

		glScalef(0.9, 0.9, 1);
		glTranslatef(-2.6, 2.2, 0);

		if(lives_remain == 3)
			glutSolidSphere(0.08, 100, 100);
		else
			glutWireSphere(0.08, 10, 10);

		glTranslatef(0.25, 0, 0);

		if(lives_remain >= 2)
			glutSolidSphere(0.08, 100, 100);
		else
			glutWireSphere(0.08, 10, 10);

		glTranslatef(0.25, 0, 0);

		if(lives_remain >= 1)
			glutSolidSphere(0.08, 100, 100);
		else
			glutWireSphere(0.08, 10, 10);

	glPopMatrix();
}
/*********************************************************************************/
//FJA ZA ISCRTAVANJE IGRACA NA STAZI
//OSIM STO ISCRTAVA IGRACA PRATI I NJEGOVE KOORDINATE RADI PROVERE KOLIZIJA
void draw_player(float x_trans){

	
	set_materials("ruby red");

	//ukoliko traje ghost_mode igraca iscrtavamo plavom bojom
	if(ghost_mode_flag)
		set_materials("light blue");

	
	glPushMatrix(); 
		
	

		//iscrtavamo igraca
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

		i_X_r =  gsl_matrix_get(&C.matrix, 3, 0) + 0.3*gsl_matrix_get(&C.matrix, 0, 0);
		i_X_l =  gsl_matrix_get(&C.matrix, 3, 0) - 0.3*gsl_matrix_get(&C.matrix, 0, 0);



	glPopMatrix(); 
}

/*********************************************************************************/
void draw_path(){
		

	glPushMatrix();	

		/*PLOCE staze predstavljaju dva izduzena kvadra koja se
		 *naizmenicno postavljaju jedan iza drugog i transliraju
		 *ka igracu, praveci tako efekat beskonacne staze
		 */
		
	    /*ukoliko neka od ploca staze prodje zadatu granicu
	     *nadovezuje se na kraj ove druge 
	     *i time se postize beskonacno generisanje staze
	     *u kodu, to se postize resetovanjem vrednosti path_trans_1
	     *i path_trans_2 na pocetnu vredost za polcu br. 2 (odnosno na
	     *pocetnu vredost za promenljivu path_trans_2 definisanu i fajlu et_main.c)
	     *kako bi se u narednom frejmu ploca koja je dostigla granicu nadovezala 
	     *na kraj druge ploce
	     */

		/*u ovom trenutnu se izvlaci i slucajan broj koji
		 *predstavlja redni broj sablona prepreka na ploci (1, 2 ili 3)
		 */ 

		/*takodje se odlucuje i o tome da li ce
		 *sve tri vrste sfere biti iscrtane ili ne
		 *(odluka o crtanju se donosi za svaku sferu posebno)
		 */

		if(path_trans_1 >= ghost_border){

			path_trans_1 = -1+path_trans_2;			
			
			int rand_num_1;

			
			srand(time(NULL));
			rand_num_1 = rand();
			pattern_1 = rand_num_1 % 3 + 1;

			srand(time(NULL));
			rand_num_1 = rand();
			draw_dark_orb_1 = rand_num_1 % 2;

			srand(time(NULL));
			rand_num_1 = rand();
			draw_life_orb_1 = rand_num_1 % 2;

			srand(time(NULL));
			rand_num_1 = rand();
			draw_ghost_orb_1 = rand_num_1 % 2;
		}


		//postavke za plocu br. 2, analogne prethodnim postavkama
		//za prvu plocu
		if(path_trans_2 >= ghost_border){

			path_trans_2 = -1+path_trans_1;
			
			int rand_num_2;

			srand(time(NULL));
			rand_num_2 = rand();
			pattern_2 = rand_num_2 % 3 + 1;

			srand(time(NULL));
			rand_num_2 = rand();
			draw_dark_orb_2 = rand_num_2 % 2;

			srand(time(NULL));
			rand_num_2 = rand();
			draw_life_orb_2 = rand_num_2 % 2;

			srand(time(NULL));
			rand_num_2 = rand();
			draw_ghost_orb_2 = rand_num_2 % 2;

		}



		/*ako je isteklo vreme trajanje darkness_moda
		 *spusta se black_flag zastavica
		 *i prekida se darkness_mod
		 *u suprotnom, dok god traje, sprecavamo iscrtavanje drugih sfera tog tipa
		 */
		if(darkness_timer == 0){
			black_flag = 0;

		}else{
			draw_dark_orb_1 = 0;
			draw_dark_orb_2 = 0;
		}


		/*analogno kao prethodni deo koda, za darkness_mod
		 *rade se iste postavke ali u ovom slucaju za ghost_mode
		 */
		if(ghost_timer == 0){
			ghost_mode_flag = 0;

		}else{
			draw_ghost_orb_1 = 0;
			draw_ghost_orb_2 = 0;
		}




		/*iscrtavanje bara koji predstavlja preostalo vreme
		 *do isteka darkness-moda
		 */
		if(black_flag){

			glPushMatrix();

				set_materials("white");

				GLUquadric* quadric = gluNewQuadric();				
				glRotatef(90, 0, 1, 0);
				glScalef(0.05  , 0.05, (0.9/500)*darkness_timer);
				glTranslatef(0, 40.5, -1.5);
				gluCylinder(quadric , 0.5, 0.5, 3, 10, 10);

			glPopMatrix();
		}


		/*iscrtavanje bara koji predstavlja preostalo vreme
		 *do isteka ghost-moda
		 */
		if(ghost_mode_flag){

			glPushMatrix();

				set_materials("blue");

				GLUquadric* quadric = gluNewQuadric();				
				glRotatef(90, 0, 1, 0);
				glScalef(0.05  , 0.05, (0.9/500)*ghost_timer);
				glTranslatef(0, 39.0, -1.5);
				gluCylinder(quadric , 0.5, 0.5, 3, 10, 10);

			glPopMatrix();
		}





		/*****KRECEMO SA ISCRTAVANJEM STAZE*********/

		/*fj-a check_collision() se poziva nakom iscrtavanja
		 *svakog obejkta i na taj nacin se pokrece mehanizam provere
		 *kolizije igraca sa svakim objektom na stazi u trenutku kada
		 *je taj objekat iscrtan
		 */

		glPushMatrix();	

			
			set_materials("brown");


			if(black_flag)
				darkness();

			
			//1. PLOCA STAZES
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5 + path_trans_1);
			glutSolidCube(1);

			
			//crtamo tunel		
			glPushMatrix();
				
				set_materials("ruby red");

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				GLUquadric* quadric = gluNewQuadric();
				glScalef(10,10,10);
				glTranslatef(0,0, -2);
				gluCylinder(quadric , 0.5, 0.5, 3, 100, 100);					

				set_materials("brown");

			glPopMatrix();
			



			//SABLON BR. 1 ZA PRVU PLOCU
			if(pattern_1 == 1){

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					
					set_materials("dirty white");


					if(black_flag)
						darkness();

						

					glTranslatef(0, 0.5, 0);
					glutSolidCube(1);
					check_collision();
					

					//>>>> NAREDNO OBJASNJENJE ZA ISCRTAVANJE ZUTE SFERE (ZIVOT)
					//>>>> PREDSTAVLJA POSTUPAK ZA ISCRTAVNJE I PLAVE I CRNE SFERE
					//>>>> TAKO DA NA DRUGIM MESTIMA NECE BITI PONOVO KOMENTARISANO
					//>>>> NI JEDNO OD TIH ISCRTAVANJA
					//>>>> JEDINA RAZLIKA JE U POSTAVLJANJU VREDNOSTI sphere_collision
					//>>>> (1 ZA CRNU SFERU I 3 ZA PLAVU)

					//ukoliko je odluceno da se crta zivot 
					//na stazi, crtamo ga					
					if(draw_life_orb_1){

						
						set_materials("brass");											

						//postavljano signalizaciju za proveru kolizije 
						//sa zutom sferom
						sphere_collision = 2;

							glPushMatrix();

								//crtamo zutu sferu
								glScalef(0.25, 0.25, 0.25);
								glTranslatef(-3.5, 0, -40);
								glutSolidSphere(1, 100, 100);

								//proveravamo kolizije sa (zutom) sferom
								check_collision();

							glPopMatrix();

						//vracamo vrednost prom. sphere_collision na staru
						//kako bi se nadalje normalno detektovale kolizije
						//sa preprekama na stazi
						sphere_collision = 0;

						//vracmo materijale na staro
						set_materials("dirty white");

						//ukoliko traje darkness_mod materijale vracamo
						//na crnu boju
						if(black_flag)
							darkness();

					}	


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

						
						if(draw_dark_orb_1){

							darkness();

							sphere_collision = 1;

								glPushMatrix();

									glScalef(1/5.2, 1/0.2, 1/0.2);
									glScalef(0.5, 0.1, 0.5);
									glTranslatef(0, -6.0, 0);
									glutSolidSphere(1, 100, 100);
									check_collision();
								
								glPopMatrix();

							sphere_collision = 0;

							
							set_materials("dirty white");

						}

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

					
					set_materials("green");


					if(black_flag)
						darkness();

					
					glScalef(1.5, 1.3, 0.8);
					glTranslatef(0, 0.5, 0);
					glutSolidCube(1);
					check_collision();

					
					if(draw_dark_orb_1){

						darkness();

						sphere_collision =1;

							glPushMatrix();

								glScalef(1/1.5, 1/1.3, 1/0.8);
								glScalef(0.4, 0.4, 0.4);
								glTranslatef(3.5, -0.5, 0);
								glutSolidSphere(1, 100, 100);
								check_collision();
								
							glPopMatrix();

						sphere_collision = 0;
		
						set_materials("green");

					}


					glPushMatrix();
						glScalef(1/1.8, 1/1.3, 1/0.8);
						glScalef(1.2, 2.5, 1);
						glTranslatef(-1.5, 0.25, 40);						
						glutSolidCube(1);
						check_collision();


						if(draw_ghost_orb_1){

							set_materials("light blue");

								sphere_collision = 3;

									glPushMatrix();
									
										glScalef(1/1.2, 1/2.5, 1/1);
										glScalef(0.4, 0.35, 0.4);
										glTranslatef(2.9, -1.8, 0);
										glutSolidSphere(1, 100, 100);
										check_collision();

									glPopMatrix();

								sphere_collision = 0;

							set_materials("green");
							
							if(black_flag)
								darkness();
						}


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

					//blue
					set_materials("blue");

					if(black_flag)
						darkness();

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

						
						if(draw_dark_orb_1){

							darkness();

							sphere_collision =1;

								glPushMatrix();

									glScalef(1/0.5, 1/2.3, 1/1);
									glScalef(0.3, 0.3, 0.3);
									glTranslatef(-2.55, -2.0, 0);
									glutSolidSphere(1, 100, 100);
									check_collision();
									
								glPopMatrix();

							sphere_collision = 0;
							
							set_materials("blue");

						}

						glScalef(7.4, 0.2, 1);
						glTranslatef(-0.433, 2.2, 0);
						glutSolidCube(1);


					glPopMatrix();

				glPopMatrix();

			}


		glPopMatrix();
		



		glPushMatrix();

			
			set_materials("brown");
				
			if(black_flag)
				darkness();

			
			//2. PLOCA STAZE
			glScalef(4, 0.125, 100);
			glTranslatef(0, -0.5, -0.5+path_trans_2);
			glutSolidCube(1);


			
			//crtamo tunel
			glPushMatrix();
				
				set_materials("ruby red");

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				quadric = gluNewQuadric();
				glScalef(10,10,10);
				glTranslatef(0,0, -2);
				gluCylinder(quadric , 0.5, 0.5, 3, 100, 100);

				set_materials("brown");

			glPopMatrix();
			



			//SABLON BR. 1 ZA DRUGU PLOCU
			if(pattern_2 == 1){	

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				glPushMatrix();

					
					set_materials("dirty white");


					if(black_flag)
						darkness();

					
					
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

						glScalef(0.4, 2.5, 0.8);
						glTranslatef(-2.0, 0.3, -15);
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
		
						
						if(draw_dark_orb_2){

							darkness();

							sphere_collision =1;

								glPushMatrix();

									glScalef(1/(2.0*0.2*10.5), 1/(1.5*2.0*0.1), 1/(1*0.7*1));
									glScalef(0.4, 0.4, 0.4);
									glTranslatef(-0.8, -4.2, 0);
									glutSolidSphere(1, 100, 100);
									check_collision();
									
								glPopMatrix();

							sphere_collision = 0;
							
							set_materials("dirty white");
						}

					glPopMatrix();

				glPopMatrix();
			}		

			//SABLON BR. 2 ZA DRUGU PLOCU
			if(pattern_2 == 2){
				
				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				set_materials("turquoise");

				if(black_flag)
					darkness();

				glPushMatrix();

					glPushMatrix();		

						glScalef(0.4, 2.8, 0.3);
						glTranslatef(-3.5, 0.5, 160);
						glutSolidCube(1);
						check_collision();

						if(draw_ghost_orb_2){

							set_materials("light blue");

								sphere_collision = 3;

									glPushMatrix();
									
										glScalef(1/0.4, 1/2.8, 1/0.3);
										glScalef(0.3, 0.25, 0.3);
										glTranslatef(2.25, -3.1, 0);
										glutSolidSphere(1, 100, 100);
										check_collision();

									glPopMatrix();

								sphere_collision = 0;

							set_materials("turquoise");
							
							if(black_flag)
								darkness();
						}

						if(draw_life_orb_2){

						
							set_materials("brass");					

								sphere_collision = 2;

									glPushMatrix();

										glScalef(1/0.4, 1/2.8, 1/0.3);
										glScalef(0.3, 0.25, 0.3);
										glTranslatef(6.75, -3.1, 0);
										glutSolidSphere(1, 100, 100);
										check_collision();

									glPopMatrix();

								sphere_collision = 0;

							
							set_materials("turquoise");

							if(black_flag)
								darkness();

						}

						glTranslatef(7, 0, 0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-3.5, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(7.0, 0.1, 1);
						glTranslatef(0, 0.5, 0);
						glutSolidCube(1);

						glTranslatef(0, 2.5, 0);
						glutSolidCube(1);

					glPopMatrix();


					glPushMatrix();

						glScalef(1.1, 1.1, 1.1);
						glTranslatef(0, 0.6, 35);
						glutSolidCube(1);
						check_collision();

						glScalef(0.2, 2.1, 0.2);
						glTranslatef(-7.9, 0.2, 0);
						glutSolidCube(1);
						check_collision();

						glTranslatef(15.8, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(20.5, 0.2, 1);
						glTranslatef(-0.4, 3.0, 0);
						glutSolidCube(1);


					glPopMatrix();

					glPushMatrix();

						glScalef(1.5, 1.2, 0.2);
						glTranslatef(-0.80, 0.5, 125);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.6, 0, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

					glPushMatrix();

						glScalef(1.2, 1.2, 1.2);
						glTranslatef(0, 0.5, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();


					glPushMatrix();

						glScalef(0.8, 3.0, 0.4);
						glTranslatef(0, 0.5, -50);
						glutSolidCube(1);
						check_collision();

						glTranslatef(1.5, 0, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

					glPushMatrix();

						glScalef(0.8, 3.0, 0.4);
						glTranslatef(0, 0.5, -100);
						glutSolidCube(1);
						check_collision();

						glTranslatef(-1.5, 0, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();


				glPopMatrix();
			}

			//SABLON BR. 3 ZA DRUGU PLOCU
			if(pattern_2 == 3){

				glScalef(1.0/4, 1.0/0.125, 1.0/100);

				set_materials("red");

				if(black_flag)
					darkness();


				glPushMatrix();

					glPushMatrix();

						glScalef(1.2, 1.2, 1.2);
						glTranslatef(0, 0.5, 0);
						glutSolidCube(1);
						check_collision();

						if(draw_ghost_orb_2){

							set_materials("light blue");

								sphere_collision = 3;

									glPushMatrix();
									
										glScalef(1/1.2, 1/1.2, 1/1.2);
										glScalef(0.3, 0.3, 0.3);
										glTranslatef(-4.0, -0.5, 0);
										glutSolidSphere(1, 100, 100);
										check_collision();

									glPopMatrix();

								sphere_collision = 0;

							set_materials("red");
							
							if(black_flag)
								darkness();
						}


					glPopMatrix();



					glPushMatrix();

						glTranslatef(-1.0, 0.5, 20);
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.0, 0, 0);
						glutSolidCube(1);
						check_collision();

					glPopMatrix();

					glPushMatrix();

						glTranslatef(-1.0, 0.5, -20);
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.0, 0, 0);
						glutSolidCube(1);
						check_collision();
						
					glPopMatrix();


					glPushMatrix();

						glScalef(0.6, 2.5, 0.6);
						glTranslatef(-1.4, 0.5, 60);
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.8, 0, 0);
						glutSolidCube(1);
						check_collision();
						
					glPopMatrix();

					glPushMatrix();

						glScalef(0.6, 2.5, 0.6);
						glTranslatef(-1.4, 0.5, -60);
						glutSolidCube(1);
						check_collision();

						glTranslatef(2.8, 0, 0);
						glutSolidCube(1);
						check_collision();
						
					glPopMatrix();

					glPushMatrix();

						glScalef(0.6, 3.0, 0.6);
						glTranslatef(-2.1, 0.5, 80);
						glutSolidCube(1);
						check_collision();

						glTranslatef(4.2, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(5.2, 0.2, 1);
						glTranslatef(-0.4, 2.5, 0);
						glutSolidCube(1);

					glPopMatrix();


					glPushMatrix();

						glScalef(0.6, 3.0, 0.6);
						glTranslatef(-2.1, 0.5, -80);
						glutSolidCube(1);
						check_collision();

						glTranslatef(4.2, 0, 0);
						glutSolidCube(1);
						check_collision();

						glScalef(5.2, 0.2, 1);
						glTranslatef(-0.4, 2.5, 0);
						glutSolidCube(1);

					glPopMatrix();

				glPopMatrix();
			}


		glPopMatrix();


	glPopMatrix();
}






