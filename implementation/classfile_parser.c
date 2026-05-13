#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>


typedef enum cp_tags {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Flaot = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldred = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12, 
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_Dynamic = 17,
    CONSTANT_InvokeDynamic = 18,
    CONSTANT_Module = 19,
    CONSTANT_Package = 20
} cp_tags;


// CONSTANT_Class_info
// CONSTANT_Fieldref_info
// CONSTANT_Methodref_info
// CONSTANT_InterfaceMethodref_info
// CONSTANT_String_info
// CONSTANT_Integer_info
// CONSTANT_Float_info
// CONSTANT_Long_info
// CONSTANT_Double_info
// CONSTANT_NameAndType_info
// CONSTANT_Utf8_info

// CONSTANT_MethodHandle_info
// CONSTANT_MethodType_info
// CONSTANT_Dynamic_info
// CONSTANT_InvokeDynamic_info
// CONSTANT_Module_info
// CONSTANT_Package_info

// Constant Pool entry format
typedef struct {
    uint8_t tag;
    uint8_t info[];
} cp_info;


// ClassFile structure 
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info **constant_pool;
} ClassFile;


int main() {
    FILE *arquivo;
    const char *file_name = "Main.class";

    arquivo = fopen(file_name, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo1");
        return 1;
    }
    
    ClassFile *cf = malloc(sizeof(ClassFile));
    if (cf == NULL){
        perror("Erro ao alocar ClassFile");
        return 1;
    }

    // MAGIC
    if (fread(&cf->magic, sizeof(cf->magic), 1, arquivo) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = ntohl(cf->magic);
    printf("magic: 0x%08X\n", cf->magic);

    // MINOR_VERSION
    if (fread(&cf->minor_version, sizeof(cf->minor_version), 1, arquivo) != 1) {
        perror("Erro ao ler 'minor_version'");
        return 1;
    }
    cf->minor_version = ntohs(cf->minor_version);
    printf("minor_version: 0x%04X\n", cf->minor_version);

    // MAJOR_VERSION
    if (fread(&cf->major_version, sizeof(cf->major_version), 1, arquivo) != 1) {
        perror("Erro ao ler 'major_version'");
        return 1;
    }
    cf->major_version = ntohs(cf->major_version);
    printf("major_version: 0x%04X\n", cf->major_version);

    // CONSTANT_POOL_COUNT
    if (fread(&cf->constant_pool_count, sizeof(cf->constant_pool_count), 1, arquivo) != 1) {
        perror("Erro ao ler 'constant_pool_count'");
        return 1;
    }
    cf->constant_pool_count = ntohs(cf->constant_pool_count);
    printf("constant_pool_count: 0x%04X\n", cf->constant_pool_count);
    
    
    for (int i = 0; i < constant_pool_count; i++) {
        uint8_t tag;
        if (fread(&tag, sizeof(tag), 1, arquivo) != 1) {
            perror("Erro na leitura da tag");
            return 1
        }
        // mapear tag para a informação
        // alocar memoria para o tipo da informacao 
        // mapear a entrada para a constant_pool
    }

    free(cf);

    if (fclose(arquivo) != 0) {
        perror("Erro ao fechar o arquivo");
        return 1;
    }


    return 0;
}