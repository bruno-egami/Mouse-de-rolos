# Mouse-de-rolos

Código para utilizar um mouse de rolos utilizando Arduino Leonardo/DUE, switchs e encoders rotativos.

Instruções detalhadas de uso e confecção dispoíveis em [Manual de Confecção](https://docs.google.com/document/d/1pMJkUQSxsF9xCENk5veryS8fIuOfQ9QLx8JjR66zWYs/edit?usp=sharing)

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
          - Movimentos lentos (<= 2 ticks) usam "precisionSpeed" (ex: 1.0). #
          - Movimentos rápidos (> 2 ticks) usam "fastSpeedMultiplier" (ex: 15.0). #
          - Use os botões de ação para os diferentes tipos de clique. #
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
