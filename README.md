**Trabalho Computacional de Estrutura de Dados e seus algoritmos**

Alunos: Barbara e Tamires

Professora: Isabel Rosseti

---

## O que se espera?

*Manipulação de árvore B+ em arquivos por meio do gerenciamento de um catálogo de pizzas*

1. Seu programa deve receber como entrada os seguintes parâmetros: o fator de ramificação (t) da árvore B+ e um catálogo inicial no formato previamente especificado. 
2. Além disso, sua implementação deve ser capaz de distinguir entre as informações principais consideradas chaves primárias (nesse caso, o código da pizza - int) e as informações subordinadas (nome da pizza - string de 50 caracteres, nome da categoria - string de 20 caracteres e preço - float). 
3. **A árvore B+ deve ser armazenada em disco.** 
4. As seguintes operações devem ser implementadas nesse trabalho:
	1. inserção e remoção de nós da árvore B+;
	2. busca das informações subordinadas, dada a chave primária;
	3. alteração SOMENTE das informações subordinadas;
	4. busca de todas as pizzas de uma determinada categoria; e
	5. remoção de todas as pizzas de uma determinada categoria. 

---

**Informações importantes:**

1. [Exemplo](http://www.ic.uff.br/~rosseti/EDA/2019-1/TC.zip) de arquivo de entrada no modo binário (que deve ser seguido pelo seu programa), bem como da estrutura usada para criá-lo, gentilmente cedida pela Professora Vanessa Braganholo. O uso das funções TPizza **- *le_pizza(FILE *in); e void imprime_pizza(TPizza *p); - ** podem ser usadas para verificar a leitura correta do arquivo binário. 
2. A **impressão** do arquivo binário **deve ser a mesma da descrita** [aqui](http://www.ic.uff.br/~rosseti/EDA/2019-1/pizza.txt);

---
  
**Datas e grupos**

1. grupo de no mínimo dois e de no máximo três discentes;
2. data limite de entrega: 01/07/2019 às 23:59h; e semana de apresentação: de 02 até 05/07/2019. 

**Mais Informações**

Podem ser encontradas no [site](http://www.ic.uff.br/~rosseti/EDA/2019-1/index.html) da professora

---

## Grupo

Tamires da Hora e Barbara Keren Guarino

---

## ANDAMENTO DO TRABALHO

**INCOMPLETO**

1. Retira
2. Retira todos de uma categoria

---

## Funções

1. Funlçoes de Inicializar
	1. Abir arquivos
	2. Inicializar arvore na memoria principal
	3. Criar nó da arvore da memoria principal

2. Funções de Impressão
	1. Imprimir pizzas
	2. Imprime a arvore em memoria principal
3. Funções de Suporte da arvore em memoria principal
	1. Adiciona o pai
	2. Adiciona o proximo
	3. Busca em arvore na memoria principal

4. Funções de inserção na arvore da memoria principal
	1. Divide os nós
	2. Insere não completo
	3. Insere

5. Funções de libere
	1. Libera arvore da memoria principal

6. *Funções de Excluir*

7. Funções para passar a arvore da memoria principal para a memoria segundaria (*arquivo*)
	1. Escreve as folhas no arquivo *"dados.dat"*
	2. Escreve os nós fantasias no arquivo *"indice.dat"*
	3. Escreve todas as folhas
	4. Escreve a arvore toda em arquivo

8. Função de Busca em memoria secundaria (*arquivo*)
	1. Busca o endereço da pizza na folha
	2. Busca em toda a arvore

9. Função de insere pizza na memoria secundaria
	1. Divide nó em arquivo
	2. Insere pizza na folha (*Nó não completo*)
	3. Busca o local de inserção
	4. Função que busca todas as outras

10. Função de imprimir de acordo com a categoria
	1. Imprime de acordo com a categoria desejada


---

## Conclusão

---
