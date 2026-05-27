# Leitor e Exibidor de Arquivos `.class` (JVM)

Implementação em C de um leitor e analisador estrutural de arquivos binários Java (`.class`). Este projeto foi desenvolvido para decodificar e exibir as estruturas internas do bytecode Java em um formato humanamente legível, seguindo as definições da **Java Virtual Machine Specification (Capítulo 4: The class File Format)**.

## 🎯 Objetivo

O programa atua como um analisador estático (semelhante à ferramenta `javap` ou à interface do *jclasslib*). Ele abre o arquivo binário, interpreta os campos respeitando o *endianness* correto, armazena as informações em memória e exibe o raio-X completo da classe. **Nota:** O projeto foca na leitura e validação estrutural, não sendo um motor de execução de bytecode.

## ⚙️ Funcionalidades Implementadas

* **Informações Gerais:** Leitura e validação do *Magic Number* (`0xCAFEBABE`), *Minor/Major Versions* (suporte a identificação até o Java 25) e *Access Flags* da classe.
* **Constant Pool:** Resolução completa das tags e mapeamento das referências indiretas (ex: resolução automática do nome real de um `Methodref` ou `Class`). Tratamento nativo para o salto de índices de `Long` e `Double`.
* **Fields e Methods:** Leitura e tradução das *access flags* (ex: de `0x000a` para `[private static]`), além da exibição de seus nomes e descritores associados à Constant Pool.
* **Attributes (Genéricos):** Identificação e listagem sumarizada de atributos em Classes, Fields e Methods (`SourceFile`, etc.).
* **Code Attribute & Disassembly:** Desempacotamento focado no atributo `Code`, com extração de informações de pilha (`max_stack`, `max_locals`) e um *disassembly* inicial convertendo os opcodes em instruções mnemônicas (ex: `aload_0`, `invokevirtual`).

## 🚀 Como Compilar e Executar

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



## 📂 Arquitetura do Projeto

A organização dos arquivos espelha diretamente as diretrizes da especificação da JVM:

| Diretório/Arquivo | Responsabilidade |
| --- | --- |
| `include/` | Cabeçalhos e definição das `structs` oficiais da JVM (`ClassFile`, `cp_info`, etc). |
| `src/main.c` | Orquestração: abre o arquivo, aloca a estrutura, comanda a leitura e limpeza. |
| `src/reader.c` | Faz o *parsing* binário sequencial dos campos do arquivo para a memória. |
| `src/constant_pool.c` | Lê, estrutura e valida as referências cruzadas dentro da Constant Pool. |
| `src/code_attribute.c` | Isola a lógica para encontrar e extrair o atributo `Code` e seus metadados. |
| `src/instruction*.c` | Contém o dicionário de opcodes da JVM e faz a tradução de bytes para instruções. |
| `src/viewer.c` | O módulo visualizador. Navega pela estrutura na memória e imprime o relatório em tela. |
| `src/utils.c` | Funções essenciais para conversão de *Endianness* (`byteswap_u2`, `byteswap_u4`). |

## 🚧 Limitações Conhecidas

Como se trata de um analisador parcial, algumas estruturas ainda estão fora do escopo atual:

* Opcodes não mapeados são exibidos genericamente como `unknown opcode 0xXX`.
* Sub-atributos aninhados dentro do `Code` (como `LineNumberTable` e `ExceptionTable`) são calculados e pulados (`fseek`), não sendo detalhados no terminal.
* Não executa validação profunda de restrições lógicas do bytecode (Verification).

## 👨‍💻 Autores

Projeto desenvolvido por estudantes de Engenharia de Computação da Universidade de Brasília (UnB) para a disciplina de Software Básico.

* Caio Medeiros Balaniuk
* Davi Henrique Vieira Lima
* Lucas Saad Rodrigues
* Lucca Schoen de Almeida
* Maria Carolina Burgum Abreu Jorge

## 📚 Referências

* [The Java Virtual Machine Specification (Java SE 26) - Chapter 4: The class File Format](https://docs.oracle.com/en/java/javase/26/docs/specs/jvms/jvms-4.html)
* Material de apoio da disciplina (`classfile-85-117.pdf`).
