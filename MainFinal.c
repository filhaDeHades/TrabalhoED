#include "arvbm.h"

void MenuPrincipal()
{
	printf("\n\e[032mMENU PRINCIPAL:\e[m\n");
	printf("\e[033m1\e[m - Inserir nova pizza\n");
	printf("\e[033m2\e[m - Remover pizza\n"); 
	printf("\e[033m3\e[m - Bucar pizza\n");
	printf("\e[033m4\e[m - Imprimir Menu\n");
	printf("\e[033m0\e[m - Sair\n");
	printf("\e[032mDigite a opção desejada:\e[m ");
}

void CodigoCategoria()
{
	printf("\n\e[033m1\e[m - Codigo\n");
	printf("\e[033m2\e[m - Categoria\n");
	printf("\e[032mDigite a opção desejada:\e[m ");
}

void PizzaCodMenu()
{
	printf("\n1 - Imprimir\n");
	printf("2 - Alterar infromações\n");
	printf("3 - Voltar\n");
	printf("Digite a opção desejada: ");
}

void ModificaMenu()
{
	printf("\n1 - Nome\n");
	printf("2 - Categoria\n");
	printf("3 - Preço\n");
	printf("Digite a opção desejada: ");
}


void main(void){

	int op, t, aux;
	printf("\e[032mDigite o fator de ramificação (t) da árvore B+:\e[m ");
	scanf("%d", &t);

	TABM *arv = NULL; //cria arvore
    FILE *o = fopen("dados_iniciais.dat", "rb"); //abre arquivo
    if(!o) exit(1);
    rewind(o); //ter certeza que o ponteiro esta no inicio do arquivo
    TPizza *p = le_pizza(o);
    int i = 0;
    while(p){
    	arv = insere(arv, p, t);
        p = le_pizza(o);
    }

    abreAquivos();
    long int raiz = escreveArvore(arv, t);

	do{

		MenuPrincipal();
		scanf("%d", &op);

		if(op == 1){
			//inserir
			char nome[50], cat[20];
			int cod;
			float preco;

			printf("\nCodigo: ");
			scanf("%d", &cod);
			printf("Nome: ");
			scanf(" %50[^\n]", nome);
			printf("Categoria: ");
			scanf(" %20[^\n]", cat);
			printf("Preço: ");
			scanf("%f", &preco);

			//INSERE PIZZA
		    TPizza *novaPizza = pizza(cod, nome, cat, preco);
		    long int resp = insereArquivo(novaPizza, t);
		    if(resp >= 0){
		    	printf("\nPizza inserida com sucesso!\n");
		    	imprime_pizza(novaPizza);
		    }
		    else if(resp == -2) printf("\nCodigo já existe. Tente novamente com outro codigo.\n");
		    else printf("\nDesculpa, não podemos realizar a operação desejada. Tente novamente mais tarde.\n");

		}else if(op == 2){
			//remover elemento
			printf("\nDesculpa, mas essa opção não esta disponivel.\n");

		}else if(op == 3){
			//Bucar pizza
			CodigoCategoria();
			scanf("%d", &op);
			if(op == 1){
				printf("\nDigite o codigo: ");
				scanf("%d", &aux);

				FILE *arquivo = fopen("indice.dat", "rb+");
				rewind(arquivo);
				long int end = BuscaArquivo(0, aux, t, arquivo);
				FILE *arqpizza = fopen("pizza.dat", "rb+");

			    if(end >= 0){

			    	PizzaCodMenu();
					scanf("%d", &op);

					fseek(arqpizza, end , SEEK_SET);
				    TPizza *piz = le_pizza(arqpizza);
					if(op == 1){
				        fclose(arqpizza);
				        fclose(arquivo);
				        printf("\n");
				        imprime_pizza(piz);

					}else if(op == 2)
					{							
						ModificaMenu();
						scanf("%d", &op);
						if(op == 1){
							//Nome
							char novo_nome[50];
							printf("\nNovo nome: ");
							scanf(" %50[^\n]", novo_nome);
							TPizza *nova = pizza(piz->cod, novo_nome, piz->descricao, piz->preco);
							fseek(arqpizza, end , SEEK_SET);
							imprime_pizza(nova);
							salva_pizza(nova, arqpizza);
							fclose(arqpizza);
				        	fclose(arquivo);

						}else if(op == 2)
						{
							//Categoria
							char novo_cat[20];
							printf("\nNova descrição: ");
							scanf(" %20[^\n]", novo_cat);
							TPizza *nova = pizza(piz->cod, piz->nome, novo_cat, piz->preco);
							fseek(arqpizza, end , SEEK_SET);
							printf("\n");
							imprime_pizza(nova);
							salva_pizza(nova, arqpizza);
							fclose(arqpizza);
				        	fclose(arquivo);

							
						}else if(op == 3){
							//preço
							float novo_preco;
							printf("\nNovo preço: " );
							scanf("%f", &novo_preco);
							TPizza *nova = pizza(piz->cod, piz->nome, piz->descricao, novo_preco);
							fseek(arqpizza, end , SEEK_SET);
							printf("\n");
							imprime_pizza(nova);
							salva_pizza(nova, arqpizza);
							fclose(arqpizza);
				        	fclose(arquivo);


						}else printf("Opção invalida.\n");

					}else {
						fclose(arquivo);
						fclose(arqpizza);
						printf("Opção não encontrada, desculpe.\n");
					}
			        
			    } else printf("\nCodigo não exite\n");


			}else if(op == 2){
				char categ[20];
				printf("\nDigite a categoria: ");
				scanf(" %20[^\n]", categ);
				printf("\n");
				listarCategoria(categ, t);

			} else{
				printf("Opção não encontrada.\n");
			}

		}else if(op == 0)
		{
			printf("\nObrigada por utilizar nosso progrma.\n");
			break;

		}else if(op == 4){
			printf("\n");
			listaMenu(t);
			printf("\n\n");

		}else{
			printf("\nDigite novamente, opção invalida.\n");
		}
	}while(1);

}