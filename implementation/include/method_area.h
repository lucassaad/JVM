/**
 * @file method_area.h
 * @brief Área de Métodos da JVM: carregamento, cache e resolução de classes e métodos.
 *
 * A Method Area é a região de memória compartilhada (por thread, nesta implementação
 * single-thread global) que armazena as classes carregadas, seus bytecodes e metadados.
 * Este módulo gerencia o ciclo de vida das classes: inicialização, registro, busca
 * em cache e carregamento sob demanda a partir do sistema de arquivos.
 *
 * Também fornece funções de resolução simbólica usadas pelo interpretador durante
 * a execução: resolução de classe por índice no constant pool, cálculo de offset
 * de campo em objetos e busca de método por nome e descritor.
 */

#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include "class_file.h"
#include "attributes.h"

/**
 * @brief Resolve uma referência simbólica a uma classe a partir do constant pool.
 *
 * Lê a entrada CONSTANT_Class no índice @p index do @p constant_pool fornecido,
 * obtém o nome binário da classe e a retorna da Method Area, carregando-a do
 * disco se ainda não estiver em cache.
 *
 * @param constant_pool Array de ponteiros para as entradas do constant pool do frame corrente.
 * @param index         Índice no constant pool para a entrada CONSTANT_Class a resolver.
 * @return Ponteiro para o ClassFile da classe resolvida, ou NULL em falha.
 */
ClassFile* method_area_resolve_class(cp_info **constant_pool, uint16_t index);

/**
 * @brief Calcula o offset (em slots de 32 bits) de um campo em um objeto.
 *
 * Percorre os campos de instância de @p class_ref na ordem de declaração,
 * somando 1 slot para tipos de 32 bits e 2 slots para `long` e `double`,
 * até encontrar o campo identificado por @p indexbyte no @p frame_cp.
 *
 * @param class_ref  ClassFile da classe que declara o campo.
 * @param frame_cp   Constant pool do frame corrente, usado para resolver o nome e descritor do campo.
 * @param indexbyte  Índice no constant pool do frame para a entrada CONSTANT_Fieldref do campo.
 * @return Offset do campo em slots de 32 bits dentro do array de campos do objeto.
 */
uint16_t resolve_field_offset(ClassFile *class_ref, cp_info **frame_cp, uint16_t indexbyte);

/**
 * @brief Busca um método em um ClassFile por nome, descritor e flags de acesso.
 *
 * Percorre o array de métodos de @p cf comparando nome e descritor com as
 * entradas do constant pool. Se @p required_flags for diferente de zero, apenas
 * métodos cujos @p access_flags contenham todos os bits indicados são retornados.
 *
 * @param cf             ClassFile onde o método será buscado.
 * @param name           Nome simples do método (e.g. "main", "<init>").
 * @param descriptor     Descritor de assinatura do método (e.g. "([Ljava/lang/String;)V").
 * @param required_flags Máscara de bits de access_flags que o método deve possuir (0 = sem filtro).
 * @return Ponteiro para o method_info encontrado, ou NULL se não encontrado.
 */
method_info *method_area_find_method(ClassFile *cf, const char *name, const char *descriptor, uint16_t required_flags);

/**
 * @brief Obtém o atributo Code de um método.
 *
 * Percorre os atributos do @p method informado e retorna o primeiro atributo
 * cujo nome seja "Code". Métodos nativos ou abstratos não possuem Code attribute.
 *
 * @param cf     ClassFile da classe do método (usado para resolver nomes do constant pool).
 * @param method Ponteiro para o method_info cujo Code attribute será buscado.
 * @return Ponteiro para o Code_attribute do método, ou NULL se não encontrado.
 */
Code_attribute *method_area_get_code(ClassFile *cf, method_info *method);

/**
 * @brief Área de Métodos global da JVM.
 *
 * Armazena todas as classes carregadas durante a execução. O array @p classes
 * é redimensionado dinamicamente conforme novas classes são registradas.
 */
typedef struct {
    ClassFile **classes;  /**< Array de ponteiros para os ClassFiles carregados. */
    uint16_t class_count; /**< Número de classes atualmente registradas. */
    uint16_t capacity;    /**< Capacidade atual do array (cresce dinamicamente por realloc). */
} MethodArea;

/**
 * @brief Instância global da Method Area.
 *
 * Declarada como extern para ser acessível por todos os módulos do interpretador.
 * Deve ser inicializada com method_area_init() antes de qualquer uso.
 */
extern MethodArea method_area;

/**
 * @brief Inicializa a Method Area global.
 *
 * Aloca o array inicial de classes com capacidade padrão e zera os contadores.
 * Deve ser chamada uma única vez na inicialização da JVM, antes de carregar
 * qualquer classe.
 */
void method_area_init(void);

/**
 * @brief Registra um ClassFile já carregado na Method Area.
 *
 * Adiciona @p cf ao array de classes. Se a capacidade estiver esgotada,
 * o array é realocado com o dobro do tamanho anterior.
 *
 * @param cf Ponteiro para o ClassFile a registrar.
 */
void method_area_register(ClassFile *cf);

/**
 * @brief Busca uma classe já carregada na Method Area pelo nome binário.
 *
 * Percorre o array de classes comparando o nome binário (e.g. "java/lang/Object")
 * com a entrada CONSTANT_Utf8 referenciada pelo campo this_class de cada ClassFile.
 *
 * @param class_name Nome binário da classe a buscar (e.g. "exemplos/Fibonacci").
 * @return Ponteiro para o ClassFile encontrado, ou NULL se a classe não estiver carregada.
 */
ClassFile *method_area_find_loaded(const char *class_name);

/**
 * @brief Carrega uma classe do disco e a registra na Method Area.
 *
 * Verifica primeiro se a classe já está em cache via method_area_find_loaded().
 * Caso contrário, constrói o caminho "exemplos/<class_name>.class", lê o arquivo
 * com read_classfile() e registra o ClassFile resultante na Method Area.
 *
 * @param class_name Nome binário da classe a carregar (e.g. "exemplos/Fibonacci").
 * @return Ponteiro para o ClassFile carregado (ou já em cache), ou NULL em falha.
 */
ClassFile *method_area_load(const char *class_name);

#endif
