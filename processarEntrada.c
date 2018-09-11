//NOME: VINICIUS COUTO ESPIDOLA	//
//RA: 188115					//
//////////////////////////////////


#include "montador.h"
#include <stdio.h>

#define CERTO 0
#define ERRO 1
#define TRUE 1
#define FALSE 0
#define N_DIR 5
#define N_INST 17

void criaToken(char* palavra, TipoDoToken tipo, int linha);
int tamlin(char* frase);
int cmplin(char* obtida, char* esperada);
int testaNumArgs(char* token, int numToks);
int removeAspas(char** tokComAspas);
char* isolaLinha(char** entrada);
char* isolaToken(char** linha);
int comparaComReservas(char* tok, char** reservas, int tamReservas);
int ehLetra(int valorAscii);
int ehNum(int valorAscii);
int testaHexa(char* tok);
int testaDec(char* tok);
int testaNome(char* tok, int fim);
int erroGramatical(unsigned indiceLinha);

/* 
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
 
int processarEntrada(char* entrada, unsigned tamanho) 
{
	unsigned contaLinha = 0, tokenAtual = 0;	///Marca a linha atual e o token atual;
	char *linha = NULL, *tok = NULL;			///ponteiros pra separa entrada/linha/token.

	///Tabelas de palavras reservadas (Instrucoes/Diretivas).
	char* instrucoes[17] = {
		"LOAD","LOAD-","LOAD|","LOADmq","LOADmq_mx","STOR","JUMP",
		"JMP+","ADD","ADD|","SUB","SUB|","MUL","DIV","LSH","RSH","STORA"
	};
	char* diretivas[5] = {".set",".org",".align",".wfill",".word"};

	while(entrada[0] != '\0'){///Loop para percorrer as linhas.
		linha = isolaLinha(&entrada);
		++contaLinha;
		tokenAtual = getNumberOfTokens();///Salva a posicao do primeiro token da linha a ser lida.
		
		while(linha[0] != '\0'){///Loop para percorrer as palavras.
			tok = isolaToken(&linha);///Separa o token do restante da linha.
			if(removeAspas(&tok)) return ERRO;///Remove aspas se necessario e verifica colocacao.

			if(tok[0] == '#'){	///Pula o restante da linha atual.
				linha[0] = '\0';
			}else if(tok[0] == '.' && comparaComReservas(tok, diretivas, N_DIR)){///Confere a diretiva a ser passada.
				criaToken(tok, Diretiva, contaLinha);
			}else if((tok[tamlin(tok)-1] == ':') && testaNome(tok, tamlin(tok)-1)){///Testa o rotulo ate os ':'.
				criaToken(tok, DefRotulo, contaLinha);
			}else if(comparaComReservas(tok, instrucoes, N_INST)){ ///Compara com inst conhecidas.
				criaToken(tok, Instrucao, contaLinha);
			}else if(testaHexa(tok)){///Caso valor heaxadecimal.
				criaToken(tok, Hexadecimal, contaLinha);
			}else if (testaDec(tok)){///Caso valor decimal.
				criaToken(tok, Decimal, contaLinha);
			}else if(testaNome(tok, tamlin(tok))){///Testa se eh um nome valido.
				criaToken(tok, Nome, contaLinha);
			}else{///Aponta um erro lexico.
				printf("ERRO LEXICO: palavra inválida na linha %i!\n", contaLinha);
				return ERRO;
			}
		}
		
		if(erroGramatical(tokenAtual)){///Busca erros gramaticais.
			printf("ERRO GRAMATICAL: palavra na linha %i!\n", contaLinha);
			return ERRO;
		}
	}	
	
	//imprimeListaTokens();
	
	return CERTO;
}

///FUNCAO PARA CRIAR E ALOCAR O NOVO TOKEN//////////////////////////////

void criaToken(char* palavra, TipoDoToken tipo, int linha) {
	Token novo;
	novo.linha = linha;
	novo.palavra = palavra;
	novo.tipo = tipo;

	adicionarToken(novo);
}

///FUNCOES DE MANIPULACAO DE STRING/////////////////////////////////////

int tamlin(char* frase){					///Substitui o strlen().
	int i = 0;
	
	while (frase[i] != '\0') { ++i; }
	
	return i;
}

int cmplin(char* obtida, char* esperada){  ///Substitui o strcmp().	
	int i = 0;
	
	for(int i = 0; i <= tamlin(esperada); ++i)
		if(obtida[i] != esperada[i]) return FALSE;
	
	return TRUE;
}

int removeAspas(char** tokComAspas){		///Tira aspas de argumentos.
	char* tok = (*tokComAspas);	
	///Cofere se as aspas foram colacadas corretamente e as remove caso precise.
	if(tok[0] == '"'){
		if(tok[tamlin(tok) - 1] != '"')
			return ERRO;///caso nao haja aspas em ambos os lados.
		else{///remove aspas.
			(*tokComAspas) = &(tok[1]);
			tok[tamlin(tok) - 1] = '\0';
		}	
	}else if(tok[tamlin(tok) - 1] == '"')
		return ERRO;///caso nao haja aspas em ambos os lados.
	
	return CERTO;
}

char* isolaLinha(char** entrada){			///Separa a linha da entrada.
	char* linha = (*entrada);
	int i = 0;
	
	///Busca a quebra de linha ou o ''\0'.
	while((linha[i] != '\n')&&(linha[i] != '\0'))
		++i;		
	
	///Se for uma quebra de linha, busca o inicio da proxima linha
	if(linha[i] == '\n'){
		linha[i] = '\0';	///Isola linha.

		///Elimina os espacos em branco no inicio da linha.
		do{
			++i;
		}while((linha[i] == ' ')||(linha[i] == '\t'));
	}		
	
	(*entrada) = &(linha[i]);///Atualiza a entrada para a proxima linha.
	
	return linha;
}

char* isolaToken(char** linha){				///Separa palavra da linha.
	char* tok = (*linha);
	int i = 0;
	
	///Busca fim do token.
	while((tok[i] != ' ')&&(tok[i] != '\0')&&(tok[i] != '\t'))
		++i;
		
	///Testa se ainda nao chegou no fim da linha.	
	if(tok[i] != '\0'){
		tok[i] = '\0';	///isola token lido.
		++i;

		///Busca inicio do proximo token ou o fim da linha.
		while((tok[i] == ' ')||(tok[i] == '\t'))
			++i;
	}
	
	(*linha) = &(tok[i]);	///Elimina o token lido da linha.
	
	return tok;
}

///FUNCOES DE TESTE/////////////////////////////////////////////////////

int comparaComReservas(char* tok, char** reservas, int tamReservas) {	///Busca a diretiva/intrucao da tabela de reservas.
	for (int i = 0; i < tamReservas; ++i){
		if (cmplin(tok, reservas[i])) return TRUE;
	}
	
	return FALSE;
}

int ehNum(int valorAscii){	///Testa se eh um numero.
	if((valorAscii>='0')&&(valorAscii<='9'))
		return TRUE;
		
	return FALSE;
}

int ehLetra(int valorAscii){	///Testa se eh uma letra.
	if((valorAscii>='A')&&(valorAscii<='Z'))
		return TRUE;
	else if((valorAscii>='a')&&(valorAscii<='z'))
		return TRUE;
	
	return FALSE;
}

int testaHexa(char* tok){	///Testa se o token obedece as regras Hexadecimal.
	if(tok[1] != 'x') return FALSE;///Testa se a base eh Hexa.

	///Percorre elementos testando se sao numeros ou letras de [A;F], se for pula a iteracao ignorando o FALSE.
	for(int i = 2; i < tamlin(tok); ++i){
		if(ehNum(tok[i])) continue;
		if((tok[i] >= 'A')&&(tok[i] <= 'F')) continue;
		if((tok[i] >= 'a')&&(tok[i] <= 'f')) continue;
		return FALSE;
	}
	
	return TRUE;
}

int testaDec(char* tok){	///Testa se os elementos sao todos numericos.
	for(int i = 0; i < tamlin(tok); ++i)
		if(!ehNum(tok[i])) return FALSE;
	
	return TRUE;
}

int testaNome(char* tok, int fim){	///Testa se a palavara eh composta apenas de alfanumericos e underscore.
	if(!ehLetra(tok[0]) && (tok[0] != '_')) return FALSE;
	
	for(int i = 1; i < fim; ++i){
		if(!ehNum(tok[i]) && !ehLetra(tok[i]) && (tok[i] != '_')) 
			return FALSE;
	}
	
	return TRUE;
}

int erroGramatical(unsigned i){		///Busca erros gramaticais na linha recem lida.	(i -> idica pos do primeiro token da linha).
	Token tok, tok2, tok3;///Macros para os possiveis tokens da linha.
	unsigned lastTok = getNumberOfTokens(), numToks;	///Posicao do ultimo token criado / Numero de tokens na linha.
	
	if (recuperaToken(i).tipo == DefRotulo) { ++i; }///Pula para o proximo token se comecar com DefRotulo.

	numToks = (lastTok - i);///Define o numero de tokens restantes na linha. 
	if(numToks == 0) return CERTO;///Caso linha em branco ou DefRotulo sem nada depois.
	if(numToks > 3) return ERRO;///Caso haja mais que o possivel de Inst/Dir/Argumento em uma linha.
	
	///Atualiza os macros dos tokens da linha dependendo do numToks da linha.
	tok = recuperaToken(i);
	if(numToks >= 2) tok2 = recuperaToken(++i);
	if(numToks == 3) tok3 = recuperaToken(++i);
	
	switch(tok.tipo){///Separa os erros em 4 casos em fucao do primeiro token: DefRotulo/Diretivas/Instrucao/Nome-Hexa-Dec
		case DefRotulo:
			return ERRO;///Sempre remove DefRotulo do comeco da linha, se houver outra eh um erro.
		case Diretiva:///Separa as diretivas pelo numero de argumentos que aceitam.			
			if(numToks == 3){///Testa se eh alguma das dir que aceitam 2 args e se os tipos dos args sao validos.
				if(cmplin(tok.palavra, ".set")){
					if(tok2.tipo != Nome) 
						return ERRO;
					if(!ehNum(tok3.palavra[0])) 
						return ERRO;
				}else if(cmplin(tok.palavra, ".wfill")){
					if(tok2.tipo != Decimal) 
						return ERRO;
					if((!ehNum(tok3.palavra[0]))&&(tok3.tipo != Nome))	
						return ERRO;
				}else return ERRO;///Se nao eh uma dir que aceita 2 args, retornda erro.
			}else if(numToks == 2){///Testa se eh alguma dir que aceita 1 arg e se o tipo do arg eh valido.
				if(cmplin(tok.palavra, ".word")){
					if((!ehNum(tok2.palavra[0]))&&(tok2.tipo != Nome))
							return ERRO;
				}else if(cmplin(tok.palavra, ".org")){
					if(!ehNum(tok2.palavra[0])) 
						return ERRO;
				}else if(cmplin(tok.palavra, ".align")){
					if(tok2.tipo != Decimal) 
						return ERRO;
				}else return ERRO;
			}else return ERRO;///Se nao houver args para a diretiva, ou exdecer o maximo de 2, retorna erro.
			break;
		case Instrucao:///Separa as instrucoes que requerem um ou nenhum arg.
			if(numToks == 2){///Testa se ha 1 arg.
				if ((!ehNum(tok2.palavra[0])) && (tok2.tipo != Nome)) ///Testa se eh um arg de tipo valido.
					return ERRO;
				else if (!testaNumArgs(tok.palavra, numToks)) ///Testa se eh uma Inst que requer pelo menos 1 arg;
					return ERRO;
			}else if(numToks == 1){
				if (!testaNumArgs(tok.palavra, numToks)) ///Testa se eh uma inst sem arg.
					return ERRO;
			}else return ERRO;///se exceder 1 arg retorna erro.
			break;
		default:///Retora erro em casos que ha apenas um tipo Nome/Hexa/Dec, ou um DefRotulo e um Nome/Hexa/Dec.
			return ERRO;///uma DefRotulo nao pode ser seguida apenas por um Nome/Hexa/Dec.
	}
	
	return CERTO;
}

int testaNumArgs(char* palavra, int numToks) {	///Testa se o numero de argumentos eh INcompativel com a instrucao.
	if (numToks == 2)///testa se eh uma inst de 1 args.
		return	(!cmplin(palavra, "RSH") && !cmplin(palavra, "LSH") && !cmplin(palavra, "LOADmq"));///se palavra nao ser nehuma destas, retorna true.
	else///testa se eh uma inst sem args.
		return (cmplin(palavra, "RSH") || cmplin(palavra, "LSH") || cmplin(palavra, "LOADmq"));///se palavra ser alguma destas, retorna true.
}

