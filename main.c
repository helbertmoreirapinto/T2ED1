#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Math.h>

#define LIBERA_ATE_INICIO 0
#define LIBERA_UNICA 1
#define LIBERA_ATE_FIM 2

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

#define ERROR 0
#define OK 1


struct Elemento{
    char *palavra;
    bool noEspecial;
    struct Elemento* ant;
    struct Elemento* prox;
};

struct{
    struct Elemento* prim;
    struct Elemento* atual;
    struct Elemento* ult;
    int tamLista;
} descr;

typedef struct Elemento Elemento;
typedef Elemento* Elemento_PTR;
typedef Elemento** Elemento_PTR_PTR;

/** Declaracao dos prototipos das funcoes **/
void liberar_memoria(Elemento_PTR_PTR, int);
void inicializar_descritor();
void imprimir_texto();
int ler_arquivo(char*);
void executar_comando(char, char*, bool);
bool verificar_palavra_especial(char*);
int inserir_palavra(char, char*, bool);
int buscar_palavra(char*);
int substuir_palavra(char*, bool);
int remover_palavra();
int mudar_pos_cursor(int);
void mudar_pos_cursor_inicio();
void mudar_pos_cursor_fim();
int converter_char_to_int(char*);

int main(){
    inicializar_descritor();

    /* Primeira parte do programa: ler o arquivo inicial */
    char *nomeFile;
    nomeFile = (char*)calloc(2, sizeof(char));
    if(!nomeFile){
        printf("Erro ao alocar memoria!\n");
        exit(0);
    }
    fscanf(stdin, "%c", &nomeFile[0]);

    if(!ler_arquivo(nomeFile)){
        printf("Erro ao ler o arquivo!\n");
        exit(0);
    }
//    free(nomeFile);

    /* Segunda parte do programa: alterar os dados lidos */
    char comando;
    char palavra[100];
    bool especial;
    while(comando != 's'){
        setbuf(stdin, NULL);
        fscanf(stdin, "%c", &comando);
        if(fgetc(stdin) == ' '){
            fgets(palavra, 100, stdin);
            printf("%s", palavra);
        }
        especial = (comando != 'g') ? verificar_palavra_especial(palavra) : false;
        executar_comando(comando, palavra, especial);
    }
//    liberar_memoria(descr.prim);
    return 0;
}

void mudar_pos_cursor_inicio(){
    descr.atual = descr.prim;
}

void mudar_pos_cursor_fim(){
    descr.atual = descr.ult;
}

int remover_palavra(){
    Elemento_PTR aux;
    if(descr.atual){
        aux = descr.atual;
        if(descr.atual->ant)
            descr.atual->ant->prox = descr.atual->prox;
        if(descr.atual->prox)
            descr.atual->prox->ant = descr.atual->ant;
        free(aux);
        return OK;
    }
    return ERROR;
}

int buscar_palavra(char* palavra){
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
        if(!descr.atual)
            return ERROR;
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
    Elemento_PTR no = malloc(sizeof(Elemento));
    if(!no)
        return ERROR;
    no->palavra = palavra;
    no->noEspecial = especial;

    if(ordem == INSERIR_ANTES){
        if(descr.atual)
            no->ant = descr.atual->ant;
        no->prox = descr.atual;
    }else if(ordem == INSERIR_DEPOIS){
        no->ant = descr.atual;
        if(descr.atual)
            no->prox = descr.atual->prox;
    }

    if(descr.ult)
        descr.ult->prox = no;
    else
        descr.prim = no;
    descr.atual = no;
    descr.ult = no;
    descr.tamLista++;
    return OK;
}

bool verificar_palavra_especial(char* palavra){
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

void executar_comando(char comando, char* palavra, bool especial){
    int aux = 0;
    switch(comando){
        case INSERIR_ANTES:
            inserir_palavra(INSERIR_ANTES, palavra, especial);
            break;
        case INSERIR_DEPOIS:
            inserir_palavra(INSERIR_DEPOIS, palavra, especial);
            break;
        case BUSCAR_PALAVRA:
            aux = buscar_palavra(palavra);
            if(aux > -1)
                printf("%d\n", aux);
            break;
        case MODO_INSERT:
            if(!substuir_palavra(palavra, especial))
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
    char *nmFile = (char*)malloc(6*sizeof(char));
    nmFile = strcat(numFile, extensao);
    FILE* arquivo = fopen(nmFile, "r");
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
            string = (char*)realloc(string, (cont+1) * sizeof(char));
            string[cont-1] = carac;
            string[cont] = '\0';
        }else{
            if(string)
                inserir_palavra(INSERIR_DEPOIS, string, false);
            if(carac != ' ' && carac != -1){
                string = (char*)malloc(2 * sizeof(char));
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
