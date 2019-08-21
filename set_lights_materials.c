#include "et.h"

/*U OVOM FAJLU SU IZDVOJENE FJE ZA POSTAVKU SVETLA I MATERIJALA
 *FJA darkness() POSTAVLJA MATERIJALE NA CRNU BOJU NAKON SKUPLJANJA
 *CRNE SFERE, ALI JE IZDVOJENA KAO POSEBNA ZBOG TOGA STO SE NJENA
 *ULOGA IPAK NA NEKI NACIN RAZLIKUJE OD OBICNIH POSTAVKI MATERILA
 *(NE PO KODU ALI PO ZNACAJU ZA IGRU)
 *OVDE SE TAKODJE NALAZI I FJA ZA ISCRTAVANJE KOORDINATNIH OSA
 *KOJA JE KORISCENA U RANOJ FAZI RAZVOJA IGRE
 */

//** GLOABLNI NIZVOVI ZA MATERIJALE **//
//** postavljamo vrednosti ovih nizova pre iscrtavanja objekata **//
GLfloat ambient_coeffs[4];
GLfloat diffuse_coeffs[4];
GLfloat specular_coeffs[4];
GLfloat shininess;

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
    //ako hocemo da i unutrasnjost bude obasjana
    //moramo dodati i odgovarajuce materija za unutrasnjost (za back-face)
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


	glShadeModel(GL_SMOOTH);

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
void darkness(){

	ambient_coeffs[0]  = 0.02; ambient_coeffs[1]  = 0.02; ambient_coeffs[2]  = 0.02; ambient_coeffs[3]  = 1;
	diffuse_coeffs[0]  = 0.01; diffuse_coeffs[1]  = 0.01; diffuse_coeffs[2]  = 0.01; diffuse_coeffs[3]  = 1;
	specular_coeffs[0] = 0.4;  specular_coeffs[1] = 0.4;  specular_coeffs[2] = 0.4; specular_coeffs[3] = 1;
	shininess = 0.18*128; 
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}
/*********************************************************************************/
void set_materials(char* material_name){


	if(strcmp(material_name, "white") == 0){
		ambient_coeffs[0]  = 0.9;   ambient_coeffs[1]  = 0.9;  ambient_coeffs[2]  = 0.9; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.55;  diffuse_coeffs[1]  = 0.55;  diffuse_coeffs[2]  = 0.55; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.7; specular_coeffs[1] = 0.7; specular_coeffs[2] = 0.7; specular_coeffs[3] = 1;
		shininess = 0.25*128;
	    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "gold") == 0){
		ambient_coeffs[0]  = 0.24725;   ambient_coeffs[1]  = 0.1995;  ambient_coeffs[2]  = 0.0745; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.75164;  diffuse_coeffs[1]  = 0.60648;  diffuse_coeffs[2]  = 0.22648; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.628281; specular_coeffs[1] = 0.555802; specular_coeffs[2] = 0.366065; specular_coeffs[3] = 1;
		shininess = 0.4*128;
	    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "ruby red") == 0){
		ambient_coeffs[0]  = 0.1745;   ambient_coeffs[1]  = 0.01175;  ambient_coeffs[2]  = 0.01175; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.61424;  diffuse_coeffs[1]  = 0.04136;  diffuse_coeffs[2]  = 0.04136; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.727811; specular_coeffs[1] = 0.626959; specular_coeffs[2] = 0.626959; specular_coeffs[3] = 1;
		shininess = 0.6*128;
	    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
	}

	if(strcmp(material_name, "brown") == 0){
		ambient_coeffs[0]  = 0.2125;    ambient_coeffs[1]  = 0.1275;   ambient_coeffs[2]  = 0.054; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.714;     diffuse_coeffs[1]  = 0.4284;     diffuse_coeffs[2]  = 0.18144; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.393548; specular_coeffs[1] = 0.271906; specular_coeffs[2] = 0.166721; specular_coeffs[3] = 1;
		shininess = 0.2*128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "dirty white") == 0){
		ambient_coeffs[0]  = 0.25;    ambient_coeffs[1]  = 0.20725;   ambient_coeffs[2]  = 0.20725; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 1.0;     diffuse_coeffs[1]  = 0.829;     diffuse_coeffs[2]  = 0.829; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.296648; specular_coeffs[1] = 0.296648; specular_coeffs[2] = 0.296648; specular_coeffs[3] = 1;
		shininess = 0.088*128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "green") == 0){
		ambient_coeffs[0]  = 0.135;    ambient_coeffs[1]  = 0.2225;   ambient_coeffs[2]  = 0.1575; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.54;     diffuse_coeffs[1]  = 0.89;     diffuse_coeffs[2]  = 0.63; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.316228; specular_coeffs[1] = 0.316228; specular_coeffs[2] = 0.316228; specular_coeffs[3] = 1;
		shininess = 0.1*128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "blue") == 0){
		ambient_coeffs[0]  = 0.4; ambient_coeffs[1]  = 0.4; ambient_coeffs[2]  = 0.9; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.4; diffuse_coeffs[1]  = 0.4; diffuse_coeffs[2]  = 0.9; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.5; specular_coeffs[1] = 0.5; specular_coeffs[2] = 0.5; specular_coeffs[3] = 1;
		shininess = 0.1*100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
	}

	if(strcmp(material_name, "brass") == 0){
		ambient_coeffs[0]  = 0.329412; ambient_coeffs[1]  = 0.223529; ambient_coeffs[2]  = 0.027451; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.780392; diffuse_coeffs[1]  = 0.568627; diffuse_coeffs[2]  = 0.113725; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.992157; specular_coeffs[1] = 0.941176; specular_coeffs[2] = 0.807843; specular_coeffs[3] = 1;
		shininess = 0.21794872*100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	if(strcmp(material_name, "turquoise") == 0){
		ambient_coeffs[0]  = 0.1; ambient_coeffs[1]  = 0.18725; ambient_coeffs[2]  = 0.1745; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.396; diffuse_coeffs[1]  = 0.74151; diffuse_coeffs[2]  = 0.69102; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.297254; specular_coeffs[1] = 0.30829; specular_coeffs[2] = 0.306678; specular_coeffs[3] = 1;
		shininess = 0.1*100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}	


	if(strcmp(material_name, "light blue") == 0){
		ambient_coeffs[0]  = 0.4; ambient_coeffs[1]  = 0.4; ambient_coeffs[2]  = 0.9; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.4; diffuse_coeffs[1]  = 0.4; diffuse_coeffs[2]  = 0.9; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.5; specular_coeffs[1] = 0.5; specular_coeffs[2] = 0.5; specular_coeffs[3] = 1;
		shininess = 100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
	}

	if(strcmp(material_name, "yellow") == 0){
		ambient_coeffs[0]  = 0.05; ambient_coeffs[1]  = 0.05; ambient_coeffs[2]  = 0.0; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.5; diffuse_coeffs[1]  = 0.5; diffuse_coeffs[2]  = 0.4; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.7; specular_coeffs[1] = 0.7; specular_coeffs[2] = 0.04; specular_coeffs[3] = 1;
		shininess = 0.078125*128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
	}

	if(strcmp(material_name, "red") == 0){
		ambient_coeffs[0]  = 0.05; ambient_coeffs[1]  = 0.0; ambient_coeffs[2]  = 0.0; ambient_coeffs[3]  = 1;
		diffuse_coeffs[0]  = 0.5; diffuse_coeffs[1]  = 0.4; diffuse_coeffs[2]  = 0.4; diffuse_coeffs[3]  = 1;
		specular_coeffs[0] = 0.7; specular_coeffs[1] = 0.04; specular_coeffs[2] = 0.04; specular_coeffs[3] = 1;
		shininess = 0.078125*128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);	
	}

}
/*********************************************************************************/

