#include "et.h"

#define DARKNESS_GHOST_DURATION 500

/************OPIS MEHANIZMA DETEKCIJE KOLIZIJA******************/
/*Mehanizam je baziran na sledcoj ideji: proveriti koordinate objekata
 *nakon svakog iscrtavanja i odraditi odredjene akcije ukoliko je doslo do 
 *kolizije. Glavni problem je bio pronaci SVETSKE KOORDINATE kako igraca
 *tako i svih drugih objekata na stazi. On je resen na sledeci nacin:
 *matrica MODELVIEW koju nudi openGL sadrzi kompoziciju svih odradjenih
 *transformacija (MODEL komponenta) i iz nje se potrebne koordinate mogu procitati. 
 *Medjutim, u toj matrici se pored trnsformacija, akumuliraju i sve 
 *postavke kamere (VIEW komponenta). Sldeci korak jeste osloboditi se VIEW
 *komponente matrice. To se postize tako sto se pre postavke kamere ucita
 *jedinicna matrica, onda se odrade postavke kamere. U tom trenutku smo sigurni
 *da se u MODELVIEW matrici nalai samo VIEW komponenta matrice. Nju cuvamo
 *i nalazimo njen inverz odnosno V^(-1) (u nastavku samo V_). Taj inverz nam
 *sluzi kako bismo citavu MODELVIEW matricu pomnozili njime sa leve strane
 *i oslobodili se VIEW komponente, a nama ostaje samo MODEL komponenta, iz
 *koje citamo potrebne koordinate.
 * 
 *  V_*VM = M
 *
 *Pronalazenje inverza V matrice, cuvanje matrica, kao i mnozenje matrica
 *realizovano je uz pomoc GSL - GNU Scientific Library 
 *(https://www.gnu.org/software/gsl/).
 */
/******************************************************************/


/*koriste se u f-ji  save_V_inverse()
 *kada su definisani globalno ujedno se i njihovi el.
 *inicijalizuju na 0
 */
double rez[16];
double inv[16];

/*skor predstavlja broj izbegnutih prepreka
 *videti fj-u check_collision gde se skor racuna
 */
int score = 0;

extern int animation_ongoing;

//matrica koja cuva inverz matrice pogleda, tj izverz
//VIEW komponente MODELVIEW matrice
gsl_matrix_view i;


extern float i_X_l; 
extern float i_X_r;

/*da li je prepreka po z-osi dosla do igraca se NE moze proveravati
 *na jednakost prednje strane igraca (lica) i trenutne z koordinate
 *prepreke iz razloga sto je taj trenutak jednakosti te dve vrednosti
 *redak i u vecini slucajeva se preskoci, sto ne bi smelo da se desava.
 *Iz tog razloga su uvedene naredne dve promenljive koje predstavljaju
 *toleranciju po z-osi, tako da se za z koordinatu prepreke vise ne 
 *proverava da li je jednaka z koordinati lica igraca, nego da li je 
 *upala u interval tolerancije
 *interval je odredjen eksperimantalno: dovoljno je veliki da se 
 *detekcija kolzije ne preskoci a dovoljno mali da se kolizija 
 *sa preprekom ne desi prerano
 */
float i_z_front_tolerance = -0.6;
float i_z_back_tolerance  =  0.0;


extern int sphere_collision; 

extern int black_flag;
extern int draw_dark_orb_1;
extern int draw_dark_orb_2;
extern int darkness_timer;

extern int lives_remain;
extern int draw_life_orb_1;
extern int draw_life_orb_2;
/*kada se igrac sudari sa preprekom treba mu smanjiti zivot za 1
 *ali kako se animacija nastavlja igrac ce nastaviti da prolazi kroz
 *prepreku. U tim trenucima prolaska igraca kroz prepreku kolizija ce 
 *ponovo da se detektuje i igracu ce se skinuti i preostala dva zivota,
 *sto nije dobar scenario. Da bi so to izbeglo, uvedena je promenljiva
 *collision_ban koja zabranjuje detekciju kolizije nakon prvog sudara
 *sa preprekom i doprinosi da se igracu oduzme samo jedan zivot prilikom
 *sudara a ne sva tri. Kada igrac prodje prepreku, vrednost ove zastavice se 
 *vraca na nulu, kako bi se koristila u istu svhu za naredne prepreke
 */
int collision_ban = 0;  

extern int draw_ghost_orb_1;
extern int draw_ghost_orb_2;
extern int ghost_mode_flag;
extern int ghost_timer;

/*kada igrac izgubi sva tri zivota igra se zavrsava i stampa se 
 *zavrsni frejm pozivom fje game_over(). Medjutim, javlja se bag
 *koji prouzrokuje da se iscrta jos jedan frejm nakom zavrsnog frejma i
 *sav tekst koji je ispisan biva uklonjen novim iscrtavanjima.
 *Da se to ne bi desilo uvedena je zastavica game_over_flag koja
 *brani svako dalje iscrtavanje frejmova
 */
int game_over_flag = 0; 

/*********************************************************************************/
//PRONALAZI INZERZ VIEW KOMPONENTE MODELVIEW MATRICE UZ POMOC LU DEKOMPOZICIJE
//I SMESTA GA U GLOBALNU MATRICU 'i'
void save_V_inverse(){


	glGetDoublev(GL_MODELVIEW_MATRIX, rez);

	gsl_matrix_view m = gsl_matrix_view_array(rez, 4, 4);

	gsl_matrix_transpose(&m.matrix);

	i = gsl_matrix_view_array(inv, 4, 4);

	gsl_permutation* p = gsl_permutation_alloc(4);
	int s;


	gsl_linalg_LU_decomp(&m.matrix, p, &s);
	gsl_linalg_LU_invert(&m.matrix, p, &i.matrix);

}
/*********************************************************************************/
//PRONALAZI Z KOORDINATU LICA PREPREKE, ODNOSNO Z KOORDINATU LICA SFERA NA STAZI
//CITAJUCI ODGOVARAJUCE KOORDINATE IZ MODEL MATRICE
double get_z_front(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	gsl_matrix_transpose(&mv.matrix);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };


	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	//nacin za mnozenje matrica koji GSL nudi
	//forma fje je: alpha*AB + beta*C
	//A = V_
	//B = VM
	//C = zeroes
	//alpha = 1.0
	//beta = 0.0
	//rezultat se smesta u C
	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &i.matrix, &mv.matrix,
					0.0, &C.matrix
				);
	
	//citamo i vracamo z koordinatu lica prepreke/sfere na stazi
	//iz MODEL matrice koja je smestena u C 
	if(sphere_collision)

		return gsl_matrix_get(&C.matrix, 2, 3) + 1*gsl_matrix_get(&C.matrix, 2, 2);
	
	else
		return gsl_matrix_get(&C.matrix, 2, 3) + 1*gsl_matrix_get(&C.matrix, 2, 2)/2.0;

}
/*********************************************************************************/
//PRONALAZI Z KOORDINATU ZADNJE STRANE PREPREKE,
//CITAJUCI ODGOVARAJUCE KOORDINATE IZ MODEL MATRICE
//POTREABNO JE NACI OVE KOORDINATE KAKO BISMO AZURIRALI SKOR KADA IGRAC USPESNO IZBEGNE
//PREPREKU
double get_z_back(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	gsl_matrix_transpose(&mv.matrix);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };


	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &i.matrix,&mv.matrix,
					0.0, &C.matrix
				);
	

	
	//citamo i vracamo z koordinatu zadnje strane prepreke
	//iz MODEL matrice koja je smestena u C
	return gsl_matrix_get(&C.matrix, 2, 3) - 1*gsl_matrix_get(&C.matrix, 2, 2)/2.0;

}
/*********************************************************************************/
//PRONALAZI X KOORDINATU DESNE IVICE PREPREKE, ODNOSNO DESNE IVICE SFERA
//POTREBANO JE PROVERITI OVE KOORDINATE ZARAD DETEKCIJE KOLIzIJE
double get_x_right(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	gsl_matrix_transpose(&mv.matrix);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };

	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &i.matrix, &mv.matrix,
					0.0, &C.matrix
				);

	//citamo i vracamo x koordinatu desne ivice prepreke/sfera na stazi
	//iz MODEL matrice koja je smestena u C
	if(sphere_collision)

		return gsl_matrix_get(&C.matrix, 0, 3) + 1.0*gsl_matrix_get(&C.matrix, 0, 0);
	
	else
		return gsl_matrix_get(&C.matrix, 0, 3) + 1.0*gsl_matrix_get(&C.matrix, 0, 0)/2.0;


}
/*********************************************************************************/
//ANALOGNO PRETHODNOJ FJI SAMO OVAJ PUT ZA X KOORDINATU LEVE IVICE OBJEKTA
double get_x_left(){

	double MV[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);

	gsl_matrix_view mv = gsl_matrix_view_array(MV, 4, 4);

	gsl_matrix_transpose(&mv.matrix);

	double zeroes[] = { 	0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00,
							0.00, 0.00, 0.00, 0.00
					  };

	gsl_matrix_view C = gsl_matrix_view_array(zeroes, 4, 4);

	gsl_blas_dgemm(	CblasNoTrans, CblasNoTrans,
					1.0, &i.matrix, &mv.matrix,
					0.0, &C.matrix
				);

	//citamo i vracamo x koordinatu leve ivice prepreke/sfera na stazi
	//iz MODEL matrice koja je smestena u C
	if(sphere_collision)

		return gsl_matrix_get(&C.matrix, 0, 3) - 1.0*gsl_matrix_get(&C.matrix, 0, 0)/2.0;
	
	else
		return gsl_matrix_get(&C.matrix, 0, 3) - 1.0*gsl_matrix_get(&C.matrix, 0, 0)/2.0;


}
/*********************************************************************************/
void check_collision(){

	//uzimamo sve potrebne informacije o objektu
	OBSTACLE obs;
	obs.z   = get_z_front();
	obs.x_l = get_x_left();
	obs.x_r = get_x_right();


	//pitamo da li provera kolizije dolazi u trenutku crtanja neke od sfera na stazi
	if(sphere_collision != 0){

		if( (obs.z >= i_z_front_tolerance && obs.z < i_z_back_tolerance) && 
						!(i_X_r < obs.x_l || i_X_l > obs.x_r) )
		{
			//ovde je doslo do kolicije sa nekom od sfera
			//zato se postavljaju odgovarajuci parametri
			//(njihovi opisi su dati na mestima gde su definisani)

			if(sphere_collision == 1){
				draw_dark_orb_1 = 0;
				draw_dark_orb_2 = 0;
 				black_flag  = 1;
				darkness_timer = DARKNESS_GHOST_DURATION;
			}

			if(sphere_collision == 2){
				draw_life_orb_1 = 0;
				draw_life_orb_2 = 0;

				if(lives_remain < 3)
					lives_remain++;
			}
			
			if(sphere_collision == 3){
				draw_ghost_orb_1 = 0;
				draw_ghost_orb_2 = 0;
				ghost_mode_flag = 1;
				ghost_timer = DARKNESS_GHOST_DURATION;
			}

		}

	}

	else{

		//ovde znamo da se provera kolizije radi u trenutku crtanja prepreke
		if( (obs.z >= i_z_front_tolerance && obs.z < i_z_back_tolerance) && 
								!(i_X_r < obs.x_l || i_X_l > obs.x_r) )
		{
			//ukoliko je doslo do kolizije sa preprekom a ghost_mode je
			//aktiviran celu koliziju preskacemo, jer je to efekat 
			//prolaska kroz zidove koji ghost_mode podrazumeva
			if(ghost_mode_flag == 0){

				if(collision_ban == 0){

					collision_ban = 1;   //pogledati definiciju prom. collision_ban za detaljno objasnjenje

					//smanjujemo jedan zivot ukoliko je igracu preostao
					//barem jedan zivot
					if(lives_remain > 0){
						lives_remain--;
					}
					
					//igracu nije preostalo vise zivota i igra ulazi 
					//u zavrsno stanje
					if(lives_remain == 0){
						animation_ongoing = 0;
						game_over_flag = 1;
					}
				}
			}		

		}	
		
		//ovde upadamo kada nije doslo do kolizije ni sa jednim objektom
		//i proveravamo da li mozda taj objekat uspesno izbegnut
		//kako bismo azurirali skor
		//za to nam treba z koord. zadnje strane prepreke koju dobijamo 
		//pozivom f-je get_z_back()

		/*racunanje skora
		 *skor = broj izbegnutih prepreka
		 */
		else{
			
			double z_back = get_z_back();
			
			//vrednosti 0 i 0.2 predstavljaju istu onu toleranciju po z-osi
			//samo sada za izbegavanje prepreke: ukoliko je zadnja stana
			//prepreke u ovom intervalu, prepreka je uspesno izbegnuta
			//i uvecavamo skor za 1
 			//takodje, u tom trenutku spustamo i collision_ban zastavicu
 			//jer smo (potencijano) prosli koz celu prepreku i necemo
 			//imati problem sa smanjivanjem zivota


			if(z_back >= 0 && z_back <= 0.2){				
				score++;				
				collision_ban = 0;
			}


		}


	}

}
