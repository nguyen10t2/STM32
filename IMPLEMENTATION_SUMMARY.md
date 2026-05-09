# UART1 Command Handling Implementation

## Completed Functions

### 1. `uint8_t hex2int(char c)`
Converts a hexadecimal character ('0'-'9', 'a'-'f', 'A'-'F') to its integer value (0-15).
- Supports both uppercase and lowercase hex digits
- Returns 0 for invalid characters

### 2. `void ProcessCommand(char* cmd)`
Processes received UART commands from the serial monitor:
- **"ADD:"** - Parses and adds card ID in hex format (e.g., "ADD:AABBCCDDEE")
- **"READ"** - Responds with a read command confirmation
- **Unknown** - Sends "Unknown command" response

Responses are sent back via UART1 at 115200 baud.

### 3. `void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)`
UART1 receive interrupt callback that:
- Accumulates received bytes into `rx_buf` until CR (\r) or LF (\n) is received
- Sets `cmd_received` flag to 1 when a complete command is received
- Restarts interrupt reception for the next character
- Safely handles buffer overflow (max 99 chars)

## Integration

### Initialization (main.c)
- `Task_Init()` called during MCU initialization
- `HAL_UART_Receive_IT()` started to enable interrupt-driven reception

### Main Loop
- `Task_Run()` executes continuously
- Checks `cmd_received` flag and processes command when set
- Automatically resets flags after processing

## Usage

Send commands via serial monitor (115200 baud):
```
ADD:0102030405
READ
UNKNOWN_CMD
```

Each command must end with CR (\r) or LF (\n).
