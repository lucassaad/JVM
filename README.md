# Implementação de uma Java Virtual Machine (JVM) em C

Implementação em C de uma **Java Virtual Machine (JVM)** capaz de ler, analisar e executar arquivos bytecode Java (`.class`). O projeto é desenvolvido como trabalho da disciplina de **Software Básico** da Universidade de Brasília (UnB) e segue as definições da **Java Virtual Machine Specification (JVMS)**.

O programa opera em dois modos distintos: um **Leitor/Exibidor** estático que decodifica e imprime a estrutura interna de arquivos `.class` em formato legível, e um **Interpretador (Motor de Execução)** que implementa o ciclo Fetch-Decode-Execute da JVM, executando o bytecode Java diretamente.

---

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
# Com o arquivo padrão (Main.class)
make run-leitor

# Com um arquivo específico
make run-leitor FILE=exemplos/fibonacci.class
```

### 3. Executar o Interpretador

```bash
# Com o arquivo padrão (Main.class)
make run

# Com um arquivo específico
make run FILE=exemplos/fatorial.class
make run FILE=exemplos/fibonacci.class
make run FILE=exemplos/double_aritmetica.class
```

### 4. Limpar arquivos de compilação

```bash
make clean
```

### Resumo dos comandos `make`

| Comando | Descrição |
|---|---|
| `make` | Apenas compila, gerando o binário em `bin/`. |
| `make run` | Compila e executa no modo Interpretador (padrão). |
| `make run-leitor` | Compila e executa no modo Leitor/Exibidor. |
| `make run-interpreter` | Equivalente a `make run`. |
| `make clean` | Remove as pastas `obj/` e `bin/`. |

### Flags de compilação

O projeto compila com `-std=c99 -Wall -Wextra -g`:

- **`-std=c99`**: Garante portabilidade e diz ao compilador para usar a versão C99 da linguagem C (padrão da disciplina).
- **`-Wall` / `-Wextra`**: Ativa todos os avisos do compilador, prevenindo bugs silenciosos.
- **`-g`**: Inclui símbolos de debug.

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
