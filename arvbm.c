#include "arvbm.h"

int vez;

//------------------------------- INICIALIZAÇÃO E CRIAÇÃO --------------------------------------------------------

//ABRE OS ARQUIVOS.
void abreAquivos()
{
  FILE *dados = fopen("dados.dat", "wb+");
  FILE *indice = fopen("indice.dat", "wb+");
  FILE *pizza = fopen("pizza.dat", "wb+");
  fclose(dados);
  fclose(indice);
  fclose(pizza);
}

//INICIALIZA ARVORE TABM EM MP
TABM *inicializa(void){
  return NULL;
}

//INICIALIZA NÓ DA ARVORE TABM EM MP
TABM *cria(TPizza *p, int T){
  TABM* novo = (TABM*)malloc(sizeof(TABM)); //aloca posição na memória
  //add os valores padrões:
  novo->nchaves = 0;
  novo->folha = 1;
  novo->chave = (int*)malloc(sizeof(int)*(T*2-1));
  novo->pizzas = (TPizza**)malloc(sizeof(TPizza*)*(T*2-1));
  novo->pai = NULL;
  novo->ant = NULL;
  novo->prox = NULL;
  novo->filho = (TABM**)malloc(sizeof(TABM*)*T*2);
  novo->posPai = -2;
  novo->posProx = -1;
  novo->posAnt = -1;
  novo->posMeu = -1;

  novo -> posPizza = (long int*)malloc(sizeof(long int)*(T*2-1));
  novo -> posFilho = (long int*)malloc(sizeof(long int)*(T*2));

  int i;

  for(i = 0; i<T*2-1; i++) novo->posPizza[i] = -1;
  for(i = 0; i<2*T; i++) novo->posFilho[i] = -1;

  for(int i=0; i<(T*2); i++)
    novo->filho[i] = NULL;
    
  for(int i = 0; i < (T*2-1); i++){
    novo->pizzas[i] = NULL;
    novo->chave[i] = INT_MIN;
  }
  
  novo->pizzas[0] = p;
  if(p){
    novo->chave[0] = p->cod;
    novo->nchaves = 1;
  }
  return novo;
}

//-------------------------------------------------------- IMPRIME -----------------------------------------------

//imprime as pizzas
void imprime(TABM *a, int t){ 
  if(a){ 
    if(!a->folha)
      imprime(a->filho[0], t);
    else{
      for(int i = 0; i < a->nchaves; i++)
        imprime_pizza(a->pizzas[i]);
    }
    if(a->prox)
      imprime(a->prox, t);
  }
}

//imprime a arvore inteira
void imprime_arvore(TABM *a, int andar, int t){
  if(a){
    int i,j;
    for(i=0; i<=a->nchaves-1; i++){
      imprime_arvore(a->filho[i],andar+1, t);
      for(j=0; j<=andar; j++) printf("      ");
      printf("%d\n", a->chave[i]);
    }
    imprime_arvore(a->filho[i],andar+1, t);
  }
}

void listaMenu(int t)
{
  FILE *indice = fopen("indice.dat", "rb");
  if(!indice) return;

  long int end = 0, folha = -1;
  int i;

  while(1)
  {
    fseek(indice, end, SEEK_SET);
    int nChave, filhoFolha, chave[t*2-1];
    long int posFilho[t*2-1], posPai;
    for(i = 0; i<t*2; i++)
      fread(&posFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
    fread(&nChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
    fread(&posPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
    fread(&filhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fread(&chave[i], sizeof(int), 1, indice); // CHAVES

    if(filhoFolha){ 
      folha = posFilho[0];
      break;
    }
    end = posFilho[0];
  }

  fclose(indice);
  if(folha == -1) return;

  FILE *dados = fopen("dados.dat", "rb");
  if(!dados) return;

  FILE *arqPizza = fopen("pizza.dat", "rb");
  if(!arqPizza) return;

  while(1)
  {
    fseek(dados, folha, SEEK_SET);
    int nChave, chave[t*2-1];
    long int endPizza[t*2-1], proxFolha; 
    fread(&nChave, sizeof(int), 1, dados);//NUMERO DE CAHVE
    for(i = 0; i<t*2-1; i++) //CHAVE = t*2-1
      fread(&chave[i], sizeof(int), 1, dados);
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
      fread(&endPizza[i], sizeof(long int), 1, dados);
    fread(&proxFolha, sizeof(long int), 1, dados); //POSIÇÃO DA PROXIMA FOLHA

    for(i = 0; i<nChave; i++){
      fseek(arqPizza, endPizza[i] , SEEK_SET);
      TPizza *pizza = le_pizza(arqPizza);
      imprime_pizza(pizza);
    }
    if(proxFolha < 0){
      fclose(arqPizza);
      fclose(dados);
      fclose(indice);
      return;
    }
    folha = proxFolha;
  }
}

// ------------------------------------- FUNÇÕES DE SUPORTE DA ARVORE EM MP ---------------------------------------------------


//Funciona
TABM* addpai(TABM *a, int t){ //adiciona o pai do nó
  TABM *ant = a;
  if(a->folha) return a;
  for(int i = 0; i <= a->nchaves; i++){
    a->filho[i]->pai = ant;
    a->filho[i] = addpai(a->filho[i], t);
  }
  return a;
}

TABM* addproximo(TABM *a, TABM *prec, int t){ //adiciona todos os próximos de uma vez
  //prec == nó que precede o nó atual
  if(!a) return NULL;

  if(a->folha){
    a->prox = NULL;
    return a;
  }else if(a->filho[0]->folha){
    if(prec){
      prec->prox = a->filho[0];
      a->filho[0]->ant = prec; //aproveita para add o ant de a
    }
    for(int i = 1; i < 2*t; i++){
      a->filho[i]->prox = a->filho[i-1];
      a->filho[i-1]->ant = a->filho[i]; //aproveita para add o ant de a
    }
  }
  else{
    for(int i = 0; i < 2*t; i++){
      if(a->filho != 0)
        a->filho[i] = addproximo(a->filho[i], a->filho[i-1]->filho[(a->filho[i-1]->nchaves)], t);
      else
        a->filho[i] = addproximo(a->filho[i], NULL, t);
    }
  }
  if(!a->pai){
    TABM *b = a;
    while(b->filho[b->nchaves]) b = b->filho[b->nchaves];
    b->prox = NULL;
  }
  return a;
}

TABM* busca(TABM *a, int cod, int t){
  if (!a) return NULL;
  int i = 0;
  if(!a->folha){
    while((a->nchaves > i)&&(cod >= a->chave[i])) i++;
    return busca(a->filho[i], cod, t);
  }
  return a; //retorna o nó cujo valor deveria estar, o valor estando presente na arvore ou não
}

//---------------------------- INSERÇÃO NA ARVORE EM MP ----------------------------------------

TABM *divisao(TABM *x, int i, TABM *y, int t){
  TABM *z = cria(NULL, t);
  z->folha = y->folha;
  z->pai = x;
  int j;
  if(!y->folha){
    z->nchaves = t-1;
    for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      y->filho[j+t] = NULL;
    }
  }
  else {
    z->nchaves = t; //z possuir� uma chave a mais que y se for folha
    for(j=0;j < t;j++) { //Caso em que y � folha, temos q passar a info para o n� da direita
      z->chave[j] = y->chave[j+t-1];
      z->pizzas[j] = y->pizzas[j+t-1];
    }
    y->prox = z;
    z->ant = y;
  }
  y->nchaves = t-1;
  //----------------------------------------------------------
  for(j=x->nchaves; j>=i; j--) {
    x->filho[j+1]=x->filho[j]; 
  }
  //----------------------------------------------------------
  x->filho[i] = z;
  //----------------------------------------------------------
  for(j=x->nchaves; j>=i; j--) {
    x->chave[j] = x->chave[j-1];
    x->pizzas[j] = x->pizzas[j-1];
  }
  //----------------------------------------------------------
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  return x;
}

TABM* insNCompleta(TABM *a, TPizza *p, int t){ //a unica alteração é adicionar as pizzas se forem folhas
  int i = a->nchaves-1;
  if(a->folha){
    while((i >= 0)&&(p->cod < a->chave[i])){
      a->chave[i+1] = a->chave[i];
      a->pizzas[i+1] = a->pizzas[i];
      i--;
    }
    a->chave[i+1] = p->cod;
    a->pizzas[i+1] = p;
    a->nchaves++;
    return a;
  } else{
    while((i >= 0)&&(p->cod < a->chave[i])) i--;
    i++;
    if(a->filho[i]->nchaves == (t*2-1)){
      a = divisao(a, (i+1), a->filho[i], t);
      if(p->cod > a->chave[i]) i++;
    }
    a->filho[i] = insNCompleta(a->filho[i], p, t);
  }
  return a;
}

TABM* insere(TABM *a, TPizza *p, int t){
  if(!a){
    a = cria(p, t);
    return a;
  }
  if(a->nchaves == (t*2-1)){
    TABM *novo = cria(NULL, t);
    novo->folha = 0;
    novo->filho[0] = a;
    if(a->pai){
      int i = 0;
      while((i < a->nchaves)||(a->pai->filho[i] != a)) i++;
      a->pai->filho[i] = novo;
      novo->pai = a->pai;
    }
    a->pai = novo;
    novo = divisao(novo, 1, a, t);
    novo = insNCompleta(novo, p, t);
    return novo;
  }
  a = insNCompleta(a, p, t);
  return a;
}

// ------------------------------------------ FUNÇÃO DE LIBERA ---------------------------------------------------------------------------

//LIBERA ARVORE EM MP
void libera(TABM *a, int t){ 
  if(a){
    for(int i = 0; i <= a->nchaves; i++){
      libera(a->filho[i], t);
    }
    /*for(int i = 0; i < a->nchaves; i++){ e
      free(a->pizzas[i]); f(i)
    } ok */
    free(a->filho);
    free(a->pizzas);
    free(a->chave);
    free(a->posPizza);
    free(a->posFilho);
    free(a);
  }
}

// -------------------------------------------------------- FUNÇÃO EXCLUI EM MP -----------------------------------------------------------------------------

//Adicionar a concatenação e a redistribuição dos nós internos caso necessário

TABM* excluiNCompleto(TABM *x, int info, int t){
  int i = 0;
  if(x->nchaves > t-1){
    while((i < x->nchaves)&&(info != x->chave[i]))i++;
    while(i < x->nchaves-1){
      x->chave[i] = x->chave[i+1];
      if(x->folha) x->pizzas[i] = x->pizzas[i+1];
      i++;
    }
    x->chave[x->nchaves-1] = INT_MIN;
    x->nchaves--;
    return x;
  }
  else{ ok
    TABM *pai = x->pai;
    while((i < pai->nchaves)&&(info != pai->filho[i]->chave[0]))i++;
    while(i < pai->nchaves-1){
      pai->chave[i] = pai->chave[i+1];
      pai->filho[i+1] = pai->filho[i+2];
    }
    pai->filho[pai->nchaves] = NULL;
    pai->chave[pai->nchaves-1] = INT_MIN;
    pai->nchaves--;
    return pai;
  }
}

TABM *redistribuiPai(TABM *pai, int i, int t){
  TABM *avo = pai->pai;
  if(i == 0){
    TABM *proximo = avo->filho[i+1];
    pai->chave[pai->nchaves] = avo->chave[i];
    pai->nchaves++;
    pai->filho[pai->nchaves] = proximo->filho[0];
    avo->chave[i] = proximo->chave[0];
    for(int j = 0; j < proximo->nchaves-1; j++){
      proximo->chave[j] = proximo->chave[j+1];
      proximo->filho[j+1] = proximo->filho[j+2];
    }
    proximo->chave[proximo->nchaves-1] = INT_MIN;
    proximo->filho[proximo->nchaves] = NULL;
    proximo->nchaves--;
  } else if(i == avo->nchaves){
    TABM *anterior = avo->filho[i-1];
    for(int j = pai->nchaves; j > 0; j--){
      pai->chave[j] = pai->chave[j-1];
      pai->filho[j] = pai->filho[j-1];
    }
    pai->chave[0] = anterior->chave[anterior->nchaves-1];
    pai->filho[0] = anterior->filho[anterior->nchaves];
    pai->nchaves++;
    anterior->chave[anterior->nchaves-1] = INT_MIN;
    anterior->filho[anterior->nchaves] = NULL;
    anterior->nchaves--;
  } else{
    TABM *anterior = avo->filho[i-1], *proximo = avo->filho[i+1];
    if(anterior->nchaves > t-1){
      for(int j = pai->nchaves; j > 0; j--){
        pai->chave[j] = pai->chave[j-1];
        pai->filho[j] = pai->filho[j-1];
      }
      pai->chave[0] = anterior->chave[anterior->nchaves-1];
      pai->filho[0] = anterior->filho[anterior->nchaves];
      pai->nchaves++;
      anterior->chave[anterior->nchaves-1] = INT_MIN;
      anterior->filho[anterior->nchaves] = NULL;
      anterior->nchaves--;
    } else{
      pai->chave[pai->nchaves] = avo->chave[i];
      pai->nchaves++;
      pai->filho[pai->nchaves] = proximo->filho[0];
      avo->chave[i] = proximo->chave[0];
      for(int j = 0; j < proximo->nchaves-1; j++){
        proximo->chave[j] = proximo->chave[j+1];
        proximo->filho[j] = proximo->filho[j+1];
      }
      proximo->filho[proximo->nchaves-1] = proximo->filho[proximo->nchaves];
      proximo->chave[proximo->nchaves-1] = INT_MIN;
      proximo->filho[proximo->nchaves] = NULL;
      proximo->nchaves--;
    }
  }
  return avo;
}

TABM* concatenaPai(TABM *pai, int i, int t){ //recebe o pai e retorna o avô
  if(i == 0){ //pai é o menor filho
    TABM *avo = pai->pai, *proximo = avo->filho[i+1];
    if(pai->nchaves + proximo->nchaves < 2*t-1){ //verifica se dá pra concatenar
      pai->chave[pai->nchaves] = avo->chave[i];
      pai->nchaves++;
      pai->filho[pai->nchaves] = proximo->filho[0];
      for(int j = 0; j < proximo->nchaves; j++){
        pai->chave[pai->nchaves] = proximo->chave[j];
        pai->nchaves++;
        pai->filho[pai->nchaves] = proximo->filho[j+1];
      }
    }
    if(avo->nchaves != t-1){
      for(int j = i; j < avo->nchaves-1; j++){
        avo->chave[j] = avo->chave[j+1];
        avo->filho[j+1] = avo->filho[j+2];
      }
      avo->chave[avo->nchaves-1] = INT_MIN;
      avo->filho[avo->nchaves] = NULL;
    } else{ //necessita de outra concatenação ou redistribuição
      TABM *tavo = avo->pai;
      if(tavo){
        int k = 0;
        while((k <= tavo->nchaves)&&(tavo->filho[k] != avo)) k++;
        tavo = concatenaPai(avo, k, t);
      }
    }
  } else{ //pai não é o menor filho
    TABM *avo = pai->pai, *anterior = avo->filho[i-1];
    if(anterior){
      if(pai->nchaves + anterior->nchaves < 2*t-1){ //concatenação c anterior
        anterior->chave[anterior->nchaves] = avo->chave[i-1];
        anterior->nchaves++;
        anterior->filho[anterior->nchaves] = pai->filho[0];
        for(int j = 0; j < pai->nchaves; j++){
          anterior->chave[pai->nchaves] = pai->chave[j];
          anterior->nchaves++;
          anterior->filho[anterior->nchaves] = pai->filho[j+1];
        }
        if(avo->nchaves != t-1){
          for(int j = i-1; j < avo->nchaves-1; j++){
            avo->chave[j] = avo->chave[j+1];
            avo->filho[j+1] = avo->filho[j+2];
          }
          avo->chave[avo->nchaves-1] = INT_MIN;
          avo->filho[avo->nchaves] = NULL;
        } else{ //necessita de outra concatenação ou redistribuição
          TABM *tavo = avo->pai;
          if(tavo){
            int k = 0;
            while((k <= tavo->nchaves)&&(tavo->filho[k] != avo)) k++;
            tavo = concatenaPai(avo, k, t);
          }
          free(avo);
        }
      }
    } else if(i != avo->nchaves-1){ // tem proximo
      TABM *avo = pai->pai, *anterior = avo->filho[i-1], *proximo =avo->filho[i+1];
      if(pai->nchaves + anterior->nchaves < 2*t-1){ //concatenação c anterior
        anterior->chave[anterior->nchaves] = avo->chave[i];
        anterior->nchaves++;
        anterior->filho[anterior->nchaves] = pai->filho[0];
        for(int j = 0; j < pai->nchaves; j++){
          anterior->chave[pai->nchaves] = pai->chave[j];
          anterior->nchaves++;
          anterior->filho[anterior->nchaves] = pai->filho[j+1];
        }
        if(avo->nchaves != t-1){
          for(int j = i; j < avo->nchaves-1; j++){
            avo->chave[j] = avo->chave[j+1];
            avo->filho[j+1] = avo->filho[j+2];
          }
          avo->chave[avo->nchaves-1] = INT_MIN;
          avo->filho[avo->nchaves] = NULL;
        } else{ //necessita de outra concatenação ou redistribuição
          TABM *tavo = avo->pai;
          if(tavo){
            int k = 0;
            while((k <= tavo->nchaves)&&(tavo->filho[k] != avo)) k++;
            tavo = concatenaPai(avo, k, t);
          }
        }
      } else if(pai->nchaves + proximo->nchaves < 2*t-1){ //concatenação c proximo
        pai->chave[pai->nchaves] = avo->chave[i];
        pai->nchaves++;
        pai->filho[pai->nchaves] = proximo->filho[0];
        for(int j = 0; j < proximo->nchaves; j++){
          pai->chave[pai->nchaves] = proximo->chave[j];
          pai->nchaves++;
          pai->filho[pai->nchaves] = proximo->filho[j+1];
        }
        if(avo->nchaves != t-1){
          for(int j = i; j < avo->nchaves-1; j++){
            avo->chave[j] = avo->chave[j+1];
            avo->filho[j+1] = avo->filho[j+2];
          }
          avo->chave[avo->nchaves-1] = INT_MIN;
          avo->filho[avo->nchaves] = NULL;
          avo->nchaves--;
        } else{ //necessita de outra concatenação ou redistribuição
          TABM *tavo = avo->pai;
          if(tavo){
            int k = 0;
            while((k <= tavo->nchaves)&&(tavo->filho[k] != avo)) k++;
            tavo = concatenaPai(avo, k, t);
          }
        }
      }
    } else{ //não tem o proximo
      TABM *avo = pai->pai, *anterior = avo->filho[i-1];
      if(pai->nchaves + anterior->nchaves < 2*t-1){ //concatenação c anterior
        anterior->chave[anterior->nchaves] = avo->chave[i];
        anterior->nchaves++;
        anterior->filho[anterior->nchaves] = pai->filho[0];
        for(int j = 0; j < pai->nchaves; j++){
          anterior->chave[pai->nchaves] = pai->chave[j];
          anterior->nchaves++;
          anterior->filho[anterior->nchaves] = pai->filho[j+1];
        }
        if(avo->nchaves != t-1){
          for(int j = i; j < avo->nchaves-1; j++){
            avo->chave[j] = avo->chave[j+1];
            avo->filho[j+1] = avo->filho[j+2];
          }
          avo->chave[avo->nchaves-1] = INT_MIN;
          avo->filho[avo->nchaves] = NULL;
        } else{ //necessita de outra concatenação ou redistribuição
          TABM *tavo = avo->pai;
          if(tavo){
            int k = 0;
            while((k <= tavo->nchaves)&&(tavo->filho[k] != avo)) k++;
            tavo = concatenaPai(avo, k, t);
          }
        }
      }
    }
  }
}

TABM* concatenaIrmao(TABM *a, int i, TABM *y, int cmp, int t){
  //a = irmao q recebe; i = 1ª chave do pai maior q o menor filho; y = irmao q some; cmp = qual irmao é maior; t = T
  TABM *pai = a->pai;
  if(cmp == 1){ //a é o irmão menor
    for(int j = 0; j < t-1; j++){
      a->chave[a->nchaves] = y->chave[j];
      y->chave[j] = INT_MIN;
      y->nchaves--;
      a->nchaves++;
    }
    if(pai->nchaves != t-1){
      for(int j = i; j < pai->nchaves-1; j++){
        pai->chave[j] = pai->chave[j+1];
        pai->filho[j+1] = pai->filho[j+2];
      }
    }
  } else{ //y é o irmão menor
    for(int j = a->nchaves; j > 0; j--)
      a->chave[j] = a->chave[j-1];
    for(int j = 0; j < t-1; j++){
      a->chave[0] = y->chave[y->nchaves-1];
      a->nchaves++;
    }
    if(pai->nchaves != t-1){
      for(int j = i; j < pai->nchaves; j++){
        pai->chave[j] = pai->chave[j+1];
        pai->filho[j] = pai->filho[j+1];
      }
    }
  }
  //excluir pai
  if(pai->nchaves != t-1){
    pai->chave[pai->nchaves-1] = INT_MIN;
    pai->filho[pai->nchaves] = NULL;
    pai->nchaves--;
    libera(y, t);
  } else{ //pai tem t-1 chaves
    TABM *avo = pai->pai;
    int j = 0;
    while((j >= avo->nchaves)&&(avo->filho[j] != pai)) j++;
    j++;
    if((j != 0)&&(j != avo->nchaves)){
      TABM *ant = avo->filho[j-1], *prox = avo->filho[j+1];
      if((pai->nchaves + ant->nchaves >= 2*t-1)||(pai->nchaves + prox->nchaves >= 2*t-1)){ d
        avo = redistribuiPai(pai, j, t);
        return pai;
      }
      if((pai->nchaves + ant->nchaves < 2*t-1)||(pai->nchaves + prox->nchaves < 2*t-1)){
        int resp = 0;
        if(ant->nchaves == t-1) resp = 1;
        avo = concatenaPai(pai, j, t);
        if(resp) return avo->filho[i-1];
        else return avo->filho[i];
      }
    } else if(j == 0){
      TABM *prox = avo->filho[j+1];
      if(pai->nchaves + prox->nchaves >= 2*t-1){
        avo = redistribuiPai(pai, j, t);
        return pai;
      }
      if(pai->nchaves + prox->nchaves < 2*t-1){
        avo = concatenaPai(pai, j, t);
        return avo->filho[i];
      }
    } else{
      TABM *ant = avo->filho[j-1];
      if(pai->nchaves + ant->nchaves >= 2*t-1){
        avo = redistribuiPai(pai, j, t);
        return pai;
      }
      if(pai->nchaves + ant->nchaves < 2*t-1){
        avo = concatenaPai(pai, j, t);
        return avo->filho[i-1];
      }
    }
  }
  libera(a, t);
  return pai;
}

TABM* redistribui(TABM *a, int cmp, TABM *y, int t){
  //a = irmão q recebera; cmp = qual irmão é maior; y = irmão que doara; t = t
  TABM *menor; ok
  if(cmp == 1) menor = a;
  else menor = y;

  TABM *pai = a->pai;
  int i = 0; ok
  while((i <= pai->nchaves)&&(pai->filho[i] != menor))i++; //descobre indice do menor
  ok
  if(menor == a){ //se oq recebe está a esquerda
    a->chave[a->nchaves] = y->chave[0];
    a->nchaves++;
    for(int j = 0; j < y->nchaves-2; j++)
      y->chave[j] = y->chave[j+1];
    y->chave[y->nchaves-1] = INT_MIN;
    y->nchaves--;
    pai->chave[i] = y->chave[0];

  } else if(menor = y){ //se oq recebe está a direita
    for(int j = a->nchaves; j > 0; j--)
      a->chave[j] = a->chave[j-1];
    a->chave[0] = y->chave[y->nchaves-1];
    a->nchaves++;
    pai->chave[i] = y->chave[y->nchaves-1];
    y->chave[y->nchaves-1] = INT_MIN;
    y->nchaves--;
  }
  return pai; //retorna o pai dos dois filhos
}

TABM* exclui(TABM *a, int info, int t){
  TABM *x = busca(a, info, t);

  //se a informação não existir no nó retorna NULL
  int i = 0;
  while((i < x->nchaves)&&(info > x->chave[i]))i++;
  if(x->chave[i] != info) return a;

  //se o nó tem mais de t-1 chaves
  if(x->nchaves > t-1){
    x = excluiNCompleto(x, info, t);
    return a;
  }
  if(x->pai->nchaves > t-1){
    x->pai = excluiNCompleto(x, info, t);
  }
  //casos 3:
  else if(x->nchaves == t-1){
    if(x->folha){
      TABM *pai = x->pai;
      int j = 0;
      TABM *anterior = NULL, *proximo = NULL;
      if(pai){
        while((j <= pai->nchaves)&&(pai->filho[j] != x)) j++;
        if(j != 0) anterior = pai->filho[j-1];
        if(j != pai->nchaves) proximo = pai->filho[j+1];
      }
      //caso 3a - Redistribuição:
      if((anterior != NULL)&&(x->nchaves + anterior->nchaves >= 2*t-1)){
        pai = redistribui(x, 0, anterior, t);
        x = busca(a, info, t);
        x = excluiNCompleto(x, info, t);
        return a;
      } else if((proximo != NULL)&&(x->nchaves + proximo->nchaves >= 2*t-1)){
        TABM *pai = x->pai;
        pai = redistribui(x, 1, x->prox, t);
        x = busca(a, info, t);
        x = excluiNCompleto(x, info, t);
        return a;
      }
      //caso 3b - Concatenação:
      //concatena com irmão da esquerda
      else if((anterior != NULL)&&(x->nchaves + anterior->nchaves < 2*t-1)){
        j = 0;
        while((j <= pai->nchaves)&&(pai->filho[j] != anterior)) j++;
        pai = concatenaIrmao(anterior, j, x, 1, t);
        anterior = excluiNCompleto(anterior, info, t);
        return a;
      } //concatena com irmão da direita
      else if((proximo != NULL)&&(x->nchaves + proximo->nchaves < 2*t-1)){
        j = 0;
        while((j <= pai->nchaves)&&(pai->filho[j] != x)) j++;
        pai = concatenaIrmao(proximo, j, x, 0, t);
        for(int k = j; k < pai->nchaves-1; k++){
          pai->chave[k] = pai->chave[k+1];
          pai->filho[k] = pai->filho[k+1];
        }
        pai->filho[pai->nchaves-1] = pai->filho[pai->nchaves];
        pai->chave[pai->nchaves-1] = INT_MIN;
        pai->filho[pai->nchaves] = NULL;
        pai->nchaves--;
        x = busca(a, info, t);
        x = excluiNCompleto(x, info, t);
        return a;
      }
    }
  }  
  return a;
}

//---------------------------------------------- FUNÇÃO EXCLUI NA MEMORIA SEGUNDARIA ---------------------------------------------------------------------------------------------------


//-------------------------------------------- FUNÇÕES PARA PASSAR A ARVORE DA MP PARA MS (ARQUIVO) ------------------------------------------------------------------------------------

/*
ESCREVE DADOS: 

- NUMERO DE CHAVES
- CHAVES
- ENDEREÇO DAS PIZZAS
- POSIÇÃO DA PROXIMA FOLHA

*/
int escreveDados(TABM *a, int T)
{
  if(!a) return -1;
  FILE *arquivo = fopen("dados.dat", "rb+");
  if(!arquivo) exit(1);
  fseek(arquivo, 0, SEEK_END);
  long int endD = a->posMeu = ftell(arquivo), endP;
  fwrite(&a->nchaves, sizeof(int), 1, arquivo); //NUMERO DE CHAVE
  int i;
  fseek(arquivo, 0, SEEK_END);
  for(i = 0; i<T*2-1; i++){ //CHAVE = T*2-1
    fwrite(&a->chave[i], sizeof(int), 1, arquivo);
  }
  FILE *arqPizza = fopen("pizza.dat", "rb+");
  fseek(arqPizza, 0, SEEK_END);
  fseek(arquivo, 0, SEEK_END);
  for(i = 0; i<a->nchaves; i++) //ENDEREÇO DAS PIZZAS
  {
    endP = ftell(arqPizza);
    salva_pizza(a->pizzas[i], arqPizza);
    fwrite(&endP, sizeof(long int), 1, arquivo);
    fseek(arqPizza, 0, SEEK_END);
  }
  fseek(arquivo, 0, SEEK_END);
  for(i = a->nchaves; i<T*2-1; i++) //SE O NUMERO DE PIZZAS FOR MENOR QUE T*2-1
  {
    long int aux = -1;
    fwrite(&aux, sizeof(long int), 1, arquivo);
  }
  fseek(arquivo, 0, SEEK_END);
  fwrite(&a->posProx, sizeof(long int), 1, arquivo); //POSIÇÃO DA PROXIMA FOLHA
  fclose(arqPizza);
  fclose(arquivo);
  TABM * b = a->ant;
  if(b) b->posProx = endD;
  b = a->prox;
  if(b) b->posAnt = endD;
  return 1;
}

/*
ESCREVE INDICE: 

- POSIÇÃO DOS FILHOS
- NÚMERO DE CHAVES
- POSIÇÃO DOS PAIS
- SE FILHO É FOLHA
- CHAVES

*/

long int escreveIndice(TABM *a, int T, int j)
{

  long int TAMINDICE = ((sizeof(int)*T*2-1) + (sizeof(int)*2) + (sizeof(long int)*T*2) + (sizeof(long int)));
  if(!a) return -1;

  FILE *arquivo = fopen("indice.dat", "rb+");
  if(!arquivo) exit(1);
  if(j == 0) fseek(arquivo, 0, SEEK_SET);
  else fseek(arquivo, (vez*TAMINDICE), SEEK_END);

  int i; long int aux2, aux = -1000000, end = ftell(arquivo);
  long int endI = a->posMeu = ftell(arquivo);

  for(i = 0; i<a->nchaves+1; i++)
  {
    if(a->filho[i]->folha){
      aux2 = a->filho[i]->posMeu;
      a->posFilho[i] = aux2;
      fwrite(&aux2, sizeof(long int), 1, arquivo); //POSIÇÃO DOS FILHOS
    }
    else{
      vez++;
      aux2 = escreveIndice(a->filho[i], T, 1);
      fwrite(&aux2, sizeof(long int), 1, arquivo);
    }
  }
  for(i = a->nchaves+1; i<T*2; i++)
  {
    aux2 = -1;
    fwrite(&aux2, sizeof(long int), 1, arquivo); //POSIÇÃO DE FILHOS QUE NÃO EXISTE
  }

  fwrite(&a->nchaves, sizeof(int), 1, arquivo); // NUMERO DE CHAVE

  if(a->pai)fwrite(&a->pai->posMeu, sizeof(long int), 1, arquivo);   // POSIÇÃO DO PAI
  else fwrite(&aux, sizeof(long int), 1, arquivo); 

  if(a->filho[0]->folha) i = 1;
  else i = 0;
  fwrite(&i, sizeof(int), 1, arquivo); //SE FILHO É FOLHA

  for(i = 0; i<T*2-1; i++)
  {
    fwrite(&a->chave[i], sizeof(int), 1, arquivo); // CHAVES
  }

  fclose(arquivo);

  return end;

}

// ESCREVER TODAS AS FOLHS DA DIREITA PARA ESQUERDA
int escreveFolhas(TABM *a, int t)
{
  if(!a) return -1;
  int i;
  while(!a->folha)
  {
    i = a->nchaves;
    a = a->filho[i];
  }
  while(a)
  {
    i = escreveDados(a, t);
    if(i != 1) return -1;
    a = a->ant;
  }
  return 1;

}

//ESCREVE A ARVORE TODA
long int escreveArvore(TABM *a, int t)
{
  vez = 1;
  int i = escreveFolhas(a, t);
  if(i == -1) return -1;
  return escreveIndice(a, t, 0);
}

//----------------------------------------- FUNÇÃO DE BUSCA PIZZA NO ARQUIVO ---------------------------------------------------

long int BuscaDados(long int end, int codigo, int t)
{
    FILE *arquivo = fopen("dados.dat", "rb");
    fseek(arquivo, end, SEEK_SET);

    int nChaveD, chaveD[t*2-1], i;
    long int endPizza[t*2-1], proxFolha; 

    fread(&nChaveD, sizeof(int), 1, arquivo);//NUMERO DE CHAVE
    for(i = 0; i<t*2-1; i++){ //CHAVE = T*2-1
      fread(&chaveD[i], sizeof(int), 1, arquivo);
    }
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
    {
        fread(&endPizza[i], sizeof(long int), 1, arquivo);
    }
    fread(&proxFolha, sizeof(long int), 1, arquivo); //POSIÇÃO DA PROXIMA FOLHA
    fclose(arquivo);

    for(i = 0; i<nChaveD; i++){
      if(chaveD[i] == codigo){
        return endPizza[i];
      }
    }
    return -1;
}

//RETORNA O ENDEREÇO DA PIZZA NO ARQUIVO "PIZZA.DAT"
long int BuscaArquivo(long int end, int codigo, int t, FILE *arquivo)
{
    //COLOCA A POSIÇÃO QUE DESEJA
    fseek(arquivo, end, SEEK_SET);
    int i;

    int nChave, filhoFolha, chave[t*2-1];
    long int posFilho[t*2-1], proxFolha, posPai; 

    for(i = 0; i<t*2; i++)
    {
        fread(&posFilho[i], sizeof(long int), 1, arquivo); //POSIÇÃO DOS FILHOS
    }

    fread(&nChave, sizeof(int), 1, arquivo); // NUMERO DE CHAVE
    fread(&posPai, sizeof(long int), 1, arquivo);   // POSIÇÃO DO PAI

    fread(&filhoFolha, sizeof(int), 1, arquivo); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
    {
        fread(&chave[i], sizeof(int), 1, arquivo); // CHAVES
    }

    for(i = 0; i<nChave; i++)
    {
      if((codigo < chave[i]) && (!filhoFolha)){
        return BuscaArquivo(posFilho[i], codigo, t, arquivo);
      }
      if((codigo < chave[i]) && (filhoFolha)){
        return BuscaDados(posFilho[i], codigo, t);
      }
      if((i == nChave-1)&&(codigo >= chave[i]))
      {
        if(!filhoFolha)
          return BuscaArquivo(posFilho[i+1], codigo, t, arquivo);
        else return BuscaDados(posFilho[i+1], codigo, t);
      }
    }
    
    return -1;
}

//----------------------------------------------- FUNÇÃO INSERE PIZZA NA MEMORIA SECUNDARIA ----------------------------------------------------

long int divideNoArquivo(long int endPai, long int endNo, int t, int tipo, int cod)
{

  int i, j;

  if(tipo == 1){ //Divide indice

    FILE *indice = fopen("indice.dat", "rb+");
    if(!indice) return -1;

    int YnChave, YfilhoFolha, Ychave[t*2-1];
    long int YposFilho[t*2-1], YposPai;

    fseek(indice, endNo, SEEK_SET);

    for(i = 0; i<t*2; i++)
      fread(&YposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
    fread(&YnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
    fread(&YposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
    fread(&YfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fread(&Ychave[i], sizeof(int), 1, indice); // CHAVES

    if(YposPai < -100)
    {
      int XnChave, XfilhoFolha, Xchave[t*2-1];
      long int XposFilho[t*2-1], XposPai;

      int ZnChave, ZfilhoFolha, Zchave[t*2-1];
      long int ZposFilho[t*2-1], ZposPai;

      ZfilhoFolha = YfilhoFolha;
      XfilhoFolha = 0;

      ZnChave = t-1;
      for(j=0;j<t-1;j++) Zchave[j] = Ychave[j+t];
      for(j=0;j<t;j++){
        ZposFilho[j] = YposFilho[j+t];
        YposFilho[j+t] = -1;
      }

      YnChave = t-1;

      Xchave[0] = Ychave[t-1];
      XnChave++;

      long int posZ, posY;

      ZposPai = 0;
      YposPai = 0; 
      XposPai = -1000000;

      //ESCREVENDO Z
      fseek(indice, 0, SEEK_END);
      posZ = ftell(indice);
      for(i = 0; i<t*2; i++)
        fwrite(&ZposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&ZnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&ZposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&ZfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Zchave[i], sizeof(int), 1, indice); // CHAVES

      //ESCREVNDO Y
      fseek(indice, 0, SEEK_END);
      posY = ftell(indice);
      for(i = 0; i<t*2; i++)
        fwrite(&YposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&YnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&YposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&YfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Ychave[i], sizeof(int), 1, indice); // CHAVES

      //ADICIONANDO ENDEREÇO DOS FILHOS
      XposFilho[0] = posY;
      XposFilho[1] = posZ;

      //ESCREVENDO X
      rewind(indice);
      for(i = 0; i<t*2; i++)
        fwrite(&XposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&XnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&XposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&XfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Xchave[i], sizeof(int), 1, indice); // CHAVES

      return 0;
    } else
    {
      fseek(indice, YposPai, SEEK_SET);
      int XnChave, XfilhoFolha, Xchave[t*2-1];
      long int XposFilho[t*2-1], XposPai;
      for(i = 0; i<t*2; i++)
        fread(&XposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fread(&XnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fread(&XposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fread(&XfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fread(&Xchave[i], sizeof(int), 1, indice); // CHAVES

      int ZnChave, ZfilhoFolha, Zchave[t*2-1];
      long int ZposFilho[t*2-1], ZposPai;

      ZfilhoFolha = YfilhoFolha;
      ZposPai = YposPai;

      ZnChave = t-1;
      for(j=0;j<t-1;j++) Zchave[j] = Ychave[j+t];
      for(j=0;j<t;j++){
        ZposFilho[j] = YposFilho[j+t];
        YposFilho[j+t] = -1;
      }

      YnChave = t-1;

      i = XnChave-1;
      int verifica = 0;
      while((i>=0) && (Ychave[t-1] < Xchave[i])) 
      {
        i--;
      }
      i++; 
      i++;
      for(j=XnChave; j>=i; j--)
      {
        XposFilho[j+1]= XposFilho[j];
      }
      for(j=XnChave; j>=i; j--) Xchave[j] = Xchave[j-1];

      Xchave[i-1] = Ychave[t-1];
      XnChave++;

      //ESCREVENDO Z
      fseek(indice, 0, SEEK_END);
      long int posZ = ftell(indice);
      XposFilho[i] = posZ; //SALVANDO A POSIÇÃO DE Z EM X
      for(i = 0; i<t*2; i++)
        fwrite(&ZposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&ZnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&ZposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&ZfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Zchave[i], sizeof(int), 1, indice); // CHAVES

      fseek(indice, YposPai, SEEK_SET);
      for(i = 0; i<t*2; i++)
        fwrite(&XposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&XnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&XposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&XfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Xchave[i], sizeof(int), 1, indice); // CHAVES

      fseek(indice, endNo, SEEK_SET);
      for(i = 0; i<t*2; i++)
        fwrite(&YposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
      fwrite(&YnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
      fwrite(&YposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
      fwrite(&YfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fwrite(&Ychave[i], sizeof(int), 1, indice); // CHAVES      

      fclose(indice);
      return YposPai;

    }
  }else if(tipo == 0){ //Divide folha
    
    FILE *dados = fopen("dados.dat", "rb+");
    if(!dados) return -1;

    fseek(dados, endNo, SEEK_SET);
    int YnChave, Ychave[t*2-1];
    long int YendPizza[t*2-1], YproxFolha; 
    fread(&YnChave, sizeof(int), 1, dados);//NUMERO DE CAHVE
    for(i = 0; i<t*2-1; i++) //CHAVE = t*2-1
      fread(&Ychave[i], sizeof(int), 1, dados);
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
      fread(&YendPizza[i], sizeof(long int), 1, dados);
    fread(&YproxFolha, sizeof(long int), 1, dados); //POSIÇÃO DA PROXIMA FOLHA
    
    FILE *indice = fopen("indice.dat", "rb+");
    if(!indice) return -1;

    fseek(indice, endPai, SEEK_SET);
    int XnChave, XfilhoFolha, Xchave[t*2-1];
    long int XposFilho[t*2-1], XposPai;
    for(i = 0; i<t*2; i++)
      fread(&XposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
    fread(&XnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
    fread(&XposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
    fread(&XfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fread(&Xchave[i], sizeof(int), 1, indice); // CHAVES

    int ZnChave, Zchave[t*2-1];
    long int ZendPizza[t*2-1], ZproxFolha; 

    ZnChave = t; 
    for(j=0;j < t;j++){
      Zchave[j] = Ychave[j+t-1];
      ZendPizza[j] = YendPizza[j+t-1];
    }
    
    ZproxFolha = YproxFolha;

    YnChave = t-1;

    i = XnChave-1;
    while((i>=0) && (cod < Xchave[i])) i--;
    i++; 
    i++;

    for(j=XnChave; j>=i; j--) XposFilho[j+1]= XposFilho[j];
    for(j= XnChave; j>=i; j--) Xchave[j] = Xchave[j-1];

    Xchave[i-1] = Ychave[t-1];
    XnChave++;

    //SALVANDO Z EM DADOS
    fseek(dados, 0, SEEK_END);
    long int posZ = ftell(dados);
    YproxFolha = posZ;
    XposFilho[i] = posZ;
    fwrite(&ZnChave, sizeof(int), 1, dados); //NUMERO DE CHAVE
    for(i = 0; i<t*2-1; i++)
      fwrite(&Zchave[i], sizeof(int), 1, dados);
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DE PIZZA
      fwrite(&ZendPizza[i], sizeof(long int), 1, dados);
    fwrite(&ZproxFolha, sizeof(long int), 1, dados); //POSIÇÃO DA PROXIMA FOLHA

    //SALVANDO Y em Dados
    fseek(dados, endNo, SEEK_SET);
    fwrite(&YnChave, sizeof(int), 1, dados); //NUMERO DE CHAVE
    for(i = 0; i<t*2-1; i++)
      fwrite(&Ychave[i], sizeof(int), 1, dados);
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DE PIZZA
      fwrite(&YendPizza[i], sizeof(long int), 1, dados);
    fwrite(&YproxFolha, sizeof(long int), 1, dados); //POSIÇÃO DA PROXIMA FOLHA

    fclose(dados);

    fseek(indice, endPai, SEEK_SET);
    for(i = 0; i<t*2; i++)
      fwrite(&XposFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
    fwrite(&XnChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
    fwrite(&XposPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
    fwrite(&XfilhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fwrite(&Xchave[i], sizeof(int), 1, indice); // CHAVES

    fclose(indice);


    return endPai;
  }
}

long int insereFolhaArq(TPizza *p, int t, long int end)
{

  int i; 

  FILE *arquivo = fopen("dados.dat", "rb+");
  if(!arquivo) return -1;
  fseek(arquivo, end, SEEK_SET);

  int nChave, chave[t*2-1];
  long int endPizza[t*2-1], proxFolha; 

  fread(&nChave, sizeof(int), 1, arquivo);//NUMERO DE CAHVE
  for(i = 0; i<t*2-1; i++){ //CHAVE = t*2-1
      fread(&chave[i], sizeof(int), 1, arquivo);
  }
  for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
  {
      fread(&endPizza[i], sizeof(long int), 1, arquivo);
  }
  fread(&proxFolha, sizeof(long int), 1, arquivo); //POSIÇÃO DA PROXIMA FOLHA
  
  i = nChave-1;

  while((i>=0) && (p->cod < chave[i])){
    chave[i+1] = chave[i];
    endPizza[i+1] = endPizza[i];
    i--;
  }
  chave[i+1] = p->cod;

  FILE *pizza = fopen("pizza.dat", "rb+");
  if(!pizza) return -1;
  fseek(pizza, 0, SEEK_END);
  long int endP = ftell(pizza);
  salva_pizza(p, pizza);
  fclose(pizza);

  endPizza[i+1] = endP;
  nChave++;

  //SALVAR FOLHA NO ARQUIVO
  fseek(arquivo, end, SEEK_SET);
  fwrite(&nChave, sizeof(int), 1, arquivo); //NUMERO DE CHAVE
  for(i = 0; i<t*2-1; i++)
    fwrite(&chave[i], sizeof(int), 1, arquivo);
  for(i = 0; i<t*2-1; i++) //ENDEREÇO DE PIZZA
    fwrite(&endPizza[i], sizeof(long int), 1, arquivo);
  fwrite(&proxFolha, sizeof(long int), 1, arquivo); //POSIÇÃO DA PROXIMA FOLHA

  fclose(arquivo);

  return end;
  
}

long int buscaInsereArq(TPizza *p, long int endPai, long int endNo, int t, int tipo)
{
  int i; 

  if(endNo < 0) {
    return -1;
  }

  if(tipo == 1) //QUER DIZER QUE AINDA ESTAMOS NO INDICE
  {
    FILE *arquivo = fopen("indice.dat", "rb+");
    if(!arquivo) return -1;
    //COLOCA A POSIÇÃO QUE DESEJA
    fseek(arquivo, endNo , SEEK_SET);

    int nChave, filhoFolha, chave[t*2-1];
    long int posFilho[t*2-1], posPai; 

    for(i = 0; i<t*2; i++)
      fread(&posFilho[i], sizeof(long int), 1, arquivo); //POSIÇÃO DOS FILHOS
    fread(&nChave, sizeof(int), 1, arquivo); // NUMERO DE CHAVE
    fread(&posPai, sizeof(long int), 1, arquivo);   // POSIÇÃO DO PAI
    fread(&filhoFolha, sizeof(int), 1, arquivo); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fread(&chave[i], sizeof(int), 1, arquivo); // CHAVES
    
    if(nChave == 2*t-1)
    {
      //Divide deve retorna o nó pai que foi dividido
      long int x = divideNoArquivo(endPai, endNo,  t, 1, p->cod); 
      //-------------------------------------------------------
      //Talvez esteja com problema
      fseek(arquivo, x, SEEK_SET);
      for(i = 0; i<t*2; i++)
        fread(&posFilho[i], sizeof(long int), 1, arquivo); //POSIÇÃO DOS FILHOS
      fread(&nChave, sizeof(int), 1, arquivo); // NUMERO DE CHAVE
      fread(&posPai, sizeof(long int), 1, arquivo);   // POSIÇÃO DO PAI
      fread(&filhoFolha, sizeof(int), 1, arquivo); //SE FILHO É FOLHA
      for(i = 0; i<t*2-1; i++)
        fread(&chave[i], sizeof(int), 1, arquivo); // CHAVES
      fclose(arquivo);
      return buscaInsereArq(p, posPai, x, t, 1);
    }
    else{
      fclose(arquivo);
      for(i = 0; i<nChave; i++)
      {
        if((p->cod < chave[i]) && (!filhoFolha)){
          return buscaInsereArq(p, endNo ,posFilho[i], t, 1);
        }
        if((p->cod < chave[i]) && (filhoFolha)){
          return buscaInsereArq(p, endNo ,posFilho[i], t, 0);
        }
        if((i == nChave-1)&&(p->cod >= chave[i]))
        {
          if(!filhoFolha){
            return buscaInsereArq(p, endNo ,posFilho[i+1], t, 1);
          }
          else {
            return buscaInsereArq(p, endNo ,posFilho[i+1], t, 0);
          }
        }
      }
    }
    return -1;
  }else if(tipo == 0){ //CHEGAMOS NA FOLHA

    FILE *arquivo = fopen("dados.dat", "rb");
    fseek(arquivo, endNo, SEEK_SET);

    int nChave, chave[t*2-1];
    long int endPizza[t*2-1], proxFolha; 

    fread(&nChave, sizeof(int), 1, arquivo);//NUMERO DE CAHVE
    for(i = 0; i<t*2-1; i++){ //CHAVE = t*2-1
        fread(&chave[i], sizeof(int), 1, arquivo);
    }
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
    {
        fread(&endPizza[i], sizeof(long int), 1, arquivo);
    }
    fread(&proxFolha, sizeof(long int), 1, arquivo); //POSIÇÃO DA PROXIMA FOLHA
    fclose(arquivo);

    if(nChave == t*2-1)
    {
      long int x = divideNoArquivo(endPai, endNo, t, 0, p->cod);
      return buscaInsereArq(p, endPai, x, t, 1);
    }
    else{
      return insereFolhaArq(p, t, endNo);
    }
    return -1;
  }
  return -1;
}

long int insereArquivo(TPizza *p, int t)
{
  FILE *arquivo = fopen("indice.dat", "rb");
  rewind(arquivo);
  long int end = BuscaArquivo(0, p->cod, t, arquivo);
  fclose(arquivo);
  if(end >= 0) return -2;
  end = buscaInsereArq(p, 0, 0, t, 1);
  if(end >= 0) return end;
  return -1;
}

//------------------------------------- LISTAR DE ACORDO COM A CATEGORIA ----------------------------------------------------------

int listarCategoria(char *nome, int t)
{
  FILE *indice = fopen("indice.dat", "rb");
  if(!indice) return -1;

  long int end = 0, folha = -1;
  int i;

  while(1)
  {
    fseek(indice, end, SEEK_SET);
    int nChave, filhoFolha, chave[t*2-1];
    long int posFilho[t*2-1], posPai;
    for(i = 0; i<t*2; i++)
      fread(&posFilho[i], sizeof(long int), 1, indice); //POSIÇÃO DOS FILHOS
    fread(&nChave, sizeof(int), 1, indice); // NUMERO DE CHAVE
    fread(&posPai, sizeof(long int), 1, indice);   // POSIÇÃO DO PAI
    fread(&filhoFolha, sizeof(int), 1, indice); //SE FILHO É FOLHA
    for(i = 0; i<t*2-1; i++)
      fread(&chave[i], sizeof(int), 1, indice); // CHAVES

    if(filhoFolha){ 
      folha = posFilho[0];
      break;
    }
    end = posFilho[0];
  }

  fclose(indice);
  if(folha == -1) return -1;

  FILE *dados = fopen("dados.dat", "rb");
  if(!dados) return -1;

  FILE *arqPizza = fopen("pizza.dat", "rb");
  if(!arqPizza) return -1;

  while(1)
  {
    fseek(dados, folha, SEEK_SET);
    int nChave, chave[t*2-1];
    long int endPizza[t*2-1], proxFolha; 
    fread(&nChave, sizeof(int), 1, dados);//NUMERO DE CAHVE
    for(i = 0; i<t*2-1; i++) //CHAVE = t*2-1
      fread(&chave[i], sizeof(int), 1, dados);
    for(i = 0; i<t*2-1; i++) //ENDEREÇO DAS PIZZAS
      fread(&endPizza[i], sizeof(long int), 1, dados);
    fread(&proxFolha, sizeof(long int), 1, dados); //POSIÇÃO DA PROXIMA FOLHA

    for(i = 0; i<nChave; i++){
      fseek(arqPizza, endPizza[i] , SEEK_SET);
      TPizza *pizza = le_pizza(arqPizza);
      if(strcmp(pizza->descricao, nome) == 0)
        imprime_pizza(pizza);
      
    }

    if(proxFolha < 0){
      fclose(arqPizza);
      fclose(dados);
      fclose(indice);
      return 1;
    }

    folha = proxFolha;
  }
}
