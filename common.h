/*
 *  common.h
 *  ipcarver
 *
 *  Created by Robert Beverly on 1/12/10.
 *  Copyright 2010 Naval Postgraduate School. All rights reserved.
 *
 */

#include <iostream>
#include <map>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netdb.h>

#include <fcntl.h>
#include <sys/mman.h>

#define BUFSIZE 80
#define PAGESIZE 4096

using namespace std;

typedef map<uint32_t, uint32_t> addr_map_t;
typedef addr_map_t::reverse_iterator addr_map_iter;

#define ETHER_ADDR_LEN      6
#define ETHERTYPE_IP        0x0800  /* IP protocol */
struct ether_addr {
    u_char octet[6];
};

struct comp_ether {
    bool operator()(const struct ether_addr e1, const struct ether_addr e2) const {
        for (int i=0;i<ETHER_ADDR_LEN;i++) {
            if (e1.octet[i] < e2.octet[i]) 
                return true;
            if (e1.octet[i] > e2.octet[i]) 
                return false;
        }
        return false;
    }
};

class Gram {
	public:
	Gram(unsigned char *data_ptr, int _size) : size(_size) {
		val = (u_char *) calloc(_size, 1);
		memcpy(val, data_ptr, _size);
	};
	void print(bool ascii) {
		for (int i=0;i<size;i++) {
			printf("%02x", val[i]);
		}
        if (ascii) {
            printf("\t[");
		    for (int i=0;i<size;i++) {
			    printf("%c", val[i]);
		    }
            printf("]");
        }
		printf("\n");
	}
	u_char *val;
	int size;

	bool operator<(const Gram& rhs) const {
		for (int i=0;i<size;i++) {
			if (val[i] < rhs.val[i])
				return true;
			if (val[i] > rhs.val[i])
				return false;
		}
		return false;
	}
};

typedef map<Gram, uint32_t> gram_histo_t;
typedef map<struct ether_addr, uint32_t, comp_ether> ether_map_t;

struct macip {
    ether_addr ether_dhost;
    ether_addr ether_shost;
    u_short ether_type;
    u_char ipv;
};

struct tcpt_object {
    uint16_t junk1;
    uint16_t pool_size;
    uint32_t sig;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t dst;
    uint32_t src;
    uint16_t dst_port;
    uint16_t src_port;
};

struct addr_object {
    uint32_t sig;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t src;
};

struct ssid_object {
	int count;
	char pregram[50];
	char ssidname[50];
	char postgram[50];
};

struct sid_object {
	char sidname[13];
};

struct bs_object {
	int count;
	char pregram[50];
	uint16_t mnc;
	char postgram[50];
};

template < typename T, typename U >
multimap< U, T > converse_map( const map< T, U >& o )
{
	multimap< U, T > result;
	for ( typename map< T, U >::const_iterator begin( o.begin() );
		 begin != o.end(); ++begin ) {
		result.insert( make_pair( begin->second, begin->first ) );
    }
	return result;
}

template < typename T, typename U, typename V >
multimap< U, T > converse_map( const map< T, U, V >& o )
{
	multimap< U, T > result;
	for ( typename map< T, U >::const_iterator begin( o.begin() );
		 begin != o.end(); ++begin ) {
		result.insert( make_pair( begin->second, begin->first ) );
    }
	return result;
}


//debugging
enum debugLevel {OFF, LOW, HIGH, DEBUG, DEVELOP};
#define debug(level,x...) do {if (verbose >= level) std::cout << x << std::endl; } while (0)

void fatal(const char *fmt, ...);
void pfatal(const char *fmt, ...);
void binDump(unsigned char *msg, int len);
void binDump(unsigned char *msg, int len, int backup);
bool isPowerOfTwo(uint8_t val);
void archSockAddr();
bool suspectIP(struct in_addr *a);
bool sanePort(uint16_t port);
bool testSockAddrIn(void *data, addr_map_t *);
bool testStructIP(void *data, addr_map_t *);
bool testEther(void *data, ether_map_t *);
char *timestamp();
unsigned long file_size(char *);
uint32_t str2int(const string &str);
string int2str(uint32_t n);
bool parsePrefix(char *prefix, struct in_addr *ip, uint32_t *mask);
bool parseEther(char *s, struct ether_addr *);
void printIP(void *data);
void printEther(struct ether_addr *);
unsigned short cksum(struct ip *ip, int len);
