# Documentacao do Leitor/Exibidor de Arquivos `.class`

Esta documentacao explica a implementacao presente em `implementation/` e relaciona as estruturas do projeto com a especificacao do formato `ClassFile` da JVM, conforme o material em `pdfs/classfile-85-117.pdf` e a JVM Specification, capitulo 4, "The class File Format".

## Objetivo do Projeto

O projeto implementa, em C, um leitor/exibidor de arquivos `.class`. Ele abre um arquivo binario Java, interpreta seus campos principais na ordem definida pela JVM, armazena essas informacoes em estruturas C e imprime uma visao legivel da classe, incluindo:

- informacoes gerais do `ClassFile`;
- entradas da constant pool;
- fields e seus atributos;
- methods e seus atributos;
- atributos da classe;
- atributo `Code` dos metodos;
- bytecodes reconhecidos, com seus opcodes convertidos para nomes de instrucoes.

O fluxo principal esta em `src/main.c`: abrir arquivo, alocar `ClassFile`, chamar `read_classfile`, validar referencias da constant pool, imprimir as secoes lidas e liberar memoria.

## Base da Especificacao

A especificacao define que os itens de um arquivo `.class` sao armazenados sequencialmente, sem padding ou alinhamento. Por isso, o leitor deve respeitar exatamente a ordem dos campos:

```c
ClassFile {
    u4             magic;
    u2             minor_version;
    u2             major_version;
    u2             constant_pool_count;
    cp_info        constant_pool[constant_pool_count-1];
    u2             access_flags;
    u2             this_class;
    u2             super_class;
    u2             interfaces_count;
    u2             interfaces[interfaces_count];
    u2             fields_count;
    field_info     fields[fields_count];
    u2             methods_count;
    method_info    methods[methods_count];
    u2             attributes_count;
    attribute_info attributes[attributes_count];
}
```

Na implementacao, essa estrutura e representada em `include/class_file.h` pela struct `ClassFile`, com arrays dinamicos para constant pool, interfaces, fields, methods e attributes.

## Tipos `u1`, `u2` e `u4`

A especificacao usa os tipos:

- `u1`: inteiro sem sinal de 1 byte;
- `u2`: inteiro sem sinal de 2 bytes;
- `u4`: inteiro sem sinal de 4 bytes.

No projeto, eles sao representados pelos tipos fixos de `stdint.h`:

- `uint8_t`;
- `uint16_t`;
- `uint32_t`.

Essa escolha e importante porque o formato `.class` depende do tamanho exato de cada campo.

## Endianness

Arquivos `.class` armazenam valores multibyte em big-endian. Em maquinas comuns, como x86/x86-64, os inteiros normalmente sao little-endian. Por isso, apos cada `fread` de `u2` ou `u4`, o codigo converte a ordem dos bytes usando as funcoes de `utils.h`, como:

- `byteswap_u2` para campos de 2 bytes;
- `byteswap_u4` para campos de 4 bytes.

Exemplo de leitura em `src/reader.c`:

```c
fread(&cf->major_version, sizeof(cf->major_version), 1, file);
cf->major_version = byteswap_u2(cf->major_version);
```

Campos `u1`, como tags da constant pool e opcodes, nao precisam de conversao.

## Estrutura Geral de Leitura

A funcao central de leitura e `read_classfile`, em `src/reader.c`. Ela segue a ordem da especificacao:

1. le `magic`;
2. le `minor_version` e `major_version`;
3. le `constant_pool_count`;
4. aloca e le a constant pool;
5. le `access_flags`, `this_class` e `super_class`;
6. le interfaces;
7. le fields;
8. le methods;
9. le atributos globais da classe.

Durante esse processo, a implementacao tambem executa algumas validacoes estruturais, como verificar se `this_class`, `super_class`, interfaces, fields, methods e attributes apontam para entradas validas da constant pool.

## Magic Number

O campo `magic` identifica um arquivo `.class`. O valor esperado e:

```text
0xCAFEBABE
```

Ele e lido como `uint32_t` e convertido de big-endian para a ordem nativa da maquina. A exibicao do valor ocorre em `print_general_information`, em `src/viewer.c`.

## Versoes Minor e Major

Os campos `minor_version` e `major_version` indicam a versao do formato do arquivo `.class`. O projeto imprime a versao major e traduz alguns valores para versoes Java usando `get_java_version_string`, em `src/viewer.c`.

Exemplos:

- major `52`: Java 8;
- major `61`: Java 17;
- major `69`: Java 25.

Na especificacao Java SE 26, os major versions validos vao de 45 ate 70. A implementacao atual reconhece ate 70 na funcao de exibicao.

## Constant Pool

A constant pool e uma tabela indexada a partir de 1. O indice 0 nao e usado. Ela guarda nomes, descritores, literais e referencias simbolicas utilizadas pelo restante do arquivo `.class`.

No projeto:

- `constant_pool_count` define a quantidade de posicoes;
- `cf->constant_pool` e um array de ponteiros;
- cada posicao contem um `cp_info`;
- `cp_info` guarda a `tag` e um ponteiro `info` para a estrutura especifica da entrada.

Em `include/class_file.h`:

```c
typedef struct {
    uint8_t tag;
    void *info;
} cp_info;
```

Em `src/reader.c`, cada tag e lida como `uint8_t`, convertida para `cp_tags` e enviada para `constant_pool_reader`.

## Tags da Constant Pool

As tags implementadas estao em `include/constant_pool.h`:

| Tag | Nome |
| --- | --- |
| 1 | `CONSTANT_Utf8` |
| 3 | `CONSTANT_Integer` |
| 4 | `CONSTANT_Float` |
| 5 | `CONSTANT_Long` |
| 6 | `CONSTANT_Double` |
| 7 | `CONSTANT_Class` |
| 8 | `CONSTANT_String` |
| 9 | `CONSTANT_Fieldref` |
| 10 | `CONSTANT_Methodref` |
| 11 | `CONSTANT_InterfaceMethodref` |
| 12 | `CONSTANT_NameAndType` |
| 15 | `CONSTANT_MethodHandle` |
| 16 | `CONSTANT_MethodType` |
| 17 | `CONSTANT_Dynamic` |
| 18 | `CONSTANT_InvokeDynamic` |
| 19 | `CONSTANT_Module` |
| 20 | `CONSTANT_Package` |

Cada tag determina quais bytes seguintes devem ser lidos. Essa logica esta no `switch` de `constant_pool_reader`, em `src/constant_pool.c`.

## Entradas Especiais: `Long` e `Double`

Pela especificacao, `CONSTANT_Long_info` e `CONSTANT_Double_info` ocupam duas posicoes na constant pool. A segunda posicao nao contem uma entrada independente.

O projeto trata isso no loop de leitura:

```c
if (tag == CONSTANT_Long || tag == CONSTANT_Double) i++;
```

A exibicao em `print_constant_pool` tambem incrementa o indice e imprime a segunda posicao como continuacao de numero grande.

## Entrada `CONSTANT_Utf8`

`CONSTANT_Utf8_info` e uma entrada de tamanho variavel. Primeiro vem o campo `length`; depois, um array de bytes com esse tamanho.

Na implementacao:

```c
typedef struct {
    uint8_t tag;
    uint16_t length;
    uint8_t bytes[];
} CONSTANT_Utf8_info;
```

O leitor aloca `sizeof(CONSTANT_Utf8_info) + length` bytes e copia o conteudo lido para `entry->bytes`. Esses bytes sao usados para nomes de classes, nomes de metodos, descritores, nomes de atributos e strings.

## Referencias Dentro da Constant Pool

Muitas entradas da constant pool nao armazenam diretamente o texto final, mas indices para outras entradas.

Exemplo:

```text
CONSTANT_Class_info
    name_index -> CONSTANT_Utf8_info
```

Exemplo de method reference:

```text
CONSTANT_Methodref_info
    class_index         -> CONSTANT_Class_info
    name_and_type_index -> CONSTANT_NameAndType_info

CONSTANT_NameAndType_info
    name_index       -> CONSTANT_Utf8_info
    descriptor_index -> CONSTANT_Utf8_info
```

A funcao `check_constant_pool_references`, em `src/constant_pool.c`, verifica se esses indices estao dentro dos limites e se apontam para tags esperadas.

## Informacoes da Classe

Apos a constant pool, `read_classfile` le:

- `access_flags`: mascara de bits que descreve propriedades da classe;
- `this_class`: indice para um `CONSTANT_Class_info` que representa a propria classe;
- `super_class`: indice para a superclasse, ou zero no caso especial de `java/lang/Object`;
- `interfaces_count`;
- `interfaces[]`.

As validacoes correspondentes estao em:

- `validate_this_class`;
- `validate_super_class`;
- `validate_interfaces`.

Essas funcoes garantem que os indices estejam dentro da constant pool e apontem para `CONSTANT_Class` quando necessario.

## Fields

Fields representam variaveis declaradas pela classe ou interface. A estrutura do projeto segue o formato da especificacao:

```c
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} field_info;
```

Cada field possui:

- flags de acesso;
- indice para o nome;
- indice para o descritor;
- lista de atributos.

O nome e o descritor devem apontar para entradas `CONSTANT_Utf8`. A funcao `validate_fields` verifica essas referencias.

A exibicao esta em `print_fields`, em `src/viewer.c`, incluindo a traducao de flags como `public`, `private`, `static`, `final`, `volatile`, `transient`, `synthetic` e `enum`.

## Methods

Methods seguem uma estrutura semelhante aos fields:

```c
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} method_info;
```

Cada method possui:

- flags de acesso;
- nome;
- descritor;
- atributos.

A validacao de nomes e descritores ocorre em `validate_methods`. A exibicao ocorre em `print_methods`, que tambem traduz flags como `public`, `static`, `final`, `synchronized`, `native`, `abstract`, `strictfp` e `synthetic`.

## Attributes

Attributes sao estruturas genericas usadas em varias partes do arquivo `.class`: classe, fields, methods e tambem dentro do atributo `Code`.

O formato geral definido pela especificacao e:

```c
attribute_info {
    u2 attribute_name_index;
    u4 attribute_length;
    u1 info[attribute_length];
}
```

No projeto:

```c
typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    void *info;
} attribute_info;
```

O campo `attribute_name_index` deve apontar para uma entrada `CONSTANT_Utf8`, que contem o nome do atributo. A funcao `read_attributes_array` faz a leitura generica de atributos, enquanto `validate_attributes` valida os atributos globais da classe.

## Atributo `Code`

O atributo `Code` é especial porque contém o bytecode executável de um método. Quando o programa está lendo os atributos e identifica o nome "**Code**" através da Constant Pool, ele direciona a extração para o arquivo `src/attributes.c` (dentro da função `read_specific_attribute_info`).

A estrutura usada no projeto e:

```c
typedef struct {
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    uint16_t exception_table_length;
    ExceptionTable_info *exception_table;
    uint16_t attributes_count;
    attribute_info *attributes;
} Code_attribute;
```

Diferente de implementações mais simples que apenas pulam partes complexas do arquivo, a nossa implementação realiza a leitura completa e profunda do atributo Code. Ela extrai:

- `max_stack`: profundidade máxima da pilha de operandos;

- `max_locals`: quantidade máxima de variáveis locais alocadas;

- `code_length` e `code`: o array contendo os opcodes e operandos brutos do método;

- `exception_table`: tabela de tratamento de erros (try-catch), lendo e armazenando com precisão os ponteiros de instrução (start_pc, end_pc, handler_pc e catch_type);

- Sub-atributos aninhados: através de uma chamada recursiva inteligente para read_attributes_array, o programa desce mais um nível na árvore estrutural para ler e desempacotar sub-atributos úteis para debug, como o LineNumberTable e o LocalVariableTable.


## Bytecode e Instrucoes

O array `code` do atributo `Code` contem instrucoes da JVM. Cada instrucao comeca por um opcode `u1`, seguido ou nao por operandos.

A funcao `view_instructions`, em `src/instruction_viewer.c`, percorre esse array usando um contador `pc`:

```c
uint32_t pc = 0;
while (pc < code->code_length) {
    uint8_t opcode = code->code[pc];
    ...
}
```

Para cada opcode reconhecido, ela imprime o nome da instrucao e avanca o `pc` pelo tamanho correto da instrucao. Exemplos:

- `iconst_0`: 1 byte;
- `bipush`: opcode + operando de 1 byte;
- `ldc_w`: opcode + indice de 2 bytes;
- `invokevirtual`: opcode + indice de 2 bytes;
- `invokeinterface`: opcode + indice de 2 bytes + `count` + byte reservado.

Instrucoes nao implementadas sao exibidas como:

```text
unknown opcode 0xXX
```

## Exibicao dos Dados

As funções de formatação e exibição visual estão concentradas em `src/viewer.c`. Um grande diferencial da arquitetura atual é que todas essas funções recebem um ponteiro `FILE *out`, permitindo que o relatório seja gerado tanto no terminal (`stdout`) quanto exportado para o arquivo `saida_exibidor.txt` simultaneamente.

As principais funções são:

- **`print_general_information`**: Mostra o cabeçalho geral do arquivo, incluindo Magic Number, versões, flags de acesso traduzidas e os nomes resolvidos da própria classe e de sua superclasse.

- **`print_constant_pool`**: Imprime as entradas da Constant Pool, resolvendo a fundo as referências indiretas para exibir os nomes reais em vez de apenas os índices numéricos.

- **`print_interfaces`**: Lista os índices e os nomes resolvidos de todas as interfaces implementadas.

- **`print_fields` e `print_methods`**: Exibem as variáveis e funções da classe, traduzindo as Access Flags (ex: `public static`) e listando o sumário de seus respectivos atributos.

- **`print_attributes`**: Agrega e numera sequencialmente todos os atributos encontrados no `.class` (sejam eles pertencentes à Classe, aos Fields ou aos Methods), invocando a leitura específica para cada um.

- **`print_specific_attribute_info`**: Função central e recursiva responsável por formatar os dados de cada atributo (como `SourceFile`, `InnerClasses`, `LineNumberTable`, etc.). É esta função que, ao identificar o atributo `Code`, exibe o Hex Dump, lê a tabela de exceções, formata os sub-atributos e invoca `view_instructions` (o Disassembler) para exibir o bytecode traduzido.

## Validacoes Implementadas

O projeto inclui validacoes importantes para evitar interpretar indices invalidos:

- `this_class` deve apontar para `CONSTANT_Class`;
- `super_class`, quando diferente de zero, deve apontar para `CONSTANT_Class`;
- cada interface deve apontar para `CONSTANT_Class`;
- nomes e descritores de fields devem apontar para `CONSTANT_Utf8`;
- nomes e descritores de methods devem apontar para `CONSTANT_Utf8`;
- nomes de atributos globais devem apontar para `CONSTANT_Utf8`;
- referencias da constant pool sao verificadas conforme a tag.

Essas validacoes estao alinhadas com a etapa de format checking descrita pela especificacao, embora a implementacao nao cubra todas as regras formais da JVM.

## Limitacoes Atuais

A implementacao e um leitor/exibidor parcial do formato `.class`. Alguns pontos ainda nao estao totalmente cobertos:

- nao valida completamente descritores de fields e methods;
- nao valida todos os bits reservados de `access_flags`;
- nao interpreta todos os atributos predefinidos pela especificacao;
- nao implementa todos os opcodes da JVM;
- nao executa bytecode, apenas exibe;
- nao verifica todos os constraints de bytecode descritos pela especificacao.

Mesmo assim, o projeto cobre a estrutura essencial do `ClassFile`: cabecalho, constant pool, informacoes da classe, fields, methods, attributes e parte do atributo `Code`.

## Como Compilar e Executar

O projeto utiliza o `make` para automatizar o processo de compilação.

1. **Compilar o projeto:**
Na raiz do diretório `implementation/`, execute:
```bash
make
```


2. **Rodar com o arquivo de teste padrão:**
O Makefile possui uma regra para testar rapidamente a execução (geralmente apontando para um arquivo como `Main.class`):
```bash
make run
```


3. **Analisar um arquivo `.class` específico:**
Graças à configuração do Makefile, você pode passar o caminho do arquivo .class diretamente:
```bash
make run caminhos/para/seu_arquivo.class

```

## Mapa dos Arquivos

| Arquivo | Responsabilidade |
| --- | --- |
| `include/attributes.h` | Define as structs principais relativas a attributes. |
| `include/class_file.h` | Define as structs principais do formato `.class`. |
| `include/constant_pool.h` | Define tags e structs da constant pool. |
| `include/reader.h` | Declara funcoes de leitura e validacao. |
| `include/instruction_viewer.h` | Declara o leitor de instrucoes bytecode. |
| `include/viewer.h` | Declara as informacoes lidas em formato legivel. |
| `include/utils.h` | Declara funcoes auxiliares, como conversao de endianness. |
| `src/attributes.c` | Implementa funções relativas a attributes. |
| `src/main.c` | Orquestra leitura, validacao, exibicao e limpeza. |
| `src/reader.c` | Le a estrutura `ClassFile` na ordem da especificacao. |
| `src/constant_pool.c` | Le e valida entradas da constant pool. |
| `src/instruction_viewer.c` | Faz o disassembly parcial dos opcodes. |
| `src/viewer.c` | Exibe as informacoes lidas em formato legivel. |
| `src/utils.c` | Contem funcoes auxiliares, como conversao de endianness. |

## O Que Cada Arquivo Faz no Programa

Esta secao serve como um guia para o grupo entender como as partes se conectam. A especificacao oficial divide o arquivo `.class` em estruturas como `ClassFile`, `cp_info`, `field_info`, `method_info`, `attribute_info` e `Code_attribute`. O projeto acompanha essa divisao: alguns arquivos definem as estruturas, outros fazem a leitura binaria, outros validam referencias e outros exibem o resultado.

### `src/main.c`

Este e o ponto de entrada do programa. Ele nao conhece todos os detalhes internos do formato `.class`; sua funcao e coordenar o fluxo geral:

1. decide qual arquivo `.class` abrir;
2. abre o arquivo em modo binario (`rb`);
3. aloca uma struct `ClassFile`;
4. chama `read_classfile` para preencher a struct;
5. chama `check_constant_pool_references` para validar referencias internas da constant pool;
6. abre o arquivo `saida_exibidor.txt` para exportacao de relatorio;
7. chama as funcoes de exibicao em `viewer.c` (passando o terminal `stdout` e o arquivo `.txt`);
8. invoca a rotina cirurgica `deep_free` (em `utils.c`) para liberar toda a arvore de memoria alocada;
9. encerra o programa.

Em termos da especificacao, `main.c` e o controlador que pega a estrutura `ClassFile` completa ja lida da memoria e decide quais partes serao exibidas para o usuario.

Um ponto importante: a JVM real carrega, verifica, prepara e executa classes. Este projeto nao executa bytecode; ele le e exibe informacoes do arquivo.

### `src/reader.c`

Este arquivo implementa a leitura principal do `ClassFile`. Ele e o arquivo que mais segue diretamente a ordem da especificacao oficial.

A funcao mais importante e:

```c
int read_classfile(ClassFile *cf, FILE *file);
```

Ela le os campos exatamente na sequencia definida em `ClassFile`:

- `magic`;
- `minor_version`;
- `major_version`;
- `constant_pool_count`;
- `constant_pool`;
- `access_flags`;
- `this_class`;
- `super_class`;
- `interfaces_count` e `interfaces`;
- `fields_count` e `fields`;
- `methods_count` e `methods`;
- `attributes_count` e `attributes`.

Como a especificacao define que os itens sao armazenados sequencialmente e sem padding, `reader.c` usa `fread` na ordem correta. Sempre que le um campo `u2` ou `u4`, converte de big-endian usando `byteswap_u2` ou `byteswap_u4`.

Tambem estao em `reader.c` algumas validacoes:

- `validate_this_class`: confere se `this_class` aponta para uma entrada `CONSTANT_Class`;
- `validate_super_class`: confere se `super_class` e zero apenas quando permitido, ou se aponta para `CONSTANT_Class`;
- `validate_interfaces`: confere se cada interface aponta para `CONSTANT_Class`;
- `validate_fields`: confere se `name_index` e `descriptor_index` dos fields apontam para `CONSTANT_Utf8`;
- `validate_methods`: faz a mesma validacao para methods;
- `validate_attributes`: confere se o nome de cada atributo global aponta para `CONSTANT_Utf8`.

### Leitura Dinâmica de Atributos

A função auxiliar `read_attributes_array` é responsável por processar os blocos de atributos tanto de Classes quanto de Fields e Methods.

```c
attribute_info* read_attributes_array(ClassFile *cf, uint16_t count, FILE *file);
```

Diferente de implementações rígidas, esta função faz uma ponte dinâmica com a "fábrica de atributos":

- Lê o attribute_name_index e localiza a estrutura correspondente na piscina de constantes.

- Extrai o array de bytes brutos da JVM e monta uma string em C terminada em nulo (\0) contendo o nome real do atributo (ex: "Code", "SourceFile").

- Repassa essa string e o fluxo do arquivo para a função global read_specific_attribute_info (em attributes.c), delegando a ela a responsabilidade de desempacotar e estruturar as informações específicas de cada tipo de atributo.

### `src/constant_pool.c`

Este arquivo cuida da constant pool, que e uma das partes mais importantes do arquivo `.class`. Pela especificacao, a constant pool armazena literais e referencias simbolicas usadas pela classe: nomes, descritores, classes, metodos, fields, strings, handles e informacoes dinamicas.

A funcao:

```c
void *constant_pool_reader(cp_tags tag, FILE *file);
```

recebe a tag ja lida por `reader.c` e decide qual estrutura concreta deve ser alocada e preenchida. Por exemplo:

- tag `7`: aloca e le `CONSTANT_Class_info`;
- tag `10`: aloca e le `CONSTANT_Methodref_info`;
- tag `12`: aloca e le `CONSTANT_NameAndType_info`;
- tag `1`: aloca e le `CONSTANT_Utf8_info`;
- tags `5` e `6`: le `CONSTANT_Long_info` e `CONSTANT_Double_info`.

Cada entrada possui campos diferentes. Por isso, `constant_pool_reader` retorna `void *`; quem recebe esse ponteiro guarda o valor dentro de `cp_info->info`.

Tambem esta neste arquivo:

```c
int check_constant_pool_references(ClassFile *cf);
```

Essa funcao percorre a constant pool e valida se os indices internos apontam para tipos esperados. Exemplos:

- `CONSTANT_Class_info.name_index` deve apontar para `CONSTANT_Utf8`;
- `CONSTANT_Fieldref_info.class_index` deve apontar para `CONSTANT_Class`;
- `CONSTANT_Fieldref_info.name_and_type_index` deve apontar para `CONSTANT_NameAndType`;
- `CONSTANT_String_info.string_index` deve apontar para `CONSTANT_Utf8`;
- `CONSTANT_NameAndType_info.name_index` e `descriptor_index` devem apontar para `CONSTANT_Utf8`;
- `CONSTANT_MethodHandle_info.reference_kind` deve estar entre 1 e 9.

Essa validacao corresponde a parte das regras de consistencia do formato descritas pela especificacao. Ela nao substitui o verificador completo da JVM, mas evita que o exibidor interprete referencias absurdas ou com tags erradas.

### `src/attributes.c`

Esta é a nossa "Fábrica de Atributos", responsável por decodificar as estruturas de tamanho variável. Na JVM, atributos podem estar nas Classes, nos Fields, nos Methods e até dentro de outros atributos!

A função principal:

```c
void* read_specific_attribute_info(ClassFile *cf, FILE *file, const char* attr_name, uint32_t attr_length);
```

Compara a string com o nome do atributo e desempacota os dados específicos. O grande destaque desta implementação é o atributo Code. Quando ele é encontrado, o código não apenas lê o tamanho do bytecode, mas também aloca e processa recursivamente os seus sub-atributos, como o LineNumberTable e o LocalVariableTable, além da Tabela de Exceções. Diferente de analisadores mais simples, nada é apenas "pulado" com fseek.

### `src/instruction_viewer.c`

O nosso Disassembler embutido. Ele traduz os opcodes brutos (hexadecimal) em instruções compreensíveis por humanos.

A função:

```c
void view_instructions(FILE *out, ClassFile* cf, Code_attribute* code);
```

Usa um Program Counter (pc) para varrer o array de bytes. Um grande switch/case traduz o opcode para o seu mnemônico correspondente (ex: 0xB6 vira invokevirtual). Fundamentalmente, ele sabe quantos bytes "pular" após cada instrução dependendo se ela possui operandos (ex: aload_0 avança 1 byte, enquanto bipush avança 2 bytes). Opcodes desconhecidos são exibidos de forma segura para não quebrar a leitura.

### `src/viewer.c`

Este arquivo é o motor de formatação e renderização visual do projeto. Ele não lê bytes diretamente do arquivo `.class`; seu papel é navegar pela struct `ClassFile` já alocada na memória e imprimir os valores de forma interpretada e humanamente compreensível.

Um grande diferencial da arquitetura atual é que todas as funções de exibição recebem um ponteiro `FILE *out`. Isso permite que o programa seja polimórfico, gerando o relatório no terminal (`stdout`) e exportando para um arquivo de texto (`saida_exibidor.txt`) simultaneamente, sem duplicar a lógica de impressão.

As principais funções são:

- `print_general_information`: imprime o cabeçalho da classe, incluindo o *Magic Number*, tamanho da constant pool, contadores e as versões. Utiliza a função `get_java_version_string` para traduzir o *Major Version* em versões comerciais do Java (suportando até o Java SE 26).

- `print_constant_pool`: percorre a piscina de constantes imprimindo cada entrada e seus respectivos dados.

- `print_interfaces`: lista os índices e exibe os nomes resolvidos das interfaces implementadas.

- `print_fields` e `print_methods`: imprimem as variáveis e funções, exibindo seus descritores, traduzindo as *Access Flags* e listando o sumário de atributos pertencentes a cada um.

- `print_attributes`: agrega e varre sequencialmente todos os atributos encontrados no `.class` (sejam eles atributos globais da classe, dos fields ou dos methods), invocando a leitura detalhada para cada um.

- `print_specific_attribute_info`: função central que formata os dados internos de atributos específicos (como `SourceFile`, `ConstantValue`, `InnerClasses`, `LineNumberTable`, etc). Quando identifica o atributo `Code`, ela exibe as informações de pilha, o *hex dump*, a tabela de exceções, formata os sub-atributos recursivamente e invoca a função `view_instructions` para realizar o disassembly do código.

O `viewer.c` também é responsável por resolver as complexas referências indiretas da constant pool. Por exemplo, para imprimir um `CONSTANT_Methodref` de forma legível, ele busca:

1. a classe em `class_index`;
2. o nome real da classe via `CONSTANT_Class_info.name_index`;
3. o `NameAndType` apontado pelo `name_and_type_index`;
4. o nome e o descritor do método.

Isso é fundamental porque a constant pool é altamente indireta: muitas entradas só fazem sentido para o usuário quando seus índices são resolvidos e cruzados com as entradas `CONSTANT_Utf8`.

Por fim, o arquivo contém funções de tradução que utilizam máscaras de bits (`&`), como `get_class_access_flags_string`, `print_field_access_flags` e `print_method_access_flags`. Elas transformam números hexadecimais em permissões textuais claras, como `public`, `private`, `static`, `final` e `abstract`.

### `src/utils.c`

Este arquivo contem funcoes auxiliares pequenas, mas essenciais para a leitura correta do formato:

```c
uint16_t byteswap_u2(uint16_t val);
uint32_t byteswap_u4(uint32_t val);
```

A especificacao define que valores multibyte do `.class` sao big-endian. Como a maquina pode usar outra ordem interna de bytes, `utils.c` converte os valores lidos para que o restante do programa trabalhe com inteiros corretos.

Sem essas funcoes, campos como `major_version`, `constant_pool_count`, indices da constant pool e tamanhos de atributos poderiam aparecer invertidos.

Além da conversão de Endianness, este arquivo abriga a função `deep_free`. Como a estrutura do `.class` é uma árvore complexa (Classes contêm Métodos, que contêm Atributos, que contêm Sub-Atributos), o `deep_free` navega por todos os ponteiros alocados garantindo que não haja vazamento de memória (*memory leaks*) ao encerrar a execução.

## Como os Arquivos Trabalham Juntos

O fluxo completo pode ser entendido assim:

```text
main.c
 ├── Abre o arquivo binário
 └── Chama reader.c

     reader.c
      ├── Lê as variáveis globais (Magic Number, versões)
      ├── Chama constant_pool.c para preencher o dicionário
      ├── Lê Interfaces, Fields e Methods
      └── Chama attributes.c para decodificar os atributos de cada um

          attributes.c
           └── Desempacota o atributo (Ex: "Code" e seus sub-atributos)

 ├── Chama constant_pool.c (Validação de Referências)
 └── Chama viewer.c passando o Terminal e o .txt

     viewer.c
      ├── Exibe General Info, Constant Pool, Fields, Methods
      └── Chama instruction_viewer.c ao encontrar o atributo Code

          instruction_viewer.c
           └── Varre os bytes e imprime o Disassembly (Mnemônicos)

 └── Limpa toda a estrutura da memória e encerra.
```

Essa separacao ajuda o projeto a seguir a organizacao da propria especificacao: primeiro os bytes sao lidos e convertidos, depois as referencias sao validadas, depois as estruturas sao exibidas.

## Referencias

- `pdfs/classfile-85-117.pdf`
- Java Virtual Machine Specification, Java SE 26, Chapter 4: The class File Format: https://docs.oracle.com/en/java/javase/26/docs/specs/jvms/jvms-4.html
