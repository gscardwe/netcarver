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
#define captures 50
ssid_object sid[captures];
int totalCap = 0;   		/*keep track of total structs */


void usage(char *prog)	{
	cout <<	"Usage:  "	<< prog << " [options] -f <file> " << endl;
	cout << "        [-s SSID carving]"	 << endl;
	cout << "        [-b BSSID carving]" << endl;
	exit(-1);
}

void dumpResults() {

    cout << endl << "Results:" << endl;
    cout << "Count\t\tPossible SSID Capture" << endl << "--------------------------------------------" << endl;
    
    for(int n = 0; n<totalCap; n++){
    	cout << sid[n].count ;
    	/*cout << "Pre Gram: \n\t" << sid[n].pregram<< endl;*/
		cout << "\t" << sid[n].ssidname << endl;
		/*cout << "Post Gam: \n\t" << sid[n].postgram << endl;*/
		cout << " "<< endl;
    }
}

int testSSID(unsigned char *data) {

    int maxlen = 50; /* initial arbitrary length */
    int num_good_chars = 0;

    
    if (data[0] == 83 && data[-1] != 66){
        if ( (data[0] == 83) && (data[1] == 83) && (data[2] == 73) && (data[3] == 68) ) {
        	for (int z = 0; z<maxlen; z++){
        		if (data[z] == 44)
        			break;
        		num_good_chars++;
        	}
        	ssid_object *tester = new ssid_object;
        	memcpy(tester->ssidname, data, num_good_chars);
        	string str1 = tester->ssidname;
			
			int newSSID = 1;
			for(int n = 0; n<=totalCap; n++){
				string str2 = sid[n].ssidname;

				if (str1.compare(str2) == 0){
					sid[n].count++;
					newSSID = 0;
					break;
				}
			}
			if (newSSID){
				sid[totalCap].count++;
				memcpy(sid[totalCap].pregram, data-maxlen, maxlen);
				memcpy(sid[totalCap].ssidname, data, num_good_chars);
				memcpy(sid[totalCap].postgram, data+maxlen, maxlen);
				totalCap++;
			}
			tester = NULL;
    	}
    }

    else
    	num_good_chars++;

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
	char *file = NULL;
	bool ssid  = false;
	bool bssid = false;
	int advance   = 0;
	
	while ((ch = getopt(argc, argv, "sbf:")) != EOF) {

		switch ((char) ch) {
		case 's':
			ssid = true;
			break;
		
		case 'b':
			bssid = true;
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
    
    while (bytes_read < (len - sizeof(struct ip))) {
    	if(ssid){
    		advance = testSSID(c);
    		if (advance){
    			c+=(advance-1);
    			bytes_read+=(advance-1);
    		}
    	}
    	c++;
    	bytes_read++;
    }    
    if (ssid)
        dumpResults();
        /*for_each (ssid_object->begin(), ssid_object.end(), dumpResults(&sid));*/
        
	return 0;
}