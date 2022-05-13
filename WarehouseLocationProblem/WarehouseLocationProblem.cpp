#include <stdio.h>
#include <stdlib.h>
using namespace std;

//GLOBAL DEGISKENLER
FILE* filePointer;
int numberWH = 0, numberCustomer = 0 ,WHChoise[1000], lastWHChoise = 0, customerWHCounter=0;
double totalCost = 0;

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
	int counter = 0, customer = 0, customerCounter = 0;
	while (fscanf(filePointer, "%lf", &temp) != EOF)
	{
		if (counter == 0)
		{
			//Dosyadan alinan depo sayisi metrigine gore dinamik dizi tanimi
			numberWH = temp;
			createArray(&capasityWH, numberWH);
		}
		else if (counter == 1)
		{
			//Dosyadan alinan musteri sayisi metrigine gore dinamik dizi tanimi
			numberCustomer = temp;
			createArray(&customerCapasity, numberCustomer);
		}
		else if (counter > 0 && counter <= numberWH * 2 + 1)
		{
			//Ilk iki degerde depo sayisi  ve musteri sayisi degerleri dizi uzunlugu parametresi olarak alinmasindan sonra degerlerin ve agirliklarin diziye alinmasi.
			if (counter % 2 == 0)	addArray(&capasityWH, temp);
			else					addArray(&buildCostHW, temp);
		}
		else
		{
			//Musterinin talep ettigi depo kapasitesini aliyoruz.
			if (customerCounter == 0)		addArray(&customerCapasity, temp);
			else							addArray(&roadCost[customer], temp);

			customerCounter++;
			if (customerCounter == numberWH + 1)
			{
				customer++;
				customerCounter = 0;
			}
		}
		counter++;
	}
	fclose(filePointer);
}

//Total Maliyet Hesabi islemi icin
void addTotalCost(double cost)
{
	totalCost += cost;
}

//Minimum depo kurma maliyetine sahip deponun sirasini buluyoruz.
int minBuildCostHW()
{
	int WHindis = 0;
	double temp = 99999999;
	for (int i = 0; i < buildCostHW.indis; i++)
	{
		if (buildCostHW.data[i] < temp)
		{
			temp = buildCostHW.data[i];
			WHindis = i;
		}
	}
	return WHindis;
}

//Maksimum depo alani talep eden musterinin indis degerini buluyoruz. Bunun sebebi carpan degerinin buyumemesi icin max depo isteginde bulunan musteriyi baz almak icin kullanacagiz.
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


//Farkli Sartlar icin satim islemi fonksiyonu
void SalesOperation(int WHindis, int customerID)
{
	capasityWH.data[WHindis] -= customerCapasity.data[customerID];
	customerCapasity.data[customerID] = 0; //Musteri talebi sonlandi.
	addTotalCost(roadCost[customerID].data[WHindis]);
	WHChoise[customerID] = WHindis;
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
	//Depoda yer yoksa onlem!
	if (capasityWH.data[temp] < customerCapasity.data[customer])
	{
		roadCost[customer].data[temp] *= 2;	//O depoda yer yoksa kullanilmasi zaten mumkun degil. Maliyetini arttiriyoruz.
	}
	else
	{
		//Mantikin Ozu -> Yeni Deponun Kurulum Maliyeti + Yeni Depoya Ulasim Maliyet > Eski Depoya Yolculuk Maliyetinden fazlaysa ve eski depoda alan varsa eski depoyu tercih et.
		if (customerWHCounter != 0 && (buildCostHW.data[temp] + roadCost[customer].data[temp]) > roadCost[customer].data[lastWHChoise] && capasityWH.data[lastWHChoise] >= customerCapasity.data[customer])
		{
			SalesOperation(lastWHChoise, customer);
		}
		else
		{
			customerWHCounter++;

			//Eger en dusuk maliyetli depo + en dusuk maliyetli depoya yolculuk maliyeti < Yeni Depo Kurulum Maliyeti + Yeni Depoya Yolculuk Maliyetinden kucuk ise en dusuk maliyetli depoyu Sec
			if (buildCostHW.data[minBCostIndis] + roadCost[customer].data[minBCostIndis] < buildCostHW.data[temp] + roadCost[customer].data[temp] && capasityWH.data[minBCostIndis] >= customerCapasity.data[customer])
			{
				SalesOperation(minBCostIndis, customer);
				lastWHChoise = minBCostIndis;	//En son secilen depo. -> Kurulum maliyeti farki icin gerekli.
			}
			else
			{
				SalesOperation(temp,customer);
				lastWHChoise = temp;	//En son secilen depo. -> Kurulum maliyeti farki icin gerekli.
			}
		}
	}
}


//Secilen Depolarin Maliyetlerini Cost'a ekle.
void builCostAdd()
{
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
		printf("%d. %.f |\n",j, capasityWH.data[j]);
	}
}

//Secilen Depolari Yazdir
void WHChoiseList()
{
	printf("\nSecilen Depolarin ID Listesi\n");
	for (int j = 0; j < numberCustomer; j++)
	{
		printf("%d | ", WHChoise[j]);
	}
	printf("\n\n");
}


int main()
{
	menu();
	fileItemGetArray();

	for (int i = 0; i < customerCapasity.indis; i++)
	{
		int max = maxCustomerCapasity();
		findCustomerMinCost(max);
	}
	builCostAdd();
	
	printf("\n\nTum Maliyet: --> %f\n", totalCost);
	WHChoiseList();
	remainingPlace();
	

	//Bellekler iade ediliyor.
	resetArray(&capasityWH);
	resetArray(&customerCapasity);
	for (int i = 0; i < 1000; i++)
	{
		resetArray(&roadCost[i]);
	}
}