#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct {
    unsigned char r, g, b;
} rgb;

typedef struct {
    int dim, padding, latime, inaltime, el;
    rgb *pixel;
} imagine;

typedef struct {
    int latime, inaltime;
    int *val;
} img_gray;

typedef struct {
    int x, y, cifra;
    double prag;
} match;

void xor32(int dim, unsigned int *r, unsigned int seed) {
    unsigned int x = seed, i;
    for (i = 1; i < dim; ++i) {
        x = x ^ x << 13;
        x = x ^ x >> 17;
        x = x ^ x << 5;
        r[i] = x;
    }
}

void permutare(int dim, unsigned int *p, unsigned int *r) {
    unsigned int i, j, x, aux;
    for (i = 0; i < dim; ++i) {
        p[i] = i;
    }
    for (i = dim - 1, j = 1; i > 0; --i, ++j) {
        x = r[j] % (i + 1);
        aux = p[x];
        p[x] = p[i];
        p[i] = aux;
    }
}

void modifica_pixeli(imagine img, unsigned int *r, unsigned int sv) {
    int i, x0, x1, x2, s1, s2, s3;
    s1 = sv & ((1 << 8) - 1);
    s2 = (sv >> 8) & ((1 << 8) - 1);
    s3 = (sv >> 16) & ((1 << 8) - 1);
    x0 = r[img.el] & ((1 << 8) - 1);
    x1 = (r[img.el] >> 8) & ((1 << 8) - 1);
    x2 = (r[img.el] >> 16) & ((1 << 8) - 1);
    img.pixel[0].r = s3 ^ img.pixel[0].r ^ x2;
    img.pixel[0].g = s2 ^ img.pixel[0].g ^ x1;
    img.pixel[0].b = s1 ^ img.pixel[0].b ^ x0;
    for (i = 1; i < img.el; ++i) {
        x0 = r[i + img.el] & ((1 << 8) - 1);
        x1 = (r[i + img.el] >> 8) & ((1 << 8) - 1);
        x2 = (r[i + img.el] >> 16) & ((1 << 8) - 1);
        img.pixel[i].r = img.pixel[i - 1].r ^ img.pixel[i].r ^ x2;
        img.pixel[i].g = img.pixel[i - 1].g ^ img.pixel[i].g ^ x1;
        img.pixel[i].b = img.pixel[i - 1].b ^ img.pixel[i].b ^ x0;
    }
}

void cripteaza(imagine img, char* nume_fisier_key) {
    FILE *fin;
    fin = fopen (nume_fisier_key, "r");

    unsigned int r0, sv, i;

    fscanf(fin, "%u %u", &r0, &sv);

    fclose(fin);

	unsigned int *r = (unsigned int *)malloc(2 * img.el * sizeof(unsigned int));
	unsigned int *p = (unsigned int *)malloc(img.el * sizeof(unsigned int));

	xor32 (2 * img.el, r, r0);
	permutare(img.el, p, r);

    rgb *vectoraux = (rgb *)malloc(img.el * sizeof(rgb));



    for (i = 0; i < img.el; ++i) {
        vectoraux[p[i]] = img.pixel[i];
	}

    free(p);

    for (i = 0; i < img.el; ++i) {
        img.pixel[i] = vectoraux[i];
    }

	free(vectoraux);

    modifica_pixeli(img, r, sv);

    free(r);
}

void intoarce_pixeli(imagine img, unsigned int *r, unsigned int sv) {
    int i, x0, x1, x2, s1, s2, s3;
    for (i = img.el; i > 0; --i) {
        x0 = r[i + img.el] & ((1 << 8) - 1);
        x1 = (r[i + img.el] >> 8) & ((1 << 8) - 1);
        x2 = (r[i + img.el] >> 16) & ((1 << 8) - 1);
        img.pixel[i].r = img.pixel[i - 1].r ^ img.pixel[i].r ^ x2;
        img.pixel[i].g = img.pixel[i - 1].g ^ img.pixel[i].g ^ x1;
        img.pixel[i].b = img.pixel[i - 1].b ^ img.pixel[i].b ^ x0;
    }
    s1 = sv & ((1 << 8) - 1);
    s2 = (sv >> 8) & ((1 << 8) - 1);
    s3 = (sv >> 16) & ((1 << 8) - 1);
    x0 = r[img.el] & ((1 << 8) - 1);
    x1 = (r[img.el] >> 8) & ((1 << 8) - 1);
    x2 = (r[img.el] >> 16) & ((1 << 8) - 1);
    img.pixel[0].r = s3 ^ img.pixel[0].r ^ x2;
    img.pixel[0].g = s2 ^ img.pixel[0].g ^ x1;
    img.pixel[0].b = s1 ^ img.pixel[0].b ^ x0;
}

void decripteaza(imagine img, char* nume_fisier_key) {
    FILE *fin;
    fin = fopen (nume_fisier_key, "r");

    unsigned int r0, sv, i;

    fscanf(fin, "%u %u", &r0, &sv);

    fclose(fin);

	unsigned int *r = (unsigned int *)malloc(2 * img.el * sizeof(unsigned int));
	unsigned int *p = (unsigned int *)malloc(img.el * sizeof(unsigned int));

	xor32 (2 * img.el, r, r0);
	permutare(img.el, p, r);

    intoarce_pixeli(img, r, sv);

    rgb *vectoraux = (rgb *)malloc(img.el * sizeof(rgb));


    for (i = 0; i < img.el; ++i) {
        vectoraux[i] = img.pixel[p[i]];
	}

    free(p);

    for (i = 0; i < img.el; ++i) {
        img.pixel[i] = vectoraux[i];
    }

	free(vectoraux);

    free(r);
}

void calculeaza_frecventa(imagine img) {
    int *fr, *fg, *fb, i;
    fr = (int *)malloc(256 * sizeof(int));
    fg = (int *)malloc(256 * sizeof(int));
    fb = (int *)malloc(256 * sizeof(int));

    for (i = 0; i < 256; ++i) {
        fr[i] = fb[i] = fg[i] = 0;
    }

    for (i = 0; i < img.el; ++i) {
        fr[ img.pixel[i].r ]++;
        fg[ img.pixel[i].g ]++;
        fb[ img.pixel[i].b ]++;
    }

    double sumr = 0, sumg = 0, sumb = 0, fbar = img.el / 256;

    for (i = 0; i < 256; ++i) {
        sumr = sumr + (1.0 * fr[i] - fbar) * (fr[i] - fbar) / fbar;
        sumg = sumg + (1.0 * fg[i] - fbar) * (fg[i] - fbar) / fbar;
        sumb = sumb + (1.0 * fb[i] - fbar) * (fb[i] - fbar) / fbar;
    }

    printf("%lf %lf %lf\n", sumr, sumg, sumb);

    free(fr);
    free(fg);
    free(fb);
}

void citire_afisare_criptare(char* nume_fisier_sursa, char* nume_fisier_destinatie, char* nume_fisier_key) {
    FILE *fin, *fout;

    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char RGB[3], aux;

    imagine img;

    fin = fopen (nume_fisier_sursa, "rb");
    if (fin == NULL){
        printf("FISIER IMAGINE INEXISTENT!");
        return;
   	}

    fout = fopen(nume_fisier_destinatie, "wb+");

    fseek(fin, 2, SEEK_SET);
    fread(&img.dim, sizeof(unsigned int), 1, fin);

    fseek(fin, 18, SEEK_SET);
    fread(&img.latime, sizeof(unsigned int), 1, fin);
    fread(&img.inaltime, sizeof(unsigned int), 1, fin);

    img.pixel = (rgb *)malloc(sizeof(rgb) * img.inaltime * img.latime);

	fseek(fin,0,SEEK_SET);

	if (img.latime % 4 != 0) {
        img.padding = 4 - (3 * img.latime) % 4;
    } else {
        img.padding = 0;
    }

    fseek(fin, 0, SEEK_SET);
	unsigned char c;
	while (fread(&c, 1, 1, fin) == 1) {
		fwrite(&c, 1, 1, fout);
		fflush(fout);
	}
	fclose(fin);
	fseek(fout, 54, SEEK_SET);
	int i, j, k = 0;
	for (i = 0; i < img.inaltime; ++i) {
        k = (img.inaltime - i - 1) * img.latime;
		for (j = 0; j < img.latime; ++j) {
            fread(RGB, 3, 1, fout);
            img.pixel[k].r = RGB[2];
            img.pixel[k].g = RGB[1];
            img.pixel[k].b = RGB[0];
        	k++;
		}
		fseek(fout, img.padding, SEEK_CUR);
	}
	img.el = img.inaltime * img.latime;

    printf("\npentru criptare valorile chi sunt:\nimaginea necriptata:");
    calculeaza_frecventa(img);

    cripteaza(img, nume_fisier_key);

    printf("imaginea criptata:");
    calculeaza_frecventa(img);

    fseek(fout, 54, SEEK_SET);
    k = 0;
    for (i = 0; i < img.inaltime; ++i) {
        k = (img.inaltime - i - 1) * img.latime;
		for (j = 0; j < img.latime; ++j) {
            fread(RGB, 3, 1, fout);
        	fseek(fout, -3, SEEK_CUR);
            RGB[2] = img.pixel[k].r;
            RGB[1] = img.pixel[k].g;
            RGB[0] = img.pixel[k].b;
            fwrite(RGB, 3, 1, fout);
        	fflush(fout);
        	k++;
		}
		fseek(fout, img.padding, SEEK_CUR);
	}

    fclose(fout);
    free(img.pixel);
}

void citire_afisare_decriptare(char* nume_fisier_sursa, char* nume_fisier_destinatie, char* nume_fisier_key) {
    FILE *fin, *fout;

    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char RGB[3], aux;

    imagine img;

    fin = fopen (nume_fisier_sursa, "rb");
    if (fin == NULL){
        printf("FISIER IMAGINE INEXISTENT!");
        return;
   	}

    fout = fopen(nume_fisier_destinatie, "wb+");

    fseek(fin, 2, SEEK_SET);
    fread(&img.dim, sizeof(unsigned int), 1, fin);

    fseek(fin, 18, SEEK_SET);
    fread(&img.latime, sizeof(unsigned int), 1, fin);
    fread(&img.inaltime, sizeof(unsigned int), 1, fin);

    img.pixel = (rgb *)malloc(sizeof(rgb) * img.inaltime * img.latime);

	fseek(fin,0,SEEK_SET);

	if (img.latime % 4 != 0) {
        img.padding = 4 - (3 * img.latime) % 4;
    } else {
        img.padding = 0;
    }

    fseek(fin, 0, SEEK_SET);
	unsigned char c;
	while (fread(&c, 1, 1, fin) == 1) {
		fwrite(&c, 1, 1, fout);
		fflush(fout);
	}
	fclose(fin);
	fseek(fout, 54, SEEK_SET);
	int i, j, k = 0;
	for (i = 0; i < img.inaltime; ++i) {
        k = (img.inaltime - i - 1) * img.latime;
		for (j = 0; j < img.latime; ++j) {
            fread(RGB, 3, 1, fout);
            img.pixel[k].r = RGB[2];
            img.pixel[k].g = RGB[1];
            img.pixel[k].b = RGB[0];
        	k++;
		}
		fseek(fout, img.padding, SEEK_CUR);
	}

	printf("pentru decriptare valorile chi sunt:\nimaginea criptata:");
    calculeaza_frecventa(img);

    decripteaza(img, nume_fisier_key);

    printf("imaginea decriptata:");
    calculeaza_frecventa(img);

    fseek(fout, 54, SEEK_SET);
    k = 0;
    for (i = 0; i < img.inaltime; ++i) {
        k = (img.inaltime - i - 1) * img.latime;
		for (j = 0; j < img.latime; ++j) {
            fread(RGB, 3, 1, fout);
        	fseek(fout, -3, SEEK_CUR);
            RGB[2] = img.pixel[k].r;
            RGB[1] = img.pixel[k].g;
            RGB[0] = img.pixel[k].b;
            fwrite(RGB, 3, 1, fout);
        	fflush(fout);
        	k++;
		}
		fseek(fout, img.padding, SEEK_CUR);
	}

    fclose(fout);
    free(img.pixel);
}

void criptare_decriptare(char* fisierINcriptare, char* fisierOUTcriptare, char* fisierKEYcriptare, char* fisierINdecriptare, char* fisierOUTdecriptare, char* fisierKEYdecriptare) {
	citire_afisare_criptare(fisierINcriptare, fisierOUTcriptare, fisierKEYcriptare);
	citire_afisare_decriptare(fisierINdecriptare, fisierOUTdecriptare, fisierKEYdecriptare);
}

void grayscale_cifra(char* nume_fisier_sursa, char* nume_fisier_destinatie, int *cifra) {
   FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], aux;

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);

   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			cifra[i * latime_img + j] = pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout,padding,SEEK_CUR);
	}
	fclose(fout);
}

void aplica_grayscale(int *cif0, int *cif1, int *cif2, int *cif3, int *cif4, int *cif5, int *cif6, int *cif7, int *cif8, int *cif9, char* sablonIN0, char* sablonIN1, char* sablonIN2, char* sablonIN3, char* sablonIN4, char* sablonIN5, char* sablonIN6, char* sablonIN7, char* sablonIN8, char* sablonIN9, char* sablonOUT0, char* sablonOUT1, char* sablonOUT2, char* sablonOUT3, char* sablonOUT4, char* sablonOUT5, char* sablonOUT6, char* sablonOUT7, char* sablonOUT8, char* sablonOUT9) {
    grayscale_cifra(sablonIN0, sablonOUT0, cif0);
    grayscale_cifra(sablonIN1, sablonOUT1, cif1);
    grayscale_cifra(sablonIN2, sablonOUT2, cif2);
    grayscale_cifra(sablonIN3, sablonOUT3, cif3);
    grayscale_cifra(sablonIN4, sablonOUT4, cif4);
    grayscale_cifra(sablonIN5, sablonOUT5, cif5);
    grayscale_cifra(sablonIN6, sablonOUT6, cif6);
    grayscale_cifra(sablonIN7, sablonOUT7, cif7);
    grayscale_cifra(sablonIN8, sablonOUT8, cif8);
    grayscale_cifra(sablonIN9, sablonOUT9, cif9);
}

double calculeaza_medie_chenar(int x, int y, img_gray img) {
    int i, j;
    double sum = 0;
    for (i = x; i < x + 15; ++i) {
        for (j = y; j < y + 11; ++j) {
            sum = sum + 1.0 * img.val[i * img.latime + j];
        }
    }
    return sum / 165;
}

double calculeaza_medie_sablon(int *cifra) {
    int i, j;
    double sum = 0;
    for (i = 0; i < 15; ++i) {
        for (j = 0; j < 11; ++j) {
            sum = sum + 1.0 * cifra[i * 11 + j];
        }
    }
    return 1.0 * sum / 165;
}

double calculeaza_numarator(int x, int y, int *cifra, img_gray img) {
    int i, j;
    double sum = 0, medieChenar = calculeaza_medie_chenar(x, y, img), medieSablon = calculeaza_medie_sablon(cifra);
    for (i = x; i < x + 15; ++i) {
        for (j = y; j < y + 11; ++j) {
            sum = sum + (1.0 * img.val[i * img.latime + j] - medieChenar) * (1.0 * cifra[(i - x) * 11 + (j - y)] - medieSablon);
        }
    }
    return sum;
}

double calculeaza_numitor(int x, int y, int *cifra, img_gray img) {
    int i, j;
    double medieSablon = calculeaza_medie_sablon(cifra), medieChenar = calculeaza_medie_chenar(x, y, img);
    double deltaS = 0, deltaF = 0;
    for (i = x; i < x + 15; ++i) {
        for (j = y; j < y + 11; ++j) {
            deltaS = deltaS + (1.0 * cifra[(i - x) * 11 + (j - y)] - medieSablon) * (1.0 * cifra[(i - x) * 11 + (j - y)] - medieSablon);
        }
    }
    for (i = x; i < x + 15; ++i) {
        for (j = y; j < y + 11; ++j) {
            deltaF = deltaF + (1.0 * img.val[i * img.latime + j] - medieChenar) * (1.0 * img.val[i * img.latime + j] - medieChenar);
        }
    }
    return sqrt(deltaF / 164) * sqrt(deltaS / 164);
}

double calcul_corespondenta(int x, int y, img_gray img, int *cifra) {
    return calculeaza_numarator(x, y, cifra, img) / (calculeaza_numitor(x, y, cifra, img) * 165);
}

int cmp(const void * a, const void * b) {
    if (((match *)a)->prag - ((match *)b)->prag > 0)
        return -1;
    return 1;
}

inline int maxim(int x, int y) {
    return (x > y ? x : y);
}

inline int minim(int x, int y) {
    return (x < y ? x : y);
}

double arie(match x, match y) {
    if (maxim(x.x, y.x) - minim(x.x, y.x) > 15 || maxim(x.y, y.y) - minim(x.y, y.y) > 11)
        return 0;
    return (1.0 * minim(x.x, y.x) + 15 - maxim(x.x, y.x)) * (minim(x.y, y.y) + 11 - maxim(x.y, y.y)) / (330 - (minim(x.x, y.x) + 15 - maxim(x.x, y.x)) * (minim(x.y, y.y) + 11 - maxim(x.y, y.y)));
}

void coloreaza_chenar(img_gray img, int cif, int x, int y, rgb *chenare) {
    rgb chenar[10] = {{255, 0, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {255, 0, 255}, {0, 0, 255}, {192, 192, 192}, {255, 140, 0}, {128, 0, 128}, {128, 0, 0}};
    int i, j;
    for (i = x; i < x + 15; ++i) {
        chenare[i * img.latime + y] = chenare[i * img.latime + y + 10] = chenar[cif];
        img.val[i * img.latime + y] = img.val[i * img.latime + y + 10] = -1;
    }
    for (j = y; j < y + 11; ++j) {
        chenare[x * img.latime + j] = chenare[(x + 14) * img.latime + j] = chenar[cif];
        img.val[x * img.latime + j] = img.val[(x + 14) * img.latime + j] = -1;
    }
}

void template_matching(double prag, char* nume_fisier_sursa, char* nume_fisier_destinatie, char* sablonIN0, char* sablonIN1, char* sablonIN2, char* sablonIN3, char* sablonIN4, char* sablonIN5, char* sablonIN6, char* sablonIN7, char* sablonIN8, char* sablonIN9, char* sablonOUT0, char* sablonOUT1, char* sablonOUT2, char* sablonOUT3, char* sablonOUT4, char* sablonOUT5, char* sablonOUT6, char* sablonOUT7, char* sablonOUT8, char* sablonOUT9) {
    int *cif0 = (int *)malloc(165 * sizeof(int));
    int *cif1 = (int *)malloc(165 * sizeof(int));
    int *cif2 = (int *)malloc(165 * sizeof(int));
    int *cif3 = (int *)malloc(165 * sizeof(int));
    int *cif4 = (int *)malloc(165 * sizeof(int));
    int *cif5 = (int *)malloc(165 * sizeof(int));
    int *cif6 = (int *)malloc(165 * sizeof(int));
    int *cif7 = (int *)malloc(165 * sizeof(int));
    int *cif8 = (int *)malloc(165 * sizeof(int));
    int *cif9 = (int *)malloc(165 * sizeof(int));
    img_gray img;
    aplica_grayscale(cif0, cif1, cif2, cif3, cif4, cif5, cif6, cif7, cif8, cif9, sablonIN0, sablonIN1, sablonIN2, sablonIN3, sablonIN4, sablonIN5, sablonIN6, sablonIN7, sablonIN8, sablonIN9, sablonOUT0, sablonOUT1, sablonOUT2, sablonOUT3, sablonOUT4, sablonOUT5, sablonOUT6, sablonOUT7, sablonOUT8, sablonOUT9);

    ///citire imagine

    FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], aux;

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    img.val = (int *)malloc(latime_img * inaltime_img * sizeof(int));
   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    rgb *imgFinal = (rgb *)malloc(inaltime_img * latime_img * sizeof(rgb));
    rgb *imgFinala = (rgb *)malloc(inaltime_img * latime_img * sizeof(rgb));

	fseek(fout, 54, SEEK_SET);
	int i, j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			imgFinal[i * latime_img + j].r = imgFinal[i * latime_img + j].r = pRGB[2];
			imgFinal[i * latime_img + j].g = imgFinal[i * latime_img + j].g = pRGB[1];
			imgFinal[i * latime_img + j].b = imgFinal[i * latime_img + j].b = pRGB[0];
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			img.val[i * latime_img + j] = pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout, padding, SEEK_CUR);
	}
	img.inaltime = inaltime_img;
	img.latime = latime_img;

    ///sfarsit citire

    int k = 0;
    double x;
    for (i = 0; i + 15 <= img.inaltime; ++i) {
        for (j = 0; j + 11 <= img.latime; ++j) {
            x = calcul_corespondenta(i, j, img, cif0);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif1);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif2);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif3);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif4);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif5);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif6);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif7);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif8);
            if (x > prag) {
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif9);
            if (x > prag) {
                k++;
            }
        }
    }

    match *chenar = (match *)malloc(k * sizeof(match));

    k = 0;
    for (i = 0; i + 15 <= img.inaltime; ++i) {
        for (j = 0; j + 11 <= img.latime; ++j) {
            x = calcul_corespondenta(i, j, img, cif0);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 0;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif1);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 1;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif2);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 2;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif3);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 3;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif4);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 4;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif5);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 5;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif6);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 6;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif7);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 7;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif8);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 8;
                k++;
            }
            x = calcul_corespondenta(i, j, img, cif9);
            if (x > prag) {
                chenar[k].x = i;
                chenar[k].y = j;
                chenar[k].prag = x;
                chenar[k].cifra = 9;
                k++;
            }
        }
    }

    qsort (chenar, k, sizeof(match), cmp);

    for (i = 0; i < k; ++i) {
        if (chenar[i].x == -1) {
            continue;
        }
        for (j = i + 1; j < k; ++j) {
            if (chenar[j].x == -1) {
                continue;
            }
            if (arie(chenar[i], chenar[j]) > 0.2) {
                chenar[j].x = -1;
            }
        }
    }

    for (i = 0 ; i < k; ++i) {
        if (chenar[i].x != -1) {
            coloreaza_chenar(img, chenar[i].cifra, chenar[i].x, chenar[i].y, imgFinal);
        }
    }

    fseek(fout, 54, SEEK_SET);
	for(i = 0; i < inaltime_img; i++) {
		for(j = 0; j < latime_img; j++) {
			pRGB[0] = imgFinal[i * img.latime + j].b;
			pRGB[1] = imgFinal[i * img.latime + j].g;
			pRGB[2] = imgFinal[i * img.latime + j].r;
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout, padding, SEEK_CUR);
	}

    fclose(fout);

    free(cif0);
    free(cif1);
    free(cif2);
    free(cif3);
    free(cif4);
    free(cif5);
    free(cif6);
    free(cif7);
    free(cif8);
    free(cif9);
    free(img.val);
    free(imgFinal);
    free(chenar);
}

int main() {
    FILE *fin;
    char fisierINcriptare[255] = "", fisierOUTcriptare[255] = "", fisierKEYcriptare[255] = "", fisierINdecriptare[255] = "", fisierOUTdecriptare[255] = "", fisierKEYdecriptare[255] = "";
    printf("calea fisierului de intrare pentru criptare este:");
    scanf("%s", fisierINcriptare);
    printf("\ncalea fisierului de iesire pentru criptare este:");
    scanf("%s", fisierOUTcriptare);
    printf("\ncalea fisierului pentru cheie la criptare este:");
    scanf("%s", fisierKEYcriptare);
    printf("\ncalea fisierului de intrare pentru decriptare este:");
    scanf("%s", fisierINdecriptare);
    printf("\ncalea fisierului de iesire pentru decriptare este:");
    scanf("%s", fisierOUTdecriptare);
    printf("\ncalea fisierului de intrare pentru decriptare este:");
    scanf("%s", fisierKEYdecriptare);


    char sablonIN0[255] = "", sablonIN1[255] = "", sablonIN2[255] = "", sablonIN3[255] = "", sablonIN4[255] = "", sablonIN5[255] = "", sablonIN6[255] = "", sablonIN7[255] = "", sablonIN8[255] = "", sablonIN9[255] = "";
    printf("\ncalea fisierului de intrare pentru sablonul0 este:");
    scanf("%s", sablonIN0);
    printf("\ncalea fisierului de intrare pentru sablonul1 este:");
    scanf("%s", sablonIN1);
    printf("\ncalea fisierului de intrare pentru sablonul2 este:");
    scanf("%s", sablonIN2);
    printf("\ncalea fisierului de intrare pentru sablonul3 este:");
    scanf("%s", sablonIN3);
    printf("\ncalea fisierului de intrare pentru sablonul4 este:");
    scanf("%s", sablonIN4);
    printf("\ncalea fisierului de intrare pentru sablonul5 este:");
    scanf("%s", sablonIN5);
    printf("\ncalea fisierului de intrare pentru sablonul6 este:");
    scanf("%s", sablonIN6);
    printf("\ncalea fisierului de intrare pentru sablonul7 este:");
    scanf("%s", sablonIN7);
    printf("\ncalea fisierului de intrare pentru sablonul8 este:");
    scanf("%s", sablonIN8);
    printf("\ncalea fisierului de intrare pentru sablonul9 este:");
    scanf("%s", sablonIN9);

    char sablonOUT0[255] = "", sablonOUT1[255] = "", sablonOUT2[255] = "", sablonOUT3[255] = "", sablonOUT4[255] = "", sablonOUT5[255] = "", sablonOUT6[255] = "", sablonOUT7[255] = "", sablonOUT8[255] = "", sablonOUT9[255] = "";
    printf("\ncalea fisierului de iesire pentru sablonul0 este:");
    scanf("%s", sablonOUT0);
    printf("\ncalea fisierului de iesire pentru sablonul1 este:");
    scanf("%s", sablonOUT1);
    printf("\ncalea fisierului de iesire pentru sablonul2 este:");
    scanf("%s", sablonOUT2);
    printf("\ncalea fisierului de iesire pentru sablonul3 este:");
    scanf("%s", sablonOUT3);
    printf("\ncalea fisierului de iesire pentru sablonul4 este:");
    scanf("%s", sablonOUT4);
    printf("\ncalea fisierului de iesire pentru sablonul5 este:");
    scanf("%s", sablonOUT5);
    printf("\ncalea fisierului de iesire pentru sablonul6 este:");
    scanf("%s", sablonOUT6);
    printf("\ncalea fisierului de iesire pentru sablonul7 este:");
    scanf("%s", sablonOUT7);
    printf("\ncalea fisierului de iesire pentru sablonul8 este:");
    scanf("%s", sablonOUT8);
    printf("\ncalea fisierului de iesire pentru sablonul9 este:");
    scanf("%s", sablonOUT9);


    char fisierINtemplate[255] = "", fisierOUTtemplate[255] = "";
    printf("\ncalea fisierului de intrare pentru template matching este:");
    scanf("%s", fisierINtemplate);
    printf("\ncalea fisierului de iesire pentru template matching este:");
    scanf("%s", fisierOUTtemplate);

    criptare_decriptare(fisierINcriptare, fisierOUTcriptare, fisierKEYcriptare, fisierINdecriptare, fisierOUTdecriptare, fisierKEYdecriptare);
    template_matching(0.5, fisierINtemplate, fisierOUTtemplate, sablonIN0, sablonIN1, sablonIN2, sablonIN3, sablonIN4, sablonIN5, sablonIN6, sablonIN7, sablonIN8, sablonIN9, sablonOUT0, sablonOUT1, sablonOUT2, sablonOUT3, sablonOUT4, sablonOUT5, sablonOUT6, sablonOUT7, sablonOUT8, sablonOUT9);



    /*
    C:\\Users\\opale\\Desktop\\peppers\\peppers.bmp
    C:\\Users\\opale\\Desktop\\peppers\\peppers_out.bmp
    C:\\Users\\opale\\Desktop\\peppers\\secret_key.bmp

    C:\\Users\\opale\\Desktop\\peppers\\enc_peppers_ok.bmp
    C:\\Users\\opale\\Desktop\\peppers\\peppers_decriptat.bmp
    C:\\Users\\opale\\Desktop\\peppers\\secret_key.bmp

    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra0.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra1.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra2.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra3.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra4.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra5.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra6.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra7.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra8.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\cifra9.bmp"

    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra0_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra1_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra2_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra3_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra4_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra5_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra6_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra7_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra8_gray.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\cifra9_gray.bmp"

    "C:\\Users\\opale\\Desktop\\template_matching\\normal\\test.bmp"
    "C:\\Users\\opale\\Desktop\\template_matching\\gray\\test_gray.bmp"

    */
	return 0;
}

