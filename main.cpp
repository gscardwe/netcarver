/*******************************************************************************
	Program:		main.cpp
	Author:			Greg Cardwell <gscardwe@nps.edu>
					Rob Beverly   <rbeverly@nps.edu>
	Description:	BSSID and WiFi SSID carver
	
*******************************************************************************/
#include "common.h"
#include <string>
enum debugLevel verbose = OFF;

/* Array for data */
#define captures 1000
str_object sid[captures];
//sid_object stump[captures];
bs_object cid[captures]; //for binary grep searches
str_object meta[captures]; //for base station format searches
str_object btm[captures];  //bluetooth data structures
int totalCap = 0;   		/*keep track of total structs */
int nGram = 1;

void usage(char *prog)	{
	cout <<	"Usage:  "	<< prog << " [options] -f <file> " << endl;
	cout << "        [-b Search for bluetooth data structure ]" << endl;
	cout << "        [-s 'input' ASCII string search ]"	<< endl;
	//cout << "        [-b 'input' Search binary SSID's]" << endl;
	cout << "        [-d 'Cell ID#' Grep Search binary CID's ]" << endl;
	cout << "        [-t 'Search for Base Station Metadata   ]" << endl;
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
		//cout << "\t" << cid[n].pregram  ;
		printf("\t %s ", cid[n].pregram); 
		//cout << "\t" << cid[n].postgram << endl;
		printf("\t %s ", cid[n].postgram);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpBSMeta(){
	cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << meta[n].count ;
		cout << "\t" << meta[n].ssidname ;
		printf("\t %s ", meta[n].pregram); 
		printf("\t %s ", meta[n].postgram);
		cout << endl;
    }
    cout << " " << endl;
}

void dumpBT(){
	cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << btm[n].count ;
		cout << "\t" << btm[n].ssidname ;
		printf("\t %s ", btm[n].pregram); 
		printf("\t %s ", btm[n].postgram);
		cout << endl;
    }
    cout << " " << endl;
}

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
		string str1 = tester->pregram;
		int newSSID = 1;

		for(int n = 0; n<=totalCap; n++){
			string str2 = cid[n].pregram;
	
			if (str1.compare(str2) == 0){
				cid[n].count++;
				newSSID = 0;
				break;
			}
		}

		if (newSSID){
			cid[totalCap].count++;
			memcpy(cid[totalCap].pregram, bcid-nGram, nGram);
			cid[totalCap].mnc = key;
			//cid[totalCap].postgram[0] = bcid[3];
			//int m = 0;
			/*while (m < nGram){
				cid[totalCap].postgram[m] = bcid[m+length];
				m++;
			}*/
			memcpy(cid[totalCap].postgram, bcid+length, nGram);
			//cout << "CID: " << cid[totalCap].mnc << endl;
			totalCap++;
		}

		tester = NULL;
	}
	
	return num_good_chars;
}

int strRetriever(unsigned char *data, char *key) {

    unsigned int maxlen = strlen(key); /* length of known wifi base */
    int num_good_chars = 0;
    bool comp = true;
    
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
    	string str1 = tester->pregram;
    	int newStr = 1;
    	
    	for(int n = 0; n<=totalCap; n++){
			string str2 = sid[n].pregram;
			
			if (str1.compare(str2) == 0){
				sid[n].count++;
				newStr = 0;
				break;
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

int btMeta(char inputSep, void *data){
	int num_good_chars = 0;
	int match = 0;		//bool value for a match
	int maxField = 2;	//max # of digits in fields
	int colon = 0;      //bs metadata should have 4 fields,3colons. counts colons.
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;

	for (int j =0; j<6; j++){	//6 fields of 2 alphanumeric representations
		int i = 0;
		while (i < maxField){
			if(isalnum(bcid[num_good_chars])){
				num_good_chars++;
			}
			else {
				match = 0;
				break;
			}
			i++;
		}
		
		if (colon == 5 && i > 0){
			match = 1;
		}
		else if (bcid[num_good_chars] == inputSep && num_good_chars){
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
		string str1 = tester->pregram;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str2 = btm[n].pregram;
			
			if (str1.compare(str2) == 0){
				btm[n].count++;
				newSSID = 0;
				break;
			}
		}
		
		if (newSSID){
			meta[totalCap].count++;
			memcpy(btm[totalCap].pregram, bcid-nGram, nGram);
			memcpy(btm[totalCap].ssidname, bcid, num_good_chars);
			memcpy(btm[totalCap].postgram, bcid+num_good_chars, nGram);
			totalCap++;
		}
		
		tester = NULL;
	}	return num_good_chars;
}

int bsMetaData(char inputSep, void *data){
	int num_good_chars = 0;
	int match = 0;		//bool value for a match
	int maxField = 4;	//max # of digits in fields
	int colon = 0;      //bs metadata should have 4 fields,3colons. counts colons.
	char *bcid = (char *) data;
	unsigned char *contain = (unsigned char *) data;
	
	for (int j =0; j<4; j++){	//4 is the number of fields:cid/lac/mnc/mcc
		int i = 0;
		while (i < maxField){
			if(isdigit(bcid[num_good_chars])){
				num_good_chars++;
			}
			else {
				match = 0;
				break;
			}
			i++;
		}
		if (colon == 3 && i > 0){
			match = 1;
		}
		else if (bcid[num_good_chars] == inputSep && num_good_chars){
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
		string str1 = tester->pregram;
		int newSSID = 1;
		
		for(int n = 0; n<=totalCap; n++){
			string str2 = meta[n].pregram;
			
			if (str1.compare(str2) == 0){
				meta[n].count++;
				newSSID = 0;
				break;
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
	bool str     = false;  //String search
	bool bt      = false;  //bluetooth search
	//bool binssid = false;
	bool bsid    = false;  //Grep binary search
	bool bsmeta  = false;  //Search for bs metadata format xxx:xxx:xx:xx
	int advance  = 0;
	char input;			   //big or little endian
	char inputSep;		   //to indicate a : or - seperator
	
	while ((ch = getopt(argc, argv, "s:d:btf:")) != EOF) {
		//cout << "greg" << endl;
		switch ((char) ch) {
				
		case 'b':
			bt = true;
			break;
				
		case 's':
			str = true;
			key  = optarg;
			break;
		
		/*case 'b':
			binssid = true;
			key  = optarg;
			break;*/
		
		case 'd':
			bsid = true;
			key = optarg;
			break;
				
		case 't':
			bsmeta = true;
			break;
			
		case 'f':
			file = optarg;
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
	
	if(bsid){
		cout << "\nPlease indicate Little Endian (l) or Big Endian (b): ";
		cin >> input;
	}
	
	if (bsmeta || bt){
		cout << "\nPlease indicate (:) or (-) seperators: ";
		cin >> inputSep;
	}
 
    cerr << "\nPlease wait, currently searching: \n";
    if (key){cerr << key << "\n" << endl;}
	
	
	int length = 1;
	if (key){length = strlen(key);}

    while (bytes_read <= len-length) {
		//cout << "greg" << endl;
    	if(str){
    		advance = strRetriever(c, key);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
		
		else if(bt){
			//cout << "greg" << endl;
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
    	
		else if(bsid){
    		advance = testbsid(input, key, c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
		else if (bsmeta){
			//cout << "greg" << endl;
			//cout << 'greg'<<endl;
			advance = bsMetaData(inputSep, c);
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
	
    if (str)
        dumpStrings();
        
	/*if (binssid)
		dumpResults3();*/
	if (bt)
		dumpBT();
    if (bsid)
    	dumpBSID();
	if (bsmeta)
		dumpBSMeta();
	return 0;
}