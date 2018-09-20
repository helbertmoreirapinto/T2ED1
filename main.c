/** Declaracao dos imports do sistema **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

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

/** Declaracao dos typedef's **/
typedef struct Elemento Elemento;
typedef Elemento* Elemento_PTR;

/** Declaracao de uma struct unitaria que servira como no descitor **/
/** Declarei esta variavel global pois ela nao eh tipada **/
struct{
    Elemento_PTR prim;
    Elemento_PTR atual;
    Elemento_PTR ult;
} descr;

/** Declaracao dos prototipos das funcoes **/
void remover_caracteres_fora_ASCII(char*);
void liberar_memoria(Elemento_PTR, int);
int inserir_palavra(char, char*, bool);
bool verificar_palavra_especial(char*);
void executar_comando(char, char*);
int substuir_palavra(char*, bool);
int converter_char_to_int(char*);
void mudar_pos_cursor_inicio();
void inicializar_descritor();
void mudar_pos_cursor_fim();
int mudar_pos_cursor(int);
int buscar_palavra(char*);
int ler_arquivo(char*);
int remover_palavra();
void imprimir_texto();
int main();

/** Funcao principal **/
int main(){
    inicializar_descritor();

    /* Primeira parte do programa: ler o arquivo inicial */
    /* Usuario insere numero do caso de teste [numero do nome do arquivo] */
    char *nomeArquivo;
    nomeArquivo = (char*) calloc(6, sizeof(char));
    if(!nomeArquivo){
        printf("Erro ao alocar memoria!\n");
        exit(0);
    }
	setbuf(stdin, NULL);
    fscanf(stdin, "%c", &nomeArquivo[0]);

    if(!ler_arquivo(nomeArquivo))
        printf("Erro ao ler o arquivo!\n");
    free(nomeArquivo);

    /* Segunda parte do programa: alterar os dados lidos */
    char* linhaComando	= NULL;
    char* parametro		= NULL;
    char comando		= '\0';

    /* Enquanto o comando for diferente de imprimir continua a execucao */
    while(comando != 's'){
        linhaComando = (char*) malloc(100 * sizeof(char));
        setbuf(stdin, NULL);
        fgets(linhaComando, 100, stdin);
		comando = linhaComando[0];
        if(comando < 'a' || comando > 'z')
            continue;
        if(linhaComando[1] == ' ')
            for(int i = 2; i < 100; i++){
                parametro = (char*) realloc(parametro, i * sizeof(char));
                if(linhaComando[i] == '\n' || linhaComando[i] == '\0'){
                    parametro[i - 2] = '\0';
                    break;
                }
                parametro[i - 2] = linhaComando[i];
            }
        executar_comando(linhaComando[0], parametro);
        free(linhaComando);
        parametro = NULL;
    }
    liberar_memoria(descr.prim, LIBERA_ATE_FIM);
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

/** Funcao que remove caracteres nao imprimiveis **/
void remover_caracteres_fora_ASCII(char* palavra){
    /* Verifica se o conteudo eh igual a <ENTER> */
	if(!strcmp(palavra, "<ENTER>")){
        strcpy(palavra, "\n\0");
        return;
    }
    int tam = strlen(palavra);
    for(int i = 0; i < tam; i++)
        if(palavra[i] < '!' || palavra[i] > '~'){ /* '!'(33) - '~'(126) Faixa de caracteres que podem ser impressos */
            palavra[i] = '\0';
            return;
        }
}

/** Funcao que remove uma palavra do texto **/
int remover_palavra(){
    Elemento_PTR aux;
    if(descr.atual){
        aux = descr.atual;
        if(descr.atual->ant)
            descr.atual->ant->prox = descr.atual->prox;
        else
            descr.prim = descr.atual->prox;

        if(descr.atual->prox)
            descr.atual->prox->ant = descr.atual->ant;
        else
            descr.ult = descr.atual->ant;

        descr.atual = (descr.atual->prox) ? descr.atual->prox :((descr.atual->ant) ? descr.atual->ant : NULL); /* Nova posicao do cursor */
        liberar_memoria(aux, LIBERA_UNICA);
        return OK;
    }
    return ERROR;
}

/** Funcao que busca na lista uma palavra identica a do parametro **/
int buscar_palavra(char* palavra){
    remover_caracteres_fora_ASCII(palavra);
    Elemento_PTR aux = descr.atual;
    Elemento_PTR ini = descr.prim;
    int cont = 0;
    while(aux){
        if(!strcmp(palavra, aux->palavra)){ /* Se achou a palavra apartir do cursor */
            while(ini){
                if(ini == aux)              /* Vai ate o ponteiro onde estava a palavra encontrada */
                    return cont;
                cont++;                     /* Comeca a contagem apartir do inicial */
                ini = ini->prox;
            }
        }
        aux = aux->prox;
    }
    return -1;
}

/** Funcao que altera o posionamento atual do cursor **/
/** .Parametro Negativo -> retarda posicionamento **/
/** .Parametro Positivo -> avanca posicionamento **/
int mudar_pos_cursor(int qtd){
    while(descr.atual){
        if(qtd > 0){
            qtd--;
            if(descr.atual->prox)
                descr.atual = descr.atual->prox;
        }else if(qtd < 0){
            qtd++;
            if(descr.atual->ant)
                descr.atual = descr.atual->ant;
        }
        if(!qtd)
            return OK;
        if(!descr.atual){
            descr.atual = (descr.prim) ? descr.prim :((descr.ult) ? descr.ult : NULL); /* So para nao dar erro */
            return ERROR;
        }
    }
    return ERROR;
}

/** Funcao que substitui a palavra do no atual pela palavra que recebe via parametro **/
int substuir_palavra(char* palavra, bool especial){
    if(descr.atual){
        free(descr.atual->palavra);
        descr.atual->palavra = palavra;
        descr.atual->noEspecial = especial;
        return OK;
    }
    return ERROR;
}

/** Funcao que insere as palavras em nos e depois os insere na lista **/
int inserir_palavra(char ordem, char* palavra, bool especial){
    Elemento_PTR no = (Elemento_PTR) malloc(sizeof(Elemento));
    if(!no || !palavra)
        return ERROR;
    no->palavra = palavra;
    no->noEspecial = especial;

    if(ordem == INSERIR_ANTES){             /** Comando 'i' **/
        if(descr.atual){                    /* Se nao eh a primeira palavra */
            no->ant = descr.atual->ant;
            if(descr.atual->ant)
                descr.atual->ant->prox = no;
            descr.atual->ant = no;
        }else
            no->ant = NULL;
        no->prox = descr.atual;             /* O proximo do no sempre sera o proximo do cursor, mesmo q este (proximo) seja NULL */
        if(descr.atual == descr.prim)       /* Caso de inserir um no antes do primeiro, aletra o primeiro */
            descr.prim = no;
    }else if(ordem == INSERIR_DEPOIS){      /** Comando 'a' **/
        no->ant = descr.atual;              /* O anterior do no sempre sera o anreiror do cursor, mesmo q este (anterior) seja NULL */
        if(descr.atual){                    /* Se nao eh a primeira palavra */
            no->prox = descr.atual->prox;
            if(descr.atual->prox)
                descr.atual->prox->ant = no;
            descr.atual->prox = no;
        }else
            no->prox = NULL;
        if(descr.atual == descr.ult)        /* Caso de inserir um no depois do ultimo, aletra o ultimo */
            descr.ult = no;
    }

    if((descr.prim && !descr.ult) || (!descr.prim && descr.ult)) /* Se primeiro ou ultimo eh nulo, e o outro nao eh, entao so ha um no inserido na lista */
        descr.prim = descr.atual = descr.ult = no;
    return OK;
}

/** Funcao que verifica se o conteudo do vetor de char eh um conteudo com caracteres especiais **/
bool verificar_palavra_especial(char* palavra){
    remover_caracteres_fora_ASCII(palavra);

    int tamPalavra = strlen(palavra);
    bool faixaPadrao;
    for(int i = 0; i < tamPalavra; i++){
        faixaPadrao  =  (palavra[i] >= 'a' && palavra[i] <= 'z') ||
                        (palavra[i] >= 'A' && palavra[i] <= 'Z') ||
                        (palavra[i] >= '0' && palavra[i] <= '9');
        if(!faixaPadrao)
            return true;
    }
    return false;
}

/** Funcao que seleciona qual acao sera realizada de acordo com o comando **/
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
                printf("Erro ao mover posicao atual do cursor\n");
            break;
        case VOLTAR_UM:
            if(!mudar_pos_cursor(-1))
                printf("Erro ao mover posicao atual do cursor\n");
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
            printf("comando %d desconhecido\n", comando);
        }
}

/** Funcao que converte uma lista de char contendo um numero, em um numero inteiro **/
/** .Ex: (char*)"-53" -> (int)-53 **/
int converter_char_to_int(char* numStr){
    remover_caracteres_fora_ASCII(numStr);
    int tam = strlen(numStr);
    int num = 0;
	int i = 0;
    for(i = tam; i > 0; i--){ /* Do ultimo para o primeiro */
        if(numStr[i-1] >= '0' && numStr[i-1] <= '9')
            num += (numStr[i-1] - '0') * pow(10, (tam-i)); /* Elevado a uma potencia de 10 */
        else if(numStr[i-1] == '-') /* Caso de numeros negativos */
            num *= -1;
    }
    return num;
}

/** Funcao deasaloca os espacos de memoria utilizados **/
/** [recebe a ordenacao que a memoria sera desalocada] **/
void liberar_memoria(Elemento_PTR no, int ord){
    Elemento_PTR aux;
    while(no){

        if(ord == LIBERA_ATE_INICIO) aux = no->ant; /* Libera a memoria do ponteiro do parametro ate o inicio */
        else if(ord == LIBERA_UNICA) aux = NULL; /* Libera a memoria apenas do ponteiro do parametro */
        else if(ord == LIBERA_ATE_FIM) aux = no->prox; /* Libera a memoria do ponteiro do parametro ate o fim */

        free(no->palavra);
        free(no);
        no = aux;
    }
}

/** Funcao que inicializa a variavel de descritor **/
void inicializar_descritor(){
    descr.prim = descr.atual = descr.ult = NULL;
}

/** Funcao que exibe o conteudo da lista encadeada **/
void imprimir_texto(){
    Elemento_PTR palavra = descr.prim;
    bool excessoes;
    while(palavra){
        printf("%s",palavra->palavra);
		/* Excessoes que considerei que nao deveriam imprimir espaco apos */
        excessoes = strcmp(palavra->palavra, "\n") &&
                    strcmp(palavra->palavra,  "-") &&
                    strcmp(palavra->palavra, "\'") &&
                    strcmp(palavra->palavra, "\"");

        if(palavra->prox && !palavra->prox->noEspecial && excessoes)
            printf(" ");
        palavra = palavra->prox;
    }
}

/** Funcao que le o arquivo [recebe o numero do arquivo como parametro] **/
int ler_arquivo(char* nmArquivo){
    nmArquivo = strcat(nmArquivo, ".ext\0");
    FILE* arquivo = fopen(nmArquivo, "r");
    if(!arquivo)
        return ERROR;

    char carac;
    char* palavra = NULL;
    int cont = 0;
    bool faixaPadrao;
    while(!feof(arquivo)){
        carac = fgetc(arquivo); /* Pegar carctere por caractere pq no arquivo pode ter palavras hibridas. Ex: Hello! */
        faixaPadrao = (carac >= 'a' && carac <= 'z') ||
                (carac >= 'A' && carac <= 'Z') ||
                (carac >= '0' && carac <= '9');

        if(faixaPadrao){
            cont++;
            palavra = (char*) realloc(palavra, (cont+1) * sizeof(char));
            palavra[cont-1] = carac;
            palavra[cont] = '\0';
        }else{
            if(palavra){ /* Se esta aqui eh pq encontrou caracter que esta fora dos comuns */
                inserir_palavra(INSERIR_DEPOIS, palavra, false); /* Insere na lista a palavra que criei no if */
                mudar_pos_cursor_fim(); /* Muda a posicao do cursor para inserir de forma correta as palavras na lista */
            }
            if(carac != ' ' && carac != -1){ /* Unicos caracteres que serao ignorados ' '(32) e EOF(-1) */
                palavra = (char*) calloc(2, sizeof(char));
                if(!palavra)
                    printf("Erro ao alocar memoria!\n");
                palavra[0] = carac;
                inserir_palavra(INSERIR_DEPOIS, palavra, true); /* Insere o caracter especial na lista de palavras */
                mudar_pos_cursor_fim(); /* Muda a posicao do cursor para inserir de forma correta as palavras na lista */
            }
            cont = 0;
            palavra = NULL;
        }
    }
    mudar_pos_cursor_inicio(); /* Retorna a posicao do cursor para o inicio */

    fclose(arquivo);
    return OK;
}
