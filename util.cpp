/****************************************************************************
   Program:     $Id: util.c 617 2009-05-08 17:57:37Z rbeverly $
   Author:      Rob Beverly <rbeverly at csail.mit.edu>
   Date:        $Date: 2009-05-08 10:57:37 -0700 (Fri, 08 May 2009) $
   Description: Utility routines
****************************************************************************/
#include "common.h"
#include <stdarg.h>

#include <sstream>
struct dnshdr {
    uint16_t id;
    uint16_t flags;
    uint16_t num;
    uint16_t ansRR;
    uint16_t authRR;
    uint16_t addRR;
    char *quest;
};

/**
 * Fatal error condition function 
 */
void fatal(const char *fmt, ...) {
    va_list ap;
    fflush(stdout);
    va_start(ap, fmt);
    fprintf(stderr, "*** ");
    vfprintf(stderr, fmt, ap);
    va_end(ap);
#ifdef _WIN32
    winpause("Press Enter to Exit.");
#endif
    exit(-1);
}

/**
 * Fatal error condition function; prints error
 */
void pfatal(const char *fmt, ...) {
    va_list ap;
    fflush(stdout);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, " ");
    perror("Error");
    va_end(ap);
    exit(-1);
}

void binDump(unsigned char *msg, int len) {
    binDump(msg, len, 0);
}

/**
 * binDump -  Debug routine for examining raw packets/buffer/char strings 
 *
 * @param[in]   msg     pointer to message buffer
 * @param[in]   len     length of buffer to print
 */
void binDump(unsigned char *msg, int len, int backup) {
    msg-=backup;
    len+=backup;
    unsigned char *msg2 = msg;
    uint32_t word;
    for (int i=1;i<=len;i++) {
        if (len >= 12) {
            /* print starting byte relative to backup offset */
            if ((i-1)%4 == 0) {
                printf("%03d: ", i-1-backup);
            }
            printf("%02x ", *msg++);
            if (i%4 == 0) {
                printf("\t");
                struct in_addr in;
                //in.s_addr = (uint32_t) *msg2;
                memcpy(&word, msg2, 4);
                in.s_addr = word;
                msg2+=4;
                printf("%s\t\t", inet_ntoa(in));
                in.s_addr = htonl(word);
                printf("%s\n", inet_ntoa(in));
            }
        } else {
            printf("%02x", *msg++);
        }
    }
    printf("\n");
}

/**
 * timestamp - form time/date string
 */
char *timestamp() {
   static char str[BUFSIZE];
   time_t clock;
   struct tm *t;

   clock = time(NULL);
   t = localtime(&clock);
   snprintf(str, sizeof(str), "%02d/%02d %02d:%02d:%02d", t->tm_mon + 1,
      t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
   return (str);
}

unsigned long file_size(char *file) {
	FILE *fp = NULL;
	long len = 0;
	if ((fp = fopen(file, "rb")) != NULL) {
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fclose(fp);
	}
	return len;
}

/* Process a string of the form "00:00:00:00:00:00" and return result in a struct ether_addr */
bool parseEther(char *s, struct ether_addr *eth) {
    unsigned int i;
    unsigned int separators = 0;
    char *end = NULL;

    for (i=0;i<strlen(s);i++) {
        if ((s[i] == ':') || (s[i] == '-') ) {
            s[i] = ' ';
            separators++;
        }
    }
    if (separators != 5)
        return false;

    end = s;
    for (i=0;i<=separators;i++) {
        eth->octet[i] = (unsigned char) strtol(end, &end, 16);
        if (end == NULL)
            return false;
        end++;
    }
    return true;
}

/* Process a string of the form "network/mask" and return result */
bool parsePrefix(char *prefix, struct in_addr *ip, uint32_t *mask) {
    unsigned int i;
    int bitmask;

    for (i=0;i<strlen(prefix);i++) {
        if (prefix[i] == '/') {
            prefix[i] = ' ';
            break;
        }
    }
    if (inet_aton(prefix, ip) <= 0) {
        cout << "Bad IP address." << endl;
        return false;
    }
    bitmask = strtol(&prefix[i+1], NULL, 10);
    if ( (bitmask < 1) || (bitmask > 32) ) {
        cout << "Bad prefix mask." << endl;
        return false;
    }
    *mask = htonl(0xffffffff << (32-bitmask));
    ip->s_addr = ip->s_addr & *mask;
    return true;
}

/* determine if an integer is a power of two; used for the TTL */
bool isPowerOfTwo(uint8_t val) {
    /* 2^8 = 256 possible bit permutations in a byte */
    static const int permutationsInByte = 1 << 8; 
    static int numBits[permutationsInByte];
    static bool initialized = false;

    if (!initialized) {
        /*
        for (int i=0;i<permutationsInByte;i++) 
            numBits[i] = 0;
        for (int i=0;i<permutationsInByte;i++) {
            for (int j=0;j<8;j++) {
                if ( i & 1<<j )
                    numBits[i]++;
            }
        }
        */
        /* More efficient and correct to just set manually */
        memset(numBits, 0, permutationsInByte);
        numBits[32] = 1;
        numBits[64] = 1;
        numBits[128] = 1;
        numBits[255] = 1;
        initialized = true;
    }

    if (numBits[val] == 1)
        return true;
    return false;
}

/* Look for, and print, a struct ip */
void printIP(void *data) {
    char *d = (char *) data;
    struct ip *ip = (struct ip *) d;
    struct tcphdr *tcp = (struct tcphdr *) (d + 20);
    struct udphdr *udp = (struct udphdr *) (d + 20);

    if ( (ip->ip_hl != 5) && (ip->ip_v != 4) ) {
        /* attempt to re-offset */
        d-=4;
        ip = (struct ip *) d;
        tcp = (struct tcphdr *) (d + 20);
        udp = (struct udphdr *) (d + 20);
        if ( (ip->ip_hl != 5) && (ip->ip_v != 4) )
            cout << __func__ << ": does not appear to be an IP packet (even after re-offset)." << endl;
    } 
    printf("%s -> ",  inet_ntoa(ip->ip_src));
    printf("%s ",  inet_ntoa(ip->ip_dst));
    printf("Len: %d ", ntohs(ip->ip_len));
    printf("ToS: %d ", ip->ip_tos);
    printf("ID: %d ", ntohs(ip->ip_id));
    printf("Chksum: %04x ", ntohs(ip->ip_sum));
    if (ip->ip_sum != cksum(ip, ip->ip_hl * 4))
        printf("[! %04x] ", cksum(ip, ip->ip_hl * 4));
    else
        printf("[good] ");
    printf("Flags:[");
    if (ntohs(ip->ip_off) & IP_DF)
        printf("DF");
    if (ntohs(ip->ip_off) & IP_RF)
        printf("RF");
    if (ntohs(ip->ip_off) & IP_MF)
        printf("MF");
    printf("] ");
    printf("Off: %04x ", ntohs(ip->ip_off) & IP_OFFMASK); 
    printf("TTL: %d ", ip->ip_ttl);
    if (isPowerOfTwo(ip->ip_ttl))
        printf("[PoT] ");
    printf("Proto: %d ", ip->ip_p);
    if (ip->ip_p == IPPROTO_TCP) {
        printf("[%d -> %d] ", ntohs(tcp->th_sport), ntohs(tcp->th_dport));
        printf("SEQ: %u ACK: %u ", ntohl(tcp->th_seq), ntohl(tcp->th_ack));
    }
    if (ip->ip_p == IPPROTO_UDP) {
        printf("[%d -> %d] ", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
        // now assume a DNS
        struct dnshdr *dns = (struct dnshdr *) (d+28);
        printf("DNS Id: %d\n", ntohs(dns->id));
        printf("DNS Flags: %d\n", ntohs(dns->flags));
        printf("DNS Num Quest: %d\n", ntohs(dns->num));
        printf("DNS ansRR: %d\n", ntohs(dns->ansRR));
        printf("DNS authRR: %d\n", ntohs(dns->authRR));
        printf("DNS addRR: %d\n", ntohs(dns->addRR));
        char *quest = d+28+(6*2);
        printf("Question: ");
        for (int j=0;j<50;j++) {
            printf("%c", *quest);
            quest++;
        }
        printf("\n");
    }
    printf("\n");
}

void printEther(struct ether_addr *e) {
    for (int i=0;i<ETHER_ADDR_LEN;i++) {
        if (i>0) printf(":");
        printf("%02X", e->octet[i]);
    }
}

/* compute an Internet-style checksum, from Stevens */
unsigned short cksum(struct ip *ip, int len) {
    long sum = 0;  /* assume 32 bit long, 16 bit short */
    unsigned short *ipp = (unsigned short *) ip;
    int octets_processed = 0;

    while (len > 1) {
        if (octets_processed != 10) {
            sum += *ipp;
            if(sum & 0x80000000)   /* if high order bit set, fold */
                sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ipp++;
        len -= 2;
        octets_processed+=2;
    }

    if (len)       /* take care of left over byte */
        sum += *ipp;

    while(sum>>16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}
