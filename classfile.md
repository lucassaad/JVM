# Arquitetura do Leitor de Bytecode `.class`
 
## 1. Estrutura do ClassFile
 
Um arquivo `.class` é um formato binário especificado pela JVM Specification. Os campos são lidos sequencialmente, na ordem exata definida pela spec:
 
```
ClassFile {
    u4 magic
    u2 minor_version
    u2 major_version
    u2 constant_pool_count
    cp_info constant_pool[constant_pool_count - 1]
    ...
}
```
 
No código, essa estrutura é representada pela struct `ClassFile`:
 
```c
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info **constant_pool;
} ClassFile;
```
 
Cada campo usa tipos de tamanho fixo (`uint32_t`, `uint16_t`) para garantir que a leitura corresponda exatamente ao formato binário independente da plataforma.
 
---
 
## 2. Leitura de Campos Estáticos
 
Os campos de tamanho fixo (`magic`, `minor_version`, `major_version`, `constant_pool_count`) são lidos com `fread` diretamente para os campos da struct:
 
```c
fread(&cf->magic, sizeof(cf->magic), 1, file);
cf->magic = ntohl(cf->magic);
```
 
O padrão seguido para cada campo é:
 
1. **`fread`** — lê os bytes do arquivo para o campo
2. **conversão de endianness** — a JVM usa big-endian; x86 usa little-endian, então é necessário converter:
   - `ntohl()` para valores de 4 bytes (`uint32_t`)
   - `ntohs()` para valores de 2 bytes (`uint16_t`)
   - valores de 1 byte (`uint8_t`) não precisam de conversão
3. **verificação de erro** — `fread` retorna o número de elementos lidos; se diferente de 1, houve erro
---
 
## 3. A Constant Pool
 
### O que é
 
A constant pool é uma tabela indexada (1-based) que armazena todas as constantes usadas pelo arquivo `.class`: nomes de classes, métodos, campos, literais, descritores, etc. O restante do arquivo referencia essas constantes por índice.
 
### `cp_tags` e as structs
 
Cada entrada da constant pool começa com 1 byte: o **tag**, que identifica o tipo da entrada. O tag determina quantos bytes seguintes devem ser lidos e como interpretá-los.
 
Os tags são definidos no enum `cp_tags`:
 
```c
typedef enum cp_tags {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
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
```
 
Para cada tag existe uma struct correspondente que representa os campos daquela entrada. Por exemplo:
 
```c
// tag 7
typedef struct {
    uint8_t tag;
    uint16_t name_index;  // índice para CONSTANT_Utf8_info
} CONSTANT_Class_info;
 
// tag 10
typedef struct {
    uint8_t tag;
    uint16_t class_index;           // índice para CONSTANT_Class_info
    uint16_t name_and_type_index;   // índice para CONSTANT_NameAndType_info
} CONSTANT_Methodref_info;
```
 
A relação entre tag e struct é direta: ao ler o tag, sabe-se qual struct alocar e quais campos ler em seguida.
 
### Referências indiretas
 
A maioria das entradas não guarda valores diretamente — guarda **índices** para outras entradas da mesma tabela. As entradas `CONSTANT_Utf8_info` são as "folhas": elas guardam os bytes reais. Por exemplo, para obter o nome de uma classe:
 
```
CONSTANT_Class_info
  └── name_index → CONSTANT_Utf8_info → bytes: "java/lang/Object"
```
 
A spec impõe restrições de tipo sobre cada índice — por exemplo, o `name_index` de um `CONSTANT_Class_info` deve obrigatoriamente apontar para uma entrada do tipo `CONSTANT_Utf8_info`.
 
### Caso especial: Long e Double
 
Entradas do tipo `CONSTANT_Long_info` (tag 5) e `CONSTANT_Double_info` (tag 6) ocupam **dois índices** na tabela. Após uma dessas entradas, o próximo índice válido é `i + 2`. Isso é tratado no loop de leitura:
 
```c
if (tag == CONSTANT_Long || tag == CONSTANT_Double) i++;
```
 
### Caso especial: Utf8
 
`CONSTANT_Utf8_info` é a única entrada de tamanho variável. O campo `length` indica o número de bytes do array `bytes[]`, que usa o formato **Modified UTF-8** da JVM (não necessariamente um byte por caractere):
 
```c
typedef struct {
    uint8_t tag;
    uint16_t length;
    uint8_t bytes[];   // flexible array member
} CONSTANT_Utf8_info;
```
 
---
 
## 4. `cp_info **constant_pool` no ClassFile
 
O campo `constant_pool` é um **array de ponteiros**:
 
```
constant_pool
     │
     ▼
[ NULL | *entry1 | *entry2 | *entry3 | ... ]
           │         │         │
           ▼         ▼         ▼
     Class_info  Utf8_info  Methodref_info
```
 
- O array é alocado com `malloc(sizeof(cp_info*) * constant_pool_count)`
- Cada posição guarda o endereço de uma entrada alocada individualmente no heap
- O índice 0 não é usado (constant pool é 1-based)
- Como cada entrada tem tipo diferente, os ponteiros são genéricos (`cp_info*` ou `void*`)
Todo ponteiro tem tamanho fixo (8 bytes em sistemas 64-bit), então o array tem tamanho previsível independente do tipo de cada entrada.
 
---
 
## 5. Como as Structs são Salvas no `cp_info`
 
A struct `cp_info` é usada como tipo genérico do array:
 
```c
typedef struct {
    uint8_t tag;
    uint8_t info[];
} cp_info;
```
 
Na prática, cada posição do array armazena um ponteiro para a struct específica do tipo correspondente (ex: `CONSTANT_Class_info*`, `CONSTANT_Utf8_info*`). Isso é possível porque todas as structs têm o `tag` como primeiro campo — um cast para `uint8_t*` sempre acessa o tag corretamente.
 
A função `constant_pool_reader` recebe o tag e o `FILE*`, aloca a struct correta, lê os campos e retorna `void*`:
 
```c
void* constant_pool_reader(cp_tags tag, FILE *file) {
    switch (tag) {
        case CONSTANT_Class: {
            CONSTANT_Class_info *entry = malloc(sizeof(CONSTANT_Class_info));
            entry->tag = tag;
            fread(&entry->name_index, sizeof(entry->name_index), 1, file);
            entry->name_index = ntohs(entry->name_index);
            return entry;
        }
        // ...
    }
}
```
 
O retorno é armazenado diretamente no array:
 
```c
cf->constant_pool[i] = constant_pool_reader(tag, file);
```
 
Para acessar os campos de uma entrada depois, faz-se cast para o tipo correto usando o tag:
 
```c
CONSTANT_Class_info *class = (CONSTANT_Class_info *) cf->constant_pool[i];
```
 
---
 
## Next Steps
 
- Implementar o exibidor da constant pool (`print_cp_entry`) com resolução de referências entre entradas
- Continuar o parsing do ClassFile após a constant pool: `access_flags`, `this_class`, `super_class`, `interfaces`
- Implementar leitura de `fields` e `methods`
- Implementar leitura de atributos (`attributes`), em especial o atributo `Code` que contém o bytecode dos métodos
- Implementar a exibição do bytecode instrução a instrução (similar ao `javap -c`)
- Implementar liberação de memória (`free`) para todas as entradas da constant pool
- Adicionar validação de referências conforme as restrições da spec (ex: verificar que `name_index` aponta para `CONSTANT_Utf8_info`)