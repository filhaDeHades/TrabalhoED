#include "adiciona.h"
#include "pizza.h"

#define TAMFOLHA sizeof(long int)+sizeof(int)+sizeof(long int)*(T*2-1)+sizeof(long int)

//A ESTRURA DA ARVORE FOI MODIFICADA
//AINDA VOU VER QUAL DAS VARIAS EU NÃO USO
typedef struct arvbm{
  int nchaves, folha, *chave;
  struct arvbm **filho, *pai, *ant, *prox; //ant e prox apenas para as folhas
  struct pizza **pizzas;
  long int posPai, posProx, posAnt, *posPizza, *posFilho, posMeu;
}TABM;

//------------------------------- INICIALIZAÇÃO E CRIAÇÃO --------------------------------------------------------

//ABRE OS ARQUIVOS.
void abreAquivos();

//INICIALIZA ARVORE TABM EM MP
TABM *inicializa(void);

//INICIALIZA NÓ DA ARVORE TABM EM MP
TABM *cria(TPizza *p, int T);

//-------------------------------------------------------- IMPRIME -----------------------------------------------

//imprime as pizzas
void imprime(TABM *a, int t);

//imprime a arvore inteira
void imprime_arvore(TABM *a, int andar, int t);

//IMPRIME MENU COMPLETO
void listaMenu(int t);

// ------------------------------------- FUNÇÕES DE SUPORTE DA ARVORE EM MP ---------------------------------------------------

//ADICIONA O PAI NA ARVORE EM MP
TABM* addpai(TABM *a, int t);

//ADICONA PAI EM ARVORE NA MP
TABM* addproximo(TABM *a, TABM *prec, int t);

//BUSCA PIZZA NA MEMORIA PRINCIPAL
TABM* busca(TABM *a, int cod, int t);

//---------------------------- INSERÇÃO NA ARVORE EM MP ----------------------------------------

TABM *divisao(TABM *x, int i, TABM *y, int t);

TABM* insNCompleta(TABM *a, TPizza *p, int t);

TABM* insere(TABM *a, TPizza *p, int t);

// ------------------------------------------ FUNÇÃO DE LIBERA ---------------------------------------------------------------------------

//LIBERA ARVORE EM MP
void libera(TABM *a, int t);

// -------------------------------------------------------- FUNÇÃO EXCLUI EM MP -----------------------------------------------------------------------------

TABM* excluiNCompleto(TABM *x, int info, int t);

TABM *redistribuiPai(TABM *pai, int i, int t);

TABM* concatenaPai(TABM *pai, int i, int t);

TABM* concatenaIrmao(TABM *a, int i, TABM *y, int cmp, int t);

TABM* redistribui(TABM *a, int cmp, TABM *y, int t);

TABM* exclui(TABM *a, int info, int t);

//---------------------------------------------- FUNÇÃO EXCLUI NA MEMORIA SEGUNDARIA ---------------------------------------------------------------------------------------------------

//-------------------------------------------- FUNÇÕES PARA PASSAR A ARVORE DA MP PARA MS (ARQUIVO) ------------------------------------------------------------------------------------

//ESCREVE AS FOLHAS DA ARVORE DA MEMORIA PRINCIPAL
// ARQUIVO: "dados.dat"
//INFORMAÇÕES: NUMERO DE CHAVES, CHAVES, ENDEREÇO DAS PIZZAS, POSIÇÃO DA PROXIMA FOLHA
int escreveDados(TABM *a, int T);

//ESCREVE OS NOS INTERNOS DA ARVORE DA MEMORIA PRINCIPAL
//ARQUIVO: "indice.dat"
//INFORMAÇÕES: POSIÇÃO DOS FILHOS, NÚMERO DE CHAVES, POSIÇÃO DOS PAIS, SE FILHO É FOLHA, CHAVES
long int escreveIndice(TABM *a, int T, int j);

// ESCREVER TODAS AS FOLHS DA DIREITA PARA ESQUERDA
int escreveFolhas(TABM *a, int t);

//ESCREVE A ARVORE TODA
long int escreveArvore(TABM *a, int t);

//----------------------------------------- FUNÇÃO DE BUSCA PIZZA NO ARQUIVO ---------------------------------------------------

long int BuscaDados(long int end, int codigo, int t);

//RETORNA O ENDEREÇO DA PIZZA NO ARQUIVO "PIZZA.DAT"
long int BuscaArquivo(long int end, int codigo, int t, FILE *arquivo);

//----------------------------------------------- FUNÇÃO INSERE PIZZA NA MEMORIA SECUNDARRIA ----------------------------------------------------

//DIVIDE OS NOS QUE ESTIVEREM COMPLETO E RETORNA O INDICE DO COM QUE A CHAVE SOBE
long int divideNoArquivo(long int endPai, long int endNo, int t, int tipo, int cod);

//INSERE A PIZZA NO NÓ INCOMPLETO
long int insereFolhaArq(TPizza *p, int t, long int end);

//BUSCA O NÓ QUE EM QUE DEVE SER INSERIDO A PIZZA
long int buscaInsereArq(TPizza *p, long int endPai, long int endNo, int t, int tipo);

//FUNÇÃO QEU CHAMA AS OUTRAS FUNÇÕES
//VERIFICA SE A PIZZA JÁ ESTAVA NO ARQUIVO
long int insereArquivo(TPizza *p, int t);

//------------------------------------- LISTAR DE ACORDO COM A CATEGORIA ----------------------------------------------------------

//LISTA A CATEGORIA DESEJADA
int listarCategoria(char *nome, int t);
