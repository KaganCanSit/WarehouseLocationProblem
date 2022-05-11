#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

//f lf

//GLOBAL DEGISKENLER
FILE* filePointer;
int numberWH = 0, numberCustomer = 0, WHChoise[1000];
double totalCost;

//Dinamik dizinin rahat ve hizli bir sekilde kullanimi icin tanimladigimiz struct yapisi.
struct  Warehouses{
	int arraySize;				//Dizi Buyuklugu
	int indis;					//Indis Degeri
	double* data;				//Depo Kapasitesi - Depo Maliyeti - Musteri Kapasitesi - Musteri Ulasim Maliyeti
};

typedef struct Warehouses mainWH;

mainWH capasityWH;
mainWH buildCostHW;
mainWH customerCapasity;
mainWH roadCost[1000];

//Dinamik dizi icin bellekten alan tahsisi.
void createArray(mainWH* d, int sizeVal)
{
	d->data = (double*)malloc(sizeVal * sizeof(double));
	d->arraySize = sizeVal;
	d->indis = 0;
}

//Dinamik dizi degerinin ilk tanimdan buyuk olmasi durumunda genisletme ve bellekten ek alan alma.
void expandArray(mainWH* d)
{
	
	if (d->indis == d->arraySize)
	{
		double* cntrl;
		d->arraySize++;

		//Bellekten alan alinamamasi durumunda uyari almasi icin duzenliyoruz.
		cntrl = (double*)realloc(d->data, sizeof(double) * d->arraySize);
		if (cntrl == NULL)
		{
			printf("Memory Fault!");
			exit(1);
		}
		d->data = cntrl;
	}
}

//Dinamik diziye bellek ekleme ve indis degerinin arttirilamasi
void addArray(mainWH* d, double v)
{
	expandArray(d);
	d->data[d->indis++] = v;
}

//Dinamik dizi alanini tamimiyla silinerek bellege iadesi.
void resetArray(mainWH* d)
{
	free(d->data);
	d->arraySize = 0;
	d->indis = 0;
}

//Dizi degerlerini yazdirilmasi icin kullandigimiz fonksiyon.
void writeArray(mainWH* d)
{
	printf("Veriler\n");
	int i;
	for (i = 0; i < d->indis; i++)
		printf("%f\n", d->data[i]);
}

//	Menu Fonksiyonu - Bu fonksiyonun amaci kullaniciya islem yapmak istedigi dosyayi sectirmek. Ve dosya ismini proje geneline hazir hale getirerek, islemleri hazirlamak.
void menu()
{
	int choise = 0;
	printf("*****************************************  MENU  *****************************************\n\n");
	printf("Warehouse Location Problem Select the file to be applied. (Enter Line Number : ->Simple; 1)\n");
	printf("1-ks_4_0.txt\n2-ks_100_0.txt\n3-ks_10000_0.txt\nSelect:");
	scanf_s("%d", &choise);

	switch (choise)
	{
		//case 1:		filePointer = fopen("wl_50_1.txt", "r");	break;
	case 1:		filePointer = fopen("deneme.txt", "r");	break;
	case 2:		filePointer = fopen("wl_200_5.txt", "r");	break;
	case 3:		filePointer = fopen("wl_1000_1.txt", "r");	break;
	default:
		system("cls"); //Console Ekrani Temizleme
		printf("***************************  Incorrect Entry! Please Try Again.  ***************************\n\n");
		menu();
	}
}

//Txt dosyasi icerisinde tanimli degerleri dosyadan okuyarak dinamik olarak bildirdigimiz dizi icerisinde tanimliyoruz.
void fileItemGetArray()
{
	double temp = 0;
	int counter = 0, costumer = 0, costumerCounter = 0, trigger = 0;
	while (fscanf(filePointer, "%lf", &temp) != EOF)
	{
		if (counter == 0)
		{
			//Dosyadan alinan eleman sayisi metrigine gore dinamik dizi tanimi
			numberWH = temp;
			createArray(&capasityWH, numberWH);
			createArray(&buildCostHW, numberWH);
			printf("\n%f", temp);
		}
		else if (counter == 1)
		{
			numberCustomer = temp;
			createArray(&customerCapasity, numberCustomer);
			printf("\t%f", temp);
		}
		else if (counter > 0 && counter <= numberWH * 2 + 1)
		{
			//Ilk iki degerde cantanin maksimum agirlik degeri, dizi uzunlugu alinmasindan sonra degerlerin ve agirliklarin diziye alinmasi.
			if (counter % 2 == 0)	addArray(&capasityWH, temp);
			else					addArray(&buildCostHW, temp);
			printf("\n%f", temp);
		}
		else
		{
			if (trigger == 0)
			{
				addArray(&customerCapasity, temp);
				printf("\n%f\n", temp);
				trigger++;
			}
			else
			{
				addArray(&roadCost[costumer], temp);
				costumerCounter++;
				printf("%f ", temp);
				if (costumerCounter == numberWH)
				{
					costumer++;
					costumerCounter = 0;
					trigger = 0;
				}
			}
		}
		counter++;
	}
	fclose(filePointer);
}

void addTotalCost(double cost)
{
	totalCost += cost;
}

void findCostumerMinCost(int capasityValue, int costumerIndis)
{
	double cost = 0;
	int WHNum = 0;

	cost = 999999999;
	for (int j = 0; j < roadCost[costumerIndis].indis; j++)
	{
		if (roadCost[costumerIndis].data[j] < cost)
		{
			cost = roadCost[costumerIndis].data[j];
			WHNum = j;
		}
	}
	printf("\n--> %f", cost);
	customerCapasity.data[costumerIndis] = 0;	//Müsterinin artik bir talebi yok.
	WHChoise[costumerIndis] = WHNum;
	addTotalCost(cost);
}

void findMaxCostumerCapasite()
{
	int tempCapasity = 0, tempIndis=0;
	for (int i = 0; i < customerCapasity.indis; i++)
	{
		if (customerCapasity.data[i] == 0) //Talep Yok.
		{

		}
		else if (tempCapasity < customerCapasity.data[i])
		{
			tempCapasity = customerCapasity.data[i];
			tempIndis = i;
		}
	}
	printf("\n--> %d - %d", tempCapasity, tempIndis);
	findCostumerMinCost(tempCapasity, tempIndis);
}


int main()
{
	menu();
	fileItemGetArray();
	for (int i = 0; i < customerCapasity.indis; i++)
	{
		if(customerCapasity.data[i]!=0)
			findMaxCostumerCapasite();
	}
	printf("--> Son: %f", totalCost);
}