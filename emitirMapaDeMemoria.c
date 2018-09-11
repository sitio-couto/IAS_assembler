// VINICIUS COUTO ESPINDOLA //
// RA: 188115				//
//////////////////////////////

/* NOTA: Para separar as palavras de 40 bits e tratar individualmente
 * as instrucoes, utilizei um vetor de 2048 dado que cada posicao
 * representa 20 bits.	Em varios momentos do codigo, valores sao 
 * multiplicados por 2 para ficar coerente com o vetor de tamanho 
 * dobrado.
 */ 

#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO 0
#define CERTO 1

///Estrutura para armazenar rotulos e simbolos definidos.
typedef struct listaRS{
	char *nome;				///apontar pro nome na lista de tokens.
	long int valor;			///decimal como padrao.
	int lado;				///0 esquerdo, 1 direito.
	struct listaRS *prox;	///apontador para proximo elemnto.
}listaRS; 

void criaPalavra(char* novo, char* novo2, long int dec, TipoDoToken caso, int lado);
void addNome(listaRS **atual, char *nome, long int valor, int lado);
void posicionamento(int indice, TipoDoToken tipo, int* end);
listaRS* buscaListaRS(listaRS* inicio, char* nome);
int testaErro(int caso, int end, char* linha);
char* intToStringHex(long int quociente);
char** montarMapa(listaRS* inicio);
char* instrucaoPraHexa(char* inst);
void freelistaRS(listaRS *atual);
void imprimirMapa(char** mapa);
void freeMapa(char** mapa);
int numArgs(char* palavra);
listaRS* lerNomes(void);

/* Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int emitirMapaDeMemoria(){
	listaRS *inicio = NULL;
	char** mapa = NULL;
	
	inicio = lerNomes();
	
	mapa = montarMapa(inicio);

	if(mapa == ERRO) return 1;
	
	imprimirMapa(mapa);

	freelistaRS(inicio);
	freeMapa(mapa);

	return 0;
}

///Imprime Mapa com a formatacao adequada.
void imprimirMapa(char** mapa){
	///Separa em casos com toda linha, apenas esquerda e apenas direita.
	for(int i = 0; i < 2048; ++i){
		if(mapa[i]){
			printf("%s ", &(intToStringHex(i/2)[7]));
			if(i%2){
				printf("00 000 ");
				printf("%c%c ", mapa[i][0], mapa[i][1]);
				printf("%s\n", &mapa[i][2]);
			}else{
				printf("%c%c ", mapa[i][0], mapa[i][1]);
				printf("%s", &mapa[i][2]);
				if(mapa[++i]){
					printf(" %c%c ", mapa[i][0], mapa[i][1]);
					printf("%s\n", &mapa[i][2]);
				}else printf(" 00 000\n");
			}
		
		}
	}
}

///Monta uma matriz com o mapa de memoria a ser impresso.
char** montarMapa(listaRS* inicio){
	char **saida = calloc(2048, sizeof(char*));	///Mapa e novos blocos (20 bits).
	char *novo = NULL, *novo2 = NULL;			///novos blocos de 20 bits.
	Token tok, tok2, tok3;						///Macros para tokens.
	listaRS *aux = NULL;						///Ponteiro para nome.
	long int valor = 0;							///valor numerico de tokens.
	int end = 0;								///Endereco a ser adicionado e valor.
	
	for(int i = 0; i < getNumberOfTokens(); ++i){	///Itera sobre tokens
		Token tok = recuperaToken(i);
			
		switch(tok.tipo){	///Seleciona tokens
			case Instrucao:
				novo = instrucaoPraHexa(tok.palavra);
				
				if(numArgs(tok.palavra)){///Testa numero de args da inst
					tok2 = recuperaToken(i + 1);	
						
					if(tok2.tipo == Nome){///Se o arg for um Nome
						aux = buscaListaRS(inicio, tok2.palavra);
						if(!aux) return ERRO;///Testa se o nome foi encontrado na lista.
			
						criaPalavra(novo, 0, aux->valor, Instrucao, aux->lado);///Ajusta corretamente a palavra.
						
					}else{ ///Se for um Hexa/Dec 
						valor = strtol(tok2.palavra, 0, 0);///Pega o valor direto do token
						
						criaPalavra(novo, 0, valor, Instrucao, 0); 
					}
				}
				else strcpy( &(novo[2]), "000");///Caso nao possua args, preenche com "000"
				
				if(!testaErro(1, 0, saida[end])) return ERRO;///Testa se nao ocorre sobrescrita.
				
				saida[end] = novo;
				break;
			case Diretiva:
				novo = calloc(6, sizeof(char));
				novo2 = calloc(6, sizeof(char));
				
				tok2 = recuperaToken(i + 1);
			
				if(!strcmp(tok.palavra, ".word")){
					if(!testaErro(0, end, 0)) return ERRO;///Testa alinhamento.
					
					///Testa se o arg eh um nome ou numero
					if(tok2.tipo == Nome){
						aux = buscaListaRS(inicio, tok2.palavra);
						if(!aux) return ERRO; ///Testa se encontrou a definicao do Nome
						printf("value->%li\n", aux->valor); 
						criaPalavra(novo, novo2, aux->valor, Diretiva, aux->lado);
					}
					else{
						valor = strtol(tok2.palavra, 0, 0);
						
						criaPalavra(novo, novo2, valor, Diretiva, 0);
					}
					
					///Testa se nao ocorre sobrescrita.
					if(!testaErro(1, 0, saida[end])) return ERRO;
					if(!testaErro(1, 0, saida[end + 1])) return ERRO;
					
					///Salva a nova linha.
					saida[end] = novo;
					saida[end + 1] = novo2;
				}
				else if(!strcmp(tok.palavra, ".wfill")){
					if(!testaErro(0, end, 0)) return ERRO; ///Testa alinhamento.
					
					tok3 = recuperaToken(i + 2);///pega o segundo arg.
					
					///Testa se o arg eh um Nome.
					if(tok3.tipo == Nome){
						aux = buscaListaRS(inicio, tok3.palavra);
						if(!aux) return ERRO;  ///Testa se encontrou a definicao do Nome
							
						criaPalavra(novo, novo2, aux->valor, Diretiva, aux->lado);
					}
					else{ 
						valor = strtol(tok3.palavra, 0, 0);
						
						criaPalavra(novo, novo2, valor, Diretiva, 0);
					}
					
					///Itera sobre posicoes alocando novos vetores.
					for(int j = 0; j < 2*strtol(tok2.palavra, 0, 0); ++j){
						if(!testaErro(1, 0, saida[end + j])) return ERRO; ///Testa se nao ocorre sobrescrita.
						
						saida[end + j] = calloc(6, sizeof(char));
						
						///Testa se a posicao eh par ou impar para saber quais 20 bits inserir.
						if((end+j)%2) strcpy(saida[end + j], novo2); ///Caso direita
						else strcpy(saida[end + j], novo); ///Caso esquerda
					}
				}
				break;
		}
		
		
		posicionamento(i, tok.tipo, &end);///Atualiza o end de escrita.	
	}
	
	return saida;	
}

///Traduz instrucao pra Hexadecimal.
char* instrucaoPraHexa(char* inst){
	char* novo = calloc(6, sizeof(char));///Aloca 5 espacos para meia palavra.
	novo[5] = '\0';///garante um caractere nulo para operacao de string.
	
	char* instrucoes[17] = {
		"LOAD","LOAD-","LOAD|","LOADmq","LOADmq_mx","STOR","JUMP",
		"JMP+","ADD","ADD|","SUB","SUB|","MUL","DIV","LSH","RSH","STORA"
	};
	char* instrucoesHexa[17] = {
		"01","02","03","0A","09","21","0D_0E",
		"0F_10","05","07","06","08","0B","0C","14","15","12_13"
	};
	
	for(int i = 0; i < 17; ++i){
		if(!strcmp(inst, instrucoes[i]))
			strcpy(novo, instrucoesHexa[i]);	///Copia a inst em Hexa.
	}
	
	return novo;
}

///Ajusta a palavra. 
void criaPalavra(char* novo, char* novo2, long int dec, TipoDoToken caso, int lado){
	char* valor = intToStringHex(dec);
	
	switch(caso){
		case Instrucao:///Ajusta a inst e o arg dela.
			if(strlen(novo) == 5){///se for necessario especificar o lado do arg (E/D) 
				if(!lado%2) novo[2] = '\0';///Se for para esquerda.
				else{///Pega instrucao com alvo a direita
					novo[0] = novo[3];
					novo[1] = novo[4];
					novo[3] = '\0';
				}
			}
			
			///Copia apenas os tres ultimos algarismos.
			strcpy(&novo[2], &valor[7]);
			break;
		case Diretiva:
			strcpy(novo2, &valor[5]);///Copia 5 algarismos menos significativos
			valor[5] = '\0';
			strcpy(novo, valor);///Copia 5 algarismos mais significativos
			break;
	} 
	
	free(valor);
}

///Converte um int ou string para uma string em Hexa.
char* intToStringHex(long int quociente){
	long int resto;
	char hexa[] = {"0123456789ABCDEF"}; 
	char* dado = calloc(11, sizeof(char));
	
	///incializa com 0s a string
	strcpy(dado, "0000000000");
		
	///Converte de decimal pra uma string em hexaDecimal 
	for(int i = 0; quociente; ++i){
		resto = quociente%16;
		quociente /= 16;
		dado[9 - i] = hexa[resto];
	}
	
	///Retorna o dado com 10 algarismos Hexademais
	return dado;	
}

///Testa se a instrucao ṕossui 1 ou nenhum args e retorna este valor (1/0).
int numArgs(char* palavra) {
	///testa se eh uma inst sem args.
	if(!strcmp(palavra, "RSH") || !strcmp(palavra, "LSH") || !strcmp(palavra, "LOADmq"))
		return 0;///nao possui args.
	else///Se possuir args.
		return 1;///possui 1 arg.
}

///Faz uma lista assimilando os rot/sim com seus repectivos end/val.
listaRS* lerNomes(void){
	listaRS *inicio = NULL;
	Token tok, tok2, tok3;
	int lado = 0, end = 0;///Posicao da memoria.
	long int aux = 0;
	
	for(int i = 0; i < getNumberOfTokens(); ++i){
		tok = recuperaToken(i);
		
		if(tok.tipo == DefRotulo){
			tok.palavra[strlen(tok.palavra) - 1] = '\0';	///Remove os dois pontos.
		
			addNome(&inicio, tok.palavra, end/2, end%2);///Assimila rotulo a um end.
		}
		else if(!strcmp(tok.palavra,".set")){
			tok2 = recuperaToken(++i);	///Pega primeiro arg.
			tok3 = recuperaToken(++i);	///Pega segundo arg.
			aux = strtol(tok3.palavra, 0, 0);	///Converte arg para decimal.
			
			addNome(&inicio, tok2.palavra, aux, 0);///Assimila simbolo a um val.
		}
		else posicionamento(i, tok.tipo, &end);
	}
	
	return inicio;	
}

///Calcula e atualiza o end da memoria.
void posicionamento(int indice, TipoDoToken tipo, int *end){
	Token tok2, tok = recuperaToken(indice);
	
	switch(tipo){
		case Diretiva:
			tok2 = recuperaToken(++indice);
		
			if(!strcmp(tok.palavra, ".org")){					
				(*end) = 2*strtol(tok2.palavra, 0, 0);
			}else if(!strcmp(tok.palavra, ".wfill")){
				(*end) += 2*strtol(tok2.palavra, 0, 0); ///Soma com deslocamento.
			}else if(!strcmp(tok.palavra, ".align")){
				while((*end)%(2*strtol(tok2.palavra, 0, 0)) != 0) 
					++(*end);  ///Incremente end ate achar uma multiplo do argumento.
			}else if(!strcmp(tok.palavra, ".word")){
				(*end) += 2;
			}
			break;
		case Instrucao:
			++(*end);
			break;
	}///Caso de hex/dec/Nome nao altera o endereco.
}

///Verifica se ha algum rotulo usado mas nao definido e recupera se houver.
listaRS* buscaListaRS(listaRS *atual, char* nome){
	
	///Percorre os nomes definidos.
	while(atual != NULL){
		if(!strcmp(nome, atual->nome)) 
			return atual;///Retorna o simbolo/rotulo correspondente.
		atual = atual->prox;
	}
	
	printf("USADO MAS NÃO DEFINIDO: %s!\n", nome);
	
	return ERRO;
}

///Checka alguns casos de erro.
int testaErro(int caso, int end, char* linha){
	int veracidade = CERTO;
	///Casos de erro.
	switch(caso){
		case 0: /// Desalinhamento.
			if(end%2) veracidade = ERRO;
			break;
		case 1: /// Sobrescita de codigo.
			if(linha) veracidade = ERRO;
			break;
	}
	
	if((end < 0)||(end > 2047)) veracidade = ERRO;	 /// Posicao de memoria invalida.
	
	if(veracidade == ERRO){
		printf("Impossível montar o código!\n");
		return ERRO;
	}
	else return CERTO;
}

///Adiciona nos a lista de rotulos e simbolos.
void addNome(listaRS **atual, char *nome, long int valor, int lado){
	listaRS *novo = calloc(1, sizeof(listaRS));
		
	novo->nome = nome;
	novo->valor = valor;
	novo->lado = lado;
	
	while((*atual) != NULL) atual = &((*atual)->prox);
	
	(*atual) = novo;
}

///Desaloca lista de rotulos e simbolos.
void freelistaRS(listaRS *atual){
	listaRS *aux;
	
	while(atual != NULL){
		aux = atual->prox;
		free(atual);
		atual = aux;
	}		
}

///Desaloca o mapa de memoria criado.
void freeMapa(char** mapa){
	
	for(int i = 0; i < 2048; ++i){
		if(mapa[i]){
			free(mapa[i]);
		}
	}
	
	free(mapa);
}
