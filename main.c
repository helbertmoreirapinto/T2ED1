#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LIBERA_ATE_INICIO 0
#define LIBERA_UNICA 1
#define LIBERA_ATE_FIM 2

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
void exibir_lista();
int ler_arquivo(char*);
void inserir_final(char*, bool);
void executar_comando(char, char*);

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
    while(comando != 's'){
        setbuf(stdin, NULL);
        fscanf(stdin, "%c", &comando);
        if(fgetc(stdin) == ' '){
            fgets(palavra, 100, stdin);
            printf("%s", palavra);
        }
        executar_comando(comando, palavra);
    }


//    liberar_memoria(descr.prim);
    return 0;
}

void executar_comando(char comando, char* palavra){
    switch(comando){
            case 'i':
                break;
            case 'a':
                break;
            case 'r':
                break;
            case 'f':
                break;
            case 'd':
                break;
            case 'n':
                break;
            case 'p':
                break;
            case 'b':
                break;
            case 'e':
                break;
            case 'g':
                break;
            case 's':
                exibir_lista();
                break;
            default:
                printf("comando desconhecido\n");
        }
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

void exibir_lista(){
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
    exit(0);
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
                inserir_final(string, false);
            if(carac != ' ' && carac != -1){
                string = (char*)malloc(2 * sizeof(char));
                string[0] = carac;
                string[1] = '\0';
                inserir_final(string, true);
            }
            cont = 0;
            string = NULL;
        }
    }
    fclose(arquivo);
    return OK;
}

void inserir_final(char* string, bool espec){
    Elemento_PTR no = malloc(sizeof(Elemento));
    no->palavra = string;
    no->noEspecial = espec;
    no->prox = NULL;
    no->ant = descr.ult;
    if(descr.ult)
        descr.ult->prox = no;
    else
        descr.prim = no;
    descr.atual = no;
    descr.ult = no;
    descr.tamLista++;
}
