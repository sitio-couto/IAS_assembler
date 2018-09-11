#include "montador.h"
#include <stdio.h>
#include <string.h>

#define MAX_DIR 5
#define MAX_INST 17

char* isolaLinha(char** entrada);
char* isolaToken(char** linha, int* pos);
Token alocaToken(char* palavra, TipoDoToken tipo, int linha);
int comparaComReservas(char* tok, char** reservas, int tamReservas);
int ehUmaLetra(int valorAscii);
int ehUmNumero(int valorAscii);
int verificarNumero(char* linha, int contaLinha);
int verifcarPalavra
(
	char* linha, 
	int contaLinha, 
	char** reservaInstrucoes
);

/*
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem;
 *  0 caso não haja erro.
 */
int processarEntrada(char* entrada, unsigned tamanho) {
	int i, j;

  char* linha;
  char* tok;

  for(i=0;i< tamanho; ++i){
  	linha = isolaLinha(&entrada);
  	printf("LINE->|%s|\n", linha);

  	for(j=0;j<strlen(linha);++j){
  		tok = isolaToken(&linha, &j);
  		printf("TOK->|%s|\n", tok);
  	}
  }

  printf("\n-------------DONE-------------------\n");
}

char* isolaLinha(char** entrada){
	char* linha = (*entrada);	//Indica o comeco da linha.
	int i = 0;
	
	while((linha[i] != '\n')&&(linha[i] != '\0'))
		++i;
	linha[i] = '\0';	//Separa a linha da entrada.
	
	(*entrada) = &(linha[++i]);	//Atualiza o inicio da entrada.

	return linha;
}

char* isolaToken(char** linha, int* pos) {
	char* tok = (*linha);	//Indica o inicio do token.
	int i = 0;

	while ((tok[i] != ' ')&&(tok[i] != '\n')&&(tok[i] != '\t'))
		++i;

	tok[i] = '\0';	//Separa o token do resto da linha.

	++i;
	while ((tok[i] == ' ')||(tok[i] == '\n')||(tok[i] == '\t'))
		++i;

	(*linha) = &(tok[i]);	//Atualiza o inicio da linha.
	(*pos) = 0; 

	//printf("%s|\n", tok);
	return tok;
}

Token alocaToken(char* palavra, TipoDoToken tipo, int linha) {
	Token novo;
	novo.linha = linha;
	novo.palavra = palavra;
	novo.tipo = tipo;

	return novo;
}

int comparaComReservas(char* tok, char** reservas, int tamReservas) {
	int i;

	for (i = 0; i < tamReservas; ++i)	//Busca a diretiva da tabela de reservas.
		if (strcmp(tok, reservas[i])) 
			return 1;

		return 0;
}

int ehUmaLetra(int valorAscii){
	if((valorAscii>='A')&&(valorAscii<='Z'))
		return 1;
	else if((valorAscii>='a')&&(valorAscii<='a'))
		return 1;
	else 
		return 0;
}

int ehUmNumero(int valorAscii){
	if((valorAscii>='0')&&(valorAscii<='9'))
		return 1;
	else
		return 0;
}

int verifcarPalavra(char* palavra, int contaLinha, char** reservaInstrucoes){

	if (palavra[strlen(palavra) - 1] == ':') {	//Verifica se eh um rotulo a ser definido.
		adicionarToken(alocaToken(palavra, DefRotulo, contaLinha));
		//Novo rotulo definido.
		return 1;
	} else if (comparaComReservas(palavra, reservaInstrucoes, MAX_INST)) {	//Busca a instrucao na tabela de reservas.
		adicionarToken(alocaToken(palavra, Instrucao, contaLinha));
		//Armazenada a nova diretiva.
		return 1;
	} else {	//Cria um nome por exclusao de possibilidades.		
		adicionarToken(alocaToken(palavra, Nome, contaLinha));
		return 1;
	}

	return 0;	//TODO caso de erro.
}

int verificarNumero(char* palavra, int contaLinha){

	if(palavra[1] == 'x')
		adicionarToken(alocaToken(palavra, Hexadecimal, contaLinha));
	else
		adicionarToken(alocaToken(palavra, Decimal, contaLinha));
}












