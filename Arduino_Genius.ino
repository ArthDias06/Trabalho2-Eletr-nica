#include <Arduino.h>  

// CONFIGURAÇÃO DOS PINOS
const int pinosEntradas[4] = {4, 5, 6, 7};         // botões conectados a GND quando pressionados  
const int pinosSaidas[4]   = {8, 9, 10 , 11};      // LEDs  


// PARÂMETROS DO JOGO
const int MAX_RODADAS = 10;                        // número máximo de rodadas  
int sequencia[MAX_RODADAS];                        // armazena índices de 0 a 3  
int rodadaAtual = -1;                              // índice da rodada atual (inicia em -1)  

 
// PROTÓTIPOS
void menuAnimacao();
void gerarProximaRodada()
void tocarSequencia();
bool lerSequenciaDoUsuario();
int esperarPressionarBotao();
void piscarTodos(int vezes, int intervalo);
void comemorarVitoria();



void setup() {
  // configura entradas com pull-up interno
  for (int i = 0; i < 4; ++i) {
    pinMode(pinosEntradas[i], INPUT_PULLUP);
  }

  // configura saídas
  for (int i = 0; i < 4; ++i) {
    pinMode(pinosSaidas[i], OUTPUT);
    digitalWrite(pinosSaidas[i], LOW);
  }

 
  // inicializa array com -1
  for (int i = 0; i < MAX_RODADAS; ++i) {
    sequencia[i] = -1;
  }


  // semente aleatória a partir de pino analógico não usado
  randomSeed(analogRead(A0));
  Serial.begin(9600);
} 

void loop() {
  // -------- MENU --------
  menuAnimacao();
  delay(500);

  // recomeça o jogo
  rodadaAtual = -1;

  while (true) {
    // -------- GERAR E MOSTRAR --------
    gerarProximaRodada();
    tocarSequencia();


    // -------- LEITURA DO JOGADOR --------
    if (!lerSequenciaDoUsuario()) {
      // ERROU: GAME OVER
      piscarTodos(3, 300);
      delay(500);
      break;
    }

    // SE COMPLETOU TODAS AS RODADAS -> VITÓRIA
    if (rodadaAtual + 1 >= MAX_RODADAS) {
      comemorarVitoria();
      delay(1000);
      break;
    }

    // ganhou esta rodada, continua para a próxima
    delay(500);
  }
}


// animação do menu: LEDs da esquerda pra direita e volta
void menuAnimacao() {
  while (true) {
    // vai e volta
    for (int dir = 1; dir >= -1; dir -= 2) {
      int start = (dir == 1) ? 0 : 3;
      int end = (dir == 1) ? 3 : 0;
      
      for (int i = start; i != end + dir; i += dir) {
        digitalWrite(pinosSaidas[i], HIGH);
        delay(1000);
        digitalWrite(pinosSaidas[i], LOW);
        
        // se apertar qualquer botão, sai do menu
        for (int b = 0; b < 4; ++b) {
          if (digitalRead(pinosEntradas[b]) == LOW) {

            // aguarda soltar
            while (digitalRead(pinosEntradas[b]) == LOW) {
                delay(1000);
              }

            return;
          }
        }
        delay(1000);
      }
    }
  }
}


// adiciona um número aleatório à sequência
void gerarProximaRodada() {
  ++rodadaAtual;
  sequencia[rodadaAtual] = random(0, 4);
}


// pisca cada LED na ordem da sequência
void tocarSequencia() {
  for (int i = 0; i <= rodadaAtual; ++i) {
    int led = pinosSaidas[sequencia[i]];
    digitalWrite(led, HIGH);
    delay(400);
    digitalWrite(led, LOW);
    delay(200);
  }
} 


// lê as teclas do usuário, comparando com a sequência
bool lerSequenciaDoUsuario() {
  for (int i = 0; i <= rodadaAtual;++i) {
    int esperado = sequencia[i];
    int recebido = esperarPressionarBotao();
    if (recebido != esperado) {
      return false;
    }
  }
  return true;
}
 

// espera o usuário pressionar um botão e retorna o índice (0..3)
int esperarPressionarBotao() {
  while (true) {
    for (int i = 0; i < 4; ++i) {
      if (digitalRead(pinosEntradas[i]) == LOW) {
        delay(50); // debounce inicial
        if (digitalRead(pinosEntradas[i]) == LOW) {
          // botão confirmado
          int idx = i;
          
          // aguarda soltar antes de prosseguir
          while (digitalRead(pinosEntradas[idx]) == LOW) {
            delay(10);
          }
          delay(50); // debounce pós-release
          return idx;
        }
      }
    }
  }
}


// pisca todos os LEDs n vezes
void piscarTodos(int vezes, int intervalo) {
  for (int i = 0; i < vezes;++i) {
    for (int j = 0; j < 4;++j) digitalWrite(pinosSaidas[j], HIGH);
    delay(intervalo);
    for (int j = 0; j < 4;++j) digitalWrite(pinosSaidas[j], LOW);
    delay(intervalo);
  }
}


// pequena comemoração na vitória
void comemorarVitoria() {
  // vai e volta rapidamente
  for (int k = 0; k < 3; ++k) {
    for (int i = 0; i < 4; ++i) {
      digitalWrite(pinosSaidas[i], HIGH);
      delay(100);
      digitalWrite(pinosSaidas[i], LOW);
    }
    for (int i = 3; i >= 0; --i) {
      digitalWrite(pinosSaidas[i], HIGH);
      delay(100);
      digitalWrite(pinosSaidas[i], LOW);
    }
  }
}
