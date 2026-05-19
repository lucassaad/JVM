#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "utils.h"


int read_classfile(ClassFile *cf, FILE *file) {
    // Read 'magic'
    if (fread(&cf->magic, sizeof(cf->magic), 1, file) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = byteswap_u4(cf->magic);

    
    // Read 'minor_version'
    if (fread(&cf->minor_version, sizeof(cf->minor_version), 1, file) != 1) {
        perror("Erro ao ler 'minor_version'");
        return 1;
    }
    cf->minor_version = byteswap_u2(cf->minor_version);


    // Read 'major_version'
    if (fread(&cf->major_version, sizeof(cf->major_version), 1, file) != 1) {
        perror("Erro ao ler 'major_version'");
        return 1;
    }
    cf->major_version = byteswap_u2(cf->major_version);


    // CONSTANT_POOL_COUNT
    if (fread(&cf->constant_pool_count, sizeof(cf->constant_pool_count), 1, file) != 1) {
        perror("Erro ao ler 'constant_pool_count'");
        return 1;
    }
    cf->constant_pool_count = byteswap_u2(cf->constant_pool_count);
    printf("constant_pool_count: 0x%04X\n", cf->constant_pool_count);
    
    cf->constant_pool = malloc(sizeof(cp_info*) * cf->constant_pool_count);
    for (int i = 1; i < cf->constant_pool_count; i++) {
        uint8_t tag_byte;
        if (fread(&tag_byte, sizeof(tag_byte), 1, file) != 1) {
            perror("Erro na leitura da tag");
            return 1;
        }
        cp_tags tag = (cp_tags) tag_byte;

        void *entry = constant_pool_reader(tag, file);
        if (entry == NULL){
            perror("Error while reading constant_pool");
            return 1;
        }

        cf->constant_pool[i] = entry;

        if (tag == CONSTANT_Long || tag == CONSTANT_Double) i++; 
    }


    // Read 'this_class'
    if (fread(&cf->this_class, sizeof(cf->this_class), 1, file) != 1) {
        perror("Erro ao ler 'this_class'");
        return 1;
    }
    cf->this_class = byteswap_u2(cf->this_class);
    
    
    // Read 'super_class'
    if (fread(&cf->super_class, sizeof(cf->super_class), 1, file) != 1) {
        perror("Erro ao ler 'super_class'");
        return 1;
    }
    cf->super_class = byteswap_u2(cf->super_class);

    return 0;
}