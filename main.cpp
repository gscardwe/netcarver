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
#define captures 800
ssid_object sid[captures];
bs_object cid[captures];
int totalCap = 0;   		/*keep track of total structs */
int nGram = 1;

void usage(char *prog)	{
	cout <<	"Usage:  "	<< prog << " [options] -f <file> " << endl;
	cout << "        [-s 'input' ASCII string search ]"	<< endl;
	cout << "        [-b Search binary SSID's]" << endl;
	cout << "        [-d Search binary CID's ]" << endl;
	exit(-1);
}

void dumpResults() {

    cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << sid[n].count ;
		cout << "\t" << sid[n].ssidname ;
		cout << "\t" << sid[n].pregram  ;
		cout << "\t" << sid[n].postgram << endl;
    }
    cout << " " << endl;
}

void dumpResults2() {

    cout << endl << "Results:" << endl;
    cout << "Count\tInput\tPre\tPost" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << cid[n].count ;
		cout << "\t" << cid[n].mnc ;
		//cout << "\t" << cid[n].pregram  ;
		printf("\t %s ", cid[n].pregram); 
		//cout << "\t" << cid[n].postgram << endl;
		cout << endl;
    }
    cout << " " << endl;
}

int binDump(unsigned char *data) {
	struct sid_object *base = (struct sid_object *) data;
	int num_good_chars = 0;
	//int maxlen = 13;  /* length of wifi base */
	char test  = 0x46;
	char test1 = 0x72;
	char test2 = 0x65;
	char test3 = 0x61;
	char test4 = 0x6B;
	char test5 = 0x79;
	if (base->sidname[0] == test) {
		num_good_chars++;
		if (base->sidname[1] == test1) {
			num_good_chars++;
			if (base->sidname[2] == test2) {
				num_good_chars++;
				if (base->sidname[3] == test3) {
					num_good_chars++;
					if (base->sidname[4] == test4) {
						num_good_chars++;
						if (base->sidname[5] == test5) {
						
       						num_good_chars = 13;
							cout << base->sidname << endl;
		//memcpy(base->sidname, base, maxlen);
						/*cout << base->sidname[0] << base->sidname[1] << base->sidname[2] << base->sidname[3] << base->sidname[4] << base->sidname[5] <<endl;*/
						}
					}
				}
			}
		}
	}
		
	else
		num_good_chars = 1;
	return num_good_chars;

}

int testbcid(void *data){
	int num_good_chars = 0;
	uint16_t *bcid = (uint16_t *) data;
	unsigned char *contain = (unsigned char *) data;

	if (bcid[0] == 4183){
	
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
			memcpy(cid[totalCap].pregram, contain-nGram, nGram);
			cid[totalCap].mnc = bcid[0];
			memcpy(cid[totalCap].postgram, contain+1, nGram);
			//cout << "CID: " << cid[totalCap].mnc << endl;
			totalCap++;
		}
		
		tester = NULL; 
	}
	
	return num_good_chars;
}

int retriever(unsigned char *data, char *key) {

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
    	ssid_object *tester = new ssid_object;
        memcpy(tester->pregram, data-nGram, nGram);
    	string str1 = tester->pregram;
    	int newSSID = 1;
    	
    	for(int n = 0; n<=totalCap; n++){
			string str2 = sid[n].pregram;
			
			if (str1.compare(str2) == 0){
				sid[n].count++;
				newSSID = 0;
				break;
			}
		}
    	if (newSSID){
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
	bool ssid    = false;
	bool binssid = false;
	bool bcid    = false;
	int advance  = 0;
	
	while ((ch = getopt(argc, argv, "s:bdf:")) != EOF) {

		switch ((char) ch) {
		case 's':
			ssid = true;
			key  = optarg;
			break;
		
		case 'b':
			binssid = true;
			break;
		
		case 'd':
			bcid = true;
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

    cerr << "Doing Stuff..." << endl;
    
    while (bytes_read < len-1) {
    	if(ssid){
    		advance = retriever(c, key);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
        else if(binssid){
    		advance = binDump(c);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
    	
    	else if(bcid){
    		advance = testbcid(c);
    		if(advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
    	
    	c++;
    	bytes_read++;
    	if (bytes_read % 10000000 == 0) cerr << ".";
    }    
    if (ssid)
        dumpResults();
        
    if (bcid)
    	dumpResults2();

	return 0;
}