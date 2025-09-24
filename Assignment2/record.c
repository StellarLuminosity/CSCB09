#include <stdio.h>
#include <string.h>
#include "record.h"

void set_sunspots(FILE *f, const char *name, unsigned short sunspots) {
    
    if (name == NULL || f == NULL) {return;}

    if (fseek(f, 0, SEEK_SET) != 0) {return;}

    record customer;

    while(fread(&customer, sizeof(record), 1, f) == 1) {
        if (strncmp(customer.name, name, strlen(name)) == 0) {
            customer.sunspots = sunspots;
            
            if (fseek(f, -sizeof(record), SEEK_CUR) != 0) {return;}
            if (fwrite(&customer, sizeof(record), 1, f) != 1) {return;}

            return;
        }   
    }

    if (ferror(f)) {return;}

    record new_customer;
        
    new_customer.sunspots = sunspots;
    new_customer.name_len = (unsigned int)strnlen(name, NAME_LEN_MAX);
    strncpy(new_customer.name, name, new_customer.name_len);

    // if (fseek(f, 0, SEEK_END) != 0) {return;}
    if (fwrite(&new_customer, sizeof(record), 1, f) != 1) {return;}

    return;
}

int get_sunspots(FILE *f, const char *name, unsigned short *psunspots) {

    if (name == NULL || psunspots == NULL || f == NULL) {return 0;}

    record customer;

    if (fseek(f, 0, SEEK_SET) != 0) {return 0;}

    while (fread(&customer, sizeof(record), 1, f) == 1) {
        if (strncmp(customer.name, name, strlen(name)) == 0) {
            *psunspots = customer.sunspots;

            return 1;
        }
    }

    return 0;
}



