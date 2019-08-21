#include "et.h"

extern int animation_ongoing;
extern float path_trans_1;
extern float path_trans_2;
extern float x_trans;

extern float ro;
extern float sigma;
extern float fi;
extern int  faktor;
extern int  LeftKeyPressed;
extern int  RightKeyPressed;

extern int  pattern_1;
extern int  pattern_2;

extern int  score;

/*globalna promenljiva u koju se na samom pocetku programa (u main f-ji)
 *ucitava aktuelni high_score
 */
int high_score; 

extern int  sphere_collision;
	
extern int	black_flag;
extern int	darkness_timer;
extern int	draw_dark_orb_1;
extern int	draw_dark_orb_2;

extern int	ghost_mode_flag;
extern int	ghost_timer;
extern int	draw_ghost_orb_1;
extern int	draw_ghost_orb_2;

extern int	lives_remain;
extern int	collision_ban;
extern int	draw_life_orb_1;
extern int	draw_life_orb_2;

extern int	game_over_flag;



/*********************************************************************************/
//FJA KOJA SE POZIVA KADA IGRA DODJE U ZAVRSNO STANJE TJ KADA JE IGRAC 
//IZGUBIO SVE ZIVOTE
//TADA SE OBRADJUJE ISPIS TEKSTA NA EKRAN I PAMTI SE NOVI high_score UKOLIKO
//JE IGRAC USPEO DA PRMASI PRETHODNI NAJBOLJI REZULTAT
void game_over(int score_f){

	glPushMatrix();				

		//tekst koji ce biti ispisan
		char* s1 = "GAME OVER  ";
		char* s2 = "YOUR SCORE: ";		
		char* s3 = "NEW HIGH SCORE!";
		char* s4 = "CURRENT HIGH SCORE: ";
		char* s5 = "PRESS C TO PLAY AGAIN";
		char* s6 = "OR";
		char* s7 = "PRESS ESC TO EXIT";
		
		
		
		//parsiranje scora
		char final_score[5];
    	char high_score_txt[5];
		
		int j = score_f%10;	
		score_f = score_f/10;
		int d = score_f%10;
		score_f = score_f/10;
		int s = score_f%10;
		score_f = score_f/10;
		int h = score_f%10;

		final_score[0] = '0' + h;
		final_score[1] = '0' + s;
		final_score[2] = '0' + d;
		final_score[3] = '0' + j;
		final_score[4] = '\0';

		//parsiranje high_scora
		int high_score_c = high_score;

		j = high_score_c%10;
		high_score_c /= 10; 
		d = high_score_c%10;
		high_score_c /= 10;
		s = high_score_c%10;
		high_score_c /= 10;
		h = high_score_c%10;

		high_score_txt[0] = '0' + h;
		high_score_txt[1] = '0' + s;
		high_score_txt[2] = '0' + d;
		high_score_txt[3] = '0' + j;
		high_score_txt[4] = '\0';



		set_materials("white");

		glRasterPos3f(-0.4, 0.3, 2.5);
		char* c;
		for(c = s1; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		for(c = s2; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		for(c = final_score; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

		//u zavisnosti od toga da li igrac premasio aktuelni high_score
		//isisuje se dgovarajuci tekst

		if(score > high_score){

			//poziva se fja za cuvanje novog high_score			
			save_new_high_score();

			glRasterPos3f(-0.21, 0.3, 2.66);
			for(c = s3; *c != '\0'; c++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}else{

			glRasterPos3f(-0.32, 0.3, 2.66);
			for(c = s4; *c != '\0'; c++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

			for(c = high_score_txt; *c != '\0'; c++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}

		//ispis preostalog teksta
		
		glRasterPos3f(-0.243, 0.3, 2.8);
		for(c = s5; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

		glRasterPos3f(-0.02, 0.3, 2.905);
		for(c = s6; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

		glRasterPos3f(-0.16, 0.3, 3.0);
		for(c = s7; *c != '\0'; c++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);



	glPopMatrix();
}
/*********************************************************************************/
//FJA KOJA SE POZIVA PRITISKOM TASTERA 'c' 
//SVE GLOBALNE PARAMETRE POSTAVLJA NA POCETNE VREDNOSTI
//KAKO BI IGRA MOGLA U DA SE RESTART-UJE
void restart_game(){

	path_trans_1 = 0;
	path_trans_2 = -1;
	x_trans = 0;

	animation_ongoing = 0;

	ro = sqrt(17);
	sigma = 0;
	fi= 0;
	faktor = 0;
	LeftKeyPressed = 0;
	RightKeyPressed = 0;


	int rand_num;
	srand(time(NULL));
	rand_num = rand();
	pattern_1 = rand_num % 3 + 1;
	
	srand(time(NULL));
	rand_num = rand();
	pattern_2 = rand_num % 3 + 1;

	score = 0;


	sphere_collision = 0;
	
	black_flag = 0;
	darkness_timer = 0;
	draw_dark_orb_1 = 0;
	draw_dark_orb_2 = 0;

	ghost_mode_flag = 0;
	ghost_timer = 0;
	draw_ghost_orb_1 = 0;
	draw_ghost_orb_2 = 0;
	
	lives_remain = 3;
	collision_ban = 0;
	draw_life_orb_1 = 0;
	draw_life_orb_2 = 0;

	game_over_flag = 0;

	load_high_score();
}
/*************************************************************************/
//FJA IZ DATOTEKE high_score.txt CITA TRENUTNI NAJBOLJI REZULTAT I UPISUJE
//GA U GLOBALNU POM. high_score
//UKOLIKO DATOTEKA NE POSTOJI ILI OTVARANJE NIJE USPELO IZ NEKOG
//DRUGOG RAZLOGA high_score SE POSTALJA NA 0
void load_high_score(){

	FILE* high_score_f;

	high_score_f = fopen("high_score.txt", "r");
	if(high_score_f == NULL){

		high_score = 0;
		return;
	}

	fscanf(high_score_f, "%d", &high_score);

	fclose(high_score_f);

}
/*************************************************************************/
//FJA KOJA UPISUJE NOVI high_score U DATOTEKU high_score.txt 
//UKOLIKO DATOTEKA NE POSTOJI KREIRACE SE, UKOLIKO POSTOJI PREBRISACE SE
//UKOLIKO DODJE DO NEKE DRUGE GRESKE NOVI NAJBOLJI SKOR SE NE PAMTI
void save_new_high_score(){

	FILE* high_score_f;

	high_score_f = fopen("high_score.txt", "w");
	if(high_score_f == NULL){

		return;
	}


	fprintf(high_score_f, "%d", score);

	fclose(high_score_f);
}
/*************************************************************************/
