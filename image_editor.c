// Copyright Croitoru Constantin-Bogdan 314CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define VALMAXIMA 255
#define DIM_MAX 100

// structura pentru matricea cu pixeli color
typedef struct {
	int r, a, v;
} color_rgb;

// structura care retine datele despre imagine
typedef struct {
	char cuv_magic[3];
	int tip;// daca tip este 0 imaginea este in tonuri gri, 1 este color
	int latime, inaltime;
	int **gri; // matricea pentru imaginea in tonuri gri
	color_rgb **color; // matricea pentru imaginea color
	int val_max;
	int ok_incarcat;// 0 daca nicio imagine nu e incarcata, 1 daca este
	int x[2], y[2];// coordonatele selectiei

} imagine;

//dezalocare memorie pt matrice de tip int
void dezalocare_memorie(int **v, int n)
{
	for (int i = 0; i < n; i++)
		free(v[i]);
	free(v);
}

//dezalocare memorie pt matrice de tipul structurii color_rgb
void dezalocare_memorie_color(color_rgb **v, int n)
{
	int i;
	for (i = 0; i < n; i++)
		free(v[i]);
	free(v);
}

//functia elibereaza din memorie imaginea
void eliberare(imagine *img)
{
	if (img->ok_incarcat == 1) {
		if (img->tip == 0)
			dezalocare_memorie(img->gri, img->inaltime);
		else
			dezalocare_memorie_color(img->color, img->inaltime);

		img->ok_incarcat = 0;
		img->inaltime = 0;
		img->latime = 0;
		img->x[0] = 0;
		img->x[1] = 0;
		img->y[0] = 0;
		img->y[1] = 0;
	}
}

//alocare dinamic matrice de tip int
int **alocare_matrice(int nr_l, int nr_c, imagine *img)
{
	int **matrice;
	int i, j;

	matrice = (int **)malloc(nr_l * sizeof(int *));
	if (!matrice) {
		fprintf(stderr, "Alocare esuata");
		eliberare(img);
		exit(-17);
	}

	for (i = 0; i < nr_l; i++) {
		matrice[i] = (int *)malloc(nr_c * sizeof(int));
		if (!matrice[i]) {
			for (j = 0; j <= i; j++)
				free(matrice[j]);
			fprintf(stderr, "Alocare esuata");
			eliberare(img);
			exit(-17);
		}
	}

	return matrice;
}

// alocare dinamic matrice de tipul strucutrii color_rgb
color_rgb **alocare_matrice_color(int nr_l, int nr_c, imagine *img)
{
	color_rgb **matrice;
	int i, j;

	matrice = (color_rgb **)malloc(nr_l * sizeof(color_rgb *));
	if (!matrice) {
		fprintf(stderr, "Alocare esuata");
		eliberare(img);
		exit(-17);
	}

	for (i = 0; i < nr_l; i++) {
		matrice[i] = (color_rgb *)malloc(nr_c * sizeof(color_rgb));
		if (!matrice[i]) {
			for (j = 0; j <= i; j++)
				free(matrice[j]);
			fprintf(stderr, "Alocare esuata");
			eliberare(img);
			exit(-17);
		}
	}

	return matrice;
}

//se atribuie valorile pentru imaginea in tonuri gri
void atribuire_gri(imagine *img, int inaltime2, int latime2, int max_v,
				   char tipul[])
{
	strcpy(img->cuv_magic, tipul);
	img->latime = latime2;
	img->inaltime = inaltime2;
	img->val_max = max_v;
	img->ok_incarcat = 1;
	img->tip = 0;  //in tonuri gri
	img->x[0] = 0;
	img->x[1] = img->latime;
	img->y[0] = 0;
	img->y[1] = img->inaltime;
}

//se atribuie valorile pentru imaginea color
void atribuire_color(imagine *img, int inaltime2, int latime2, int max_v,
					 char tipul[])
{
	strcpy(img->cuv_magic, tipul);
	img->latime = latime2;
	img->inaltime = inaltime2;
	img->val_max = max_v;
	img->ok_incarcat = 1;
	img->tip = 1;  //color
	img->x[0] = 0;
	img->x[1] = img->latime;
	img->y[0] = 0;
	img->y[1] = img->inaltime;
}

// se elimina comentariile din fisier
void eliminare_comentarii(FILE *in)
{
	char litera;
	fscanf(in, "%c", &litera);
	//daca prima litera este # se citeste linia pana la final, inclusiv '\n'
	if (litera == '#')
		while (litera != '\n')
			fscanf(in, "%c", &litera);
	else// in caz  contrar cursorul se muta cu o pozitie in spate in fisier
		fseek(in, -1, SEEK_CUR);
}

//functia incarcare in memorie poza citita
void incarcare(imagine *img, char nume_fisier[])
{
	if (img->ok_incarcat == 1)//daca exista vreo imagine incarcata se sterge
		eliberare(img);

	FILE *in = fopen(nume_fisier, "rt");//deschidere fisier
	if (!in) {
		printf("Failed to load %s\n", nume_fisier);
		eliberare(img);
		return;
	}

	char tip[3];
	int latime2, inaltime2, max_v;
	eliminare_comentarii(in);
	fscanf(in, "%s", tip);
	eliminare_comentarii(in);
	fscanf(in, "%d%d", &latime2, &inaltime2);
	eliminare_comentarii(in);
	fscanf(in, "%d", &max_v);
	eliminare_comentarii(in);

	if (strcmp(tip, "P2") == 0) {//tonuri gri
		atribuire_gri(img, inaltime2, latime2, max_v, tip);
		img->gri = alocare_matrice(inaltime2, latime2, img);
		//se citesc valorile pixelilor pentru imaginea in tonuri gri
		for (int i = 0; i < inaltime2; i++)
			for (int j = 0; j < latime2; j++)
				fscanf(in, "%d", &img->gri[i][j]);

	} else if (strcmp(tip, "P3") == 0) {//color
		atribuire_color(img, inaltime2, latime2, max_v, tip);
		img->color = alocare_matrice_color(inaltime2, latime2, img);
		int r, a, v;
		//se citesc valorile pixelilor pentru imaginea color
		for (int i = 0; i < img->inaltime; i++)
			for (int j = 0; j < img->latime; j++) {
				fscanf(in, "%d", &r);
				fscanf(in, "%d", &v);
				fscanf(in, "%d", &a);
				img->color[i][j].r = r;
				img->color[i][j].v = v;
				img->color[i][j].a = a;
			}

	} else {
		int aux = ftell(in);//se retine pozitia in fisier
		fclose(in);
		in = fopen(nume_fisier, "rb");//se deschide ca fisier binar pt citire
		fseek(in, aux + 1, SEEK_SET);// se pozitioneaza in fisier unde a ramas
		if (strcmp(tip, "P5") == 0) {//in tonuri gri
			atribuire_gri(img, inaltime2, latime2, max_v, tip);
			img->gri = alocare_matrice(inaltime2, latime2, img);
			unsigned char a;
			//se citesc valorile pixelilor pentru imaginea in tonuri gri
			for (int i = 0; i < inaltime2; i++)
				for (int j = 0; j < latime2; j++) {
					fread(&a, sizeof(unsigned char), 1, in);
					img->gri[i][j] = (int)a;
				}

		} else if (strcmp(tip, "P6") == 0) {//color
			atribuire_color(img, inaltime2, latime2, max_v, tip);
			img->color = alocare_matrice_color(inaltime2, latime2, img);
			unsigned char r, v, a;
			//se citesc valorile pixelilor pentru imaginea color
			for (int i = 0; i < inaltime2; i++)
				for (int j = 0; j < latime2; j++) {
					fread(&r, sizeof(unsigned char), 1, in);
					fread(&v, sizeof(unsigned char), 1, in);
					fread(&a, sizeof(unsigned char), 1, in);
					img->color[i][j].r = (int)r;
					img->color[i][j].v = (int)v;
					img->color[i][j].a = (int)a;
				}
		}
	}
	fclose(in);
	printf("Loaded %s\n", nume_fisier);
}

//functia care realizeaza operatia SELECT x1 y1 x2 y2
void selectare_xy(imagine *img, int x1, int x2, int y1, int y2)
{
	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	//se ordoneaza intervarele x1 x2 si y1 y2
	if (x1 > x2) {
		int aux = x1;
		x1 = x2;
		x2 = aux;
	}
	if (y1 > y2) {
		int aux = y1;
		y1 = y2;
		y2 = aux;
	}
	//se verifica conditiile
	if (img->ok_incarcat == 1) {
		if (x1 < 0 || x1 >= img->latime || x2 <= 0 || x2 > img->latime) {
			printf("Invalid set of coordinates\n");
			return;
		}
		if (y1 < 0 || y1 >= img->inaltime || y2 <= 0 || y2 > img->inaltime) {
			printf("Invalid set of coordinates\n");
			return;
		}
		if (x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
			return;
		}

		//se retin valorile citite
		img->x[0] = x1;
		img->x[1] = x2;
		img->y[0] = y1;
		img->y[1] = y2;
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	}
}

//functia care realizeaza operatia SELECT ALL
void selectare_tot(imagine *img)
{
	if (img->ok_incarcat == 1) {
		img->x[0] = 0;
		img->x[1] = img->latime;
		img->y[0] = 0;
		img->y[1] = img->inaltime;
		printf("Selected ALL\n");
	} else {
		printf("No image loaded\n");
	}
}

//functia decide ce operatie de SELECT trebuie facuta
void alegere_selectare(imagine *img, char comanda[])
{
	char *p, s[100];
	p = strtok(comanda, "\n ");// p retine primul cuvant(SELECT)
	p = strtok(NULL, "\n ");//retine al doilea cuvant
	strcpy(s, p);

	if (strcmp(s, "ALL") == 0) {
		selectare_tot(img);
	} else {
		int x1, x2, y1, y2, ok = 0;
		x1 = (int)atoi(s);

		p = strtok(NULL, "\n ");//se retine urmatorul numar(y1)
		if (p) {
			if (p[0] >= 'a' && p[0] <= 'z')//daca este litera nu e bun
				ok = 17;
			y1 = (int)atoi(p);
		} else {
			ok = 17;
		}
		p = strtok(NULL, "\n ");//se retine urmatorul numar(x2)

		if (p) {
			if (p[0] >= 'a' && p[0] <= 'z')//daca este litera nu e bun
				ok = 17;
			x2 = (int)atoi(p);
		} else {
			ok = 17;
		}

		p = strtok(NULL, "\n ");//se retine urmatorul numar(y2)

		if (p) {
			if (p[0] >= 'a' && p[0] <= 'z')//daca este litera nu e bun
				ok = 17;
			y2 = (int)atoi(p);
		} else {
			ok = 17;
		}

		if (ok == 17)//daca cel putin unul din coordonate nu a fost numar
			printf("Invalid command\n");
		else
			selectare_xy(img, x1, x2, y1, y2);
	}
}

//functia realiza suma elementelor din vectorul frecventa pana la un anumit
// elemnet inclusiv el
int suma(int *frecventa, int element)
{
	int s = 0;
	for (int i = 0; i <= element; i++)
		s += frecventa[i];
	return s;
}

// functia realizea clamp
int clampul_meu(double x)
{
	if (x < 0)
		x = 0;
	else if (x > 255)
		x = 255;
	return (int)x;
}

// realizeaza operatia de EQUALIZE
void egalizare(imagine *img)
{
	int i, j;
	if (img->ok_incarcat == 1 && img->tip == 1) {
		printf("Black and white image needed\n");
		return;
	}
	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	int *frecventa;//vector care retine frecventa pixelilor
	frecventa = (int *)calloc(256, sizeof(int));

	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++)
			frecventa[img->gri[i][j]]++;//se calculeaza frecventa

	double arie = (double)img->inaltime * img->latime;//se calculeaza aria

	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++) {
			int sum = suma(frecventa, img->gri[i][j]);// se face suma
			// se calculeza noua valoare
			double aux = VALMAXIMA * (double)(1 / arie) * sum;
			aux = clampul_meu(aux);
			img->gri[i][j] = aux;
		}
	printf("Equalize done\n");
	free(frecventa);
}

//functia realizea operatia de CROP pentru imagine in tonuri gri
void crop_gri(imagine *img)
{
	int **aux;
	int nr_l = img->y[1] - img->y[0];
	int nr_c = img->x[1] - img->x[0];
	// se aloca matricea auxiliara cu dimensiunea cuprinsa intre selectii
	aux = alocare_matrice(nr_l, nr_c, img);

	int i, j;
	//se copiaza bucata selectata
	for (i = 0; i < nr_l; i++)
		for (j = 0; j < nr_c; j++)
			aux[i][j] = img->gri[i + img->y[0]][j + img->x[0]];
	dezalocare_memorie(img->gri, img->inaltime);// se elibereaza memoria

	img->gri = alocare_matrice(nr_l, nr_c, img);// se aloca dinamic matricea
	for (i = 0; i < nr_l; i++)
		for (j = 0; j < nr_c; j++)
			img->gri[i][j] = aux[i][j];//se copiaza elementele

	img->inaltime = nr_l;
	img->latime = nr_c;
	img->x[0] = 0;
	img->x[1] = nr_c;
	img->y[0] = 0;
	img->y[1] = nr_l;

	dezalocare_memorie(aux, nr_l);//se elibereaza matricea axiliara
}

//realizeaza operatia CROP pentru imagine color
void crop_color(imagine *img)
{
	color_rgb **aux;
	int nr_l = img->y[1] - img->y[0];
	int nr_c = img->x[1] - img->x[0];
	// se aloca matricea auxiliara cu dimensiunea cuprinsa intre selectii
	aux = alocare_matrice_color(nr_l, nr_c, img);

	int i, j;
	//se copiaza bucata selectata
	for (i = 0; i < nr_l; i++)
		for (j = 0; j < nr_c; j++) {
			aux[i][j].r = img->color[i + img->y[0]][j + img->x[0]].r;
			aux[i][j].a = img->color[i + img->y[0]][j + img->x[0]].a;
			aux[i][j].v = img->color[i + img->y[0]][j + img->x[0]].v;
		}
	//se elibereaza memoria
	dezalocare_memorie_color(img->color, img->inaltime);
	//se aloca dinamic
	img->color = alocare_matrice_color(nr_l, nr_c, img);

	for (i = 0; i < nr_l; i++)// se copiaza pixelii
		for (j = 0; j < nr_c; j++) {
			img->color[i][j].r = aux[i][j].r;
			img->color[i][j].a = aux[i][j].a;
			img->color[i][j].v = aux[i][j].v;
		}

	img->inaltime = nr_l;
	img->latime = nr_c;
	img->x[0] = 0;
	img->x[1] = nr_c;
	img->y[0] = 0;
	img->y[1] = nr_l;

	dezalocare_memorie_color(aux, nr_l);//se elibereaza memoria
}

//functia apeleaza operatia CROP potrivita pentru tipul imaginii
void crop(imagine *img)
{
	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	if (img->tip == 0)// in tonuri gri
		crop_gri(img);

	if (img->tip == 1)// color
		crop_color(img);

	printf("Image cropped\n");
}

// se aloca dinamic o matrice de tip double
double **alocare_double(int nr_l, int nr_c, imagine *img)
{
	double **matrice;
	int i, j;

	matrice = (double **)malloc(nr_l * sizeof(double *));
	if (!matrice) {
		fprintf(stderr, "Alocare esuata\n");
		eliberare(img);
		exit(-17);
	}

	for (i = 0; i < nr_l; i++) {
		matrice[i] = (double *)malloc(nr_c * sizeof(double));
		if (!matrice[i]) {
			for (j = 0; j <= i; j++)
				free(matrice[j]);
			fprintf(stderr, "Alocare esuata\n");
			eliberare(img);
			exit(-17);
		}
	}

	return matrice;
}

//functia initiaza matricea in functie de tipul filtrului
void initiere_matrici(double **matrice, int ok)
{
	if (ok == 0) { //EDGE
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrice[i][j] = -1;
		matrice[1][1] = 8;

		return;
	}

	if (ok == 1) { // SHARPEN
		matrice[0][0] = 0;
		matrice[0][1] = -1;
		matrice[0][2] = 0;
		matrice[1][0] = -1;
		matrice[1][1] = 5;
		matrice[1][2] = -1;
		matrice[2][0] = 0;
		matrice[2][1] = -1;
		matrice[2][2] = 0;

		return;
	}

	if (ok == 2) { // BLUR
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrice[i][j] = (double)1 / 9;

		return;
	}

	if (ok == 3) { // GAUSSIAN_BLUR
		matrice[0][0] = (double)1 / 16;
		matrice[0][1] = (double)1 / 16 * 2;
		matrice[0][2] = (double)1 / 16;
		matrice[1][0] = (double)1 / 16 * 2;
		matrice[1][1] = (double)1 / 16 * 4;
		matrice[1][2] = (double)1 / 16 * 2;
		matrice[2][0] = (double)1 / 16;
		matrice[2][1] = (double)1 / 16 * 2;
		matrice[2][2] = (double)1 / 16;

		return;
	}
}

//se verifica ce filtru s-a citit
int verificare_filtru(char filtru[])
{
	if (strcmp(filtru, "EDGE") == 0)
		return 0;

	if (strcmp(filtru, "SHARPEN") == 0)
		return 1;

	if (strcmp(filtru, "BLUR") == 0)
		return 2;

	if (strcmp(filtru, "GAUSSIAN_BLUR") == 0)
		return 3;

	return -1;
}

//functia realizeaza operatia APPLY
void aplicare_filtru(imagine *img, char tip_filtru)
{
	int i, j;
	double **mat_filtru;
	mat_filtru = alocare_double(3, 3, img);
	initiere_matrici(mat_filtru, tip_filtru);// se initializeaza matricea

	double aux_r = 0, aux_a = 0, aux_v = 0;

	color_rgb **aux;//se realizeaza o copie
	aux = alocare_matrice_color(img->inaltime, img->latime, img);

	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++) {
			aux[i][j].r = img->color[i][j].r;
			aux[i][j].a = img->color[i][j].a;
			aux[i][j].v = img->color[i][j].v;
		}
	//se aplica filtrul
	for (i = img->y[0]; i < img->y[1]; i++)
		for (j = img->x[0]; j < img->x[1]; j++) {
			if (i > 0 && i < img->inaltime - 1 &&
				j > 0 && j < img->latime - 1) {
				aux_r = 0, aux_a = 0, aux_v = 0;
				for (int k = i - 1; k <= i + 1; k++)
					for (int l = j - 1; l <= j + 1; l++) {
						int nr_l = k - i + 1, nr_c = l - j + 1;
						aux_r += img->color[k][l].r * mat_filtru[nr_l][nr_c];
						aux_a += img->color[k][l].a * mat_filtru[nr_l][nr_c];
						aux_v += img->color[k][l].v * mat_filtru[nr_l][nr_c];
					}

				aux_r = clampul_meu(round(aux_r));
				aux_a = clampul_meu(round(aux_a));
				aux_v = clampul_meu(round(aux_v));
				aux[i][j].r = aux_r;
				aux[i][j].a = aux_a;
				aux[i][j].v = aux_v;
			}
		}
	//se retin valorile in matricea img->color
	for (i = img->y[0]; i < img->y[1]; i++)
		for (j = img->x[0]; j < img->x[1]; j++) {
			img->color[i][j].r = aux[i][j].r;
			img->color[i][j].a = aux[i][j].a;
			img->color[i][j].v = aux[i][j].v;
		}

	dezalocare_memorie_color(aux, img->inaltime);
	for (i = 0; i < 3; i++)
		free(mat_filtru[i]);
	free(mat_filtru);
}

//se verifica conditiile pentru operatia APPLY
void aplicare_verificare(imagine *img, char s[])
{
	char filtru[DIM_MAX];

	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	char *p;
	p = strtok(s, "\n ");
	p = strtok(NULL, "\n ");
	if (!p) {
		printf("Invalid command\n");
		return;
	}
	strcpy(filtru, p);

	int tip_filtru = verificare_filtru(filtru);
	if (tip_filtru == -1) {
		printf("APPLY parameter invalid\n");
		return;
	}

	if (img->tip == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	aplicare_filtru(img, tip_filtru);
	printf("APPLY %s done\n", filtru);
}

//salveaza imaginea intr-un fisier de tip text
void salvare_text(imagine *img, char nume_fisier[])
{
	FILE *out = fopen(nume_fisier, "wt");// se deschide fisierul
	if (!out) {
		printf("Nu s-a putut deschide fisierul\n");
		return;
	}

	int i, j;
	if (img->tip == 0) {  //este gri
		//se introduc valorile in fisier
		fprintf(out, "%s\n", "P2");
		fprintf(out, "%d %d\n%d\n", img->latime, img->inaltime, img->val_max);

		for (i = 0; i < img->inaltime; i++) {
			for (j = 0; j < img->latime; j++)
				fprintf(out, "%d ", img->gri[i][j]);
			fprintf(out, "\n");
		}
	}
	if (img->tip == 1) { // este color
		//se introduc valorile in fisier
		fprintf(out, "%s\n", "P3");
		fprintf(out, "%d %d\n%d\n", img->latime, img->inaltime, img->val_max);

		for (i = 0; i < img->inaltime; i++) {
			for (j = 0; j < img->latime; j++) {
				fprintf(out, "%d ", img->color[i][j].r);
				fprintf(out, "%d ", img->color[i][j].v);
				fprintf(out, "%d ", img->color[i][j].a);
			}
			fprintf(out, "\n");
		}
	}

	fclose(out);
}

//salveaza imaginea intr-un fisier de tip binar
void salvare_binar(imagine *img, char nume_fisier[])
{
	FILE *out = fopen(nume_fisier, "wb");// se deschide fisierul
	if (!out) {
		printf("Nu s-a putut deschide fisierul\n");
		return;
	}

	int i, j;
	if (img->tip == 0) {  //este gri
		//se introduc valorile in fisier
		fprintf(out, "%s\n", "P5");
		fprintf(out, "%d %d\n%d\n", img->latime, img->inaltime, img->val_max);
		unsigned char a;
		for (i = 0; i < img->inaltime; i++)
			for (j = 0; j < img->latime; j++) {
				a = (unsigned char)img->gri[i][j];
				fwrite(&a, sizeof(unsigned char), 1, out);
			}
	}

	if (img->tip == 1) { //este color
		//se introduc valorile in fisier
		fprintf(out, "%s\n", "P6");
		fprintf(out, "%d %d\n%d\n", img->latime, img->inaltime, img->val_max);
		unsigned char r, v, a;
		for (i = 0; i < img->inaltime; i++)
			for (j = 0; j < img->latime; j++) {
				r = (unsigned char)img->color[i][j].r;
				v = (unsigned char)img->color[i][j].v;
				a = (unsigned char)img->color[i][j].a;
				fwrite(&r, sizeof(unsigned char), 1, out);
				fwrite(&v, sizeof(unsigned char), 1, out);
				fwrite(&a, sizeof(unsigned char), 1, out);
			}
	}

	fclose(out);
}

//realizeaza aperatia SAVE
void salvare(imagine *img, char s[])
{
	char nume_fisier[100];
	char *p;
	p = strtok(s, "\n ");//retine primul cuvant
	p = strtok(NULL, "\n ");//retine al doilea cuvant(numele fisierului)
	strcpy(nume_fisier, p);

	p = strtok(NULL, " ");//retine al treilea cuvant(ascii) daca exista

	int ok = 0;//ok ne spune cum sa salvam fisierul 0=binar, 1=text
	if (p)
		if (p[0] == 'a')
			ok = 1;

	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	if (ok == 1)
		salvare_text(img, nume_fisier);
	else
		salvare_binar(img, nume_fisier);

	printf("Saved %s\n", nume_fisier);
}

//realizeaza operatia EXIT
void iesire(imagine *img)
{
	if (img->ok_incarcat == 0)
		printf("No image loaded\n");

	if (img->ok_incarcat == 1)
		eliberare(img);//eliberare memorie
}

//aduce dimensiunea vectorului frecventa la nr_bin adunand valorile
void egalare(int frecventa[], int nr, int nr_bin, imagine *img)
{
	int x = nr / nr_bin;

	int *frecventa2, nr2 = 0, i, j;
	frecventa2 = (int *)calloc(256, sizeof(int));// se aloca dianmic
	if (!frecventa2) {
		fprintf(stderr, "Alocare esuata");
		eliberare(img);
		exit(-17);
	}
	//se aduna elemente
	for (i = 0; i < nr; i = i + x) {
		for (j = i; j < i + x; j++)
			frecventa2[nr2] += frecventa[j];
		nr2++;
	}
	for (i = 0; i < nr_bin; i++)
		frecventa[i] = frecventa2[i];

	free(frecventa2);
}

//calculeaza numarul de stelute ce trebuie afisat
int calculare_stelute(int valoare, int maxi, int x)
{
	double nr = 0;
	nr = (double)valoare / maxi * x;

	return (int)nr;
}

//functia verifica daca numarul a este putere a lui 2
int verif_putere2(int a)
{
	while (a % 2 == 0)
		a = a / 2;

	return a;
}

//realizeaza operatia HISTOGRAM
void histograma(imagine *img, char s[])
{
	int x = -1, y = -1;

	char *p;
	p = strtok(s, "\n ");//retine primul cuvant(HISTOGRAM)
	p = strtok(NULL, "\n ");//retine al doilea
	if (p)
		x = atoi(p);
	p = strtok(NULL, "\n ");//retine al treilea
	if (p)
		y = atoi(p);
	p = strtok(NULL, "\n ");//retine si ultimul cuvant daca mai exista

	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	if (p || x == -1 || y == -1) {
		printf("Invalid command\n");
		return;
	}

	if (y > 256 || verif_putere2(y) != 1) {
		printf("Invalid set of parameters\n");
		return;
	}
	if (img->tip == 1) {
		printf("Black and white image needed\n");
		return;
	}

	int *frecventa, i, j;
	frecventa = (int *)calloc(256, sizeof(int));// se aloca dinamic
	if (!frecventa) {
		fprintf(stderr, "Alocare esuata");
		eliberare(img);
		exit(-17);
	}
	//se calculeaza vectorul de frecvanta pentru pixeli
	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++)
			frecventa[img->gri[i][j]]++;

	egalare(frecventa, 256, y, img);//se aduce la numarul de binuri
	int maxi = 0;
	//se calculeaza maximul
	for (i = 0; i < y; i++)
		if (maxi < frecventa[i])
			maxi = frecventa[i];
	//se afiseaza histograma
	for (int i = 0; i < y; i++) {
		int nr_stelute = calculare_stelute(frecventa[i], maxi, x);
		printf("%d\t|\t", nr_stelute);
		for (int j = 0; j < nr_stelute; j++)
			printf("*");
		printf("\n");
	}
	free(frecventa);
}

//roteste imaginea
void rotatie_imagine_gri(imagine *img)
{
	int **aux = alocare_matrice(img->latime, img->inaltime, img);
	int i, j;
	//se retine in aux matrice rotita
	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++)
			aux[j][i] = img->gri[img->inaltime - 1 - i][j];

	dezalocare_memorie(img->gri, img->inaltime);// se elibereaza memoria
	//se schimba valorile
	int aux2;
	aux2 = img->inaltime;
	img->inaltime = img->latime;
	img->latime = aux2;
	int aux3 = img->x[1];
	img->x[1] = img->y[1];
	img->y[1] = aux3;

	img->gri = alocare_matrice(img->inaltime, img->latime, img);

	//se copiaza valorile in matrice
	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++)
			img->gri[i][j] = aux[i][j];
	dezalocare_memorie(aux, img->inaltime);
}

void rotatie_90grade_gri(imagine *img)
{
	//daca selectia este pe toata imaginea se apeleaza urmatoarea functie
	if (img->y[0] == 0 && img->y[1] == img->inaltime && img->x[0] == 0 &&
		img->x[1] == img->latime) {
		rotatie_imagine_gri(img);
	} else {
		int nr_lc = img->x[1] - img->x[0];
		int **aux = alocare_matrice(nr_lc, nr_lc, img);
		int i, j, a1, a2;
		a1 = img->y[1] - 1;
		a2 = img->x[0];
		//se retine in aux matricea rotita
		for (i = img->y[0]; i < img->y[1]; i++) {
			a1 = img->y[1] - 1;
			for (j = img->x[0]; j < img->x[1]; j++) {
				aux[i - img->y[0]][j - img->x[0]] = img->gri[a1][a2];
				a1 -= 1;
			}
			a2++;
		}

		//se copiaza valorile din aux
		for (i = img->y[0]; i < img->y[1]; i++)
			for (j = img->x[0]; j < img->x[1]; j++)
				img->gri[i][j] = aux[i - img->y[0]][j - img->x[0]];

		dezalocare_memorie(aux, nr_lc);
	}
}

void rotatie_imagine_color(imagine *img)
{
	color_rgb **aux;
	aux = alocare_matrice_color(img->latime, img->inaltime, img);
	int i, j;
	//se retine in aux matricea rotita
	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++) {
			aux[j][i].r = img->color[img->inaltime - 1 - i][j].r;
			aux[j][i].v = img->color[img->inaltime - 1 - i][j].v;
			aux[j][i].a = img->color[img->inaltime - 1 - i][j].a;
		}

	dezalocare_memorie_color(img->color, img->inaltime);
	//se schimba valorile intre ele
	int aux2;
	aux2 = img->inaltime;
	img->inaltime = img->latime;
	img->latime = aux2;
	int aux3 = img->x[1];
	img->x[1] = img->y[1];
	img->y[1] = aux3;

	img->color = alocare_matrice_color(img->inaltime, img->latime, img);

	//se copiaza valorile in matrice
	for (i = 0; i < img->inaltime; i++)
		for (j = 0; j < img->latime; j++) {
			img->color[i][j].r = aux[i][j].r;
			img->color[i][j].v = aux[i][j].v;
			img->color[i][j].a = aux[i][j].a;
		}

	dezalocare_memorie_color(aux, img->inaltime);
}

void rotatie_90grade_color(imagine *img)
{
	//daca selectia este pe toata imaginea se apeleaza urmatoarea functie
	if (img->y[0] == 0 && img->y[1] == img->inaltime && img->x[0] == 0 &&
		img->x[1] == img->latime) {
		rotatie_imagine_color(img);
	} else {
		int nr_lc = img->x[1] - img->x[0];
		color_rgb **aux = alocare_matrice_color(nr_lc, nr_lc, img);
		int i, j, a1, a2;
		a1 = img->y[1] - 1;
		a2 = img->x[0];
		//se realizeaza rotatia si se retine in matricea aux
		for (i = img->y[0]; i < img->y[1]; i++) {
			a1 = img->y[1] - 1;
			for (j = img->x[0]; j < img->x[1]; j++) {
				aux[i - img->y[0]][j - img->x[0]].r = img->color[a1][a2].r;
				aux[i - img->y[0]][j - img->x[0]].v = img->color[a1][a2].v;
				aux[i - img->y[0]][j - img->x[0]].a = img->color[a1][a2].a;
				a1 -= 1;
			}
			a2++;
		}
		//se copiaza in matrice
		for (i = img->y[0]; i < img->y[1]; i++)
			for (j = img->x[0]; j < img->x[1]; j++) {
				img->color[i][j].r = aux[i - img->y[0]][j - img->x[0]].r;
				img->color[i][j].v = aux[i - img->y[0]][j - img->x[0]].v;
				img->color[i][j].a = aux[i - img->y[0]][j - img->x[0]].a;
			}

		dezalocare_memorie_color(aux, nr_lc);
	}
}

//functia realizeaza operatia ROTATE
void rotatie(imagine *img, char s[])
{
	int grade;

	grade = atoi(s);
	if (img->ok_incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	if (grade % 90 != 0 || grade > 360 || grade < -360) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (img->y[1] != img->inaltime || img->y[0] != 0 ||
		img->x[1] != img->latime || img->x[0] != 0)
		if (img->y[1] - img->y[0] != img->x[1] - img->x[0]) {
			printf("The selection must be square\n");
			return;
		}
	if (img->tip == 0) {//daca este gri
		if (grade / 90 == 1 || grade / 90 == -3)
			rotatie_90grade_gri(img);

		if (grade / 90 == 2 || grade / 90 == -2) {
			rotatie_90grade_gri(img);
			rotatie_90grade_gri(img);
		}

		if (grade / 90 == 3 || grade / 90 == -1) {
			rotatie_90grade_gri(img);
			rotatie_90grade_gri(img);
			rotatie_90grade_gri(img);
		}
	} else {//daca este color
		if (grade / 90 == 1 || grade / 90 == -3)
			rotatie_90grade_color(img);

		if (grade / 90 == 2 || grade / 90 == -2) {
			rotatie_90grade_color(img);
			rotatie_90grade_color(img);
		}

		if (grade / 90 == 3 || grade / 90 == -1) {
			rotatie_90grade_color(img);
			rotatie_90grade_color(img);
			rotatie_90grade_color(img);
		}
	}
	printf("Rotated %d\n", grade);
}

int main(void)
{
	char comanda[DIM_MAX], s[DIM_MAX], comanda2[DIM_MAX];
	char *p;
	imagine img;
	img.ok_incarcat = 0;
	int ok = 1;
	while (ok) {
		fgets(s, 100, stdin);
		char s_cpy[100];
		strcpy(s_cpy, s);

		p = strtok(s, "\n ");
		strcpy(comanda, p);
		p = strtok(NULL, "\n ");
		if (p)
			strcpy(comanda2, p);

		if (strcmp(comanda, "LOAD") == 0) {
			incarcare(&img, comanda2);
		} else if (strcmp(comanda, "SELECT") == 0) {
			alegere_selectare(&img, s_cpy);
		} else if (strcmp(comanda, "HISTOGRAM") == 0) {
			histograma(&img, s_cpy);
		} else if (strcmp(comanda, "EQUALIZE") == 0) {
			egalizare(&img);
		} else if (strcmp(comanda, "ROTATE") == 0) {
			rotatie(&img, comanda2);
		} else if (strcmp(comanda, "CROP") == 0) {
			crop(&img);
		} else if (strcmp(comanda, "APPLY") == 0) {
			aplicare_verificare(&img, s_cpy);
		} else if (strcmp(comanda, "SAVE") == 0) {
			salvare(&img, s_cpy);
		} else if (strcmp(comanda, "EXIT") == 0) {
			iesire(&img);
			ok = 0;
		} else {
			printf("Invalid command\n");
		}
	}
}
