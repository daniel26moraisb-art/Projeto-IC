#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int leds[4] = {13, 12, 11, 10};
int botoes[4] = {6, 5, 4, 3};
int pinBuzzer = 8;

int tempoF1 = 1200;
int tempoF2 = 800;
int tempoF3 = 400;

int ledAtual = 0; 
bool botaoJaApertado = false;

unsigned long millisRodada;
unsigned long millisBotao;
unsigned long millisPausa;

int estadoJogo = 0; //0 p/sortear, 1 p/jogador jogar, 2 p/pausa entre rodadas
int tempoPausa = 1000;

int erros = 0;
int errosAnterior = 0;
int pontuacao = 0;
int pontAnterior = 0;

bool acertou = false;



void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  for (int i=0; i<4; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(botoes[i], INPUT);
  }
  lcd.init();
  lcd.backlight();
  lcd.home();
  lcd.print("Comecando");
  delay(1000);
  millisRodada = millis();
  millisBotao = millis();
  millisPausa = millis();
}



void loop()
{
  if (pontuacao < 20 && erros < 5) { //jogo rolando
    if (pontuacao < 5) { // nível 1
      jogo(tempoF1);
      lcd.setCursor(0, 1);
      lcd.print("Fase 1");
    } else if (pontuacao < 10) { // nível 2
      jogo(tempoF2);
      lcd.setCursor(0, 1);
      lcd.print("Fase 2");
    }
    else { // nível 3
      jogo(tempoF3);
      lcd.setCursor(0, 1);
      lcd.print("Fase 3");
    }

    lcd.home();
    if (pontuacao != pontAnterior) { // caso de acerto (aumentou a pontuação)
      lcd.clear();
      lcd.print("Pontuacao:");
      lcd.setCursor(12, 0);
      lcd.print(pontuacao);
    } else if (erros != errosAnterior) { // caso de erro (aumentou os erros)
      lcd.clear();
      lcd.print("Erros:");
      lcd.setCursor(8, 0);
      lcd.print(erros);
    }
  } 
  
  if (pontuacao == 20 && pontuacao != pontAnterior) { // ganhou
    lcd.clear();
    lcd.home();
    lcd.print("Parabens!");
    lcd.setCursor(0,1);
    lcd.print("Voce ganhou!");
  } else if (erros == 5 && erros != errosAnterior) { // perdeu
    lcd.clear();
    lcd.home();
    lcd.print("Game Over!");
    lcd.setCursor(0, 1);
    lcd.print("Voce perdeu!");
  }

  pontAnterior = pontuacao;
  errosAnterior = erros;
}


void jogo(int tempo_rodada) 
{
    if (estadoJogo == 0) {
    ledAtual = random(0, 4); //sorteia
    Serial.print("num escolhido foi: ");
    Serial.println(ledAtual);
    Serial.println("Pronto");
    digitalWrite(leds[ledAtual], HIGH); //acende
    millisRodada = millis(); //"reseta" o tempo da rodada 
    estadoJogo = 1; // permitir o jogador jogar e evita mudar o led sorteado antes das ações do jogador
  }
  
  if (estadoJogo == 1) {
    if (millis() - millisRodada < tempo_rodada) { // se tiver dentro do tempo de ação
      if (digitalRead(botoes[ledAtual]) == HIGH && botaoJaApertado == false) { // aperta botão  -> acertou 
        if (millis() - millisBotao > 50) { //debounce
          digitalWrite(leds[ledAtual], LOW); //apaga o led
          pontuacao++; //aumenta a pontuação
          botaoJaApertado = true; //evita contar 2 apertos de botão em caso de segurar
          millisBotao = millis(); // "reseta" millis de debounce
          estadoJogo = 2; //vai pro tempo de pausa entre rodadas
          millisPausa = millis(); //"reseta" tempo do millis de pausa
          tone(pinBuzzer, 1400, 70); //função que aciona o buzzer (som de acerto)
        }
       }
    }

    if (digitalRead(botoes[ledAtual]) == LOW) { //soltou botão
      botaoJaApertado = false;
    }

    if (millis() - millisRodada > tempo_rodada) { //passou o tempo de ação -> errou
      digitalWrite(leds[ledAtual], LOW); //apaga led
      estadoJogo = 2; //vai pro tempo de pausa entre rodadas
      millisPausa = millis(); //"reseta" tempo do millis de pausa
      erros++; // aumenta a quantiadade de erros
      tone(pinBuzzer, 300, 180); //função que aciona o buzzer (som de erro)
    }
  }
  
  if (estadoJogo == 2) {
    if (millis() - millisPausa >= tempoPausa) { //tempo de pausa
      estadoJogo = 0; // permite sortear dnv
    }
  }
}