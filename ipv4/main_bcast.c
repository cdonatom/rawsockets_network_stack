#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IPv4_ADDR_SIZE 4
#define IPv4_ADDR_STR_LENGTH 16

typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];

void ipv4_addr_str ( ipv4_addr_t addr, char* str )
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d",
            addr[0], addr[1], addr[2], addr[3]);
  }
}

int ipv4_str_addr ( char* str, ipv4_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[IPv4_ADDR_SIZE];
    int len = sscanf(str, "%d.%d.%d.%d", 
                     &addr_int[0], &addr_int[1], 
                     &addr_int[2], &addr_int[3]);

    if (len == IPv4_ADDR_SIZE) {
      int i;
      for (i=0; i<IPv4_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }
      
      err = 0;
    }
  }
  
  return err;
}


void calculate_ipv4_broadcast (ipv4_addr_t addr, ipv4_addr_t subnet, ipv4_addr_t* res_bcast){
	if (addr == NULL || subnet == NULL)
		return;
	int i;
	unsigned char byte[IPv4_ADDR_SIZE];

	for(i=0; i < IPv4_ADDR_SIZE; i++){
		byte[i] = ~(subnet[i]);
		byte[i] |= addr[i];
	}
	
	memcpy( res_bcast, &byte, IPv4_ADDR_SIZE);
	
}

int main ( int argc, char * argv[] ){
	if (argc != 3) {
		printf("Uso: %s <ipv4> <subred> \n", argv[0]); 
		exit(-1);
	}
	ipv4_addr_t addr;
	ipv4_addr_t subnet;
	ipv4_addr_t res;
	char* ipv4_str = argv[1];
	char* subnet_str = argv[2];
	char res_str[IPv4_ADDR_STR_LENGTH];
	
	ipv4_str_addr(ipv4_str, addr);
	ipv4_str_addr(subnet_str, subnet);
	
	calculate_ipv4_broadcast ( addr, subnet, &res);
	
	ipv4_addr_str(res, res_str);
	printf("%s\n", res_str);
	return 0;
}