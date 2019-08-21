# RG162-eternal-trail
**Eternal Trail**

Igra predstavlja endless-runner: igrač se kreće po stazi koja se neprekidno generiše, izbegavajuću prepreke
koje se nalaze na stazi. Pored prepreka, na stazi se mogu naći i objekti koji su tu da pomognu igaču u dostizanju
najboljeg rezultata, ali i oni objekti koji ga u tom cilju ometaju. Igra traje dokle god igrač ne izgubi sva tri života.

**Prevođenje i pokretanje programa**

U igru je uključena biblioteka GSL - GNU Scientific Library (https://www.gnu.org/software/gsl/) tako da je za prevođenje
neophodno malo izmeniti Makefile: kao vrednost promenljivama IGSL i LGSL u Makefile-u potrebno je dati putanju do mesta na
kom se GSL biblioteka nalazi i to tako da IGSL sadrži putanju do include direktorijuma biblioteke a LGSL putanju do lib 
direktorijuma biblioteke. Nakon izmena te dve promenljive bi trebalo da izgledaju ovako:

IGSL = -I/putanja_do_include_direktorijuma

LGSL = -L/putanja_do_lib_direktoriuma

Time je obezbeđeno statičko linkovanje biblioteke. Potrebno je još obezbediti i dinamičko linkovanje, pri pokretanju
izvršnog fajla, sledećom komandom u terminalu:

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/putanja_do_lib_direktoriuma

Sada se program može prevesti komandom **make** i pokrenuti komandom **./eternal_trail**

**U okviru poslednjeg commit-a postoji izvršni fajl koji se može odmah pokrenuti, bez potrebe za prethodno navedenim izmenama Makefile-a.**



Taster | Događaj
----- | ------
<kbd>W</kbd> <kbd>S</kbd> <kbd>A</kbd> <kbd>D</kbd> | Kretanje kamere po zamišljenoj sferi
<kbd>←</kbd> <kbd>→</kbd> | Pomeranje igrača levo-desno
<kbd>R</kbd> | Vraćanje kamere u početni položaj
<kbd>B</kbd> | Pokretanje igre
<kbd>P</kbd> | Pauziranje igre
<kbd>C</kbd> | Restart igre
<kbd>Esc</kbd> | Izlazak iz programa
