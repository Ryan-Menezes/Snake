#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <conio.h>
#include <conio.c>												// Biblioteca necess�ria para colorir o jogo
#include <locale.h>
#include <time.h>
#include <stdbool.h>

#define LINHAS 25												// N�mero de linhas da matriz do mapa
#define COLUNAS 70												// N�mero de colunas da matriz do mapa
#define BLOCO 219												// C�dico ASCII que representa um bloco, tanto para a parede do mapa, tando para o corpa da cobrinha
#define BLOCO_MATRIZ 1											// N�mero que representa na matriz do mapa uma parede
#define ESPACO_MATRIZ 0											// N�mero que representa na matriz do mapa um espa�o v�zio para a cobra se mover
#define MACA 1													// C�dico ASCII de um caracter que representa a ma�a do jogo
#define SORTEIAYMACA() (rand() % (LINHAS - 2)) + 1				// Macro que sorteia uma posi��o Y para a ma�a
#define SORTEIAXMACA() (rand() % (COLUNAS - 2)) + 1				// Macro que sorteia uma posi��o X para a ma�a
#define FILERECORDISTAS "RECORDISTAS.bin"						// Nome do arquivo bin�rio que armazena os recordistas do jogo
#define TITULOJOGO "SNAKE"										// Nome do jogo
#define VERSAO 1.0												// Vers�o do jogo
#define VELOCIDADEINICIAL 150									// Velocidade inicial da cobrinha(Este valor ser� usado como par�metro para a fun��o Sleep)
#define LIMITEVELOCIDADE 50										// Limite de velocidade que a cobrinha pode alcan�ar
#define LIMITEMACAS 10											// Limite de ma�as sorteadas que ficar�o no mapa para a cobrinha comer
#define LIMPATELA() system("cls");								// Macro que limpa a tela do console(OBS: Por aqui voc� pode alterar o comando para limpar o console de acordo com seu S.O.)

// Constantes para as teclas direcionais

typedef enum{
	ENTER = 13,
//	UP = 72,
//	RIGHT = 75,
//	LEFT = 77,
//	DOWN = 80
	UP = 'W',
	RIGHT = 'A',
	LEFT = 'D',
	DOWN = 'S'
}TECLAS;

void inicia();													// Fun��o que inicia o jogo
void desenhaMapa();												// Fun��o que desenha o mapa do jogo
void draw();													// Fun��o que desenha toda a parte gr�fica do jogo
void update();													// Fun��o que atualiza posi��o da cobrinha, marca pontos, sorteia novas ma�as e etc.
void loop();													// Loop do jogo, onde as fun��es draw() e update() ser�o chamadas recursivamente num loop at� o final do jogo
void menu();													// Fun��o que desenha o menu do jogo
void recodistasJogo();											// Fun��o que apresenta os recordistas do jogo
void ajuda();													// Fun��o que apresenta as regras do jogo, tirando todas as d�vidas do jogador
void info();													// Fun��o que apresenta informa��es do jogo(Desenvolvedor e Vers�o)
void buscaRecordistas();										// Fun��o que l� o arquivo bin�rio com os recordistas do jogo
bool adicionaRecord();											// Fun��o que adiciona um novo recordista
unsigned char * pegarNome(unsigned char *, COLORS, COLORS);		// Fun��o que pega o nome de um jogador que bateu um novo recorde
void mensagem(bool);											// Fun��o que apresenta uma mensagem de sucerro ou derrota ao jogador no final do jogo
void tabelaGame(int, int, int, int, COLORS, COLORS);			// Fun��o que desenha um ret�ngulo colorido na tela
void infoGame();												// Fun��o que atualiza informa��es da gameplay durante o jogo									
void limpaRastro();												// Fun��o que limpa o rastro deixado pela cobra durante o jogo
void gotoXY(int, int);											// Fun��o que altera a posi��o do ponteiro do mouse

typedef struct{
	int X, Y;
}Coordenadas;

// Dados da cobra

struct{
	Coordenadas posicoes[LINHAS * COLUNAS];
	int tamanho, vel, posX, posY, direX, direY, indiceProx;
	COLORS corpo, cabeca;
}cobrinha;

// Dados da ma�a

typedef struct M{
	int posX, posY;	
}MACA_TYPE;

MACA_TYPE *macas = NULL;

// Dados do jogo

struct{bool jogando; int pontos; int total_macas; int macas_pegas; int nivel;}jogo;

// Estrutura para armazenar os 10 recordistas do jogo

typedef struct record{
	unsigned char nome[4];
	int pontuacao;
}RECORDS;

unsigned char totalRecordistas = 0;
RECORDS *recordistas = NULL;

// Mapa do jogo

int mapa[LINHAS][COLUNAS] = {ESPACO_MATRIZ};

void main(int argc, char *argv[]) {
	menu();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void inicia(){
	LIMPATELA();
	
	// Preenchendo o mapa
	
	srand(time(NULL));
	
	// Parte superior
	for(int i = 0; i < COLUNAS; i++) mapa[0][i] = BLOCO_MATRIZ;
	
	// Parte inferior
	for(int i = 0; i < COLUNAS; i++) mapa[LINHAS - 1][i] = BLOCO_MATRIZ;
	
	// Parte lateral esquerda
	for(int i = 0; i < LINHAS; i++) mapa[i][0] = BLOCO_MATRIZ;
	
	// Parte lateral direita
	for(int i = 0; i < LINHAS; i++) mapa[i][COLUNAS - 1] = BLOCO_MATRIZ;
	
	// Inicializando cobrinha
	
	cobrinha.tamanho = 0;
	cobrinha.vel = VELOCIDADEINICIAL;
	cobrinha.posX = (COLUNAS - 1) / 2;
	cobrinha.posY = (LINHAS - 1) / 2;
	cobrinha.direX = 1;
	cobrinha.direY = 0;
	cobrinha.indiceProx = 0;
	cobrinha.posicoes[cobrinha.indiceProx].X = cobrinha.posX;
	cobrinha.posicoes[cobrinha.indiceProx].Y = cobrinha.posY;
	cobrinha.corpo = GREEN;
	cobrinha.cabeca = LIGHTGREEN;
	
	// Inicializando maca
	
	macas = (MACA_TYPE*) malloc(sizeof(MACA_TYPE));
	macas[0].posX = SORTEIAXMACA();
	macas[0].posY = SORTEIAYMACA();
	
	// Buscando recordistas do jogo
	
	buscaRecordistas();
	
	// Inicializando jogo
	
	jogo.pontos = 0;
	jogo.jogando = true;
	jogo.total_macas = 1;
	jogo.macas_pegas = 0;
	jogo.nivel = 1;
	
	desenhaMapa();
	loop();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void draw(){	
	// Desenhando a cobrinha
	
	textbackground(BLUE);
	
	// Cabe�a

	textcolor(cobrinha.cabeca);
	gotoXY(cobrinha.posX, cobrinha.posY);
	printf("%c", BLOCO);
	
	// Desenhando a ma�as
	
	for(int i = 0; i < jogo.total_macas; i++){
		if(macas[i].posX >= 0 || macas[i].posY >= 0){
			textcolor(RED);
			gotoXY(macas[i].posX, macas[i].posY);
			printf("%c", MACA);
			textcolor(WHITE);
		}
	}
	
	// Exibe pontua��o
	
	infoGame();
	
	// Resolve problema na cor do primeiro pixel da tela
	
	gotoXY(0, 0);
	textcolor(LIGHTBLUE); 
	printf("%c", BLOCO); 
	textcolor(BLACK);
	
	Sleep(cobrinha.vel);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void update(){
	srand(time(NULL));
	
	unsigned char tecla = '\0';
	
	if(kbhit()){
		tecla = getch();
		
		switch(toupper(tecla)){
			case UP:
				cobrinha.direX = 0;
				cobrinha.direY = 0;
				cobrinha.direY = -1;
				break;
			case RIGHT:
				cobrinha.direX = 0;
				cobrinha.direY = 0;
				cobrinha.direX = -1;
				break;
			case DOWN:
				cobrinha.direX = 0;
				cobrinha.direY = 0;
				cobrinha.direY = 1;
				break;
			case LEFT:
				cobrinha.direX = 0;
				cobrinha.direY = 0;
				cobrinha.direX = 1;
				break;
		}
	}
	
	// Atualiza cor do corpo da cobrinha
	
	textcolor(cobrinha.corpo);
	gotoXY(cobrinha.posX, cobrinha.posY);
	printf("%c", BLOCO);
	
	// Atualiza posi��o da cobra
		
	cobrinha.posX += cobrinha.direX;
	cobrinha.posY += cobrinha.direY;
	
	// Limpando o rastro da cobrinha
	
	limpaRastro();
	
	// Verificando se a cobra pegou alguma ma�a
	
	for(int i = 0; i < jogo.total_macas; i++){
		if(macas[i].posX == cobrinha.posX && macas[i].posY == cobrinha.posY){
			jogo.pontos++;
			cobrinha.tamanho++;
			if(cobrinha.vel > LIMITEVELOCIDADE) cobrinha.vel -= 5;
			
			macas[i].posX = -1;
			macas[i].posY = -1;
			
			jogo.macas_pegas++;
			
			// A cada duas ma�as pegas, Ser� sorteado uma nova ma�a e atualizado novas posi��es para cada
			
			if(jogo.macas_pegas == jogo.total_macas){
				if(jogo.total_macas < LIMITEMACAS) jogo.total_macas++;
				jogo.macas_pegas = 0;
				jogo.nivel++;
				
				macas = (MACA_TYPE*) realloc(macas, sizeof(MACA_TYPE) * jogo.total_macas);
				
				for(int i = 0; i < jogo.total_macas; i++){
					macas[i].posX = SORTEIAXMACA();
					macas[i].posY = SORTEIAYMACA();
				}
			}
		}
	}
	
	// Verificando se a cobra colidiu com uma parede ou consigo mesmo
	
	if(mapa[cobrinha.posY][cobrinha.posX] == BLOCO_MATRIZ) jogo.jogando = false;
	
	for(int i = 0; i < cobrinha.tamanho; i++)
		if(cobrinha.posX == cobrinha.posicoes[i].X && cobrinha.posY == cobrinha.posicoes[i].Y && i != cobrinha.indiceProx - 1) jogo.jogando = false;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void loop(){
	draw();
	update();
	
	if(jogo.jogando) loop();
	else mensagem(adicionaRecord());
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void buscaRecordistas(){
	free(recordistas);
	recordistas = (RECORDS*) calloc(10, sizeof(RECORDS));
	totalRecordistas = 0;
	
	FILE* arquivo = fopen(FILERECORDISTAS, "r");
	
	if(arquivo)
		while(!feof(arquivo) && totalRecordistas < 10){
			if(fread(&recordistas[totalRecordistas], sizeof(RECORDS), 1, arquivo)) totalRecordistas++;
		}
	else arquivo = fopen(FILERECORDISTAS, "w");
	
	fclose(arquivo);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

bool adicionaRecord(){
	buscaRecordistas();
	RECORDS guardaR;
	RECORDS *guarda = (RECORDS*) calloc(10, sizeof(RECORDS));;
	
	bool bateuRecorde = false;
	
	RECORDS usuario;
	
	if(jogo.pontos > 0){ // Verifica se o jogador fez pelo menos uma pontua��o maior que zero
		if(totalRecordistas < 10){ // Verifica se o n�mero de recordistas � menor que o limite
			strncpy(usuario.nome, pegarNome("Congratulations !, you made it into the top 10!!!", LIGHTGREEN, GREEN), 4);
			usuario.pontuacao = jogo.pontos;
			
			recordistas[totalRecordistas] = usuario;
			totalRecordistas++;
			bateuRecorde = true;
		}else{
			int indiceRecord = totalRecordistas - 1;
			
			for(int i = totalRecordistas - 1; i >= 0; i--){
				if(recordistas[i].pontuacao < jogo.pontos){
					indiceRecord = i;
					bateuRecorde = true;
				}
			}	
				
			// Ordena os recordistas ap�s a inser��o de um novo
			
			if(bateuRecorde){
				if(indiceRecord < totalRecordistas - 1)
					for(int i = totalRecordistas - 1; i > indiceRecord; i--) recordistas[i] = recordistas[i - 1];
					
				strncpy(usuario.nome, pegarNome("Congratulations !, you made it into the top 10!!!", LIGHTGREEN, GREEN), 4);
				usuario.pontuacao = jogo.pontos;
				
				recordistas[indiceRecord] = usuario;
			}
		}
	}
	
	
	// Ordena records
	
	if(bateuRecorde){
		bool troca = true;
	
		do{
			troca = false;
			
			for(int i = 0; i < totalRecordistas - 1; i++){
				if(recordistas[i].pontuacao < recordistas[i + 1].pontuacao){
					guardaR = recordistas[i];
					recordistas[i] = recordistas[i + 1];
					recordistas[i + 1] = guardaR;
					troca = true;
				}
			}
		}while(troca);
				
		// Salvar record
		
		FILE* arquivo = fopen(FILERECORDISTAS, "w");
		
		if(arquivo)
			for(int i = 0; i < totalRecordistas; i++)
				fwrite(&recordistas[i], sizeof(RECORDS), 1, arquivo);
		
		fclose(arquivo);
		
		buscaRecordistas();
	}
	
	free(guarda);
	
	return bateuRecorde;
} 

// --------------------------------------------------------------------------------------------------------------------------------------------------

void mensagem(bool resultado){
	const int X = 2;
	const int Y = 2;
	
	// Mensagem que aparecer� na tela do jogador ap�s o termino do jogo
	unsigned char *msg = (resultado) ? "Congratulations !, you made it into the top 10!!!" : "GAME OVER!, You failed to enter the top 10 !!!";
	
	// Cores das bordas e fundo da caixa de mensagem configuradas de acordo com o resultado da partida
	COLORS borda = (resultado) ? LIGHTGREEN : LIGHTRED;
	COLORS fundo = (resultado) ? GREEN : RED;
	
	// Criando a caixa de mensagem e apresentando a mensagem final do jogo
	textbackground(BLACK);
	LIMPATELA();	
	tabelaGame(6, strlen(msg) + 3, X, Y, borda, fundo);

	textbackground(fundo);	
	gotoXY(X + 2, Y + 2);
	printf(msg);
	gotoXY(X + 2, Y + 4);
	printf("SCORE: %i", jogo.pontos);
	textbackground(BLACK);
	getch();
	
	menu();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void menu(){
	const int X = 2;
	const int Y = 2;
	const int L = 7;
	const int C = 26;
	const int CENTRO = (C * 2) / 2 - 2;
	unsigned char tecla = '\0';
	int pos = 0;
	
	SetConsoleTitle(TITULOJOGO);
	buscaRecordistas(); // Busca recordistas do jogo
	
	int titulo[7][26] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0},
		{0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	
	unsigned char *opMenu[] = {"New Game", "Records", "Help", "Info", "Exit"};
	
	for(int l = 0; l < L; l++){
		gotoXY(X, Y + l);
		
		for(int c = 0; c < C; c++){
			if(titulo[l][c]){
				textbackground(LIGHTGREEN);
				printf("  ");
			}else{
				textbackground(GREEN);
				printf("  ");
			}
			
			textbackground(BLACK);
		}
	}
	
	for(int i = 0; i < 5; i++){
		gotoXY(CENTRO, L + Y + i + 1);
		printf("%s", opMenu[i]);
	}
	
	while(tecla != ENTER){
		gotoXY(CENTRO - 3, L + Y + pos + 1);
		textcolor(GREEN); printf(">>"); textcolor(WHITE);
		tecla = getch();
		
		switch(toupper(tecla)){
			case UP:
				if(pos > 0){
					gotoXY(CENTRO - 3, L + Y + pos + 1);
					printf("  ");
					pos--;	
				}
				break;
			case DOWN:
				if(pos < 4){
					gotoXY(CENTRO - 3, L + Y + pos + 1);
					printf("  ");
					pos++;
				}
				break;
		}
	}
	
	// Verificando qual op��o foi escolhida
	
	switch(pos){
		case 0:
			inicia();
			break;
		case 1:
			recodistasJogo();
			break;
		case 2:
			ajuda();
			break;
		case 3:
			info();
			LIMPATELA();
			exit(0);
			break;	
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void recodistasJogo(){
	const int X = 2, Y = 2, LARGURA = 29, ALTURA = 4;
	unsigned char titulo[] = "GAME RECORDERS";
	
	LIMPATELA();
	
	tabelaGame(ALTURA, LARGURA, Y, X, LIGHTRED, RED);
	
	textbackground(RED);
	
	gotoXY(X + LARGURA / 2 - strlen(titulo) / 2, Y + 2);
	printf(titulo);
	
	textbackground(BLACK);
	
	gotoXY(X, Y + ALTURA + 1);
	for(int j = 0; j < 30; j++) printf("-");
	
	gotoXY(X, Y + ALTURA + 2);
	printf("| POSITION ");
	gotoXY(X + 11, Y + ALTURA + 2);
	printf("| NAME ");
	gotoXY(X + 18, Y + ALTURA + 2);
	printf("| SCORE ");
	
	gotoXY(X + 29, Y + ALTURA + 2);
	printf("|");
	
	gotoXY(X, Y + ALTURA + 3);
	for(int j = 0; j < 30; j++) printf("-");
	
	for(int cont = 0, i = 0; i < totalRecordistas; cont += 2, i++){
		gotoXY(X, Y + ALTURA + 4 + cont);
		printf("| %i ", i + 1);
		gotoXY(X + 11, Y + ALTURA + 4 + cont);
		printf("| %s ", recordistas[i].nome);
		gotoXY(X + 18, Y + ALTURA + 4 + cont);
		printf("| %i", recordistas[i].pontuacao);
		gotoXY(X + 29, Y + ALTURA + 4 + cont);
		printf("|");
		
		gotoXY(X, Y + ALTURA + 4 + cont + 1);
		for(int j = 0; j < 30; j++) printf("-");
	}
	
	textbackground(BLACK);
	
	getch();
	LIMPATELA();
	menu();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char * pegarNome(unsigned char * titulo, COLORS borda, COLORS fundo){
	const int X = 2, Y = 2, LARGURA = strlen(titulo) + 3, ALTURA = 4, LIMITE = 5;
	const int L = 2, C = 13;
	int l, c, indice = 0, i;
	unsigned char tecla = '\0';
	unsigned char *username = (unsigned char *) calloc(4, sizeof(unsigned char *));
	
	unsigned char letras[2][13] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	bool continua = false;
	
	do{
		textbackground(BLACK);
		LIMPATELA();
		memset(username, 0, sizeof(unsigned char) * 4); 
		tecla = '\0';
		indice = 0;

		tabelaGame(ALTURA, LARGURA, Y, X, borda, fundo);
	
		textbackground(fundo);
		gotoXY(X + 2, Y + 2);
		printf(titulo);
		
		textbackground(BLACK);
		gotoXY(X, Y + ALTURA + 3);
		
		printf(" USERNAME: ___");
		
		// Teclada para obter o username do usu�rio
		
		textbackground(BLACK);
		gotoXY(X, Y + ALTURA + 5);
		
		for(l = 0; l < L; l++){
			for(c = 0; c < C; c++) printf("| %c ", letras[l][c]);
			
			printf("|");
			gotoXY(X, Y + ALTURA + l + 6);	
		}
		
		l = c = 0;
		
		textcolor(RED);
		gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
		printf("%c\b", letras[l][c]);
		
		while(indice < 3){
			tecla = getch();
			
			switch(toupper(tecla)){
				case UP:
					textcolor(WHITE);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					
					if(l > 0) l--;
					
					textcolor(RED);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					break;
				case RIGHT:
					textcolor(WHITE);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					
					if(c > 0) c--;
					
					textcolor(RED);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					break;
				case DOWN:
					textcolor(WHITE);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					
					if(l < L - 1) l++;
					
					textcolor(RED);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					break;
				case LEFT:
					textcolor(WHITE);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					
					if(c < C - 1) c++;
					
					textcolor(RED);
					gotoXY((X + 2) * (c + 1), Y + ALTURA + l + 5);
					printf("%c\b", letras[l][c]);
					break;
				case ENTER:
					username[indice] = letras[l][c];
					indice++;
					
					textbackground(BLACK);
					textcolor(WHITE);
					gotoXY(X, Y + ALTURA + 3);
					
					printf(" USERNAME: %s", username);
					break;
			}
		}
		
		username[indice] = '\0';
		
		LIMPATELA();
		
		i = 1;
		tecla = '\0';
		
		textbackground(BLACK);
		textcolor(WHITE);
		gotoXY(X, Y);
		
		printf(" DO YOU WANT TO WRITE YOUR USERNAME?");
		
		gotoXY(X + 4, Y + 2);
		printf("YES");
		
		gotoXY(X + 4, Y + 3);
		printf("NO");
		
		gotoXY(X + 1, Y + 2 + i);
		textcolor(RED); printf(">>"); textcolor(WHITE);
		
		while((tecla = getch()) != ENTER){
			switch(toupper(tecla)){
				case UP:
					gotoXY(X + 1, Y + 2 + i);
					printf("  ");
					
					if(i > 0) i--;
					
					gotoXY(X + 1, Y + 2 + i);
					textcolor(RED); printf(">>"); textcolor(WHITE);
					break;
				case DOWN:
					gotoXY(X + 1, Y + 2 + i);
					printf("  ");
					
					if(i < 1) i++;
					
					gotoXY(X + 1, Y + 2 + i);
					textcolor(RED); printf(">>"); textcolor(WHITE);
					break;
			}
		}
		
		if(i == 0) continua = true;
		else continua = false;
	}while(continua);
	
	return username;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void ajuda(){
	const int X = 2, Y = 2, LARGURA = 61, ALTURA = 13;
	
	LIMPATELA();
	
	tabelaGame(ALTURA, LARGURA, Y, X, LIGHTGRAY, DARKGRAY);
	
	textbackground(DARKGRAY);
	
	gotoXY(X + 2, Y + 2);
	printf("The snake game, also known as the worm game, is an old and");
	gotoXY(X + 2, Y + 3);
	printf("famous game in which the goal is to eat like berries for a");
	gotoXY(X + 2, Y + 4);
	printf("snake to grow.");
	
	gotoXY(X + 2, Y + 6);
	printf(" - Use the \"W, A, S, D\" arrows on the keyboard to control");
	gotoXY(X + 2, Y + 7);
	printf("   the snake;");
	gotoXY(X + 2, Y + 8);
	printf(" - Eat the fruits for the snake to grow;");
	gotoXY(X + 2, Y + 9);
	printf(" - Avoid hitting obstacles, walls and yourself;");
	gotoXY(X + 2, Y + 10);
	printf(" - Eat the amount of fruit indicated to proceed");
	gotoXY(X + 2, Y + 11);
	printf("   to the next phase.");
	
	textbackground(BLACK);
	
	getch();
	LIMPATELA();
	menu();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void info(){
	const int X = 2, Y = 2, LARGURA = 50, ALTURA = 6;
	
	LIMPATELA();
	
	tabelaGame(ALTURA, LARGURA, Y, X, LIGHTGRAY, DARKGRAY);
	
	textbackground(DARKGRAY);
	
	gotoXY(X + 2, Y + 2);
	printf("Game developed by Ryan de Menezes Nobre Ciriaco");
	gotoXY(X + 2, Y + 4);
	printf("VERSION %.1f", VERSAO);
	
	textbackground(BLACK);
	
	getch();
	LIMPATELA();
	menu();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void tabelaGame(int A, int L, int X, int Y, COLORS CORBORDA, COLORS CORFUNDO){	
	// Preenchimento
	
	gotoXY(X, Y);
	
	for(int i = Y; i <= A + Y; i++){
		for(int j = 0; j < L; j++){
			textbackground(CORFUNDO);
			printf(" ");
			textbackground(CORBORDA);
		}
		gotoXY(X, i);
	}
	
	//Parte superior
	
	gotoXY(X, Y);
	
	for(int i = 0; i < L; i++){
		textcolor(CORBORDA);
		printf("%c", BLOCO);
	}
	
	// Parte inferior
	
	gotoXY(X, A + Y);
	
	for(int i = 0; i < L; i++){
		textcolor(CORBORDA);
		printf("%c", BLOCO);
	}
	
	// Parte lateral esquerda
	
	gotoXY(X, A + Y);
	
	for(int i = Y; i <= A + Y; i++){
		textcolor(CORBORDA);
		printf("%c", BLOCO);
		gotoXY(X, i);
	}
	
	// Parte lateral direita
	
	gotoXY(X, A + Y);
	
	for(int i = Y; i <= A + Y + 1; i++){
		textcolor(CORBORDA);
		printf("%c", BLOCO);
		gotoXY(X + L, i);
	}
	
	textbackground(BLACK);
	textcolor(WHITE);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void infoGame(){
	textbackground(RED);
	gotoXY(COLUNAS + 4, 3);
	printf("SCORE: %i", jogo.pontos);
	gotoXY(COLUNAS + 4, 4);
	printf("SPEED: %i", VELOCIDADEINICIAL - cobrinha.vel + 5);
	gotoXY(COLUNAS + 4, 5);
	printf("LEVEL: %i", jogo.nivel);
	textbackground(BLUE);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void desenhaMapa(){	
	for(int l = 0; l < LINHAS; l++){
		for(int c = 0; c < COLUNAS; c++)
			if(mapa[l][c] == BLOCO_MATRIZ){
				textcolor(LIGHTBLUE); 
				printf("%c", BLOCO); 
				textcolor(BLACK);
			}else{
				textbackground(BLUE);
				printf(" ");
				textbackground(BLACK);
			}
			
		printf("\n");
	}
	
	textcolor(WHITE);
	
	// Menu de Informa��es
	
	tabelaGame(6, 25, COLUNAS + 2, 1, LIGHTRED, RED);
}

void limpaRastro(){
	if(cobrinha.indiceProx > cobrinha.tamanho) cobrinha.indiceProx = 0;
	
	gotoXY(cobrinha.posicoes[cobrinha.indiceProx].X, cobrinha.posicoes[cobrinha.indiceProx].Y);
	printf(" ");
	
	// Salvando as coordenadas da cobra de acordo com seu tamanho
	
	cobrinha.posicoes[cobrinha.indiceProx].X = cobrinha.posX;
	cobrinha.posicoes[cobrinha.indiceProx].Y = cobrinha.posY;
	
	cobrinha.indiceProx++;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

void gotoXY(int x, int y){
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleCursorPosition(h, (COORD){x, y});
}
