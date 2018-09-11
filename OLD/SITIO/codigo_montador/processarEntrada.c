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
	
	int cursor = 0, pos = 0, contaLinha = 0;
	char* linha = NULL;
	char* palavra = NULL;

	char* reservaDiretivas[MAX_INST] = {
		"LOAD","LOAD-","LOAD|","LOADmq","LOADmq_mx","STOR","JUMP",
		"JMP+","ADD","ADD|","SUB","SUB|","MUL","DIV","LSH","RSH","STORA"
	};
	
	char* reservaInstrucoes[MAX_DIR] = {
		".set",".org",".align",".wfill",".word"
	};
	
	//TODO o que fazer em relacao ao TAB?
	while (cursor < tamanho) {
	
		linha = isolaLinha(&entrada);	//Isola linha por linha.
		cursor += strlen(linha);	//Conta as linhas (comecando do 1).
		//printf("|%s|\n", entrada);

		for (pos = 0; pos < strlen(linha); ++pos) {
			switch (linha[pos]) {
				case '#':	//Pula o restante da linha atual.
					pos = strlen(linha);
					break;
				case '.':	//Confere a diretiva a ser passada.
					palavra = isolaToken(&linha, &pos);
					if (comparaComReservas(palavra, reservaDiretivas, MAX_DIR))
						adicionarToken(alocaToken(palavra, Diretiva, contaLinha));
					break;					
				default:	//Caso seja um letra ou um numero.
					if (ehUmaLetra(linha[pos])) {
						palavra = isolaToken(&linha, &pos);
						if (verifcarPalavra(palavra, contaLinha, reservaInstrucoes));
					}else if (ehUmNumero(linha[pos])) {
						palavra = isolaToken(&linha, &pos);
						verificarNumero(palavra, contaLinha);
					}
					break; 
			}
		}

	}
	
	palavra = NULL;
	
	//imprimeListaTokens();
	
	return 0;
}

char* isolaLinha(char** entrada){
	char* linha = (*entrada);	//Indica o comeco da linha.
	int i = 0;
	
	//Busca o fim da linha.
	while(i < strlen(*entrada))
		if(linha[i] != '\n')
			++i;
	linha[i] = '\0';	//Separa a linha da entrada.
	
	//Busca o comeco da proxima linha.
	++i;
	while(i < strlen(*entrada))
		if((linha[i] == ' ')||(linha[i] == '\t'))
			++i;
	
	(*entrada) = &(linha[i]);	//Atualiza o inicio da entrada.
	
	printf("LINE->|%s|\n", linha);
	return linha;
}

char* isolaToken(char** linha, int* pos) {
	char* tok = (*linha);	//Indica o inicio do token.
	int i = 0;

	//busca o fim do token.
	while (i < strlen(*linha))
		if((tok[i] != ' ')&&(tok[i] != '\t'))
			++i;
	tok[i] = '\0';	//Separa o token do resto da linha.

	//Busca o inicio do proximo token.
	++i;
	while (i < strlen(*linha))
		if((tok[i] == ' ')||(tok[i] == '\t'))
			++i;
		else if(tok[i] == '\0')
		

	(*linha) = &(tok[i]);	//Atualiza o inicio da linha.
	(*pos) = 0; 

	//printf("TOK->|%s|\n", tok);
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












