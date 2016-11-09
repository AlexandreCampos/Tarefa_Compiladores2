#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include "tabela_hash.c"

Tabela tst2[MAX_TABELA];

int ehLetra(char ch);
int ehNumero(char ch);
int ehEspecial(char* cadeia,FILE *arq_saida);
char next_char(char* cadeia);
void salvar_token(char* lexema, int indice_hash, FILE *arq_saida);
void imprime_tnt(char nome_arquivo_saida[]);

int indice_cadeia; // variavel global que guarda a posicao corrente DENTRO da linha (cadeia)
int numero_linha = 0;

int list_source = 1;
int list_token = 0;
int list_tst = 0;
int list_tnt = 0;



void lexanalysis(char* cadeia, FILE *arq_saida) {
	
	indice_cadeia = -1;
	char simbolo;
	char lexema[40];
	char tipo_token[20];
	
	if (list_source) printf("Linha %d: %s \n", numero_linha, cadeia);
	
	simbolo = next_char(cadeia);

	// percorre a LINHA
	while (simbolo != '\0') {
		int estado = 0;
		int outros = 0;
		int erro = 0;
		lexema[0] = '\0';
		int i = 0; // indice lexema (posicao corrente do lexema)
		int indice_hash_retornado;
		int alfanumerico = 0;
		int decimal = 0;
		int inteiro = 0;
		int constante_alfanumerica = 0;		
		
		// percorre o LEXEMA
		while (!outros) {
			//printf("Estado: %d - ", estado);
			//printf("Simbolo: %c \n", simbolo);
			
			// AUTOMATOS FINITOS (AEF)
			switch (estado) {
				
			case 0:
				
				if (ehLetra(simbolo)) {
					estado = 4;
					lexema[i] = simbolo;
					i++;
				} else if (ehNumero(simbolo)) {
					estado = 1;
					lexema[i] = simbolo;
					i++;
				} else if (simbolo == '.') {
					estado = 2;
					lexema[i] = simbolo;
					i++;
				} else if (simbolo == ' '){
					//printf("espaco \n");										
				} else if (simbolo == '\t'){
					//printf("TAB \n");										
				} else if (simbolo == '"'){
					estado = 5;
					lexema[i] = simbolo;
					i++;									
				} else if (simbolo == '\''){
					estado = 9;
					lexema[i] = simbolo;
					i++;									
				} else if (simbolo == '/'){
					estado = 6;
					//lexema[i] = simbolo;
					//i++;									
				}else if (simbolo == '#'){
					estado = 8;
					lexema[i] = simbolo;
					i++;									
				}else {				
					ehEspecial(cadeia,arq_saida);					
					outros = 1;
					simbolo = next_char(cadeia);
				}
				break;

			case 1: // NUMERO INTEIRO

				//printf("Passei no case 1 \n");	
				if (ehNumero(simbolo)) {
					estado = 1;
					lexema[i] = simbolo;
					i++;
				} else if (simbolo == '.') {
					estado = 3;
					lexema[i] = simbolo;
					i++;
				}

				else {
					outros = 1;
					inteiro = 1; 
				}
				break;

			case 2: // NUMERO FLOAT

				//printf("Passei no case 2 \n");
				if (ehNumero(simbolo)) {
					estado = 3;
					lexema[i] = simbolo;
					i++;
				} else {
					outros = 1;
				}
				break;

			case 3: // NUMERO FLOAT

				//printf("Passei no case 3 \n");
				if (ehNumero(simbolo)) {
					estado = 3;
					lexema[i] = simbolo;
					i++;
				} else {
					outros = 1;
					decimal = 1;
				}
				break;

			case 4: // CARACTERE ALFANUMERICO (ID, PALAVRAS RESERVADA)

				//printf("Passei no case 4 \n");	
				if (ehLetra(simbolo) || ehNumero(simbolo)) {
					estado = 4;
					lexema[i] = simbolo;
					i++;
				} else {
					outros = 1;
					alfanumerico = 1;

				}
				break;
						
			case 5: // CONSTANTE ALFANUMERICA. Exemplo: "sou_uma_constante_alfanumerica"
				
				//printf("Passei no case 5 \n");		
				if (simbolo != '"'&& simbolo != '\n') {					
					estado = 5;
					lexema[i] = simbolo;
					i++;
				} else if (simbolo == '\n'){
					lexema[i] = '\0';
					printf("Erro. Constante alfanumerica sem fim. Linha: %d. Local: %s \n",numero_linha,lexema);
					estado = 0;
					erro = 1;
				} else {					
					lexema[i] = simbolo;
					i++;	
					simbolo = next_char(cadeia);
					outros = 1;
					constante_alfanumerica = 1;		
				}
				break;
			
			case 6: // COMENTARIOS

				//printf("Passei no case 6 \n");
				if (simbolo == '/') {
					estado = 7;
					//lexema[i] = simbolo;
					//i++;
				} else {
					printf("Erro. Simbolo '/' sozinho. Linha %d\n", numero_linha);					
					erro = 1;
					outros = 1;
				}
				break;

			case 7: // COMENTARIOS

				//printf("Passei no case 7 \n");
				if (simbolo != '\0') {	
					estado = 7;				
					//lexema[i] = simbolo;
					//i++;
				} else {
					//printf("Comentarios (descartando). \n");
					outros = 1;
				}
				break;
			
			case 8: // DIRETIVAS DE COMPILACAO

				//printf("Passei no case 8 \n");
				if (simbolo != '\0') {	
					estado = 8;				
					lexema[i] = simbolo;
					i++;
				} else {
					lexema[i-1] = '\0';
					outros = 1;						
					if (strcmp(lexema,"#list_source_on") == 0)	
					{
						printf("Diretiva list_source_on ativada. \n");
						list_source = 1;						
					}		
					else if(strcmp(lexema,"#list_source_off") == 0)
					{
						printf("Diretiva list_source_off ativada. \n");
						list_source = 0;						
					}
					else if(strcmp(lexema,"#list_token_on") == 0)
					{
						printf("Diretiva list_token_on ativada. \n");
						list_token = 1;						
					}								
					else if(strcmp(lexema,"#list_token_off") == 0)
					{
						printf("Diretiva list_token_off ativada. \n");
						list_token = 0;						
					}
					else if(strcmp(lexema,"#list_tst") == 0)
					{
						printf("Diretiva list_tst ativada. \n");
						list_tst = 1;						
					}
					else if(strcmp(lexema,"#list_tnt") == 0)
					{
						printf("Diretiva list_tnt ativada. \n");
						list_tnt = 1;						
					}																																
					else
					{
						printf("Erro. Diretiva desconhecida: %s. Linha: %d\n", lexema, numero_linha);						
					}
				}
				break;
			
			case 9: // CONSTANTE ALFANUMERICA. Exemplo: 'a'
				
				//printf("Passei no case 9 \n");		
				if (simbolo != '\''&& simbolo != '\n') {					
					estado = 10;
					lexema[i] = simbolo;
					i++;
				} else if (simbolo == '\n'){
					lexema[i] = '\0';
					printf("Erro. Constante alfanumerica sem fim. Linha: %d. Local: %s \n",numero_linha,lexema);
					estado = 0;
					erro = 1;
				}
				break;
			
			case 10: // CONSTANTE ALFANUMERICA. Exemplo: 'a'
				
				//printf("Passei no case 9 \n");		
				if (simbolo == '\''&& simbolo != '\n') {					
					lexema[i] = simbolo;
					i++;	
					simbolo = next_char(cadeia);
					outros = 1;
					constante_alfanumerica = 1;
				} else if (simbolo != '\n'){
					printf("Erro. 'char' deve ter 1 digito. Linha: %d. Local: %s \n",numero_linha,lexema);
					estado = 0;
					outros = 1;
					erro = 1;										
				} else {
					lexema[i] = '\0';
					printf("Erro. Constante alfanumerica sem fim. Linha: %d. Local: %s \n",numero_linha,lexema);
					estado = 0;
					erro = 1;
				}
				break;
				
			}
			// FIM DA CODIFICACAO DOS AUTOMATOS FINITOS (AEF)
			// prossegue no lexema até encontrar simbolo desconhecido (EOF do lexema = outros)
			if (!outros && !erro)
				simbolo = next_char(cadeia);

		}
		// FIM DO LEXEMA
		lexema[i] = '\0';
		// VERIFICA QUAL TIPO DE LEXEMA FOI ENCONTRADO
		// FORMA O TOKEN (par = lexema / significado)
		// Exemplo: variavel / ID		
		if (alfanumerico) {
			indice_hash_retornado = consulta_Insere(tst2, lexema, 'C', 0);
			if (indice_hash_retornado != -1) {
				salvar_token(lexema, indice_hash_retornado, arq_saida);			
			} else {
				salvar_token(lexema, 226, arq_saida);
			}
		} else if (inteiro) {
			salvar_token(lexema, 227, arq_saida);
		} else if (decimal) {
			salvar_token(lexema, 228, arq_saida);
		} else if (constante_alfanumerica) {
			salvar_token(lexema, 229, arq_saida);			
		}else {
			// branco, tab, comentarios, etc
		}
		// FIM DO LOOP DO LEXEMA
		// CONTINUA NA MESMA LINHA, IDENTIFICANDO OUTROS LEXEMAS, ATE CHEGAR AO FIM DA LINHA
		
	}
	// ACABOU A LINHA e volta pro "fgets"(main)
	printf("\n");

}

int ehLetra(char ch) {
	if (isalpha(ch) || ch == '_')
		return 1;
	else
		return 0;
}

int ehNumero(char ch) {
	if (isdigit(ch))
		return 1;
	else
		return 0;
}

char next_char(char* cadeia) {
	indice_cadeia++;
	return cadeia[indice_cadeia];
}

void salvar_token(char* lexema, int indice_hash, FILE *arq_saida){
	if (list_token)	printf("Lexema: %5s - Tipo-token: %3d - Conteudo: TST[%3d].simbolo: %s \n", lexema, indice_hash, indice_hash, tst2[indice_hash].simbolo);
	fprintf(arq_saida, "%s | %d \n", lexema, indice_hash);	
}

// FUNCAO QUE PROCURA CARACTERE ESPECIAL ("+","==", etc) NA TST
// SE CARACTERE NAO ENCONTRADO = SIMBOLO INEXISTENTE NA LINGUAGEM 
int ehEspecial(char* cadeia,FILE *arq_saida){
	int achei_simbolo = 0;	
	char especial[4];
	int indice_simb_especial;
	especial[0] = cadeia[indice_cadeia];
	especial[1] = cadeia[indice_cadeia+1];
	especial[2] = cadeia[indice_cadeia+2];	
	especial[3] = '\0';	
	int j;
	
	// CASO 1 - SIMBOLO DE TAMANHO 3 (ex. >>=)
	if (cadeia[indice_cadeia+1] == '\0' ) especial[2] = '\0';
	indice_simb_especial = consulta_Insere(tst2, especial, 'C', 0);
	if (indice_simb_especial != -1) {
		salvar_token(especial, indice_simb_especial, arq_saida);
		achei_simbolo = 1;
		indice_cadeia = indice_cadeia + 2;
	} 

	// CASO 2 - SIMBOLO DE TAMANHO 2 (ex. ==)
	if (achei_simbolo == 0)
	{
		especial[2] = '\0';		
		indice_simb_especial = consulta_Insere(tst2, especial, 'C', 0);
		if (indice_simb_especial != -1) {
			salvar_token(especial, indice_simb_especial, arq_saida);
			achei_simbolo = 1;
			indice_cadeia = indice_cadeia + 1;
		} 
	}
	
	// CASO 3 - SIMBOLO DE TAMANHO 1 (ex. =)
	if (achei_simbolo == 0)
	{
		especial[1] = '\0';
		indice_simb_especial = consulta_Insere(tst2, especial, 'C', 0);
		if (indice_simb_especial != -1) {
			salvar_token(especial, indice_simb_especial, arq_saida);
			achei_simbolo = 1;
		} 
	}
	if (achei_simbolo == 0)
	{
		/*if (cadeia[indice_cadeia] == '\n') printf ("Quebra de linha \n"); else*/
		if (cadeia[indice_cadeia] != '\n') 
		printf("Erro! simbolo inexistente na linguagem: %c . Linha %d\n", cadeia[indice_cadeia],numero_linha);
	}
		
}

void imprime_tnt(char nome_arquivo_saida[])
{	
	FILE *a;
	a = fopen(nome_arquivo_saida, "r");	
	if(a == NULL)
		printf("Erro, nao foi possivel abrir o arquivo de saida\n");
	else
	{
		char info[100];
		while( (fgets(info, sizeof(info), a))!=NULL )
		{
			printf("%s ", info);
		}
		fclose(a);
	}
}

	

void main(int argc, char *argv[])
{
	
	char nome_arquivo_fonte[MAX_PALAVRA]; //= "codigo_fonte_a_analisar.c";	// PASSADO VIA PARAMETRO	
	char nome_arquivo_saida[MAX_PALAVRA]; //= "tokens.txt";					// PASSADO VIA PARAMETRO	
	char nome_arquivo_binario[MAX_PALAVRA]; //= "dados_binario";			// PASSADO VIA PARAMETRO	
		
	strcpy (nome_arquivo_fonte,argv[1]);
	strcpy (nome_arquivo_saida,argv[2]);
	strcpy (nome_arquivo_binario,argv[3]);	
	
	FILE *arq_binario;
	le_Binario(tst2, arq_binario, nome_arquivo_binario);
	printf("\n");
	
	FILE *arq_saida;
	arq_saida = fopen(nome_arquivo_saida, "w");
	/*if(arq_saida == NULL)
			printf("Erro, nao foi possivel abrir o arquivo de saida\n");*/
		
	FILE *arq;
	arq = fopen(nome_arquivo_fonte, "r");
	if (arq == NULL)
		printf("Erro, nao foi possivel abrir o arquivo \n");
	else	
	{
		char linha[500];	
		while ((fgets(linha, sizeof(linha), arq)) != NULL)
		{			
			numero_linha++;
			//printf("linha %d: %s \n", numero_linha, linha);
			lexanalysis(linha, arq_saida);
		}
	}
	
	fclose(arq);	
	fclose(arq_saida);
	
	if (list_tst) imprime_tst (tst2);	
	if (list_tnt) imprime_tnt(nome_arquivo_saida);	
		
		
	

}
