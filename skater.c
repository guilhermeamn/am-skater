#include <skater.h>
#include <stdint.h>
#include <stdlib.h>
#include "driverlib/systick.h"
#include "Nokia5110.h"
#include "bitmaps.h"
#include "botoes.h"
#include <stdbool.h>

/*==================================== VARIÁVEIS ===========================================*/
//------------Posição------------------//
static int i = 0;
static int skater_x = 0;                            //posicao do skater em x
static int skater_y = 22;                           //posicao do skater em y
//------------Botão---------------------//
static int botao = 3;
static int botao_anterior = 3;
//------------Plano de fundo------------//
int nuvem = 0;
int posicao_nuvem = 60;
int posicao_nuvem2 = 80;
int inverte = 0;
int pos_skate = 10;
//------------Obstáculo-----------------//
int posicao_obstaculo_X[15] = {70,65,60,55,50,45,40,35,30,25,20,15,10,5,0}; //vetor com as posições do obstaculo em X
int p = 0;                                          //posicao no vetor posicao_obstaculo_X
int obs = 5;                                        //posicao no vetor obstac de bitmaps (obstaculo 1,2,3 ou 4)
//------------Skater--------------------//
int pos = 0;                                        //skater (1,2,3 ou 4)
//------------Tempo---------------------//
static int systickTime = 37000;
int debouncer = 0;
int MAX = 2;
int debouncerJump = 0;
int debouncerNuvem = 0;
int debouncerNuvem2 = 0;
int debouncerSkate = 0;
//------------Pontuação-----------------//
int pts = 0;
int stage_pos = 0;                                  //posicao no vetor de stages de bitmaps (stage 1,2,3,4 ou 5)
int pontuacaoTOTAL = 0;                             //pontuação total do jogo
int ptsMax = 5;                                     //obstaculos para passar de fase

/*============================== FUNÇÕES =======================================*/

/* Plano de fundo */
void background(void){
    Nokia5110_PrintBMP2(0,46,bg_Bitmap,80,1);           //chao
    Nokia5110_PrintBMP2(0,0,scorebg_Bitmap,16,8);       //score
    Nokia5110_PrintBMP2(61,0,numeros_Bitmap[pontuacaoTOTAL/1000],5,5);              //primeiro digito
    Nokia5110_PrintBMP2(67,0,numeros_Bitmap[(pontuacaoTOTAL%1000)/100],5,5);        //segundo
    Nokia5110_PrintBMP2(73,0,numeros_Bitmap[((pontuacaoTOTAL%1000)%100)/10],5,5);   //terceiro
    Nokia5110_PrintBMP2(79,0,numeros_Bitmap[pontuacaoTOTAL%10],5,5);                //quarto
    if(obs != 5) Nokia5110_PrintBMP2(posicao_obstaculo_X[p],40,obstac[obs],11,6); //obstaculo
}

/* Animação de início */
void inicio(void){
    for(i = 0; i < 65; i++){
        if(i > 35 && i < 60){
            pos = 3;
            skater_y = 16;
        }
        else{
            pos = 0;
            skater_y = 22;
        }
        Nokia5110_PrintBMP2(skater_x+i,skater_y,skater[pos],26,26);
        Nokia5110_PrintBMP2(50,40,obsinicio_Bitmap,22,6);
        SysCtlDelay(300000);
        Nokia5110_DisplayBuffer();
        Nokia5110_ClearBuffer();
    }
}

/* Menu
 * 1 -> AJUDA
 * 2 -> PLAY
*/
int menu(void){
    botao = le_matriz();

    if(botao_anterior == 3 || botao_anterior == 4) Nokia5110_DrawFullImage(menu_Bitmap);
    else if(botao_anterior == 1) while(ajuda()){}
    else if(botao_anterior == 2) return 0;

    if(botao != 0) botao_anterior = botao;

    return 1;

}

/* Ajuda
 * SW1 -> Menu
 * SW2 -> Jogo
 * SW3 -> Instrucoes
 * SW4 -> Voltar
*/
int ajuda(void){
    botao = le_matriz();
    if(botao_anterior == 2) Nokia5110_DrawFullImage(ajuda_Bitmap);
    else if(botao_anterior == 3) Nokia5110_DrawFullImage(ajuda3_Bitmap);
    else if(botao == 4) return 0;
    else Nokia5110_DrawFullImage(ajuda1_Bitmap);

    if(botao != 0) botao_anterior = botao;

    return 1;
}

/* SW1 -> Pula (Flip)
 * SW2 -> Manual
 * SW3 -> Nose Manual
 * SW4 -> Especial
 */
void botoes(void){
    botao = le_matriz();

    if(botao == 1){                                 //flip
        debouncerJump++;
        if(debouncerJump == 15){                    //não deixar o skater ficar no ar por mais de 15 execuções seguidas
            return;
        }
        else if(debouncerJump < 15){
            skater_y -= 13;
            pos = 2;
        }
    }
    else if(botao == 2 && stage_pos > 1){           //manual, só liberado apos 2a fase
        skater_y--;
        pos = 1;
        pontuacaoTOTAL++;
    }
    else if(botao == 3 && stage_pos > 2){           //nose manual, śo liberado apos 3a fase
        pos = 3;
        pontuacaoTOTAL++;
    }
    else if(botao == 4 && stage_pos > 3){           //especial, só liberado apos 4a fase
        pos = 4;
        pontuacaoTOTAL += 5;
    }

    if(botao != 0) botao_anterior = botao;

    if(botao == 0) debouncerJump = 0;               //se soltar o botao 1, resetar o debouncer

}

/* Começa o jogo */
void start(void){
    if(stage_pos == 0){
        while(botao != 1){
            Nokia5110_DrawFullImage(stage[stage_pos]);
            botao = le_matriz();
        }
        stage_pos++;
    }

    botoes();

    Nokia5110_PrintBMP2(skater_x,skater_y,skater[pos],26,26); //OBS.: tem que ser exatamente do tamanho da imagem
    Nokia5110_DisplayBuffer();                                //fazer todos skaters do mesmo tamanho
    Nokia5110_ClearBuffer();

    verifica();
    skater_y = 22;                                            //skater no chão
    pos = 0;
    skate();

    cloud();

    if(stage_pos == 5) ptsMax = 20;                           //bonus stage maior numero de obstaculos
    else ptsMax = 5;                                          //5 obstaculos para passar de fase

    if(pts == ptsMax){                                        //passou de fase
        while(botao != 4 && stage_pos < 5){
            Nokia5110_DrawFullImage(stage[stage_pos]);
            botao = le_matriz();
        }
        pts = 0;
        systickTime += 1500;
        SysTickPeriodSet(systickTime);
        if(stage_pos == 5){
            //zerou
            Nokia5110_ClearBuffer();
            Nokia5110_DisplayBuffer();
            ganhou();
        }
        else stage_pos++;
    }
}

/* Reinicia as variáveis para recomeçar o jogo */
void restart(void){
    systickTime = 37000;
    skater_x = 0;
    skater_y = 22;
    pts = 0;
    stage_pos = 0;
    p = 0;
    pontuacaoTOTAL = 0;
    SysTickEnable();                                        //ativa cenario novamente
    SysTickIntEnable();
    SysTickPeriodSet(systickTime);
}

/* Personagem morreu */
void morre(void){
    while(botao != 3){                                          //TRY AGAIN
        SysTickDisable();                                       //desativa cenario

        //GAME OVER
        if(i > 100){
            Nokia5110_DrawFullImage(game_over_Bitmap);
            i++;
            if(i > 200) i = 0;
        }else if(i <= 100){
            Nokia5110_DrawFullImage(game_over2_Bitmap);
            i++;
        }

        botao = le_matriz();
    }

    over();

    restart();
}

/* Jogador zerou o jogo */
void ganhou(void){
    while(botao != 3){                                      //TRY AGAIN
        SysTickDisable();

        //YOU WIN
        if(i > 100){
            Nokia5110_DrawFullImage(youwin_Bitmap);
            i++;
            if(i > 200) i = 0;
        }else if(i <= 100){
            Nokia5110_DrawFullImage(youwin2_Bitmap);
            i++;
        }

        botao = le_matriz();
    }

    over();

    restart();
}

/* Mostra no display o score final, após o jogador morrer ou ganhar o jogo */
void over(void){
    while(botao != 4){
        //animacao do skate
        debouncerSkate++;
        if(debouncerSkate == 8){
            pos_skate++;
            if(pos_skate == 65) pos_skate = 10;
            debouncerSkate = 0;
        }

        Nokia5110_PrintBMP2(25,13,circulo_Bitmap,35,17);                                //circulo em volta do score

        Nokia5110_PrintBMP2(30,2,scorefinal_Bitmap,27,12);                              //score
        Nokia5110_PrintBMP2(30,18,numeros_Bitmap[pontuacaoTOTAL/1000],5,5);              //primeiro digito
        Nokia5110_PrintBMP2(36,18,numeros_Bitmap[(pontuacaoTOTAL%1000)/100],5,5);        //segundo
        Nokia5110_PrintBMP2(42,18,numeros_Bitmap[((pontuacaoTOTAL%1000)%100)/10],5,5);   //terceiro
        Nokia5110_PrintBMP2(48,18,numeros_Bitmap[pontuacaoTOTAL%10],5,5);                //quarto

        Nokia5110_PrintBMP2(pos_skate,37,skate_Bitmap,21,4);                            //skate

        if(i > 100){
            Nokia5110_PrintBMP2(28,33,scorefinal2_Bitmap,27,12);
            i++;
            if(i > 200) i = 0;
        }else if(i <= 100){
            Nokia5110_PrintBMP2(28,33,scorefinal3_Bitmap,27,12);
            i++;
        }

        Nokia5110_DisplayBuffer();
        Nokia5110_ClearBuffer();

        botao = le_matriz();
    }
}

/* Gera um obstaculo aleatório */
void func_obstaculo(void){
    static int random = 0;
    srand(SysTickValueGet());
    random = rand()%4;                                      //0 <= numero aleatorio < 4
    if(random == 0) obs = 0;                                //obstaculo 1
    else if(random == 1) obs = 1;                           //obstaculo 2
    else if(random == 2) obs = 2;                           //obstaculo 3
    else if(random == 3) obs = 3;                           //obstaculo 4
}

/* Verifica se o personagem morreu */
void verifica(void){
    if( (p==11||p==12||p==13) && skater_y == 22) morre();              //se estiver no chao na posicao do obstaculo
    else if( (p==11||p==12||p==13) && skater_y == 21) morre();         //se estiver fazendo manual na posicao do obstaculo
}

/* Muda o obstaculo de posição */
void skate(void){
    debouncer++;                                            //debouncer para nao parar a execucao do codigo
    if(debouncer == MAX){
        Nokia5110_ClearBuffer();
        p++;                                                //muda a posicao do obstaculo
        if(obs == 5) func_obstaculo();                      //obs == 5 estado inicial ou acabou um obstaculo,gera outro aleatorio
        if(p == 15){                                        //obstaculo chegou ao final da tela
            obs = 5;
            p = 0;
            pts++;                                          //passou pelo obstaculo, aumenta a pontuação
            pontuacaoTOTAL++;
        }
        debouncer = 0;
    }
}

/* Movimentação das nuvens */
void cloud(void){
    //nuvem 1 (fica flutuando)
    debouncerNuvem++;
    if(debouncerNuvem == 8){
        if(posicao_nuvem == 55) inverte = 1;                //se movimenta para direita
        else if(posicao_nuvem == 60) inverte = 0;           //se movimenta para esquerda
        if(inverte) posicao_nuvem++;
        else posicao_nuvem--;
        debouncerNuvem = 0;
    }

    //nuvem 2 (percorre todo o cenario)
    debouncerNuvem2++;
    if(debouncerNuvem2 == 2){
        if(posicao_nuvem2 == 8) posicao_nuvem2 = 70;
        else posicao_nuvem2--;
        debouncerNuvem2 = 0;
    }

    //movimentacao cima/baixo
    nuvem++;
    if(nuvem > 12){
        Nokia5110_PrintBMP2(posicao_nuvem,6,bg1_Bitmap,9,9);
        Nokia5110_PrintBMP2(posicao_nuvem2,1,bg1_Bitmap,9,9);
        if(nuvem == 20) nuvem = 0;
    }
    else if(nuvem <= 12){
        Nokia5110_PrintBMP2(posicao_nuvem,7,bg1_Bitmap,9,9);
        Nokia5110_PrintBMP2(posicao_nuvem2,0,bg1_Bitmap,9,9);
    }
}
