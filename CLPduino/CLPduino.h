#include <EEPROM.h>

// Mapeamento dos estados
#define HIGH_STATE '^'
#define LOW_STATE '_'
#define TOGGLE_STATE '~'

// Mapeamento das operações
#define OP_GREATER '>'
#define OP_LESS '<'
#define OP_AND '&'
#define OP_OR '|'
#define OP_EQUAL '='
#define OP_NOT_EQUAL '!'

// Comandos
#define clear_EEPROM     "clearEEPROM"
#define print_EEPROM     "printEEPROM"
#define print_size_EEPROM "printSizeEEPROM"
#define print_states     "printStates"

#define instructionLength 8

// Estrutura para os pinos mapeados
struct PinMapping 
{
    char id;
    int pin;
    int atualState;
    int oldState;
};

struct TimerMapping
{
  char id;
  uint16_t time;
  int atualState;
  int oldState;
};

struct commandMapping
{
  char input1;
  char state1;
  char operation;
  char input2;
  char state2;
  char execution;
  char output;
  char outputState;
};
