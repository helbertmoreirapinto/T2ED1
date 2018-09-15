/** Declaracao dos imports do sistema **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Math.h>

/** Declaracao dos define's de liberacao de memoria **/
#define LIBERA_ATE_INICIO 0
#define LIBERA_UNICA 1
#define LIBERA_ATE_FIM 2

/** Declaracao dos define's de comandos **/
#define INSERIR_ANTES   'i'
#define INSERIR_DEPOIS  'a'
#define MODO_INSERT     'r'
#define BUSCAR_PALAVRA  'f'
#define REMOVER_PALAVRA 'd'
#define AVANCAR_UM      'n'
#define VOLTAR_UM       'p'
#define CURSOR_INICIO   'b'
#define CURSOR_FIM      'e'
#define CURSOR_X        'g'
#define IMPRIMIR_TEXTO  's'

/** Declaracao dos define's de retorno de funcao **/
#define ERROR 0
#define OK 1

/** Declaracao da struct principal **/
struct Elemento{
    char *palavra;
    bool noEspecial;
    struct Elemento* ant;
    struct Elemento* prox;
};

/** Declaracao de uma struct unitaria que servira como no descitor **/
struct{
    struct Elemento* prim;
    struct Elemento* atual;
    struct Elemento* ult;
    int tamLista;
} descr;

/** Declaracao dos typedef's **/
typedef struct Elemento Elemento;
typedef Elemento* Elemento_PTR;
typedef Elemento** Elemento_PTR_PTR;

/** Declaracao dos prototipos das funcoes **/
void liberar_memoria(Elemento_PTR_PTR, int);
void inicializar_descritor();
void imprimir_texto();
int ler_arquivo(char*);
void executar_comando(char, char*);
bool verificar_palavra_especial(char*);
int inserir_palavra(char, char*, bool);
int buscar_palavra(char*);
int substuir_palavra(char*, bool);
int remover_palavra();
int mudar_pos_cursor(int);
void mudar_pos_cursor_inicio();
void mudar_pos_cursor_fim();
int converter_char_to_int(char*);
void remover_enter(char*);

/** Funcao principal **/
int main(){
    inicializar_descritor();

    /* Primeira parte do programa: ler o arquivo inicial */
    char *nomeArquivo;
    nomeArquivo = (char*)calloc(2, sizeof(char));
    if(!nomeArquivo){
        printf("Erro ao alocar memoria!\n");
        exit(0);
    }
    fscanf(stdin, "%c", &nomeArquivo[0]);

    if(!ler_arquivo(nomeArquivo))
        printf("Erro ao ler o arquivo!\n");
//    free(nomeArquivo);

    /* Segunda parte do programa: alterar os dados lidos */
    char comando;
    char* palavra = NULL;
    while(comando != 's'){
        setbuf(stdin, NULL);
        fscanf(stdin, "%c", &comando);
        if(fgetc(stdin) == ' '){
            palavra = (char*) malloc(100 * sizeof(char));
            fgets(palavra, 100, stdin);
        }
        executar_comando(comando, palavra);
        palavra = NULL;
    }
//    liberar_memoria(descr.prim);
    return 0;
}

/** Funcao muda cursor para inicio da lista **/
void mudar_pos_cursor_inicio(){
    descr.atual = descr.prim;
}

/** Funcao muda cursor para fim da lista **/
void mudar_pos_cursor_fim(){
    descr.atual = descr.ult;
}

/** Funcao que remove o enter do fim da palavra **/
void remover_enter(char* palavra){
    int tam = strlen(palavra);
    for(int i = 0; i < tam; i++)
        if(palavra[i] == '\n'){
            palavra[i] = '\0';
            return;
        }
}

int remover_palavra(){
    Elemento_PTR aux;
    if(descr.atual){
        aux = descr.atual;
        if(descr.atual->ant)
            descr.atual->ant->prox = descr.atual->prox;
        else
            descr.prim = descr.atual->prox;
        if(descr.atual->prox){
            descr.atual->prox->ant = descr.atual->ant;
            descr.atual = descr.atual->prox;
        }else{
            descr.ult = descr.atual->ant;
            descr.atual = descr.ult;
        }
        descr.tamLista--;
        liberar_memoria(&aux, LIBERA_UNICA);
        return OK;
    }
    return ERROR;
}

int buscar_palavra(char* palavra){
    remover_enter(palavra);
    Elemento_PTR aux = descr.prim;
    int cont = 0;
    while(aux){
        if(!strcmp(palavra, aux->palavra))
            return cont;
        cont++;
        aux = aux->prox;
    }
    return -1;
}

int mudar_pos_cursor(int qtd){
    while(descr.atual){
        if(qtd > 0){
            qtd--;
            descr.atual = descr.atual->prox;
        }else if(qtd < 0){
            qtd++;
            descr.atual = descr.atual->ant;
        }
        if(!qtd)
            return OK;
        if(!descr.atual){
            descr.atual = (descr.prim) ? descr.prim :((descr.ult) ? descr.ult : NULL);
            return ERROR;
        }
    }
    return ERROR;
}

int substuir_palavra(char* palavra, bool especial){
    if(descr.atual){
        free(descr.atual->palavra);
        descr.atual->palavra = palavra;
        descr.atual->noEspecial = especial;
        return OK;
    }
    return ERROR;
}

int inserir_palavra(char ordem, char* palavra, bool especial){
    Elemento_PTR no = (Elemento_PTR) malloc(sizeof(Elemento));
    if(!no || !palavra)
        return ERROR;
    no->palavra = palavra;
    no->noEspecial = especial;

    if(ordem == INSERIR_ANTES){
        if(descr.atual){
            no->ant = descr.atual->ant;
            if(descr.atual->ant)
                descr.atual->ant->prox = no;
            descr.atual->ant = no;
        }else
            no->ant = NULL;
        no->prox = descr.atual;
        if(descr.atual == descr.prim)
            descr.prim = no;
    }else if(ordem == INSERIR_DEPOIS){
        no->ant = descr.atual;
        if(descr.atual){
            no->prox = descr.atual->prox;
            if(descr.atual->prox)
                descr.atual->prox->ant = no;
            descr.atual->prox = no;
        }else
            no->prox = NULL;
        if(descr.atual == descr.ult)
            descr.ult = no;
    }
    descr.atual = no;

    if((descr.prim && !descr.ult) || (!descr.prim && descr.ult))
        descr.prim = descr.atual = descr.ult = no;
    descr.tamLista++;
    return OK;
}

bool verificar_palavra_especial(char* palavra){
    remover_enter(palavra);
    int tamPalavra = strlen(palavra);
    bool range_comum;
    for(int i = 0; i < tamPalavra; i++){
        range_comum  =  (palavra[i] >= 'a' && palavra[i] <= 'z') ||
                        (palavra[i] >= 'A' && palavra[i] <= 'Z') ||
                        (palavra[i] >= '0' && palavra[i] <= '9');
        if(!range_comum)
            return true;
    }
    return false;
}

void executar_comando(char comando, char* palavra){
    int aux = 0;
    switch(comando){
        case INSERIR_ANTES:
            inserir_palavra(INSERIR_ANTES, palavra, verificar_palavra_especial(palavra));
            break;
        case INSERIR_DEPOIS:
            inserir_palavra(INSERIR_DEPOIS, palavra, verificar_palavra_especial(palavra));
            break;
        case BUSCAR_PALAVRA:
            aux = buscar_palavra(palavra);
            if(aux > -1)
                printf("%d\n", aux);
            break;
        case MODO_INSERT:
            if(!substuir_palavra(palavra, verificar_palavra_especial(palavra)))
                printf("Erro ao substituir palavra\n");
            break;
        case REMOVER_PALAVRA:
            if(!remover_palavra())
                printf("Erro ao remover palavra\n");
            break;
        case AVANCAR_UM:
            if(!mudar_pos_cursor(1))
                printf("Erro ao mover posicao do cursor atual\n");
            break;
        case VOLTAR_UM:
            if(!mudar_pos_cursor(-1))
                printf("Erro ao mover posicao do cursor atual\n");
            break;
        case CURSOR_INICIO:
            mudar_pos_cursor_inicio();
            break;
        case CURSOR_FIM:
            mudar_pos_cursor_fim();
            break;
        case CURSOR_X:
            if(!mudar_pos_cursor(converter_char_to_int(palavra)))
                printf("Erro ao mover posicao do cursor atual\n");
            break;
        case IMPRIMIR_TEXTO:
            imprimir_texto();
            exit(0);
        default:
            printf("comando desconhecido\n");
        }
}

int converter_char_to_int(char* numStr){
    remover_enter(numStr);
    int tam = strlen(numStr);
    int num = 0;
	int i = 0;
    for(i = tam; i > 0; i--){
        if(numStr[i-1] >= '0' && numStr[i-1] <= '9')
            num += (numStr[i-1] - '0') * pow(10, (tam-i));
        else if(numStr[i-1] == '-')
            num *= -1;
    }
    return num;
}

void liberar_memoria(Elemento_PTR_PTR no, int ord){
    Elemento_PTR aux;
    while(*no){

        if(ord == LIBERA_ATE_INICIO) aux = (*no)->ant;
        else if(ord == LIBERA_UNICA) aux = NULL;
        else if(ord == LIBERA_ATE_FIM) aux = (*no)->prox;

        free((*no)->palavra);
        free(*no);
        *no = aux;
    }
    free(no);
}

void inicializar_descritor(){
    descr.prim = descr.atual = descr.ult = NULL;
    descr.tamLista = 0;
}

void imprimir_texto(){
    Elemento_PTR palavra = descr.prim;
    bool excessoes;

    while(palavra){
        printf("%s",palavra->palavra);

        excessoes = strcmp(palavra->palavra, "\n") &&
                    strcmp(palavra->palavra,  "-") &&
                    strcmp(palavra->palavra, "\'") &&
                    strcmp(palavra->palavra, "\"");

        if(palavra->prox && !palavra->prox->noEspecial && excessoes)
            printf(" ");
        palavra = palavra->prox;
    }
}

int ler_arquivo(char* numFile){
    char extensao[5] = ".ext\0";
    char *nmFile = (char*) malloc(6*sizeof(char));
    nmFile = strcat(numFile, extensao);
    FILE* arquivo = fopen(nmFile, "r");
//    free(nmFile);

    if(!arquivo)
        return ERROR;

    char carac;
    char* string = NULL;
    int cont = 0;
    bool range_comum;
    while(!feof(arquivo)){
        carac = fgetc(arquivo);
        range_comum = (carac >= 'a' && carac <= 'z') ||
                (carac >= 'A' && carac <= 'Z') ||
                (carac >= '0' && carac <= '9');

        if(range_comum){
            cont++;
            string = (char*) realloc(string, (cont+1) * sizeof(char));
            string[cont-1] = carac;
            string[cont] = '\0';
        }else{
            if(string)
                inserir_palavra(INSERIR_DEPOIS, string, false);
            if(carac != ' ' && carac != -1){
                string = (char*) malloc(2 * sizeof(char));
                string[0] = carac;
                string[1] = '\0';
                inserir_palavra(INSERIR_DEPOIS, string, true);
            }
            cont = 0;
            string = NULL;
        }
    }
    fclose(arquivo);
    return OK;
}
