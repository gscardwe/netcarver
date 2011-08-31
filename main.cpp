/*******************************************************************************
	Program:		main.cpp
	Author:			Greg Cardwell <gscardwe@nps.edu>
	Description:	BSSID and WiFi SSID carver
	
*******************************************************************************/
#include "common.h"
#include <string>
#include <iostream>
enum debugLevel verbose = OFF;

/* Array for data */
#define captures 1000
str_object sid[captures];
bs_object cid[captures];	//for binary grep searches
str_object meta[captures];  //for base station format searches
str_object btm[captures];   //bluetooth data structures
str_object ssid[captures];  //ssid search
str_object ip[captures];    //ip search
str_object sim[captures];   //sim search
ip_object bin[captures];    //binary ip search
ipgrep_object bgrep[captures];	// binary ip grep struct big endian
ip_object bgrep1[captures]; // binary ip grep struct little endian
str_object bin2[captures];
struct in_addr ip_addr;
struct in_addr ip_addrL;
int totalCap = 0;   		//keep track of total structs
int nGram = 1;

void usage(char *prog)	{
	cout <<	"Usage:  "	<< prog << " [options] -f <file> " << endl;
	cout << "        -s [input] ASCII grep"	<< endl;
	cout << "        -b [: -] MAC data structure" << endl;	
	//cout << "        [-b 'input' Search binary SSID's]" << endl;
	//cout << "        [-d [CID#]  Grep Search CID's   ]" << endl;
	cout << "        -t [: -] BS Metadata " << endl;
	cout << "        -i IP ASCII grep  "    << endl;
	cout << "        -m SIM grep"           << endl;
	cout << "        -a SSID grep"          << endl;
	cout << "        -p Binary IP"          << endl;
	cout << "        -g Binary IP Grep"     << endl;
	cout << "        -y nGram Size"         << endl;
	exit(-1);
}

void dumpStrings() {

    cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << sid[n].count ;
		cout << "\t" << sid[n].ssidname ;
		//cout << "\t" << sid[n].pregram  ;
		printf("\t%s", sid[n].pregram);
		cout << "\t" << sid[n].postgram << endl;
    }
    cout << " " << endl;
}

void dumpBSID() {

    cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << cid[n].count ;
		cout << "\t" << cid[n].mnc ;
		printf("\t %s ", cid[n].pregram); 
		printf("\t %s ", cid[n].postgram);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpBSMeta(){
	cout << endl << "Results:" << endl;
    cout << "Count\tBS Metadata\tPre\tPost" << endl << "-------------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
		int reformpre  = 0x0000ff & meta[n].pregram[0];
		int reformpost = 0x0000ff & meta[n].postgram[0];
    	cout << meta[n].count ;
		cout << "\t" << meta[n].ssidname ;
		printf("\t0x%x ", reformpre); 
		printf("\t 0x%x ", reformpost);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpBT(){
	cout << endl << "Results:" << endl;
    cout << "Count\t\tMAC\t   Pre\tPost" << endl << "--------------------------------------------" << endl;
	
    for(int n = 0; n<totalCap; n++){
		int reformpre  = 0x0000ff & btm[n].pregram[0];
		//int reformpre1  = 0x0000ff & btm[n].pregram[1];
		int reformpost = 0x0000ff & btm[n].postgram[0];
    	cout << btm[n].count ;
		cout << "\t" << btm[n].ssidname ;
		printf("  0x%x", reformpre); 
		//printf("  %x", reformpre1);
		printf("\t0x%x", reformpost);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpIP(){
	cout << endl << "Results:" << endl;
    cout << "Count\tIP Address\t  Pre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
		int reformpre  = 0x0000ff & ip[n].pregram[0];
		int reformpost = 0x0000ff & ip[n].postgram[0];
    	cout << ip[n].count ;
		printf("\t %s", ip[n].ssidname);
		if (strlen(ip[n].ssidname) < 15){
			printf("\t  0x%x ", reformpre);
		}
		else {printf("  0x%x ", reformpre);}
		printf("\t  0x%x ", reformpost);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpSSID(){
	cout << endl << "Results:" << endl;
    cout << "Count\tSSID" << endl << "----------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << ssid[n].count ;
		cout << "\t" << ssid[n].ssidname ;
		//printf("\t0x%x", ssid[n].pregram[0]); 
		//printf("\t0x%s", ssid[n].postgram[0]);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpSIM(){
	cout << endl << "Results:" << endl;
    cout << "Count\tSIM Number" << endl << "--------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << sim[n].count ;
		cout << "\t" << sim[n].ssidname ;
		//printf("\t0x%2x ", sim[n].pregram[0]); 
		//printf("\t 0x%x ", sim[n].postgram[0]);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpBIN(){
	cout << endl << "Results:" << endl;
    cout << "Count\tPotential IP Address" << endl << "--------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << bin[n].count ;
		printf("\t %d.", bin[n].t1);
		printf("%d.", bin[n].t2);
		printf("%d.", bin[n].t3);
		printf("%d", bin[n].t4);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpIPGrep(char *key){
	cout << endl << "Results:" << endl;
    cout << "Count\t"<<key<<" "<<"\t"<<nGram<<" gram"<< endl; 
	cout << "--------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << bgrep[n].count ;
		printf("\t%2x ", bgrep[n].t1);
		printf("%2x ", bgrep[n].t2);
		printf("%2x ", bgrep[n].t3);
		printf("%2x ", bgrep[n].t4);
		printf("  %X ", bgrep[n].pregram);
		cout << endl;
    }
    cout << " " << endl;
	//delete bgrep;
}

void str_sorter(str_object *value){
	str_object *xx = new str_object;
	
	int i = 0;
	while (i < totalCap){
		
		for(int n = 1; n<totalCap; n++){
			int len = strlen(value[n].ssidname);
			if (value[n].count > value[n-1].count){
				xx->count = value[n-1].count;
				memcpy(xx->ssidname, value[n-1].ssidname, len);
				memcpy(xx->pregram, value[n-1].pregram, 1);
				memcpy(xx->postgram, value[n-1].postgram, 1);
				
				value[n-1].count = value[n].count;
				memcpy(value[n-1].ssidname, value[n].ssidname, len);
				memcpy(value[n-1].pregram, value[n].pregram, 1);
				memcpy(value[n-1].postgram, value[n].postgram, 1);

				value[n].count = xx->count;
				memcpy(value[n].ssidname, xx->ssidname, len);
				memcpy(value[n].pregram, xx->pregram, 1);
				memcpy(value[n].postgram, xx->postgram, 1);
				
			}
		}
		i++;
	}
	xx = NULL;
}

void ip_sorter(ip_object *bvalue){
	ip_object *xx = new ip_object;
	int i = 0;
	while (i < totalCap){
		
		for(int n = 1; n<totalCap; n++){
			if (bvalue[n].count > bvalue[n-1].count){
				xx->count = bvalue[n].count;
				xx->t1 = bvalue[n].t1;
				xx->t2 = bvalue[n].t2;
				xx->t3 = bvalue[n].t3;
				xx->t4 = bvalue[n].t4;
				memcpy(xx->pregram, bvalue[n].pregram, 1);
				memcpy(xx->postgram, bvalue[n].postgram, 1);
				
				bvalue[n].count = bvalue[n-1].count;
				bvalue[n].t1 = bvalue[n-1].t1;
				bvalue[n].t2 = bvalue[n-1].t2;
				bvalue[n].t3 = bvalue[n-1].t3;
				bvalue[n].t4 = bvalue[n-1].t4;
				memcpy(bvalue[n].pregram, bvalue[n-1].pregram, 1);
				memcpy(bvalue[n].postgram, bvalue[n-1].postgram, 1);
				
				bvalue[n-1].count = xx->count;
				bvalue[n-1].t1 = xx->t1;
				bvalue[n-1].t2 = xx->t2;
				bvalue[n-1].t3 = xx->t3;
				bvalue[n-1].t4 = xx->t4;
				memcpy(bvalue[n-1].pregram, xx->pregram, 1);
				memcpy(bvalue[n-1].postgram, xx->postgram, 1);
				
			}
		}
		i++;
	}
	xx = NULL;
}

/*
void ipgrep_sorter(ipgrep_object *bvalue){
	ip_object *xx = new ip_object;
	int i = 0;
	while (i < totalCap){
		
		for(int n = 1; n<totalCap; n++){
			if (bvalue[n].count > bvalue[n-1].count){
				xx->count = bvalue[n].count;
				xx->t1 = bvalue[n].t1;
				xx->t2 = bvalue[n].t2;
				xx->t3 = bvalue[n].t3;
				xx->t4 = bvalue[n].t4;
				memcpy(xx->pregram, bvalue[n].pregram, 1);
				memcpy(xx->postgram, bvalue[n].postgram, 1);
				
				bvalue[n].count = bvalue[n-1].count;
				bvalue[n].t1 = bvalue[n-1].t1;
				bvalue[n].t2 = bvalue[n-1].t2;
				bvalue[n].t3 = bvalue[n-1].t3;
				bvalue[n].t4 = bvalue[n-1].t4;
				memcpy(bvalue[n].pregram, bvalue[n-1].pregram, 1);
				memcpy(bvalue[n].postgram, bvalue[n-1].postgram, 1);
				
				bvalue[n-1].count = xx->count;
				bvalue[n-1].t1 = xx->t1;
				bvalue[n-1].t2 = xx->t2;
				bvalue[n-1].t3 = xx->t3;
				bvalue[n-1].t4 = xx->t4;
				memcpy(bvalue[n-1].pregram, xx->pregram, 1);
				memcpy(bvalue[n-1].postgram, xx->postgram, 1);
				
			}
		}
		i++;
	}
	xx = NULL;
}*/

/*void dumpBIN2(){
	cout << endl << "Results:" << endl;
	cout << "Count\t\tIP CRAP ADDRESS!!!" << endl << "--------------------------------------------" << endl;

	for(int n = 0; n<totalCap; n++){
		cout << bin2[n].count;
		printf("\t%x.",bin2[n].ssidname[0]);
		printf("%x.",bin2[n].ssidname[1]);
		printf("%x.",bin2[n].ssidname[2]);
		printf("%x",bin2[n].ssidname[3]);
		
		//printf("\t%s", bin2[n].pregram[0]); 
		//printf("\t 0x%x ", bin2[n].postgram[0]);
		cout << endl;
	}
	cout << " " << endl;
}
*/

/*
THIS CODE PROVIDES NO REAL VALUE - GSC 1 JUL 2011
void dumpResults3() {
    cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << stump[n].count ;
		cout << "\t" << stump[n].sidname ;
		//cout << "\t" << stump[n].pregram  ;
		printf("\t%s", stump[n].pregram);
		printf("\t%s", stump[n].postgram);
		cout << endl;
    }
    cout << " " << endl;
}

int binDump(unsigned long bytes_read, unsigned char *data, char *key) {
	unsigned char *base = data;
	int num_good_chars = 0;
	bool comp = true;
	//struct sid_object *stump;
	int length = strlen(key);	
	int i = 0;
	while (i < length){
		if (base[i] == key[i]){
			num_good_chars++;
		}
		else {
			comp = false;
			break;
		}
		i++;
	}
	if (comp) {
    	sid_object *tester = new sid_object;
        memcpy(tester->pregram, data-nGram, nGram);
    	string str1 = tester->pregram;
    	int newSSID = 1;
    	
    	for(int n = 0; n<=totalCap; n++){
			string str2 = stump[n].pregram;
			
			if (str1.compare(str2) == 0){
				stump[n].count++;
				newSSID = 0;
				break;
			}
		}
    	if (newSSID){
			cout << bytes_read;
			stump[totalCap].count++;
			memcpy(stump[totalCap].pregram, data-nGram, nGram);
			memcpy(stump[totalCap].sidname, data, num_good_chars);
			memcpy(stump[totalCap].postgram, data+num_good_chars, nGram);
			totalCap++;
		}
		tester = NULL;
	}
	return num_good_chars;
}
 */

int testbsid(char input, char *key, void *data){
	int length = strlen(key);
	int num_good_chars = 0;
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	int match = 0;

	if (input == 'b'){
		int i = 0;
		while (i <length){
			if (bcid[i] == key[i]){
				num_good_chars++;
			}
			else {
				break;
			}
			i++;
		}
		if (length-1 == i) {
			match = 1;
		}
	}
	
	else if (input == 'l'){
		int j = length - 1;
		while (j >= 0){
			if (bcid[j] == key[j]){
				num_good_chars++;
				j--;
			}
			else {
				break;
			}
		}
		if (j < 0) {
			match = 1;
		}
	}
	if (match){
		bs_object *tester = new bs_object;
		memcpy(tester->pregram, contain-nGram, nGram);
		memcpy(tester->mnc, contain, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->mnc;
		int newSSID = 1;

		for(int n = 0; n<=totalCap; n++){
			string str3 = cid[n].pregram;
			string str4 = cid[n].mnc;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					cid[n].count++;
					newSSID = 0;
					break;
				}
			}
		}

		if (newSSID){
			cid[totalCap].count++;
			memcpy(cid[totalCap].pregram, bcid-nGram, nGram);
			cid[totalCap].mnc = key;
			memcpy(cid[totalCap].postgram, bcid+length, nGram);
			totalCap++;
		}

		tester = NULL;
	}
	
	return num_good_chars;
}

int strRetriever(unsigned char *data, char *key) {
	unsigned int maxlen = strlen(key); 
    int num_good_chars = 0;
    bool comp = true;
	unsigned char *contain = (unsigned char *) data;
    
    for (unsigned int z = 0; z<maxlen; z++){
    	if(data[z] != key[z]){
    		comp = false;
    		break;
    	}
    	num_good_chars++;
	}
	
    if (comp){
    	str_object *tester = new str_object;
        memcpy(tester->pregram, data-nGram, nGram);
		memcpy(tester->ssidname, contain, num_good_chars);
    	string str1 = tester->pregram;
		string str2 = tester->ssidname;
    	int newStr = 1;
    	
		for(int n = 0; n<=totalCap; n++){
			string str3 = sid[n].pregram;
			string str4 = sid[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					sid[n].count++;
					newStr = 0;
					break;
				}
			}
		}
    	if (newStr){
			sid[totalCap].count++;
			memcpy(sid[totalCap].pregram, data-nGram, nGram);
			memcpy(sid[totalCap].ssidname, data, num_good_chars);
			memcpy(sid[totalCap].postgram, data+num_good_chars, nGram);
			totalCap++;
		}
		tester = NULL;
	}

    return num_good_chars;
}

int btMeta(char *inputSep, void *data){
	int num_good_chars = 0;
	int match = 0;		//bool value for a match
	int maxField = 2;	//max # of digits in fields
	int colon = 0;      //bs metadata should have 4 fields,3colons. counts colons.
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	char inputS = inputSep[0];

	for (int j =0; j<6; j++){	//6 fields of 2 alphanumeric representations
		int i = 0;
		while (i < maxField){
			if(isalnum(bcid[num_good_chars])){
				num_good_chars++;
				match = 1;
			}
			else {
				match = 0;
				break;
			}
			i++;
		}
		
		if (match && colon == 5 && i > 0){
			match = 1;
		}
		else if (match && bcid[num_good_chars] == inputS && num_good_chars){
			num_good_chars++;
			colon++;
			match = 1;
		}
		else {
			j = 6;
			match = 0;
		}
	}
	
	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram, nGram);
		memcpy(tester->ssidname, contain, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = btm[n].pregram;
			string str4 = btm[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					btm[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		
		if (newSSID){
			btm[totalCap].count++;
			memcpy(btm[totalCap].pregram, bcid-nGram, nGram);
			memcpy(btm[totalCap].ssidname, bcid, num_good_chars);
			memcpy(btm[totalCap].postgram, bcid+num_good_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}	
	//cout << num_good_chars;
	return num_good_chars;
}

int bsMetaData(char *inputSep, void *data){
	int num_good_chars = 0;
	int match = 0;		//bool value for a match
	int maxField = 4;	//max # of digits in fields
	int colon = 0;      //bs metadata should have 4 fields,3colons. counts colons.
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	char inputS = inputSep[0];
	
	for (int j =0; j<4; j++){	//4 is the number of fields:cid/lac/mnc/mcc
		int i = 0;
		//int s = 0;              //ensure at least one digit per field
		while (i < maxField){
			if(isdigit(bcid[num_good_chars])){
				num_good_chars++;
		//		int s = 1;
			}
			else {
				match = 0;
				break;
			}
			i++;
		}
		if (colon == 3 && j == 3){
			if (bcid[num_good_chars] == inputS || isalnum(bcid[num_good_chars])){match = 0; break;}
			else if (isalnum(bcid[-1])) {match = 0; break;}
			else {match = 1;}
		}
		else if (bcid[num_good_chars] == inputS && num_good_chars){
			num_good_chars++;
			colon++;
			match = 1;
		}
		else {
			j = 4;
			match = 0;
		}

	}
	
	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram, nGram);
		memcpy(tester->ssidname, contain, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = meta[n].pregram;
			string str4 = meta[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					meta[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		if (newSSID){
			meta[totalCap].count++;
			memcpy(meta[totalCap].pregram, bcid-nGram, nGram);
			memcpy(meta[totalCap].ssidname, bcid, num_good_chars);
			memcpy(meta[totalCap].postgram, bcid+num_good_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}	
	return num_good_chars;
}

int ssidSearch(void *data){
	 //pulls SSIDs from wpa_supplicant.conf format
	int num_good_chars = 0;
	int num_ssid_chars = 0;
	int match = 0;		//bool value for a match
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	char *file = "ssid=\"";
	char *end = "\"";
		
	for (int i =0; i<strlen(file);i++){
		if(bcid[i] == file[i]){
			num_ssid_chars++;
			match = 1;
		}
		else {match = 0; break;}
		
	}
	
	if(match){
		for(int j=num_ssid_chars; j<38; j++){	//38 = 6 ssid=" chars + 32 max ssid parameter
			if (bcid[j] == end[0]){break;}
			else {num_good_chars++;}
		}
	}
		
	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram+num_ssid_chars, nGram);
		memcpy(tester->ssidname, contain+num_ssid_chars, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = ssid[n].pregram;
			string str4 = ssid[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					ssid[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		if (newSSID){
			ssid[totalCap].count++;
			memcpy(ssid[totalCap].pregram, bcid-nGram+num_ssid_chars, nGram);
			memcpy(ssid[totalCap].ssidname, bcid+num_ssid_chars, num_good_chars);
			memcpy(ssid[totalCap].postgram, bcid+num_good_chars+num_ssid_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}
	return num_good_chars + num_ssid_chars;
}

int ipSearch(void *data){
	// seaches for ip data structures
	
	int num_good_chars = 0;
	int match = 0;		//bool value for a match
	int maxField = 3;
	int decimal = 0;
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	char *inputS = ".";
	
	for (int j =0; j<4; j++){	//4 fields of up to 3 digits
		int i = 0;
		while (i < maxField){
			if(isdigit(bcid[num_good_chars])){
				num_good_chars++;
				match = 1;
			}
			else {
				match = 0;
				break;
			}
			i++;
		}
		
		if (decimal == 3 && i > 0){
			match = 1;
		}
		else if (bcid[num_good_chars] == inputS[0] && num_good_chars){
			num_good_chars++;
			decimal++;
			match = 1;
		}
		else {
			j = 4;
			match = 0;
		}
	}
	
	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram, nGram);
		memcpy(tester->ssidname, contain, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = ip[n].pregram;
			string str4 = ip[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					ip[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		if (newSSID){
			ip[totalCap].count++;
			memcpy(ip[totalCap].pregram, bcid-nGram, nGram);
			memcpy(ip[totalCap].ssidname, bcid, num_good_chars);
			memcpy(ip[totalCap].postgram, contain+num_good_chars, nGram);
			//ip[totalCap].postgram = 0x000000ff & bcid[num_good_chars];
			//tester->t4 = 0x000000ff & bcid[21];
			totalCap++;
		}
		
		tester = NULL;
	}
	return num_good_chars;
}

int simSearch(void *data){
	// will pull sim as written by accounts.db and CheckinService.xml files
	int num_good_chars = 0;
	int num_sim_chars  = 0;
	int match = 1;		//bool value for a match
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	char *simgram = "lastSim\">00000000000000000018\n";
	char *imsi    = "imsi";
	int sgram = 0;
	int igram = 0;

	for (unsigned int z = 0; z<strlen(simgram); z++){
    	if(bcid[z] != simgram[z]){
    		match = 0;
 			break;
    	}
    	num_sim_chars++;
	}

	if(match){
		sgram = 1;}
	else {
		num_sim_chars = 0;
		for (unsigned int z = 0; z<strlen(imsi); z++){
			if(bcid[z] != imsi[z]){
				match = 0;
				break;
			}
			match = 1;
			num_sim_chars++;
		}
		if (match){igram = 1;}
	}
	
	
	int j = num_sim_chars;
	char *end = "<";
	char *iend = " ";
	
	if(sgram){
		while (bcid[j] != end[0]){
			num_good_chars++;
			j++;
		}
	}
	else if(igram){
		while (bcid[j] != iend[0]){
			num_good_chars++;
			j++;
		}
	}
		

	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram+num_sim_chars, nGram);
		memcpy(tester->ssidname, contain+num_sim_chars, num_good_chars);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = sim[n].pregram;
			string str4 = sim[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					sim[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		if (newSSID){
			sim[totalCap].count++;
			memcpy(sim[totalCap].pregram, bcid-nGram+num_sim_chars, nGram);
			memcpy(sim[totalCap].ssidname, bcid+num_sim_chars, num_good_chars);
			memcpy(sim[totalCap].postgram, bcid+num_good_chars+num_sim_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}
	return num_good_chars+num_sim_chars;
}

int binSearch(void * data){
	int num_good_chars = 0;
	int num_bin_chars = 0;
	char *bcid = (char *) data;
	//uint32_t *bcit = (uint32_t *) data;
	//unsigned char *contain = (unsigned char *) data;
	int match = 1;
	char bing[5] = {255, 255, 02, 01, 06};
	
	for (int z = 0; z<5; z++){
		if(bcid[z] != bing[z]){
    		match = 0;
 			break;
    	}
		num_bin_chars++;
	}
	
	if (match){
		num_good_chars = 13;
	}
	
	if (match){
		
		ip_object *tester = new ip_object;
		memcpy(tester->pregram, bcid-nGram, nGram);
		tester->t1 = 0x000000ff & bcid[18];
		tester->t2 = 0x000000ff & bcid[19];
		tester->t3 = 0x000000ff & bcid[20];
		tester->t4 = 0x000000ff & bcid[21];
	
		string str1 = tester->pregram;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = bin[n].pregram;
			
			if (tester->t1 == bin[n].t1){
				if (tester->t2 == bin[n].t2){
					if (tester->t3 == bin[n].t3){
						if (tester->t4 == bin[n].t4){
							if(str1.compare(str3)== 0){
								bin[n].count++;
								newSSID = 0;
								break;
							}
						}
					}
				}
			}
		}
		
		if (newSSID){
			bin[totalCap].count++;
			memcpy(bin[totalCap].pregram, bcid-nGram, nGram);
			bin[totalCap].t1 = 0x000000ff & (bcid[18]);
			bin[totalCap].t2 = 0x000000ff & (bcid[19]);
			bin[totalCap].t3 = 0x000000ff & (bcid[20]);
			bin[totalCap].t4 = 0x000000ff & (bcid[21]);
			totalCap++;
		}
		
		tester = NULL;
	}
	
	return num_good_chars+num_bin_chars;
}

int binaryGrep(char	* key, void * data){
	int num_good_chars = 0;
	int num_bin_chars = 0;
	char *bcid = (char *) data;
	uint32_t * bcit = (uint32_t *) data;
	uint32_t * comp_addr;
	//if (bcid[0] == key[0]){cout << "greg rulx"<<endl;}

	int match = 0;
	//int bing[3];
	//char * bout[4];
	//string key1 = (string)key;
	char sen[14], sen1[14], sen2[14], sen3[14];
	char per[1], per1[1], per2[2];

	sscanf(key, "%[^.] %c %[^.] %c %[^.] %c %[^=\n]", sen, per, sen1, per1, sen2, per2, sen3);
	strcat(sen3, ".");
	strcat(sen3, sen2);
	strcat(sen3, ".");
	strcat(sen3, sen1);
	strcat(sen3, ".");
	strcat(sen3, sen);
	
	inet_aton(sen3, &ip_addrL);
	//printf("%x \n", ip_addrL.s_addr);
	//uint32_t *greg = (uint32_t *)ip_addr.s_addr;
	//printf("ggg=%x   iiii=%x      \n", greg, bcit);
	if ((ip_addr.s_addr == *bcit) || (ip_addrL.s_addr == *bcit)) {
		match = 1;
	}
	
	//comp_addr = 0x0000ff & bcit[z]
	//cout <<comp_addr<<endl;
	//char greg = 192;
	//sscanf(sen, "%x", &i);
	//int buffer[1];
	//printf("%x\n", bcid[0]);
	
	//printf("%2x", buffer);
	/*if(bcid[0] == key[0]){
		cout << "greg rulx"<<endl;
		cout << bcid[0] << " " << key[0] << endl;
		num_bin_chars++;
		if (bcid[1] == 0xA8){
			num_bin_chars++;
			if (bcid[2] == 0x01){
				num_bin_chars++;
				if (bcid[3] == 0x01){
					num_bin_chars++;
					match = 1;
					cout << "greg rulx"<<endl;
					//break;
				}
			}
		}
	}
	
	for (int x = 0; x < strlen(key); x++){
		if (bcid[x] == key[x]){
			match = 1;
			num_bin_chars++;
		}
		else {match=0; x = strlen(key);}
	}*/
	//if(match){cout<<"greg laksdjfklas"<<endl;}
	

	
	/*
	int spin = 0;
	int spin2 = 0;
	for (int z = 0; z<4; z++){
		int y = spin2;
		int spin = 0;
		if(key[y] == '.'){
			y++;}
		while(key[y] != '.'){
			y++;
			spin++;
			spin2++;
		}
		bing[z] = spin;

	}
	ipgrep_object *key1 = new ipgrep_object;
	
	memcpy(key1->t1, key, 2);
	bout[0] = key1->t1;

	memcpy(key1->t2, key+bing[0]+1, bing[1]);
	bout[1] = key1->t2;

	memcpy(key1->t3, key+bing[0]+bing[1]+2, bing[2]);
	bout[2] = key1->t3;

	memcpy(key1->t4, key+bing[0]+bing[1]+bing[2]+3, bing[3]);
	bout[3] = key1->t4;
	
	key1 = NULL;
	
	for (int z = 0; z<4; z++){
		if(bcid[z] != bout[z] ){
    		match = 0;
 			break;
    	}
		num_bin_chars++;
	}
	*/
	//match = 0;
	if (match){
		//cout<<"MAKE IT HERE YET?"<<endl;
		ipgrep_object *tester = new ipgrep_object;
		//memcpy(tester->pregram, bcid-nGram, nGram);
		char * bob1 = (char *) data-nGram;
		tester->pregram = bob1[0];
		//tester->t1 = 0x000000ff & bing[0];
		//tester->t2 = 0x000000ff & bing[1];
		//tester->t3 = 0x000000ff & bing[2];
		//tester->t4 = 0x000000ff & bing[3];
		
		//string str1 = tester->pregram;
		int newSSID = 1;
		
		//for (int j = 1; j<=nGram; j++){
			
		
		/*for(int n = 0; n<=totalCap; n++){
			string str3 = bgrep[n].pregram;
			
			if (tester->t1 == bgrep[n].t1){
				if (tester->t2 == bgrep[n].t2){
					if (tester->t3 == bgrep[n].t3){
						if (tester->t4 == bgrep[n].t4){
							if(str1.compare(str3)== 0){
								bgrep[n].count++;
								newSSID = 0;
								break;
							}
						}
					}
				}
			}
		}*/
		
		if (newSSID){
			bgrep[totalCap].count++;
			
			//memcpy(bgrep[totalCap].t1, bcid[0], 1);
			char * bob = (char *) data-nGram;
			//memcpy(bgrep[totalCap].pregram, bob, nGram);
			bgrep[totalCap].pregram = bob[1] & bob[0];
			bgrep[totalCap].t1 = 0x000000ff & (bcid[0]);
			//printf("%x\n", bgrep[totalCap].pregram);
			bgrep[totalCap].t2 = 0x000000ff & (bcid[1]);
			bgrep[totalCap].t3 = 0x000000ff & (bcid[2]);
			bgrep[totalCap].t4 = 0x000000ff & (bcid[3]);
			totalCap++;
		}
		
		tester = NULL;
	}
	
	return num_good_chars;
  
}

/*
int binSearch22(void *data){
	int num_good_chars = 0;
	int num_bin_chars = 0;
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	int match = 1;
	char bing[5] = {255, 255, 02, 01, 06};
	
	for (int z = 0; z<5; z++){
		if(bcid[z] != bing[z]){
    		match = 0;
 			break;
    	}
		num_bin_chars++;
	}
	
	if (match){
		num_good_chars = 13;
	}
	
	if (match){
		str_object *tester = new str_object;
		memcpy(tester->pregram, contain-nGram, nGram);
		memcpy(tester->ssidname, contain+num_good_chars+num_bin_chars, 4);
		string str1 = tester->pregram;
		string str2 = tester->ssidname;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str3 = bin2[n].pregram;
			string str4 = bin2[n].ssidname;
			
			if (str2.compare(str4) == 0){
				if(str1.compare(str3)== 0){
					bin2[n].count++;
					newSSID = 0;
					break;
				}
			}
		}
		if (newSSID){
			for (int y = 0; y< 21; y++){
				//cout << hex << (uint8_t)(bcid[y])<<" "<<endl;
				printf("%1x\n", (unsigned int)contain[y]);
			}
			bin2[totalCap].count++;
			memcpy(bin2[totalCap].pregram, bcid-nGram, nGram);
			memcpy(bin2[totalCap].ssidname, bcid+num_good_chars+num_bin_chars, 4);
			//memcpy(bin2[totalCap].postgram, bcid+num_good_chars+num_bin_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}
	
	return num_good_chars+num_bin_chars;
}
*/
unsigned long mmapOpen(char *file, void **data) {
    unsigned long len = file_size(file);
    if (len <= 0)
        pfatal("bad file");
    debug(OFF, ">> Reading: " << file << " (len: " << len << ")");
    int fd = open(file, O_RDONLY);
    *data = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    if (MAP_FAILED == data) 
        pfatal("mmap"); 
    return len;
}

int main (int argc, char * const argv[]) {
	int ch;
	char *key    = NULL;
	char *file   = NULL;
	
	bool ssidbool    = false;  //ssid search
	bool ipbool      = false;  //IP search
	bool strbool     = false;  //String search
	bool btbool      = false;  //bluetooth search
	bool simbool     = false;  //sim data
	bool binarybool  = false;  //dhcp lease binary ip data
	bool binGrep	 = false;  //binary IP grep
	//bool binssid = false;
	bool bsidbool    = false;  //Grep binary search
	bool bsmetabool  = false;  //Search for bs metadata format xxx:xxx:xx:xx
	int advance  = 0;
	char input;			   //big or little endian
	char *inputSep;		   //to indicate a : or - seperator
	
	while ((ch = getopt(argc, argv, "s:d:b:t:g:y:aimpf:")) != EOF) {
		
		switch ((char) ch) {
				
		case 'a':
			ssidbool = true;
			break;
				
		case 'b':
			btbool = true;
			inputSep = optarg;
			break;
				
		case 's':
			strbool = true;
			key  = optarg;
			break;
		
		/*case 'b':
			binssid = true;
			key  = optarg;
			break;*/
		
		case 'd':
			bsidbool = true;
			key = optarg;
			break;
				
		case 't':
			bsmetabool = true;
			inputSep = optarg;
			break;
			
		case 'f':
			file = optarg;
			break;
				
		case 'i':
			ipbool = true;
			break;
				
		case 'm':
			simbool = true;
			break;
				
		case 'p':
			binarybool = true;
			//nGram = 4;
			break;
		case 'g':
			binGrep = true;
			key = optarg;
			inet_aton(optarg, &ip_addr);
			printf("greg is searching for: %x\n",ip_addr.s_addr);
			break;
				
		case'y':
			nGram = atoi(optarg);
		    //cout << "greg" <<nGram<<endl;
			break;
		
		default:
			usage(argv[0]);
			break;
		}
	}
	
	if (!file)
		usage(argv[0]);
		
	unsigned long pagesize = getpagesize();
	debug(HIGH, ">> System page size: " << pagesize);
    void *data = NULL;
    unsigned long len = mmapOpen(file, &data);
    unsigned char *c = (unsigned char *) data;
  
    unsigned long bytes_read = 0;
 
    cerr << "\nPlease wait, currently searching: \n";
    if (key){cerr << key << "\n" << endl;}
	
	
	int length = 5;
	if (key){length = strlen(key);}
	if(binGrep){length = strlen(key);}

    while (bytes_read < len-length-1) {
    	if(strbool){
    		advance = strRetriever(c, key);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
		
		else if (ssidbool){
			advance = ssidSearch(c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}		
		}
		
		else if(btbool){
			advance = btMeta(inputSep ,c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}		
		}
		
		/*
        else if(binssid){
    		advance = binDump(bytes_read, c, key);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
    	*/
    	
		else if(bsidbool){
    		advance = testbsid(input, key, c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
		else if (bsmetabool){
			advance = bsMetaData(inputSep, c);
			if(advance){
				c+=(advance-1);
				bytes_read+=(advance-1);
			}
		}
		
		else if (ipbool){
			advance = ipSearch(c);
			if(advance){
				c+=(advance-1);
				bytes_read+=(advance-1);
			}
		}
		
		else if (simbool){
			advance = simSearch(c);
			if(advance){
				c+=(advance-1);
				bytes_read+=(advance-1);
			}
		}
		
		else if (binarybool){
			advance = binSearch(c);
			if(advance){
				c+=(advance-1);
				bytes_read+=(advance-1);
			}
		}
		
		else if(binGrep){
			
    		advance = binaryGrep(key, c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
		
    	c++;
    	bytes_read++;
    	if (bytes_read % 10000000 == 0) cerr << ".";
    }
	cout << "\n" <<endl;
	
    if (strbool){
		str_sorter(sid);
        dumpStrings();
	}
	/*if (binssid)
		dumpResults3();*/
	if (btbool){
		str_sorter(btm);
		dumpBT();
	}
	
    if (bsidbool){
    	dumpBSID();
	}
	if (bsmetabool){
		str_sorter(meta);
		dumpBSMeta();
	}
	if (ssidbool){
		str_sorter(ssid);
		dumpSSID();
	}
	if (ipbool){
		str_sorter(ip);
		dumpIP();
	}
	if (simbool){
		str_sorter(sim);
		dumpSIM();
	}
	if (binarybool){
		ip_sorter(bin);
		dumpBIN();
	}
	if (binGrep){
		//ip_sorter(bgrep);
		dumpIPGrep(key);
	}
	return 0;
}