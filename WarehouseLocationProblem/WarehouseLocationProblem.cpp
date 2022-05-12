#include <stdio.h>
#include <stdlib.h>
using namespace std;

//GLOBAL DEGISKENLER
FILE* filePointer;
int numberWH = 0, numberCustomer = 0 ,WHChoise[1000], lastWHChoise = 0, customerWHCounter=0;
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

//	Menu Fonksiyonu - Bu fonksiyonun amaci kullaniciya islem yapmak istedigi dosyayi sectirmek. Ve dosya ismini proje geneline hazir hale getirerek, islemleri hazirlamak.
void menu()
{
	int choise = 0;
	printf("*****************************************  MENU  *****************************************\n\n");
	printf("Warehouse Location Problem Select the file to be applied. (Enter Line Number : ->Simple; 1)\n");
	printf("0-soruornegi.txt\n1-wl_50_1.txt\n2-wl_200_5.txt\n3-wl_1000_1.txt.txt\nSelect:");
	scanf_s("%d", &choise);

	switch (choise)
	{
		case 0:		filePointer = fopen("soruornegi.txt", "r");		break;
		case 1:		filePointer = fopen("wl_50_1.txt", "r");	break;
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
		}
		else if (counter == 1)
		{
			numberCustomer = temp;
			createArray(&customerCapasity, numberCustomer);
		}
		else if (counter > 0 && counter <= numberWH * 2 + 1)
		{
			//Ilk iki degerde cantanin maksimum agirlik degeri, dizi uzunlugu alinmasindan sonra degerlerin ve agirliklarin diziye alinmasi.
			if (counter % 2 == 0)	addArray(&capasityWH, temp);
			else					addArray(&buildCostHW, temp);
		}
		else
		{
			if (trigger == 0)
			{
				addArray(&customerCapasity, temp);
				trigger++;
			}
			else
			{
				addArray(&roadCost[customer], temp);
				customerCounter++;
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
	int deger = 0;
	printf("\n\nMusteri Talepleri:");
	for (int i = 0; i < customerCapasity.indis; i++)
	{
		deger += customerCapasity.data[i];
	}
	printf("%d", deger);
	fclose(filePointer);
}

//Total Maliyet Hesabi islemi icin
void addTotalCost(double cost)
{
	totalCost += cost;
}

//Minimum depo kurma maliyetine sahip deoyu buluyoruz.
int minBuildCostHW()
{
	int WHindis = 0;
	for (int i = 0; i < buildCostHW.indis; i++)
	{
		if (buildCostHW.data[i] < buildCostHW.data[0])
		{
			WHindis = i;
		}
	}
	return WHindis;
}

//Maksimum depo alani talep eden musteriyi buluyoruz. Bunun sebebi carpan degerinin buyumesi.
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
//Greedy Yaklasimi
//Maksimum depo alani talep eden musterinin minimum maliyet ile hangi depoaya gidebilecegini buluyoruz.
void findCustomerMinCost(int customer)
{
	double minCost=999999999;
	int temp = 0;
	int minBCostIndis = minBuildCostHW();

	//O musterinin minimum hangi depoya gittigine bakacagiz
	for (int j = 0; j < roadCost[customer].indis; j++)
	{
		if (minCost > roadCost[customer].data[j])
		{
			//Depo Seçimi
			minCost = roadCost[customer].data[j];
			temp = j;//Secilen Depo
		}
	}
	if (capasityWH.data[temp] < customerCapasity.data[customer])
	{
		roadCost[customer].data[temp] *= 2;
	}
	else
	{
		//printf("\nYeni Insa Maliyeti + Yeni Ulasim Maliyeti: %f >  Eski Depoya Gore Ulasim Maliyeti:%f ---- Eski Kapasite: %f > Musterinin Istek Kapasitesi: %f", (buildCostHW.data[temp] + roadCost[customer].data[temp]) , roadCost[customer].data[lastWHChoise] , capasityWH.data[lastWHChoise] , customerCapasity.data[customer]);
		//Mantikin Ozu -> Yeni Deponun Kurulum Maliyeti + Yeni Depoya Ulasim Maliyet > Eski Depoya Yolculuk Maliyetinden fazlaysa ve eski depoda alan varsa eski depoyu tercih et.
		if (customerWHCounter != 0 && (buildCostHW.data[temp] + roadCost[customer].data[temp]) > roadCost[customer].data[lastWHChoise] && capasityWH.data[lastWHChoise] >= customerCapasity.data[customer])
		{
			//printf("\tGirdi");
			capasityWH.data[lastWHChoise] -= customerCapasity.data[customer];
			customerCapasity.data[customer] = 0; //Musteri talebi sonlandi.
			addTotalCost(roadCost[customer].data[lastWHChoise]);
			WHChoise[customer] = lastWHChoise;
		}
		else
		{
			//printf("\tGirmedi");
			customerWHCounter++;

			//Eger en dusuk maliyetli depo + en dusuk maliyetli depoya yolculuk maliyeti < Yeni Depo Kurulum Maliyeti + Yolculuk Maliyetinden Kucuk Depoyu Sec
			if (buildCostHW.data[minBCostIndis] + roadCost[customer].data[minBCostIndis] < buildCostHW.data[temp] + roadCost[customer].data[temp] && capasityWH.data[minBCostIndis] >= customerCapasity.data[customer])
			{
				capasityWH.data[minBCostIndis] -= customerCapasity.data[customer];
				customerCapasity.data[customer] = 0; //Musteri talebi sonlandi.
				addTotalCost(roadCost[customer].data[minBCostIndis]);
				WHChoise[customer] = minBCostIndis;
				lastWHChoise = minBCostIndis;	//En son secilen depo. -> Kurulum maliyeti farki icin gerekli.
			}
			else
			{
				capasityWH.data[temp] -= customerCapasity.data[customer];
				customerCapasity.data[customer] = 0; //Musteri talebi sonlandi.
				addTotalCost(roadCost[customer].data[temp]);
				WHChoise[customer] = temp;
				lastWHChoise = temp;	//En son secilen depo. -> Kurulum maliyeti farki icin gerekli.
			}
		}
	}
}

//Secilen Depolarin Maliyetlerini Cost'a ekle.
void builCostAdd()
{
	int counter = 0;
	for (int j = 0; j < numberCustomer; j++)
	{
		addTotalCost(buildCostHW.data[WHChoise[j]]);
		//Depoya birden fazla urun yerlestirilmis olabilir. Her seferinde depo maliyetini almamiza gerek yok. Depoya bir urun atandigi anda deponun kuruldugunu farz ediyoruz.
		buildCostHW.data[WHChoise[j]] = 0;	
	}
}

//Atamalar sonrasi kalan yerleri yazdir
void remainingPlace()
{
	printf("\nDepolarda Kalan Yerler:\n");
	for (int j = 0; j < capasityWH.indis; j++)
	{
		printf("%d. %f |\n",j, capasityWH.data[j]);
	}
}

//Secilen Depolari Yazdir
void WHChoiseList()
{
	printf("\nSecilen Depolarin ID Listesi\n");
	for (int j = 0; j < numberCustomer; j++)
	{
		printf("%d |\t", WHChoise[j]);
	}
	printf("\n\n");
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
	builCostAdd();
	
	printf("\n\nTum Maliyet: --> %f\n", totalCost);
	remainingPlace();
	WHChoiseList();

	//Bellekler iade ediliyor.
	resetArray(&capasityWH);
	resetArray(&customerCapasity);
	for (int i = 0; i < 1000; i++)
	{
		resetArray(&roadCost[i]);
	}
}	