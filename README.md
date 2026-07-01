# Implementação de uma Java Virtual Machine (JVM) em C

Implementação em C de uma **Java Virtual Machine (JVM)** capaz de ler, analisar e executar arquivos bytecode Java (`.class`). O projeto é desenvolvido como trabalho da disciplina de **Software Básico** da Universidade de Brasília (UnB) e segue as definições da **Java Virtual Machine Specification (JVMS)**.

O programa opera em dois modos distintos: um **Leitor/Exibidor** estático que decodifica e imprime a estrutura interna de arquivos `.class` em formato legível, e um **Interpretador (Motor de Execução)** que implementa o ciclo Fetch-Decode-Execute da JVM, executando o bytecode Java diretamente.

---

## 🏗️ Arquitetura e Diagrama de Blocos

O projeto foi cuidadosamente modularizado para separar as responsabilidades entre a leitura dos arquivos binários, o gerenciamento de memória e o motor de execução. O diagrama abaixo ilustra o fluxo de comunicação e as dependências entre os principais módulos do sistema:

<img src="https://raw.githubusercontent.com/lucassaad/JVM/main/implementation/midia/diagrama_de_blocos.png" width="800"/>

### 🧩 Visão Geral dos Módulos

* **Ponto de Entrada (`main.c`):** Responsável por orquestrar o programa, fazendo o *parse* dos argumentos da linha de comando e despachando o fluxo para o modo Leitor ou Interpretador.
* **Análise e Leitura (Utils):** Conjunto de módulos (`reader.c`, `viewer.c`, `instruction_viewer.c`) encarregados de realizar o *parsing* seguro da estrutura do `.class` e fornecer sua representação visual (Disassembler).
* **Motor de Execução (Core):** O coração da máquina virtual. O `interpreter.c` gerencia o ciclo *Fetch-Decode-Execute*, enquanto o `instruction.c` centraliza a lógica de implementação de cada *opcode* da especificação.
* **Áreas de Memória:** Módulos que mantêm o estado dinâmico do programa em tempo de execução:
  * **Method Area (`method_area.c`):** Cache dinâmico de classes carregadas.
  * **JVM Stack & Frames (`jvm_stack.c`, `frame.c`):** Gerenciamento da pilha de execução de métodos e pilha de operandos.
  * **Heap Genérica (`java_types.c`):** Alocação e controle de objetos e arrays em memória nativa.

## 🎯 Funcionalidades

### Modo Leitor/Exibidor (`-l`)
- Leitura e validação do *Magic Number* (`0xCAFEBABE`), versões Minor/Major e *Access Flags* da classe.
- Resolução completa da **Constant Pool**, mapeando referências indiretas (ex: nome real de um `Methodref` ou `Fieldref`) e tratando corretamente entradas duplas de `Long` e `Double`.
- Exibição de **Fields** e **Methods** com tradução das *access flags* (ex: `0x0009` → `[public static]`).
- Desempacotamento profundo do **atributo `Code`**, incluindo `max_stack`, `max_locals`, bytecodes, tabela de exceções e sub-atributos (`LineNumberTable`, `LocalVariableTable`).
- **Disassembly** do bytecode: converte opcodes brutos em mnemônicos legíveis (ex: `0xB6` → `invokevirtual`).
- Geração simultânea do relatório no terminal e em `saida_exibidor.txt`.

### Modo Interpretador (`-i`)
- Ciclo **Fetch-Decode-Execute** completo com despacho por `switch(opcode)`.
- **Pilha de Frames da JVM** com criação/destruição dinâmica de frames a cada chamada e retorno de método.
- **Área de Métodos** com carregamento dinâmico de classes a partir de arquivos `.class`.
- Geração de log de acompanhamento em `execucao_jvm.txt`.

#### Instruções Implementadas
| Categoria | Instruções |
|---|---|
| Constantes | `aconst_null`, `iconst_<n>`, `lconst_<n>`, `fconst_<n>`, `dconst_<n>`, `bipush`, `sipush`, `ldc`, `ldc_w`, `ldc2_w` |
| Carregamento | `iload`, `lload`, `fload`, `dload`, `aload` e variantes `_0` a `_3` |
| Armazenamento | `istore`, `lstore`, `fstore`, `dstore`, `astore` e variantes `_0` a `_3` |
| Pilha | `pop`, `dup`, `dup2`, `swap` |
| Aritmética int | `iadd`, `isub`, `imul`, `idiv`, `irem`, `ineg`, `ishl`, `ishr`, `iushr`, `iand`, `ior`, `ixor`, `iinc` |
| Aritmética long | `ladd`, `lsub`, `lmul`, `ldiv`, `lrem` |
| Aritmética float/double | `fadd`, `fsub`, `fmul`, `fdiv`, `frem`, `dadd`, `dsub`, `dmul`, `ddiv`, `drem`, `dneg` |
| Conversão de tipo | `i2f`, `i2d`, `i2b`, `i2c`, `i2s`, `f2i`, `d2i`, `d2l`, `d2f` |
| Comparação | `dcmpl`, `dcmpg`, `ifeq`, `ifne`, `iflt`, `ifge`, `ifgt`, `ifle`, `if_icmp<cond>`, `ifnull`, `ifnonnull` |
| Desvio | `goto`, `tableswitch`, `lookupswitch` |
| Arrays | `newarray`, `anewarray`, `multianewarray`, `arraylength`, `*aload`, `*astore` (todos os tipos) |
| Objetos | `new`, `getfield`, `putfield`, `getstatic`, `putstatic` |
| Invocação | `invokevirtual`, `invokespecial`, `invokestatic`, `invokeinterface` |
| Retorno | `return`, `ireturn`, `lreturn`, `freturn`, `dreturn`, `areturn` |

---

## 🚀 Como Compilar e Executar

O projeto usa `make` e é compatível com **Linux, macOS e Windows** (via MinGW/MSYS2).

### 1. Compilar

Na raiz do diretório `implementation/`, execute:

```bash
make
```

### 2. Executar o Leitor/Exibidor

```bash
# Com um arquivo específico (É essencial que todos arquivos .class estejam na pasta exemplos/)
make run-leitor FILE=exemplos/fibonacci.class
```

### 3. Executar o Interpretador

```bash
# Com um arquivo específico (É essencial que todos arquivos .class estejam na pasta exemplos/)
make run-interpreter FILE=exemplos/fatorial.class
make run-interpreter FILE=exemplos/fibonacci.class
make run-interpreter FILE=exemplos/double_aritmetica.class
```

### 4. Limpar arquivos de compilação

```bash
make clean
```

### Resumo dos comandos `make`

| Comando | Descrição |
|---|---|
| `make` | Apenas compila, gerando o binário em `bin/`. |
| `make run-leitor` | Compila e executa no modo Leitor/Exibidor. |
| `make run-interpreter` | Compila e executa o Interpretador. |
| `make clean` | Remove as pastas `obj/` e `bin/`. |

### Flags de compilação

O projeto compila com `-std=c99 -Wall -Wextra -g -m32`:

- **`-std=c99`**: Garante portabilidade e diz ao compilador para usar a versão C99 da linguagem C (padrão da disciplina).
- **`-Wall` / `-Wextra`**: Ativa todos os avisos do compilador, prevenindo bugs silenciosos.
- **`-g`**: Inclui símbolos de debug.
- **`-m32`**: Força a geração de um executável de 32 bits, independentemente da máquina ser de 64 bits. No contexto desta JVM, isso garante que os endereços de memória (ponteiros) tenham 32 bits e caibam perfeitamente nas variáveis uint32_t da Pilha de Operandos, resolvendo o problema de truncamento que causava o Segmentation Fault.

### ⚠️ Observação Crítica sobre a Arquitetura de 32 Bits (`-m32`)

A flag **`-m32`** é um pilar arquitetônico fundamental para a corretude desta implementação. Ela força o GCC a compilar a JVM em modo **x86 nativo (32 bits)**, independentemente de o sistema hospedeiro rodar em uma arquitetura de 64 bits.

> 🌐 **Nota para usuários Linux (Ubuntu/Debian):**
> Caso ocorra um erro de compilação relacionado a bibliotecas ausentes de 32 bits (*multilib*), instale o pacote de compatibilidade rodando o seguinte comando no terminal do sistema:
>
> ```bash
> sudo apt-get update && sudo apt-get install gcc-multilib g++-multilib
> ```

---

## 📂 Arquitetura do Projeto

```
implementation/
├── include/          # Cabeçalhos (.h) — definição de structs e assinaturas
├── src/              # Implementações (.c)
├── exemplos/         # Arquivos .class para teste
├── Makefile
└── bin/              # Binário gerado (criado pelo make)
```

### Mapa dos arquivos

| Arquivo | Responsabilidade |
|---|---|
| `include/class_file.h` | Structs principais do formato `.class` (`ClassFile`, `field_info`, `method_info`, `attribute_info`). |
| `include/constant_pool.h` | Tags e structs de cada tipo de entrada da Constant Pool. |
| `include/attributes.h` | Structs de atributos especiais (`Code_attribute`, `ExceptionTable_info`, etc.). |
| `include/frame.h` | Struct `Frame` e funções de acesso à Pilha de Operandos e Variáveis Locais. |
| `include/jvm_stack.h` | Struct `JVMStack`, registro PC e funções de gerenciamento da pilha de frames. |
| `include/method_area.h` | Struct `MethodArea` e funções de carregamento e busca de classes/métodos. |
| `include/interpreter.h` | Assinatura do motor de execução (`execute_engine`). |
| `include/instruction.h` | Assinaturas de todas as funções de instrução implementadas. |
| `include/instruction_viewer.h` | Assinatura do disassembler de bytecode. |
| `include/viewer.h` | Assinaturas das funções de exibição do Leitor/Exibidor. |
| `include/utils.h` | Funções auxiliares de conversão de endianness (`byteswap_u2`, `byteswap_u4`). |
| `src/main.c` | Ponto de entrada: orquestra leitura, exibição ou execução e limpeza de memória. |
| `src/reader.c` | Parser binário do arquivo `.class` seguindo a especificação. |
| `src/constant_pool.c` | Leitura, estruturação e validação de referências cruzadas da Constant Pool. |
| `src/attributes.c` | Fábrica de atributos: desempacota `Code`, `LineNumberTable`, `LocalVariableTable`, etc. |
| `src/frame.c` | Implementação do Frame: criação, destruição, acesso a variáveis locais e pilha de operandos. |
| `src/jvm_stack.c` | Implementação da JVM Stack: push/pop de frames, registro PC e controle de memória. |
| `src/method_area.c` | Implementação da Área de Métodos: registro, busca e carregamento dinâmico de classes. |
| `src/interpreter.c` | Motor de execução: ciclo Fetch-Decode-Execute com despacho por `switch(opcode)`. |
| `src/instruction.c` | Implementação de todas as instruções da JVM. |
| `src/instruction_viewer.c` | Disassembler: converte opcodes em mnemônicos para o modo Leitor/Exibidor. |
| `src/viewer.c` | Motor de formatação: navega pela `ClassFile` na memória e imprime o relatório. |
| `src/utils.c` | Funções de conversão de endianness e liberação profunda de memória (`deep_free`). |

---

## 🏗️ Arquitetura Interna da JVM

### Estruturas de Dados em Tempo de Execução

#### Frame (`frame.h` / `frame.c`)

Cada chamada de método cria um novo `Frame`, que contém:

- **Vetor de Variáveis Locais** (`local_vars`): array dinâmico de slots de 32 bits. Armazena parâmetros e variáveis locais do método. `long` e `double` ocupam 2 slots consecutivos em big-endian.
- **Pilha de Operandos** (`operand_stack`): array LIFO de slots de 32 bits. Os tamanhos de ambos (`max_locals`, `max_stack`) são lidos do atributo `Code` em tempo de compilação.
- **Referência à Constant Pool** da classe do método corrente.
- **Ponteiro para o bytecode** (`code`) e **Program Counter local** (`pc`).
- **Ponteiro para o frame anterior** (`previous_frame`), formando uma lista encadeada de chamadas.

Os slots usam `uint32_t` como representação genérica de bits. Funções tipadas (`frame_push_int`, `frame_pop_double`, `frame_set_local_long`, etc.) fazem a conversão via `memcpy`, evitando undefined behavior de cast direto entre tipos.

#### JVM Stack (`jvm_stack.h` / `jvm_stack.c`)

A `JVMStack` mantém o frame corrente (`current_frame`) e controla o uso de memória (`current_size_bytes` vs `max_size_bytes`). Detecta e reporta `StackOverflowError` e `OutOfMemoryError`. O `pc_register` global reflete sempre o PC do frame corrente.

#### Área de Métodos (`method_area.h` / `method_area.c`)

Array dinâmico de ponteiros para `ClassFile`. Suporta:
- Registro de classes já lidas (`method_area_register`).
- Busca por nome (`method_area_find_loaded`).
- **Carregamento dinâmico**: quando uma instrução `invoke*` referencia uma classe ainda não carregada, `method_area_load` abre o `.class` correspondente do disco e o registra automaticamente.
- Resolução de fields por nome/descritor para `getfield`/`putfield` (`resolve_field_offset`).

### Ciclo de Execução

```
execute_engine(stack)
 └── while (!jvm_stack_is_empty)
      ├── Fetch:  opcode = frame->code[frame->pc++]
      ├── Decode: switch(opcode)
      └── Execute: instrução correspondente modifica
                   frame->operand_stack / frame->local_vars / stack
```

### Chamada e Retorno de Métodos

Ao encontrar `invokestatic`, `invokevirtual`, `invokespecial` ou `invokeinterface`:
1. O interpretador resolve o método na Área de Métodos (carregando a classe se necessário).
2. `frame_push_method` cria o novo frame, move os argumentos do topo da Pilha de Operandos do chamador para as Variáveis Locais do novo frame, e empilha o frame na JVM Stack.

Ao encontrar `return`, `ireturn`, `lreturn`, etc.:
1. `frame_pop_method` lê o valor de retorno do topo da pilha do frame corrente.
2. Destrói o frame corrente e libera sua memória.
3. Empilha o valor de retorno na Pilha de Operandos do frame chamador.

### Representação de Objetos e Arrays

Como o heap da JVM não foi implementado, objetos e arrays são alocados diretamente com `malloc`/`calloc`:

- **Array**: `struct Array { int32_t length; int32_t *elements; }` — alocado por `newarray`/`anewarray`, o ponteiro é convertido para `uint32_t` e empilhado como referência.
- **Objeto**: `struct Object { ClassFile *class_ref; int32_t *fields; }` — alocado por `new`, com `fields` como vetor contíguo indexado por `resolve_field_offset`.

Referências (`uint32_t`) na Pilha de Operandos armazenam o endereço de memória do objeto/array convertido para inteiro. Por isso o projeto requer uma plataforma onde ponteiros caibam em 32 bits, ou deve ser compilado com `make` sem otimizações de ponteiro de 64 bits para testes.

---

## 📋 Fluxo Completo do Programa

```
main.c
 ├── Abre o arquivo .class
 └── reader.c: parseia ClassFile na ordem da especificação
      ├── constant_pool.c: lê e valida cada entrada da CP
      └── attributes.c: desempacota Code, LineNumberTable, etc.

 ├── constant_pool.c: valida referências cruzadas
 ├── method_area.c: registra a classe principal

 ── MODO LEITOR (-l) ──────────────────────────────────────
 └── viewer.c: imprime ClassFile em stdout e saida_exibidor.txt
      └── instruction_viewer.c: disassembly do bytecode

 ── MODO INTERPRETADOR (-i) ───────────────────────────────
 ├── method_area.c: localiza o método main
 ├── frame.c: cria o frame inicial
 ├── jvm_stack.c: inicializa a JVM Stack e empilha o frame
 └── interpreter.c: execute_engine — ciclo Fetch-Decode-Execute
      ├── instruction.c: executa cada instrução
      ├── frame.c: manipula Variáveis Locais e Pilha de Operandos
      ├── method_area.c: carrega classes dinamicamente (invoke*)
      └── jvm_stack.c: push/pop de frames (invoke*/return)

 └── Limpeza de memória e encerramento
```

---

## 🗂️ Arquivos de Exemplo

A pasta `exemplos/` contém arquivos `.class` para testar a JVM:

| Arquivo | O que testa |
|---|---|
| `fibonacci.class` | Recursão, chamada de método, aritmética int |
| `FibonacciRec.class` | Recursão com objeto, `invokespecial` |
| `fatorial.class` | Laço, `iinc`, comparação e desvio |
| `double_aritmetica.class` | Operações com `double`, `ldc2_w`, `dadd`, `dmul` |
| `double_cast.class` | Conversões de tipo (`i2d`, `d2i`, `d2f`) |
| `double_logico.class` | Comparações com `dcmpl`/`dcmpg` |
| `tableswitch.class` | Instrução `tableswitch` (switch denso) |
| `lookupswitch.class` | Instrução `lookupswitch` (switch esparso) |
| `vetor2.class` / `vetor_8.class` | Arrays (`newarray`, `iaload`, `iastore`) |
| `multi.class` | Arrays multidimensionais (`multianewarray`) |
| `soma_certo.class` | Método estático com parâmetros inteiros |
| `Somar.class` | Classe com campo e `putfield`/`getfield` |
| `Belote.class`, `Carta.class`, `Jogador.class` | Múltiplas classes, `invokeinterface` |
| `interface_test.class` | Implementação de interface |
| `method_test.class` | Chamadas de método encadeadas |

---

## 🔧 Endianness e Representação de Tipos

Arquivos `.class` armazenam valores multibyte em **big-endian**. Como CPUs x86/x64 são little-endian, toda leitura de `u2` ou `u4` passa por conversão:

```c
fread(&cf->major_version, sizeof(uint16_t), 1, file);
cf->major_version = byteswap_u2(cf->major_version); // big → little
```

Os slots da Pilha de Operandos e do Vetor de Variáveis Locais são `uint32_t`. Conversões entre tipos são feitas via `memcpy` para evitar undefined behavior:

```c
// Correto: analisa os bits sem conversão numérica
int32_t value;
memcpy(&value, &raw, sizeof(int32_t));

// Errado: cast direto pode causar UB ou resultado incorreto
int32_t value = (int32_t) raw; // evitado no projeto
```

---

## 🚧 Limitações Conhecidas

- **Ponteiros como referências**: objetos e arrays são representados como ponteiros convertidos para `uint32_t`, o que funciona apenas em ambientes de 32 bits ou requer adaptação para 64 bits.
- **Sem coletor de lixo**: objetos e arrays alocados por `new`/`newarray` durante a execução não são liberados automaticamente.
- **Sem tratamento de exceções em runtime**: `NullPointerException`, `ArrayIndexOutOfBoundsException` e similares não são capturadas nem despachadas pela tabela de exceções do `Code`.
- **Sem verificador de bytecode**: a etapa de verificação formal (bytecode verifier) da JVM real não está implementada.
- **Biblioteca padrão Java limitada**: métodos nativos como `System.out.println` têm suporte parcial via `getstatic`/`invokevirtual` com tratamento especial no interpretador.
- **`invokedynamic` não implementado**: instruções dinâmicas (lambdas, streams) não são suportadas.

---

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

----

## 👨‍💻 Autores

Projeto desenvolvido por estudantes de Engenharia de Computação da Universidade de Brasília (UnB) para a disciplina de **Software Básico**.

- Caio Medeiros Balaniuk
- Davi Henrique Vieira Lima
- Lucas Saad Rodrigues
- Lucca Schoen de Almeida
- Maria Carolina Burgum Abreu Jorge

---

## 📚 Referências

- [The Java Virtual Machine Specification, Java SE 26 — Chapter 4: The class File Format](https://docs.oracle.com/en/java/javase/26/docs/specs/jvms/jvms-4.html)
- [The Java Virtual Machine Specification, Java SE 26 — Chapter 6: The Java Virtual Machine Instruction Set](https://docs.oracle.com/en/java/javase/26/docs/specs/jvms/jvms-6.html)
- Lindholm, T. & Yellin, F. — *The Java Virtual Machine Specification, Second Edition*
- Material de apoio da disciplina
