#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
//funcoes
void sair();
void controle();
int colidi();

//variaveis globais

int sai = 0, width = 740, height = 700, atirar = 0;


struct obj{
	int x; //posicao x do invader no arquivo original
	int y; //posicao y do invader no arquivo original
	int wx; // posiçao x onde ele sera colocado no jogo
	int wy; // posiçao x onde ele sera colocado no jogo
	int w; //largura da imagem
	int h; // altura da imagem
	
}invader[5][10]; //cada invader e suas respectivas infomaçoes sao salvas em uma matriz

struct obj 
	nave = {338,600,338,600,66,42}, //posição original da vane (no arquivo de sprites)
	disco = {435,220,435,220,62,40},
	tiro = {-10,-10,350,620,3,8}, //posição original do tiro (no arquivo de sprites)
	tiroInv[2] = {-10,-10,350,620,3,8, -10,-10,350,620,3,8}, //posição original do tiro dos invaders (no arquivo de sprites)
	explode = {48,250,77,275,32,32} //posicao original da explosao (no arquivo de sprites)
;

int main(){
	//inicializacoes
	FILE *hScore;
	int highScore;
	hScore = fopen("hScore.dat","r");
	if(hScore == NULL){ //caso o arquivo n exista o jogo entende como não ha historico de pontos e apenas define o atual maior como 0, e no fim do jogo um arquivo é criado
		highScore = 0;
	}else{
		fscanf(hScore,"%i", &highScore);
	}
	
	allegro_init();
	install_keyboard();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	set_color_depth(32);
	set_window_title("Space Invaders Jose Geraldo");
	set_close_button_callback(sair);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, width, height,0,0);
	
	//variaveis locais
	int passo = 0;
	int discx = 1000;
	int time = 30;
	int t = 0; //contador do tempo para indicar quando os invaders devem andar
	int face = 0;
	int sent = 0; //0 direita, 1 esquerda
	int abaixa = 0;  // pode ou nao abaixar os invaders
	int pontos = 0;
	int vidas = 3;
	int vida = 0;
	int play = 0;
	int atingido = 0;
	int nInvaders = 0;
	int nInvadOld = 0;
	
	
	
	//Imagens
	BITMAP *buffer = create_bitmap(width, height);
	BITMAP *fundo = load_bitmap("img/spaceinvadersfundo.bmp", NULL);
	BITMAP *invaders = load_bitmap("img/invaders.bmp", NULL);
	
	//fontes
	FONT   *font     = load_font("img/f20.pcx", NULL, NULL);
	FONT   *fnt28    = load_font("img/f28.pcx", NULL, NULL);
	
	//sons
	SAMPLE *hit   = load_sample("sounds/hit.wav"); 
	SAMPLE *disc  = load_sample("sounds/disco.wav");   
	SAMPLE *fase  = load_sample("sounds/fase.wav");    	
	SAMPLE *tiros = load_sample("sounds/tiro.wav");
	SAMPLE *morto = load_sample("sounds/morto.wav");
	
	
	
	
	// programa roda enquanto n preciona esc ou fecha a janela
	while(!(sai || key[KEY_ESC])){
		
		while(!(sai || key[KEY_ESC] || play)){ //MENU PRINCIPAL
			atirar = 0;
			discx = 2000;
			atingido = 0;
			stop_sample(disc);
			stop_sample(fase);
			tiroInv[0].x = -10;
			tiroInv[0].y = height/2;
			tiroInv[1].x = -10;
			tiroInv[1].y = height/2;
			rectfill(buffer, 0, 0, width, height, 0);
			if(vida || !pontos){
				textout_centre_ex(buffer, font,"Press Start", width/2, height/2,0xffffff, 0);
			}else{
				if(pontos > highScore){
						highScore = pontos;
						hScore = fopen("hScore.dat","w");
						fprintf(hScore, "%i", highScore);
						fclose(hScore);
				}
				textout_centre_ex(buffer, font,"Game Over", width/2, height/2, 0xffffff, 0); 
			}
			textout_centre_ex(buffer, fnt28,"SPACE INVADERS", width/2, height/6,0xffffff, 0);
			textprintf_centre_ex(buffer, font, width/2, height/10, 0xffffff, -1,"SCORE  %i", pontos);			
			textprintf_centre_ex(buffer, font, 100, height/1.1, 0xffffff, -1,"highScore %i", highScore);
			textprintf_centre_ex(buffer, font, width/1.2, height/1.1, 0xffffff, -1,"Vidas 0%i", vida);				
			draw_sprite(screen, buffer, 0, 0);
			
			//inciando
			if(key[KEY_SPACE]) {
				play = 1;
				play_sample(fase, 255, 128, 700, 1); //inicia a musica
				rest(100);					
				if(!vida || nInvaders >= 50){
					for(int i = 0; i<5 ; i++){ //inicia todos os invasores
						for(int j = 0; j<10;j++){
							invader[j][i].x = 43 + j*62;
							invader[j][i].y = 258 + i*51;
							invader[j][i].wx = 43;
							invader[j][i].wy = 298 + i*51;
							invader[j][i].w = 35;
							invader[j][i].h = 24;
						}
					}
					time = 65;
					nInvaders = 0;
					nave.x = width/2;
				}
				if(!vida){
					
					vida = vidas;
					pontos = 0;
				}
			}
			rest(30);
			clear(buffer);
		}
		
		controle(); // inicia os controles do jogo
		
		nInvadOld = nInvaders;	
		t++;
		passo = 0;
		if(t > time){
			passo = 1;
			t = 0;
			face = !face;
		}
		// Fundo
		draw_sprite(buffer, fundo, 0,0); 
		
		//emite som de tiro
		if(atirar && tiro.y> nave.y + nave.h/3){
			play_sample(tiros, 255, 128, 1000, 0);
		}
		
		//Disco voador
		discx = discx - 1;
		disco.x = discx;
		if(disco.x == 740){
			play_sample(disc,255,128,1000,1);
		}
		if(disco.x < -disco.w){
			stop_sample(disc);
			discx=2500;
		} 
		masked_blit(invaders, buffer, disco.wx, disco.wy, disco.x, disco.y, disco.w, disco.h);
		
		//matar o disco
		if(colidi(tiro.x, tiro.y, disco.x, disco.y, tiro.w, tiro.h, disco.w, disco.h)){
			masked_blit(invaders, buffer, explode.wx, explode.wy, disco.x, disco.y, explode.w, explode.h);
			stop_sample(disc);
			play_sample(morto, 255,128,1000,0);
			pontos += 120;
			discx = 1500;
			atirar = 0;
		}
		
		// Tiro dos invaders
		int sort = rand()%10;
		if(tiroInv[0].y < height){
			tiroInv[0].y += 3;
		}
		if(tiroInv[1].y < height){
			tiroInv[1].y += 3;
		}
		
		//nave atingida
		if(colidi(nave.x, nave.y+5, tiroInv[0].x, tiroInv[0].y-5, nave.w, nave.h, tiroInv[0].w, tiroInv[0].h) || colidi(nave.x, nave.y+5, tiroInv[1].x, tiroInv[1].y-5, nave.w, nave.h, tiroInv[1].w, tiroInv[1].h)){
			atingido = 1;
		}
		//Movimentos dos invaders e posicionamento de Sprites
		for(int i = 0; i<5 ; i++){ 
			
			if(tiroInv[1].y >= height && invader[sort][4-i].y < height){
				tiroInv[1].x = invader[sort][4-i].x + invader[sort][4-i].w/2;
				tiroInv[1].y = invader[sort][4-i].y + invader[sort][4-i].h/2;
			}
			
			for(int j = 0; j<10;j++){
				
				if(invader[j][i].y < height){
					if(invader[j][i].x < 10 && !sent){
						abaixa = 1;
						invader[j][i].x += passo*5;
					}
					if(invader[j][i].x > 704 && sent){
						abaixa = 1;
						invader[j][i].x -= passo*5;
					}
					sent?(invader[j][i].x += passo*5) : (invader[j][i].x -= passo*5);
				}
				
				if(invader[9-j][4-i].x == nave.x && tiroInv[0].y >= height && invader[9-j][4-i].y < height){ //verifica se o invader pode atirar
					tiroInv[0].x = invader[9-j][4-i].x + invader[9-j][4-i].w/2;
					tiroInv[0].y = invader[9-j][4-i].y + invader[9-j][4-i].h/2;
					}
				
				
				masked_blit(invaders, buffer, invader[j][i].wx + face*62, invader[j][i].wy, invader[j][i].x, invader[j][i].y, invader[j][i].w, invader[j][i].h); //posiciona o invader na tela
				
				// verifica se o tiro acertou o invader
				if(colidi(tiro.x, tiro.y, invader[j][i].x, invader[j][i].y, tiro.w, tiro.h, invader[j][i].w, invader[j][i].h )){ 
					//gera a explosao
					masked_blit(invaders, buffer, explode.wx, explode.wy, invader[j][i].x, invader[j][i].y, explode.w, explode.h);
					play_sample(hit, 255,128,1000,0);
					invader[j][i].y = height; //se o invader for atingido ele émovido para fora da tela e considerado morto
					nInvaders++;
					pontos += 15;
					atirar = 0;
				}
		}
	}
	// velocidade dos invasores
	if(nInvaders != nInvadOld){			
		    if(nInvaders == 10){
				stop_sample(fase);
				time = 25;
				play_sample(fase, 255, 128, 780, 1);
			}
			if(nInvaders == 30){
			stop_sample(fase);
			time = 15;
			play_sample(fase, 255, 128, 900, 1);
			}
			if(nInvaders == 45){
				stop_sample(fase);
				time = 7;
				play_sample(fase, 255, 128, 1000, 1);
			}
			if(nInvaders == 48){
				stop_sample(fase);
				time = 4;
				play_sample(fase, 255, 128, 1500, 1);
				}	 			
		}
	//abatendo todos os invaders
	if(nInvaders == 50){
		play = 0;
	}
	//abaixa os ivaders
	if(abaixa){
		sent = !sent;
		for(int i = 0; i<5 ; i++){ 
			for(int j = 0; j<10;j++){
				invader[j][i].y = invader[j][i].y + 20;
				if(invader[j][i].y + invader[j][i].h >= nave.y  && invader[j][i].y < height){ // se os invasores alcanssarem a nave = GAME OVER
					play = 0;
					vida = 0;
				}
			}
		}
		abaixa = 0;	
	}
		//retira os elementos do arquivo invaders e posiciona no plano de fundo
		//explode a nave;
		if(atingido){
			play_sample(morto, 255,128,1000,0);
			masked_blit(invaders, buffer, explode.wx-14, explode.wy, nave.x, nave.y, nave.w, nave.h);
		}else{
			 
			masked_blit(invaders, buffer, nave.wx, nave.wy, nave.x, nave.y, nave.w, nave.h); // desenha a nave
			masked_blit(invaders, buffer, tiro.wx, tiro.wy, tiro.x, tiro.y, tiro.w, tiro.h); // desenha os tiros da nave
			masked_blit(invaders, buffer, tiroInv[0].wx, tiroInv[0].wy, tiroInv[0].x, tiroInv[0].y, tiroInv[0].w, tiroInv[0].h); // desenha os tiros dos invaders
			masked_blit(invaders, buffer, tiroInv[1].wx, tiroInv[1].wy, tiroInv[1].x, tiroInv[1].y, tiroInv[1].w, tiroInv[1].h); // desenha os tiros dos invaders
		
		}
		
		if(atingido){
			vida--;
			play = 0;
			rest(500);
			atingido = 0;
			stop_sample(fase);
			
		}	
		
			
		//Pontos e vida
		textprintf_centre_ex(buffer, font, width/2, 6, 0xffffff, -1, "SCORE %i", pontos);
		textprintf_centre_ex(buffer, font, width/1.2, 6, 0xffffff, -1, "VIDAS %i", vida);
		
		
		draw_sprite(screen, buffer, 0,0);
		rest(1);
		clear(buffer);
	}
	
	
	//finalizacao
	destroy_bitmap(buffer);
	return 0;
	}
	END_OF_MAIN();
	
	
	void controle(){
		if(key[KEY_LEFT] && nave.x > 30)nave.x -=3; //seta pra esquerda move a nave pra esquerda
		if(key[KEY_RIGHT] && nave.x < 670)nave.x +=3; //seta pra direita move a nave pra direita
		if(key[KEY_SPACE] && tiro.y > nave.y && !atirar)atirar = 1;//verifica se pode atirar, vendo se nao existe outro tiro ja em execucao e atirar verdadeiro 
		//espaço faz o tiro que esta escondido atras da nave se movimentar
		if(atirar == 1){
			tiro.y -=4; // atira
		}
		//reinicia o tiro
		if(tiro.y < height/4) atirar=0; 
		if(atirar == 0){ // cada novo tiro é gerado na posicao atual da nave
			tiro.x = nave.x + nave.w/2.1;
			tiro.y = nave.y + nave.h/2;
		}
	}
	END_OF_FUNCTION(controle);
	
	
	int colidi(int Ax, int Ay, int Bx, int By, int Aw, int Ah, int Bw, int Bh){
		if(Ax +Aw > Bx && Ax < Bx + Bw && Ay +Ah > By && Ay < By + Bh){
			return 1;
		}
		return 0;
	}
	
	END_OF_FUNCTION(colidi);
	void sair(){
		sai = 1;
	}
	END_OF_FUNCTION(sair);
