#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

//f lf

//GLOBAL DEGISKENLER
FILE* filePointer;
int numberWH = 0, numberCustomer = 0, WHChoise[1000];
double totalCost = 0;

//Fonksiyon Bildirimleri
void addTotalCost(double);

//Dinamik dizinin rahat ve hizli bir sekilde kullanimi icin tanimladigimiz struct yapisi.
struct  Warehouses{
	int arraySize;				//Dizi Buyuklugu
	int indis;					//Indis Degeri
	double* data;				//Depo Kapasitesi - Depo Maliyeti - Musteri Kapasitesi - Musteri Ulasim Maliyeti
};

typedef struct Warehouses mainWH;

mainWH capasityWH;
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
	int counter = 0, customer = 0, customerCounter = 0, trigger = 0;
	while (fscanf(filePointer, "%lf", &temp) != EOF)
	{
		if (counter == 0)
		{
			//Dosyadan alinan eleman sayisi metrigine gore dinamik dizi tanimi
			numberWH = temp;
			createArray(&capasityWH, numberWH);
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
			else					addTotalCost(temp);
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
				addArray(&roadCost[customer], temp);
				customerCounter++;
				printf("%f ", temp);
				if (customerCounter == numberWH)
				{
					customer++;
					customerCounter = 0;
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

int maxCustomerCapasity()
{
	//Maksimum depo istegini ve hangi musterinin istedigini bulduk.
	double temp = 0;
	int counterIndis = 0;
	
	for (int i = 0; i < customerCapasity.indis; i++)
	{
		if (customerCapasity.data[i] > temp)
		{
			temp = customerCapasity.data[i];
			counterIndis = i;
		}
	}
	return counterIndis;
}

void findCustomerMinCost(int customer)
{
	double minCost=999999999;
	int temp;
	//O musterinin minimum hangi depoya gittigine bakacagiz
	for (int j = 0; j < roadCost[customer].indis; j++)
	{
		if (minCost > roadCost[customer].data[j])
		{
			//Depo Seçimi
			minCost = roadCost[customer].data[j];
			temp = j;
		}
	}
	if (capasityWH.data[temp] < customerCapasity.data[customer])
	{
		roadCost[customer].data[temp] *= 2;
	}
	else
	{
		capasityWH.data[temp] -= customerCapasity.data[customer];
		customerCapasity.data[customer] = 0; //Musteri talebi sonlandi.
		addTotalCost(roadCost[customer].data[temp]);
		WHChoise[customer] = temp;
	}

}


int main()
{
	int max = 0;
	int triggerCount = 0;

	menu();
	fileItemGetArray();
	for (int i = 0; i < customerCapasity.indis; i++)
	{
		max = maxCustomerCapasity();
		findCustomerMinCost(max);
	}
	printf("\n Cost --> %f\n", totalCost);

	for (int j = 0; j < capasityWH.indis; j++)
	{
		printf("\t--> %d", capasityWH.data[j]);
	}

	for (int j = 0; j < 1000; j++)
	{
		printf("\n--> %d", WHChoise[j]);
	}
}	