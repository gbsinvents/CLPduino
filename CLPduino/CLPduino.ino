#include "CLPduino.h"

// Tabela de mapeamento de entradas digitais
PinMapping inputDigitalPins[] = 
{
    {'A', 2}, {'B', 3},
    {'C', 4}, {'D', 5}
};

// Tabela de mapeamento de entradas Analógicas
PinMapping inputAnalogPins[] =
{
  {'Y', A0}, {'Z', A1}
};

// Tabela de mapeamento de saídas digitais
PinMapping outputPins[] = 
{
    {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}
};

// Tabela de mapeamento de memórias
PinMapping virtualRelay[] =
{
  {'q'}, {'r'}
};

// Tabela de mapeamento de temporizadores
TimerMapping timer[] =
{
  {'T'}, {'U'}
};

// Função para executar a operação lógica
bool executeOperation(uint16_t val1, uint16_t val2, char operation) 
{
    switch (operation) 
    {
        case OP_GREATER:
            return val1 > val2;
        case OP_LESS:
            return val1 < val2;
        case OP_AND:
            return val1 && val2;
        case OP_OR:
            return val1 || val2;
        case OP_EQUAL:
            return val1 == val2;
        case OP_NOT_EQUAL:
            return val1 != val2;
        default:
            return false;
    }
}

// Função para obter o estado de um pino a partir do ID
bool getPinState(char id, char state) 
{
  bool atualState, output;
    for (auto pin : inputDigitalPins) 
    {
        if (pin.id == id) 
        {
          atualState = pin.atualState;

          switch (state)
          {
            case HIGH_STATE:
              output = (atualState == HIGH)? true : false;
            break;

            case LOW_STATE:
              output = (atualState == LOW)? true : false;
            break;

            case TOGGLE_STATE:
              output = (pin.atualState != pin.oldState) ? true : false;
            break;

            default:
              output = false;

          }
          pin.oldState = atualState;
          return output;
        }
    }
}

// Função para atualizar o estado de um pino de saída
void setPinState(char id, char state) 
{
  for (auto &pin : outputPins)  // Substituímos o 'auto' por um loop tradicional
  {
    if (pin.id == id) 
    {
      pin.atualState = state;
      
      if (state == HIGH_STATE) 
      {
        pin.atualState = true;
      }
      else if (state == LOW_STATE) 
      {
        pin.atualState = false;
      }
      else if (state == TOGGLE_STATE) 
      {
        pin.atualState = !pin.atualState; // Alterna o estado
      }
      break; // Pino encontrado e atualizado; sair do loop
    }
  }

  for (auto &pin : virtualRelay)  // Substituímos o 'auto' por um loop tradicional
  {
    if (pin.id == id) 
    {
      pin.atualState = state;
      
      if (state == HIGH_STATE) 
      {
        pin.atualState = true;
      }
      else if (state == LOW_STATE) 
      {
        pin.atualState = false;
      }
      else if (state == TOGGLE_STATE) 
      {
        pin.atualState = !pin.atualState; // Alterna o estado
      }
      break; // Pino encontrado e atualizado; sair do loop
    }
  }

  for (auto &pin : timer)  // Substituímos o 'auto' por um loop tradicional
  {
    if (pin.id == id) 
    {
      pin.atualState = state;
      
      if (state == HIGH_STATE) 
      {
        pin.atualState = true;
      }
      else if (state == LOW_STATE) 
      {
        pin.atualState = false;
      }
      else if (state == TOGGLE_STATE) 
      {
        pin.atualState = !pin.atualState; // Alterna o estado
      }
      break; // Pino encontrado e atualizado; sair do loop
    }
  }
  
}

// Função para processar os dados que chegam via UART
void processProtocol (void)
{
  if (!Serial.available()) 
  {
    //Serial.println("ERRO: UART não disponível");
    return;
  }
   
  String command = Serial.readStringUntil('\n'); // Lê o comando da serial
  command.trim(); // Remove espaços em branco

  // Certifique-se de que o comando não está vazio
  if (command.length() == 0)
  {
    Serial.println("ERRO: UART vazia");
    return;
  }

  // Lista de comandos padrões
  if(command == clear_EEPROM)
  {
    Serial.println("Limpando...");
    clearEEPROM();
    Serial.println("EEPROM limpa com sucesso.");
    return;
  }
      
  if(command == print_EEPROM)
  {
    printEEPROMCommands();
    return;
  }

  if(command == print_size_EEPROM)
  {
    Serial.print("EEPROM com: ");
    Serial.print(getEEPROMSize());
    Serial.println(" instruções");
    return;
  }

  if(command == print_states)
  {
    printStates();
    return;
  }

  // Processa novas instruções
  if (command.length() == instructionLength) 
  {
    commandMapping thisCommand;

    thisCommand.input1      = command[0];
    thisCommand.state1      = command[1];
    thisCommand.operation   = command[2];
    thisCommand.input2      = command[3];
    thisCommand.state2      = command[4];
    thisCommand.execution   = command[5];
    thisCommand.output      = command[6];
    thisCommand.outputState = command[7];

    // Valida a execução (deve ser '=')
     if (thisCommand.execution != OP_EQUAL) 
    {
      Serial.println("Erro: Execução inválida.");
      return;
    }

    // Verifica se ainda há espaço na EEPROM
    if (setEEPROMCommand(thisCommand)) 
    {
      Serial.println("Comando gravado com sucesso.");
    }
    else
    {
      Serial.println("Erro: EEPROM cheia. Não é possível gravar mais comandos.");
    }
  }
  else 
  {
    Serial.println("Erro: Comando inválido. O comprimento deve ser exatamente 8 caracteres.");
  }
  
  Serial.flush(); // Limpa o buffer da serial para garantir que o próximo comando seja lido corretamente

}

// Função que exibe na serial o estado de cada pino, memoria ou timer
void printStates ()
{
   for (auto pin : inputDigitalPins)
  {
    //pin.atualState = !digitalRead(pin.pin);
    Serial.print(pin.id);
    Serial.print(":");
    Serial.print(pin.atualState);
    Serial.print(" - ");
  } 
  for (auto pin : inputAnalogPins)
  {
    //pin.atualState = analogRead(pin.pin);
    Serial.print(pin.id);
    Serial.print(":");
    Serial.print(pin.atualState);
    Serial.print(" - ");
  }
  for (auto pin : outputPins)
  {
    //pin.atualState = digitalRead(pin.pin);
    Serial.print(pin.id);
    Serial.print(":");
    Serial.print(pin.atualState);
    Serial.print(" - ");
  }

  for (auto thisVirtualRelay : virtualRelay)
  {
    Serial.print(thisVirtualRelay.id);
    Serial.print(":");
    Serial.print(thisVirtualRelay.atualState);
    Serial.print(" - ");
  }

  for (auto thisTimer : timer)
  {
    Serial.print(thisTimer.id);
    Serial.print(":");
    Serial.print(thisTimer.time);
    Serial.print(" - ");
  }

  Serial.println();
}

// Função que atualiza entradas, saidas, temporizadores e memórias
void updateStates() 
{
  for (auto &pin : inputDigitalPins) 
  {
    pin.atualState = !digitalRead(pin.pin);
  }
  for (auto &pin : inputAnalogPins) 
  {
    pin.atualState = analogRead(pin.pin);
  }
  for (auto &pin : outputPins) 
  {
    digitalWrite(pin.pin, pin.atualState);
  }
}

// Função que exibe na UART todos os comando armazenados
void printEEPROMCommands() 
{
  int sizeEEPROM = getEEPROMSize() * 8;

  if(sizeEEPROM == 0)
  {
    Serial.println("EEPROM Vazia");
    return;
  }

  Serial.println("Comandos na EEPROM:");
  for(int i = 0; i < sizeEEPROM; i += 8) 
    {
        char input1 = EEPROM[i];
        char state1 = EEPROM[i + 1];
        char operation = EEPROM[i + 2];
        char input2 = EEPROM[i + 3];
        char state2 = EEPROM[i + 4];
        char exec = EEPROM[i + 5];
        char output = EEPROM[i + 6];
        char outputState = EEPROM[i + 7];

        // Verifica se o comando é válido (padrão EEPROM vazio é 0xFF)
        if (input1 == 0xFF) break;

        int intruction = i / 8;
        Serial.print(intruction);
        Serial.print(": ");
        Serial.print(input1);
        Serial.print(state1);
        Serial.print(operation);
        Serial.print(input2);
        Serial.print(state2);
        Serial.print(exec);
        Serial.print(output);
        Serial.print(outputState);
        Serial.println();
    }
}

// Função para limpar toda a EEPROM
void clearEEPROM() 
{
    for (int i = 0; i < EEPROM.length(); i++) 
    {
        EEPROM.write(i, 0xFF); // Define todas as posições como 0xFF
    }
}

// Função para ler comandos da EEPROM
commandMapping readEEPROMCommand(int index) 
{
    commandMapping thisCommand;
    
    // Certifique-se de que o índice está dentro do intervalo válido
    if (index < 0 || index > (EEPROM.length() - 8)) 
    {
        Serial.println("Erro: índice fora do intervalo da EEPROM.");
        return thisCommand; // Retorna vazio em caso de erro
    }

    thisCommand.input1 = EEPROM[index];
    thisCommand.state1 = EEPROM[index + 1];
    thisCommand.operation = EEPROM[index + 2];
    thisCommand.input2 = EEPROM[index + 3];
    thisCommand.state2 = EEPROM[index + 4];
    thisCommand.execution = EEPROM[index + 5];
    thisCommand.output = EEPROM[index + 6];
    thisCommand.outputState = EEPROM[index + 7];


    return thisCommand;
}

// Função que armazena comandos na EEPROM
bool setEEPROMCommand(commandMapping thisCommand)
{
    int eepromAddress = getEEPROMSize() * 8; // Endereço inicial na EEPROM

    Serial.print("Instrução: ");
    Serial.println(eepromAddress / 8);

    if (eepromAddress == 256) // Limite de EEPROM atingido
    {
        return false;
    }

    // Grava o comando na EEPROM
    EEPROM[eepromAddress] = thisCommand.input1;
    EEPROM[eepromAddress + 1] = thisCommand.state1;
    EEPROM[eepromAddress + 2] = thisCommand.operation;
    EEPROM[eepromAddress + 3] = thisCommand.input2;
    EEPROM[eepromAddress + 4] = thisCommand.state2;
    EEPROM[eepromAddress + 5] = thisCommand.execution;
    EEPROM[eepromAddress + 6] = thisCommand.output;
    EEPROM[eepromAddress + 7] = thisCommand.outputState;

    return true; // Comando gravado com sucesso
}

// Função que retorna o tamanho da EEPROM considerando blocos de 8 Bytes
int getEEPROMSize()
{
    int count = 0;

    for (int i = 0; i < 256; i += 8) // Verifica todos os blocos de 8 bytes
    {
        char execution = EEPROM[i + 5]; // Verifica o byte de execução de cada comando
        if (execution == '=') 
        {
            count++;
        }
        else 
        {
            // Interrompe a verificação ao encontrar uma posição inválida
            break;
        }
    }

    return count; // Retorna a quantidade de comandos válidos
}

// Função para converter valores ASCII (centenas, dezenas e unidades) em um inteiro
int asciiToInt(char hundreds, char tens, char units) 
{
    return (100 * (hundreds - '0')) + (10 * (tens - '0')) + (units - '0');
}

// Função para avaliar comparações analógicas
bool evaluateComparison(int analogValue, int compareValue, char comparisonOperator) 
{
    switch (comparisonOperator) 
    {
        case '<': return analogValue < compareValue;
        case '>': return analogValue > compareValue;
        case '=': return analogValue == compareValue;
        case '!': return analogValue != compareValue;
        default:
            Serial.println("Erro: Operador inválido.");
            return false;
    }
}

// Função para executar comandos analógicos
bool handleAnalogCommand(commandMapping cmd) 
{
    int analogValue = 0;

    // Localiza o pino analógico correspondente
    for (auto pin : inputAnalogPins) 
    {
        if (pin.id == cmd.input1) 
        {
            analogValue = analogRead(pin.pin);
            break;
        }
    }

    // Converte os valores ASCII para comparação
    int compareValue = asciiToInt(cmd.operation, cmd.input2, cmd.state2);

    // Avalia a comparação e retorna o resultado
    return evaluateComparison(analogValue, compareValue, cmd.state1);
}

// Função para executar comandos digitais
bool handleDigitalCommand(commandMapping cmd) 
{
    bool val1 = getPinState(cmd.input1, cmd.state1);
    bool val2 = getPinState(cmd.input2, cmd.state2);
    return executeOperation(val1, val2, cmd.operation);
}

// Função para ler o tempo armazenado em cada temporizador
uint16_t getTimerTime(char timerID) 
{
    // Itera sobre os elementos na tabela de timers
    for (auto &timerMapping : timer) 
    {
        if (timerMapping.id == timerID) 
        {
            return timerMapping.time; // Retorna o tempo associado ao ID encontrado
        }
    }
    // Caso o timerID não seja encontrado, retornar 0 ou outro valor padrão
    return 0;
}

// Função para executar comandos relacionados a timer
bool handleTimerCommand(commandMapping cmd) 
{
    uint16_t val1 = getTimerTime(cmd.input1);
    uint16_t val2 = asciiToInt(cmd.operation, cmd.input2, cmd.state2);
    bool result = executeOperation(val1, val2, cmd.state1);

    if(result)
    {
      for(auto &pin : timer)
      {
        if(pin.id == cmd.input1)
        {
          pin.time = 0;
          pin.atualState = 0;
        }
      }
    }

    return result;
}

// Essa função verifica se a insturção está utilizando uma entrada analógica
bool isAnalogInput(char input) 
{
    // Itera sobre a tabela de mapeamento de entradas analógicas
    for (const auto& pinMapping : inputAnalogPins) 
    {
        if (pinMapping.id == input) 
        {
            return true;
        }
    }
    return false;
}

bool isTimer(char input) 
{
    // Itera sobre a tabela de mapeamento de entradas analógicas
    for (const auto& timerMapping : timer) 
    {
        if (timerMapping.id == input) 
        {
            return true;
        }
    }
    return false;
}
// Função principal para executar os comandos
void executeCommands() 
{
    int sizeEEPROM = getEEPROMSize() * 8;

    if (sizeEEPROM == 0) 
    {
        //Serial.println("Nenhum comando na EEPROM.");
        return;
    }

    // Faz Varredura na EEPROM lendo os comandos
    for (int i = 0; i < sizeEEPROM; i += 8) 
    {
        // Lê o comando da EEPROM
        commandMapping cmd = readEEPROMCommand(i);
        bool result = false;

        if (isAnalogInput(cmd.input1)) 
        {
          // Executa comandos de entradas analógicos
          result = handleAnalogCommand(cmd);
        }

        else if (isTimer(cmd.input1))
        {
          // Executa comandos relacionados ao timer
          result = handleTimerCommand(cmd);
        } 

        else 
        {
          // Executa comandos relacionados a lógica digital
          result = handleDigitalCommand(cmd);
        }

        // Se o resultado for verdadeiro, ajusta o estado do pino de saída
        if (result) 
        {
          setPinState(cmd.output, cmd.outputState);
        }
    }
}

// Função que inicializa Timer 1
void initTimer1 (void)
{
  noInterrupts(); // Desabilitar interrupções enquanto configuramos o timer

  // Configurar Timer1
  TCCR1A = 0;                // Modo Normal
  TCCR1B = 0;                // Limpar registradores
  TCNT1 = 0;                 // Zerar o contador
  OCR1A = 15999;             // Valor de comparação para 1ms (16 MHz / 1000 - 1)
  TCCR1B |= (1 << WGM12);    // Habilitar CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << CS10);     // Configurar prescaler para 1
  TCCR1B |= (1 << CS11);     // (Prescaler final: 64)
  TIMSK1 |= (1 << OCIE1A);   // Habilitar interrupção de comparação do Timer1

  interrupts(); // Habilitar interrupções  
}

// Função de interrupção do Timer1
ISR(TIMER1_COMPA_vect) 
{
  for(auto &timerMapping : timer)
  {
    if(timerMapping.atualState)
    {
      if(timerMapping.time >= 999)
      {
        timerMapping.time = 0;
      }
      timerMapping.time++;
    }
  }
}

void setup() 
{
  initTimer1();

  // Configuração dos pinos como entrada/saída
  for (auto pin : inputDigitalPins) pinMode(pin.pin, INPUT_PULLUP);
  for (auto pin : outputPins) pinMode(pin.pin, OUTPUT);

  for (auto thisVirtualRelay: virtualRelay) thisVirtualRelay.atualState = 0;

  for (auto thisTimer: timer) thisTimer.time = 0;

  Serial.begin(9600);
}

void loop() 
{
  updateStates();
  processProtocol();
  executeCommands();  
}
