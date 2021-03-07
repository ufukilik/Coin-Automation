#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "inc/tm4c123gh6pm.h"

#define RW 0x20
#define RS 0x40
#define E  0x80

volatile unsigned long delay;
char otomat[6][200] = {{"20,20,10"},{"1,su,30,50 Kurus"},{"2,cay,20,1 TL"},{"3,kahve,15,1.5 TL"},{"4,cikolata,50,1.75 TL"},{"5,biskuvi,100,2 TL"}};
float paraOtomat;
int totalGiris, ucret;
int UrunID[5], UrunStok[5];
double UrunFiyat[5];
char UrunAD[5][10], UrunFiyatCins[5][5];
int btYirmiBesK = 0, btElliK = 0, btBirL = 0, yirmibeskrs, ellikrs, birlira;
int su = 0, biskuvi = 0, cikolata = 0, cay = 0, kahve = 0;


void aktif() {
	volatile unsigned long bekle;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;      //Buton  E0,1,2,3,4,5
	bekle = SYSCTL_RCGC2_R;						//5. buton reset
	GPIO_PORTE_DIR_R |= 0x00;					//0,1,2,3,4 ürün butonu
	GPIO_PORTE_AFSEL_R &= ~0x00;
	GPIO_PORTE_PUR_R |= 0xFF;
	GPIO_PORTE_DEN_R |= 0xFF;

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;       //Buton C4,5,6,7
	bekle = SYSCTL_RCGC2_R;						//4,5,6 para girisi
	GPIO_PORTC_DIR_R |= 0x00;					//7. buton bitis
	GPIO_PORTC_AFSEL_R &= ~0x00;
	GPIO_PORTC_PUR_R |= 0xF0;
	GPIO_PORTC_DEN_R |= 0xF0;

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	bekle = SYSCTL_RCGC2_R;
	GPIO_PORTA_AMSEL_R &= ~0b11100000;
	GPIO_PORTA_PCTL_R &= ~0xFFF00000;
	GPIO_PORTA_DIR_R |= 0b11101100;
	GPIO_PORTA_AFSEL_R &= ~0b11101100;
	GPIO_PORTA_DEN_R |= 0b11101100;
	GPIO_PORTA_DR8R_R |= 0b11100000;

	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	bekle = SYSCTL_RCGC2_R;
	GPIO_PORTB_AMSEL_R &= ~0b11111111;
	GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF;
	GPIO_PORTB_DIR_R |= 0b11111111;
	GPIO_PORTB_AFSEL_R &= ~0b11111111;
	GPIO_PORTB_DEN_R |= 0b11111111;
	GPIO_PORTB_DR8R_R |= 0b11111111;
}

void LCDayar(unsigned char bildiri){
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);
	GPIO_PORTB_DATA_R = bildiri;
	GPIO_PORTA_DATA_R |= E;
	GPIO_PORTA_DATA_R &= ~(RS+RW);
	for (delay = 0 ; delay < 1; delay++);
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);
	for (delay = 0 ; delay < 1000; delay++);
}

void LCDveri(unsigned char veri){
	GPIO_PORTB_DATA_R = veri;
	GPIO_PORTA_DATA_R |= RS+E;
	GPIO_PORTA_DATA_R &= ~(RW);
	for (delay = 0 ; delay < 23 ; delay++);
	GPIO_PORTA_DATA_R &= ~(RS+RW+E);
	for (delay = 0 ; delay < 1000; delay++);
}

void LCDaktif(){
	for (delay = 0 ; delay < 15000; delay++);
	LCDayar(0x38);
	for (delay = 0 ; delay < 5000; delay++);
	LCDayar(0x38);
	for (delay = 0 ; delay < 150; delay++);
	LCDayar(0x0C);
	LCDayar(0x01);
	LCDayar(0x06);
	for (delay = 0 ; delay < 50000; delay++);
}

void LCDyaz(unsigned int satir,unsigned int sutun, unsigned char *krk){
	unsigned int lineCode;
	if(satir == 1){
		lineCode = 0x80;
	} else {
		lineCode = 0xC0;
	}
	LCDayar(lineCode + sutun);
	while(*krk != 0){ LCDveri(*krk++); }
}

void parsing(){
	int prsSayaci = 0;

	    char *prs;
	    prs = strtok(otomat[0],",");
	    while(prs != NULL){
	        if(prsSayaci == 0){
	            yirmibeskrs = atoi(prs);
	        } else if(prsSayaci == 1){
	            ellikrs = atoi(prs);
	        } else if(prsSayaci == 2){
	            birlira = atoi(prs);
	        }
	        prsSayaci++;
	        prs = strtok(NULL, ",");
	    }
	    prsSayaci = 0;
	    int index2 = 0, prsSayaci2 = 0, prsSayaci3 = 0;
	    int index;
	    for(index = 1; index <= 5; index++){
	        prs = strtok(otomat[index], ",");
	         while(prs != NULL){
	                if(prsSayaci == 0){
	                    UrunID[index2] = atoi(prs);
	                } else if(prsSayaci == 1){
	                    strcpy(UrunAD[index2], prs);
	                } else if(prsSayaci == 2){
	                    UrunStok[index2] = atoi(prs);
	                } else if(prsSayaci == 3){
	                    prs = strtok(prs, " ");
	                    while(prs != NULL){
	                        if(prsSayaci2 == 0){
	                        	if(prsSayaci3 == 0){
	                      		    UrunFiyat[0] = 0.5;
	                        	} else if(prsSayaci3 == 1){
	                        		UrunFiyat[1] = 1.0;
	               		    	} else if(prsSayaci3 == 2){
	                        		UrunFiyat[2] = 1.5;
	                           	} else if(prsSayaci3 == 3){
	               		    		UrunFiyat[3] = 1.75;
	                           	} else if(prsSayaci3 == 4){
	               		    		UrunFiyat[4] = 2.0;
	                  	    	}
	               		    	prsSayaci3++;
	                        } else if(prsSayaci2 == 1){
	                            strcpy(UrunFiyatCins[index2], prs);
	                        }
	                        prsSayaci2++;
	                        prs = strtok(NULL, " ");
	                    }
	                    index2++;
	                    prsSayaci = -1;
	                    prsSayaci2 = 0;
	                }
	                prs = strtok(NULL, ",");
	                prsSayaci++;
	            }
	    }

	    paraOtomat = (yirmibeskrs*0.25) + (ellikrs*0.5) + (birlira*1);
}

void paracoz(int *totalGiris1, int deger){
	char yazilacakpara[5];
	LCDayar(0x01);
	    if(totalGiris < 100 || ucret < 100){
	    	yazilacakpara[0] = '0';
	    	yazilacakpara[1] = '0';
	    	yazilacakpara[2] = '.';
	    	yazilacakpara[3] = totalGiris1[2] + 48;
	    	yazilacakpara[4] = totalGiris1[3] + 48;
	    	if(deger == 0){
	    		LCDyaz(1,0,yazilacakpara);
	    		LCDyaz(1,6,"TL girildi");
	    		LCDyaz(2,0,"Urun seciniz.");
	    	}
	    	if(deger == 1){
	    		LCDyaz(1,0,"Tutar: ");
	    		LCDyaz(1,7,yazilacakpara);
	    		LCDyaz(1,13,"TL");
	    	}
	    }
	    if((totalGiris >= 100 && totalGiris < 1000) || (ucret >= 100 && ucret < 1000)){
	    	yazilacakpara[0] = '0';
	    	yazilacakpara[1] = totalGiris1[1] + 48;
	       	yazilacakpara[2] = '.';
	       	yazilacakpara[3] = totalGiris1[2] + 48;
	       	yazilacakpara[4] = totalGiris1[3] + 48;
	       	if(deger == 0){
	    		LCDyaz(1,0,yazilacakpara);
	    		LCDyaz(1,6,"TL girildi");
	    		LCDyaz(2,0,"Urun seciniz.");
	       	}
	       	if(deger == 1){
	       		LCDyaz(1,0,"Tutar: ");
	       		LCDyaz(1,7,yazilacakpara);
	       		LCDyaz(1,13,"TL");
	       	}
	    }
	    if((totalGiris >= 1000 && totalGiris < 10000) || (ucret >= 1000 && ucret < 10000)){
	    	yazilacakpara[0] = totalGiris1[0] + 48;
	    	yazilacakpara[1] = totalGiris1[1] + 48;
	    	yazilacakpara[2] = '.';
	    	yazilacakpara[3] = totalGiris1[2] + 48;
	    	yazilacakpara[4] = totalGiris1[3] + 48;
	    	if(deger == 0){
	    		LCDyaz(1,0,yazilacakpara);
	    		LCDyaz(1,6,"TL girildi");
	    		LCDyaz(2,0,"Urun seciniz.");
	    	}
	    	if(deger == 1){
	    		LCDyaz(1,0,"Tutar: ");
	    		LCDyaz(1,7,yazilacakpara);
	    		LCDyaz(1,13,"TL");
	       	}
	    }
}

int stokKontrol(char adet, int UrunID, int UrunStok){
	char str[2];
	if(UrunID == 1 && UrunStok < adet){
		LCDayar(0x01);
		itoa(adet,str,10);
		LCDyaz(1,0,str);
		LCDyaz(1,3," adet su yok");
		LCDyaz(2,0,"Reset Bekleniyor");
		return 1;
	}
	if(UrunID == 2 && UrunStok < adet){
		LCDayar(0x01);
		itoa(adet,str,10);
		LCDyaz(1,0,str);
		LCDyaz(1,3," adet cay yok");
		LCDyaz(2,0,"Reset Bekleniyor");
		return 1;
	}
	if(UrunID == 3 && UrunStok < adet){
		LCDayar(0x01);
		itoa(adet,str,10);
		LCDyaz(1,0,str);
		LCDyaz(1,3," adet kahve yok");
		LCDyaz(2,0,"Reset Bekleniyor");
		return 1;
	}
	if(UrunID == 4 && UrunStok < adet){
		LCDayar(0x01);
		itoa(adet,str,10);
		LCDyaz(1,0,str);
		LCDyaz(1,3," adet cikolata yok");
		LCDyaz(2,0,"Reset Bekleniyor");
		return 1;
	}
	if(UrunID == 5 && UrunStok < adet){
		LCDayar(0x01);
		itoa(adet,str,10);
		LCDyaz(1,0,str);
		LCDyaz(1,3," adet biskuvi yok");
		LCDyaz(2,0,"Reset Bekleniyor");
		return 1;
	} else {
		return 0;
	}
}

void paraUstu(){
	int pustu;
	int bozuk25, bozuk50, bozuk1;
	char str[2];
	if(totalGiris >= ucret){
		pustu = totalGiris - ucret;
		yirmibeskrs = btYirmiBesK + yirmibeskrs;
		ellikrs = btElliK + ellikrs;
		birlira = btBirL + birlira;
		for(delay = 0; delay < 5000000; delay++);
		UrunStok[0] = UrunStok[0] - su;
		UrunStok[1] = UrunStok[1] - cay;
		UrunStok[2] = UrunStok[2] - kahve;
		UrunStok[3] = UrunStok[3] - cikolata;
		UrunStok[4] = UrunStok[4] - biskuvi;
		int para, elde;
		if(birlira == 0){
			LCDayar(0x01);
			LCDyaz(1,0,"1 TL kalmadi.");
			for(delay = 0; delay < 100000; delay++);
		} else if((pustu >= 1000 && pustu < 10000) || (pustu >= 100 && pustu < 1000)){
			LCDayar(0x01);
			bozuk1 = (pustu/1000)*10 + ((pustu/100)%10);
			if(bozuk1 > birlira){
				elde = bozuk1 - birlira;
				pustu = (pustu - (birlira*100)) + elde*100;
				itoa(birlira,str,10);
				LCDyaz(1,0,str);
				LCDyaz(1,2,"adet 1 TL");
				birlira = bozuk1 - birlira;
				for(delay = 0; delay < 5000000; delay++);
			} else {
				birlira = birlira - bozuk1;
				pustu = (pustu - (bozuk1*100));
				itoa(bozuk1,str,10);
				LCDyaz(1,0,str);
				LCDyaz(1,2,"adet 1 TL");
				for(delay = 0; delay < 5000000; delay++);
			}
		}
		if(ellikrs == 0){
			LCDayar(0x01);
			LCDyaz(1,0,"50 Kurus kalmadi");
			for(delay = 0; delay < 100000; delay++);
		} else if(pustu >= 50){
		LCDayar(0x01);
		bozuk50 = pustu/50;
		if(bozuk50 > ellikrs){
			elde = bozuk50 - ellikrs;
			pustu = (pustu - (ellikrs*50)) + elde*50;
			itoa(ellikrs,str,10);
			LCDyaz(1,0,str);
			LCDyaz(1,2,"adet 50 Kurus");
			ellikrs = bozuk50 - ellikrs;
			for(delay = 0; delay < 5000000; delay++);
		} else {
			ellikrs = ellikrs - bozuk50;
			pustu = (pustu - (bozuk50*50));
			itoa(bozuk50,str,10);
			LCDyaz(1,0,str);
			LCDyaz(1,2,"adet 50 Kurus");
			for(delay = 0; delay < 5000000; delay++);
		}
		}
		if(yirmibeskrs == 0){
			LCDayar(0x01);
			LCDyaz(1,0,"25 Kurus kalmadi.");
			for(delay = 0; delay < 100000; delay++);
		} else if(pustu >= 25){
		LCDayar(0x01);
		bozuk25 = pustu/25;
		if(bozuk25 > yirmibeskrs){
			elde = bozuk25 - yirmibeskrs;
			pustu = (pustu - (yirmibeskrs*25)) + elde*25;
			itoa(yirmibeskrs,str,10);
			LCDyaz(1,0,str);
			LCDyaz(1,2,"adet 25 Kurus");
			yirmibeskrs = bozuk25 - yirmibeskrs;
			for(delay = 0; delay < 5000000; delay++);
		} else {
			yirmibeskrs = yirmibeskrs - bozuk25;
			pustu = (pustu - (bozuk25*25));
			itoa(bozuk25,str,10);
			LCDyaz(1,0,str);
			LCDyaz(1,2,"adet 25 Kurus");
			for(delay = 0; delay < 5000000; delay++);
		}
		}
	}
	LCDayar(0x01);
	LCDyaz(1,0,"Otomatta");
	itoa(yirmibeskrs,str,10);
	LCDyaz(2,0,str);
	LCDyaz(2,2,"tane 25 Kurus");
	for(delay = 0; delay < 5000000; delay++);
	LCDayar(0x01);
	itoa(ellikrs,str,10);
	LCDyaz(1,0,str);
	LCDyaz(1,2,"tane 50 Kurus");
	for(delay = 0; delay < 5000000; delay++);
	LCDayar(0x01);
	itoa(birlira,str,10);
	LCDyaz(1,0,str);
	LCDyaz(1,2,"tane 1 TL var");
	for(delay = 0; delay < 5000000; delay++);
}

void otomatCalistir(){
	aktif();
	LCDaktif();
	parsing();


	   char str[2];

	    start:LCDayar(0x01);
	    LCDyaz(1,0,"Para giriniz");
	    while(1){
	    	if((GPIO_PORTE_DATA_R & 0b00001) == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	    		btYirmiBesK++;
	    		itoa(btYirmiBesK,str,10);
	    		LCDyaz(1,0,str);
	    		for(delay = 0; delay < 100000; delay++);
	   		}
	    	if((GPIO_PORTE_DATA_R & 0b00010) == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	   			btElliK += 1;
	   			itoa(btElliK,str,10);
	   			LCDyaz(1,0,str);
	       		for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTE_DATA_R & 0b00100) == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	    		btBirL += 1;
	    		itoa(btBirL,str,10);
	    		LCDyaz(1,0,str);
	    		for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTE_DATA_R & 0b01000) == 0){
	    		for(delay = 0; delay < 100000; delay++);
	    		totalGiris = ((btYirmiBesK*0.25) + (btElliK*0.5) + (btBirL*1))*100;
	    		int totalGiris1[4] = {((totalGiris/1000)),((totalGiris/100)%10),((totalGiris/10)%10),(totalGiris%10)};
	    		for(delay = 0; delay < 100000; delay++);
	    		paracoz(totalGiris1, 0);
	    		for(delay = 0; delay < 100000; delay++);
	    		break;

	    	}
	    }

	    int kontrol = 0;
	    while(1){
	    	if((GPIO_PORTC_DATA_R & 0b10000000) == 0 && kontrol == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	    		su += 1;
	    		itoa(su,str,10);
	    		LCDyaz(1,0,str);
	    		for(delay = 0; delay < 100000; delay++);
	    		kontrol = stokKontrol(su , UrunID[0], UrunStok[0]);
	       		for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTC_DATA_R & 0b01000000) == 0 && kontrol == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	       		cay += 1;
	       		itoa(cay,str,10);
	       		LCDyaz(1,0,str);
	       		for(delay = 0; delay < 100000; delay++);
	       		kontrol = stokKontrol(cay , UrunID[1], UrunStok[1]);
	       		for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTC_DATA_R & 0b00100000) == 0 && kontrol == 0){
	    	    LCDayar(0x01);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    kahve += 1;
	    	    itoa(kahve,str,10);
	    	    LCDyaz(1,0,str);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    kontrol = stokKontrol(kahve , UrunID[2], UrunStok[2]);
	    	    for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTC_DATA_R & 0b00010000) == 0 && kontrol == 0){
	    	    LCDayar(0x01);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    cikolata += 1;
	    	    itoa(cikolata,str,10);
	    	    LCDyaz(1,0,str);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    kontrol = stokKontrol(cikolata , UrunID[3], UrunStok[3]);
	    	    for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTE_DATA_R & 0b00010000) == 0 && kontrol == 0){
	    	    LCDayar(0x01);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    biskuvi += 1;
	    	    itoa(biskuvi,str,10);
	    	    LCDyaz(1,0,str);
	    	    for(delay = 0; delay < 100000; delay++);
	    	    kontrol = stokKontrol(biskuvi , UrunID[4], UrunStok[4]);
	    	    for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTE_DATA_R & 0b01000) == 0 && kontrol == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	    	    ucret = ((su*UrunFiyat[0]) + (cay*UrunFiyat[1]) + (kahve*UrunFiyat[2]) + (cikolata*UrunFiyat[3]) + (biskuvi*UrunFiyat[4]))*100;
	    	    int ucret1[4] = {((ucret/1000)),((ucret/100)%10),((ucret/10)%10),(ucret%10)};
	    	    for(delay = 0; delay < 100000; delay++);
	    	    if(totalGiris < ucret){
	    	    	kontrol = 1;
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	LCDyaz(1,0,"Tutar asti");
	    	    	LCDyaz(2,0,"Reset bekleniyor.");
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    } else {
	    	    	paracoz(ucret1, 1);
	    	    	for(delay = 0; delay < 5000000; delay++);
	    	    	break;
	    	    }
	    	    for(delay = 0; delay < 100000; delay++);
	    	}
	    	if((GPIO_PORTE_DATA_R & 0b000100000) == 0){
	    		LCDayar(0x01);
	    		for(delay = 0; delay < 100000; delay++);
	    		su = 0;
	    		for(delay = 0; delay < 100000; delay++);
	    		cay = 0;
	    		for(delay = 0; delay < 100000; delay++);
				kahve = 0;
				for(delay = 0; delay < 100000; delay++);
				cikolata = 0;
				for(delay = 0; delay < 100000; delay++);
				biskuvi = 0;
				for(delay = 0; delay < 100000; delay++);
				kontrol = 0;
				for(delay = 0; delay < 100000; delay++);
				LCDyaz(1,0,"Urun seciniz.");
				for(delay = 0; delay < 100000; delay++);
	    	}
	    }

	    int rast, kontrol2 = 0;


	    	while(1){
	    		rast = 1 + rand()%4;
	    	    if(rast == 2 && kontrol2 == 0){
	    	    	LCDayar(0x01);
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	GPIO_PORTA_DATA_R |= 0b00001000;
	    	    	GPIO_PORTA_DATA_R &= ~0b00000100;
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	kontrol2 = 1;
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	LCDyaz(1,0,"Para Sikisti!");
	    	    	LCDayar(0x01);
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	LCDyaz(1,0,"Para iadesi");
	    	    	LCDyaz(2,0,"Reset Bekleniyor.");
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    }
	    	    if((rast == 1 || rast == 3 || rast == 4) && kontrol2 == 0){
	    	    	LCDayar(0x01);
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	GPIO_PORTA_DATA_R &= ~0b00001000;
	    	       	GPIO_PORTA_DATA_R |= 0b00000100;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	kontrol2 = 1;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	paraUstu();
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	LCDyaz(1,0,"Reset Bekleniyor");
	    	       	for(delay = 0; delay < 100000; delay++);
	    	    }
	    	    if((GPIO_PORTE_DATA_R & 0b000100000) == 0){
	    	    	LCDayar(0x01);
	    	    	GPIO_PORTA_DATA_R &= ~0b00001000;
	    	    	GPIO_PORTA_DATA_R &= ~0b00000100;
	    	    	for(delay = 0; delay < 100000; delay++);
	    	    	su = 0;
	    	    	for(delay = 0; delay < 100000; delay++);
	    	       	cay = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	   	        	kahve = 0;
	   	        	for(delay = 0; delay < 100000; delay++);
	    	       	cikolata = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	     	       	biskuvi = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	btYirmiBesK = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	btElliK = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	btBirL = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	       	kontrol = 0;
	    	       	for(delay = 0; delay < 100000; delay++);
	    	    	goto start;
	    	    }
	    	}
}





int main(){
    otomatCalistir();
}
