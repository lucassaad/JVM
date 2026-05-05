#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Maximum range of bytes 
# define MAX 4

typedef struct {
    uint8_t tag;
    uint8_t info[];
} cp_info;

typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
} ClassFile;

// fgetc -> usada para ler um unico caractere de um arquivo.
//  Retorna o valor ASCII do caractere lido ou EOF se o final do arquivo for alcancado ou ocorrer um erro
int main() {
    FILE *arquivo;
    const char *file_name = "Main.class";

    unsigned char buffer[MAX];
    size_t lidos;

    arquivo = fopen(file_name, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo1");
        return 1;
    }
    
    printf("Lendo conteúdo do arquivo!\n");

    // ClassFile *cf = malloc(sizeof(ClassFile));


    cf.magic = fread(&magic, 4, 1, )
    lidos = fread(buffer, sizeof(unsigned char), sizeof(buffer), arquivo);

    for (size_t i = 0; i < lidos; i++) {
        printf("%02X ", buffer[i]);
    }

    printf("\n");

    if (fclose(arquivo) != 0) {
        perror("Erro ao fechar o arquivo");
        return 1;
    }

    printf("Leitura concluída com sucesso\n");



    return 0;
    
}