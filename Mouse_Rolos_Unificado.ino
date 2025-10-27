#include <Mouse.h>
#include <Keyboard.h> 

/*
 ================================================================================
  Mouse de Rolos (Encoders) 27/10/2025 - Funções de Modo e Correções
 ================================================================================
 Descrição Geral:
  Este código transforma um Arduino com capacidade USB nativa em um dispositivo
  híbrido de mouse e teclado, controlado por botões. O dispositivo possui três
  modos de operação principais: Modo Mouse (padrão), Modo Scroll e Modo Teclado,
  permitindo uma ampla gama de interações com o computador.

  Compatibilidade:
  - Placas: Funciona em Arduinos com capacidade HID nativa, como Leonardo,
    Pro Micro e Due.
  - Bibliotecas: Requer as bibliotecas padrão "Mouse.h" e "Keyboard.h".
 ================================================================================ 
  Modos de Operação:
  
  1. Modo Mouse (Padrão):
     - Controle o cursor com os botões direcionais.
     - Aceleração: Implementada uma curva de aceleração de duas velocidades, isso 
     permite movimentos precisos de 1 pixel sem sacrificar a velocidade em movimentos amplos.
          - Movimentos lentos (<= 2 ticks) usam "precisionSpeed" (ex: 1.0).
          - Movimentos rápidos (> 2 ticks) usam "fastSpeedMultiplier" (ex: 15.0).
          - Use os botões de ação para os diferentes tipos de clique.
  2. Modo Scroll:
     - Clique no botão scrool para entrar/sair deste modo.
     - Enquanto estiver ativo, os botões Cima e Baixo controlam
       a roda de rolagem (scroll wheel) do mouse.
     - Pressionar qualquer outro botão de ação (cliques, modo teclado) desativa
       automaticamente o modo scroll.
  3. Modo Teclado:
     - Clique no botão teclado para entrar/sair deste modo.
     - Enquanto estiver ativo, os quatro botões direcionais se transformam nas
       setas do teclado, permitindo navegar em menus, textos, etc.
     - Pressionar qualquer outro botão de ação (cliques, modo scroll) desativa
       automaticamente o modo teclado.
 ================================================================================
    Pinos:
  --------------------------------------------------------------------------------
  BOTÕES DE AÇÃO:
  * Pino 6: Clique esquerdo (mouseButton)
  * Pino 7: Clique direito (rightmouseButton)
  * Pino 11: Clique esquerdo longo (leftLong)
  * Pino 8: Clique duplo esquerdo (doubleleft)
  
  BOTÕES DE MODO:
  * Pino 13: Ativa/Desativa MODO SCROLL (scrollButton)
  * Pino 12: Ativa/Desativa MODO TECLADO (keyboardModeButton)

  ENCODERS:
  * Pino 2: Encoder Horizontal CLK
  * Pino 3: Encoder Horizontal DT
  * Pino 0: Encoder Vertical CLK (Movido do pino 9)
  * Pino 1: Encoder Vertical DT (Movido do pino 10)
  ================================================================================
*/

#define DEBOUNCING 50

//Pinos dos botões de clique
const int mouseButton = 6;
const int rightmouseButton = 7;
const int leftLong = 11;
const int doubleleft = 8;

//Pinos dos botões de modo
const int scrollButton = 13;
const int keyboardModeButton = 12;

//Pinos dos Encoders
#define encoder0PinA  2  // Encoder Horizontal (X) - CLK
#define encoder0PinB  3  // Encoder Horizontal (X) - DT
#define encoder1PinA  0  // Encoder Vertical (Y) - CLK
#define encoder1PinB  1  // Encoder Vertical (Y) - DT

// ***** AJUSTE DE VELOCIDADE (CURVA DE ACELERAÇÃO) *****
// Para movimentos precisos (1-2 ticks), use o multiplicador de precisão.
const float precisionSpeed = 4.0; // alterar este valor muda a velocidade do cursor em movimentos lentos; 
// Quantos ticks (pulsos) do encoder por ciclo de 10ms para acionar a aceleração.
const int accelerationThreshold = 2; // alterar este valor muda o limiar entre movimentos lentos e rápidos;
// Para movimentos rápidos (>3 ticks), use o multiplicador de velocidade (como o 15.0 que você usou).
const float fastSpeedMultiplier = 25.0; // alterar este valor muda a velocidade do cursor em movimentos rápidos; 


//Variaveis de controle dos encoders
volatile long encoder0Pos = 0; // Posição X (Horizontal)
volatile long encoder1Pos = 0; // Posição Y (Vertical)
volatile int vel, vel1;

//Variaveis de controle dos botões
boolean variavel = 0, variavel2 = 1, variavel3 = 1, variavel4 = 1, variavel5 = 1;

//Variáveis de Modo
bool scrollMode = false;
bool keyboardMode = false;
int lastScrollButtonState = HIGH;
int lastKeyboardModeButtonState = HIGH;


void setup() {
  pinMode(mouseButton, INPUT_PULLUP);
  pinMode(leftLong, INPUT_PULLUP);
  pinMode(doubleleft, INPUT_PULLUP);
  pinMode(rightmouseButton, INPUT_PULLUP);
  pinMode(scrollButton, INPUT_PULLUP);
  pinMode(keyboardModeButton, INPUT_PULLUP);

  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  pinMode(encoder1PinA, INPUT_PULLUP);
  pinMode(encoder1PinB, INPUT_PULLUP);

  // Lógica de interrupção (v6.5)
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoderB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder1PinA), doEncoderC, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder1PinB), doEncoderD, CHANGE);

  Mouse.begin();
  Keyboard.begin();
  Serial.begin(9600);
}

void loop() {
  //Leitura dos botões de ação
  boolean mouseLeftState = (digitalRead(mouseButton) == LOW);
  boolean mouseRightState = (digitalRead(rightmouseButton) == LOW);
  boolean doubleLeftState = (digitalRead(doubleleft) == LOW);
  boolean leftLongState = (digitalRead(leftLong) == LOW);

  //Leitura dos botões de modo
  int currentScrollButtonState = digitalRead(scrollButton);
  int currentKeyboardModeButtonState = digitalRead(keyboardModeButton);

  // --- Lógica para gerenciar os MODOS (Scroll e Teclado) ---
  if (currentScrollButtonState == LOW && lastScrollButtonState == HIGH) {
    if (keyboardMode) {
      Keyboard.releaseAll();
      keyboardMode = false;
    }
    scrollMode = !scrollMode;
    delay(DEBOUNCING);
  }

  if (currentKeyboardModeButtonState == LOW && lastKeyboardModeButtonState == HIGH) {
    if (scrollMode) {
      scrollMode = false;
    }
    keyboardMode = !keyboardMode;
    if (!keyboardMode) {
      Keyboard.releaseAll();
    }
    delay(DEBOUNCING);
  }

  lastScrollButtonState = currentScrollButtonState;
  lastKeyboardModeButtonState = currentKeyboardModeButtonState;

  bool anyActionButtonPressed = mouseLeftState || mouseRightState || doubleLeftState || leftLongState;
  if (anyActionButtonPressed) {
    if (scrollMode) scrollMode = false;
    if (keyboardMode) {
      Keyboard.releaseAll();
      keyboardMode = false;
    }
  }

  // --- Lógica de movimento (com curva de aceleração) ---
  long rawX = encoder0Pos; // Salva o valor bruto
  long rawY = encoder1Pos; // Salva o valor bruto

  // Lógica de Aceleração X (Horizontal)
  vel = abs(rawX);
  if (vel <= accelerationThreshold) {
    vel = vel * precisionSpeed; // Movimento Lento/Preciso
  } else {
    vel = vel * fastSpeedMultiplier; // Movimento Rápido
  }

  // Lógica de Aceleração Y (Vertical)
  vel1 = abs(rawY);
  if (vel1 <= accelerationThreshold) {
    vel1 = vel1 * precisionSpeed; // Movimento Lento/Preciso
  } else {
    vel1 = vel1 * fastSpeedMultiplier; // Movimento Rápido
  }


  // --- Execução das Ações Baseado no Modo Atual ---
  if (scrollMode) {
    // --- MODO SCROLL ATIVADO ---
    // Eixo X (Horizontal)
    if (rawX < 0) {
      Mouse.move(vel, 0, 0); // INVERTIDO
    } else if (rawX > 0) {
      Mouse.move(-vel, 0, 0); // INVERTIDO
    }
    // Eixo Y (Vertical)
    if (rawY < 0) {
      Mouse.move(0, 0, -vel1); // INVERTIDO
    } else if (rawY > 0) {
      Mouse.move(0, 0, vel1); // INVERTIDO
    }

  } else if (keyboardMode) {
    // --- MODO TECLADO ATIVADO ---
    if (rawX < 0) {
      Keyboard.write(KEY_RIGHT_ARROW); // INVERTIDO
    } else if (rawX > 0) {
      Keyboard.write(KEY_LEFT_ARROW); // INVERTIDO
    }
    if (rawY < 0) {
      Keyboard.write(KEY_DOWN_ARROW); // INVERTIDO
    } else if (rawY > 0) {
      Keyboard.write(KEY_UP_ARROW); // INVERTIDO
    }
    
  } else {
    // --- MODO NORMAL (MOUSE) ---
    // Eixo X (Horizontal)
    if (rawX < 0) {
      Mouse.move(vel, 0, 0); // INVERTIDO
    } else if (rawX > 0) {
      Mouse.move(-vel, 0, 0); // INVERTIDO
    }
    // Eixo Y (Vertical)
    if (rawY < 0) {
      Mouse.move(0, -vel1, 0); // INVERTIDO (Mouse.move Y: + é baixo, - é cima)
    } else if (rawY > 0) {
      Mouse.move(0, vel1, 0); // INVERTIDO
    }

    // --- Lógica dos botões de clique ---
    if (mouseLeftState) {
      if (variavel2 == 1) {
        delay(DEBOUNCING);
        if (digitalRead(mouseButton) == LOW) {
          Mouse.press(MOUSE_LEFT);
          variavel2 = 0;
        }
      }
    } else {
      if (variavel2 == 0 && variavel5 == 0) { 
        Mouse.release(MOUSE_LEFT);
        variavel2 = 1;
      }
    }
    if (mouseRightState) {
      if (variavel3 == 1) {
        delay(DEBOUNCING);
        if (digitalRead(rightmouseButton) == LOW) {
          Mouse.press(MOUSE_RIGHT);
          variavel3 = 0;
        }
      }
    } else {
      Mouse.release(MOUSE_RIGHT);
      variavel3 = 1;
    }
    if (doubleLeftState) {
      if (variavel4 == 1) {
        delay(DEBOUNCING);
        if (digitalRead(doubleleft) == LOW) {
          if (!Mouse.isPressed(MOUSE_LEFT)) {
            Mouse.click(MOUSE_LEFT);
            delay(100);
            Mouse.click(MOUSE_LEFT);
            variavel4 = 0;
          }
        }
      }
    } else {
      variavel4 = 1;
    }
    if (leftLongState) {
      if (digitalRead(leftLong) == LOW && variavel5 == 0) {
        if (variavel == 0) {
          Mouse.press(MOUSE_LEFT);
          delay(100);
          variavel5 = !variavel5;
          variavel = !variavel;
        } else {
          Mouse.release(MOUSE_LEFT);
          delay(100);
          variavel5 = !variavel5;
          variavel = !variavel;
        }
      }
    } else {
      variavel5 = 0;
    }
  }
  
  //Reseta a posição dos encoders a cada ciclo
  encoder0Pos = 0;
  encoder1Pos = 0;
  
  delay(10); // Delay de estabilidade
}

// =================================================================
//     FUNÇÕES DE INTERRUPÇÃO (v6.5 - Lógica robusta)
// =================================================================

// Encoder 0 (Horizontal)
void doEncoderA() {
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++; 
  } else {
    encoder0Pos--;
  }
}
void doEncoderB() {
  if (digitalRead(encoder0PinA) != digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
}

// Encoder 1 (Vertical)
void doEncoderC() {
  if (digitalRead(encoder1PinA) == digitalRead(encoder1PinB)) {
    encoder1Pos++;
  } else {
    encoder1Pos--;
  }
}
void doEncoderD() {
  if (digitalRead(encoder1PinA) != digitalRead(encoder1PinB)) {
    encoder1Pos++;
  } else {
    encoder1Pos--;
  }
}
