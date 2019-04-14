# gsm

Library for the interfacing of GSM modems to the CIAA platform. 

Based on SIMCOM's SIM808 module and the EDU-CIAA-NXP board, though implementation remains generic in most aspects.

## General description

GSM modems traditionally work with serial RS-232 communication. The usual protocol is informally called *AT protocol*, though it's technical name is the *Hayes command set*. It is an old protocol, harking back to a [specific command language](https://en.wikipedia.org/wiki/Hayes_command_set) originally developed by Dennis Hayes for the Hayes Smartmodem 300 baud modem in 1981. It has since been expanded greatly to accomodate all the features of cellular telephony.

The AT protocol generally works in a command-response fashion, although certain conditions in the modem (such as startup messages or receiving an SMS) may give rise to what is called an URC (UnRequested Code). Thus, we need to send commands (checking their echo to confirm reception), interpret the responses and also be on the lookout for URCs, which may come up at any time without any preceding command and even interrupt a series of responses. 

An sample command-response interaction would be as follows:

```CMD: **AT**```                       (this command does nothing, just confirms modem availability)  
```RSP: **OK**```                    (almost all commands are closed with an OK message)  
```CMD: **AT+CREG?**```               (asking if we are connected to the GSM network)  
```RSP: **+CREG: 0,1**```               (we are conected to the network)  
```RSP: **OK**```  
```CMD: **AT+CMGS="+5491151751810"**``` (send an SMS to the indicated number)  
```RSP: **`> `**```                     (prompts us to write the text and then enter Ctrl+Z)  
```CMD: **Hola!**```                    (we enter the text and enter Crtl+Z)  
```RSP: **+CMGS: 4**```                 (the message was sent successfully and 4 is the storage code)  
```RSP: **OK**```  

The function of the library is to allow the user to interact with the modem at a higher abstraction level than that of AT commands. For most of the tasks the user is interested in (send or read SMSs, open a TCP or UDP port, get GPS information, etc.), several commands need to be sent, analyzing their responses and any possible error messages. The intention is to hide this as much as possible from the user and present to him a streamlined interface.

## Abstraction layers

The library was designed with a strong emphasis on defining a number of abstraction layers and keeping a vertical-communication scheme as much as possible. That is, layers should ideally communicate only with the layer above and below it.

    +--------------+
    |              |
    |     User     |
    |              |
    +--------------+
            | 
            |
     +------------+
     |            |
     |  Protocol  |
     |            |
     +------------+
            |
            |
     +------------+
     |            |
     |   Comms    |
     |            |
     +------------+

The **User** layer involves all the functions the user needs to implement the library into his program (startup functions, timers, etc.) as well as the actual GSM modem-related functions (sending an SMS, reading an SMS, opening a TCP port, etc.). Almost all the lower level details are hidden from him.

The **Protocol** layer handles all the issues related to the AT command protocol. This means interpreting the actual commands and responses, keeping an internal status indicating whether a command has been processed fully, etc.

The **Comms** layer handles the serial communication and the tokenization process; that is, turning the continuous serial traffic into discrete *tokens* which can later be analized by the Protocol layer under AT command classification schemes.

## Software structure

While the intention is to have the software structure mimic the abstraction layers, there is some overlap. However, it has been minimized as much as possible and the three software structure layers try to correspond to the three abstraction layers. The software is divided into five modules, as the following image indicates.

    +--------------+
    |              |
    | gsmInterface |
    |              |
    +--------------+
           |
           |
     +------------+     +------------+     +-------------+
     |            |     |            |     |             |
     | gsmEngine  |-----|  gsmParser |-----| gsmCommands |
     |            |     |            |     |             |
     +------------+     +------------+     +-------------+
           |
           |
    +--------------+
    |              |
    | gsmTokenizer |
    |              |
    +--------------+

The `gsmInterface` module handles all user-facing functions. The user-level functions often involve several AT commands for completion; we call this sequence of commands with a specific goal *formulas*. Each formula is a two-level FSM which initializes the input data, processes all the needed AT commands to completion and reports results and errors. All this is handled one token at a time by succesive calls to the `gsmProcess` function. 

The `gsmEngine` module handles the general state of the library, as well as an important FSM which determines if a command is currently being processed and if so, in which state of the command processing cycle we currently are.

The `gsmParser` module is comprised of a single function, but it's importance is central. It turns the raw tokens received from the gsmTokenizer module into processed AT commands, classified according to the protocol's rules.

The `gsmCommands` module is rather simple; it just lists the recognized AT commands and URCs. It also provides a search function.

The `gsmTokenizer` module interacts directly with the serial connection to the GSM modem. It breaks up the raw serial transit into tokens, which are then sent to the upper layers for classification and processing. The library includes a special variable-length ring buffer (VLRB) library, which extends the functionality of standard ring buffers to items of arbitrary length. This allows for a more efficient use of memory.

## Timing modes

The library can work in two modes: **FTE (fixed-time execution) mode** and **RTOS mode**.

In **FTE mode**, the idea is to have the `gsmProcess` function execute at fixed time intervals. Each successful execution of `gsmProcess` processes a single token.

To implement this, the user determines a specific period of time in ms in the constant `DELAY_PROC` in the `gsmInterface.h` file. This will be the initial value of the `procCnt` counter. The `gsmSysTickHandler` function must be called from the standard `SysTickHandler` function; this will decrement the `procCnt` counter until it reaches 0. Last, the user he must periodically call the `gsmProcess` function. If the time indicated by `DELAY_PROC` has elapsed since the last execution, the function will process a single token and reset the `procCnt` counter. If not, the function will not execute.

In **RTOS mode**, the scheduling of `gsmProcess` calls is left to the user and his specific RTOS. The idea is that the timing issues will be determined by the user RTOS and that each invocation of *`gsmProcess` should execute at that exact moment.

To implement this, we need only set `DELAY_PROC` to 0. This will guarantee that each invocation of `gsmProcess` (handled from the user's defined RTOS tasks or proccesses) will execute. Note that we still need to call the `gsmSysTickHandler` function from the standard `SysTickHandler` function.

## Acknowledgements
This was possible thanks to the Argentine Open Industrial Computer project (Proyecto CIAA). See more about the project at http://www.github.com/ciaa and http://www.proyecto-ciaa.com.ar.
