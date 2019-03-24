# gsm
Library for the interfacing of GSM modems to the CIAA platform. 

Based on SIMCOM's SIM808 module and the EDU-CIAA-NXP board, though implementation remains generic in most aspects.

## General description

GSM modems traditionally work with AT commands through a serial RS-232 port. This is an old protocol, harking back to a [specific command language](https://en.wikipedia.org/wiki/Hayes_command_set) originally developed by Dennis Hayes for the Hayes Smartmodem 300 baud modem in 1981. It generally works in a command-responde fashion, although certain conditions in the modem (such as startup messages or receiving an SMS) may give rise to what is called an URC (UnRequested Code). Thus, we need to send commands (checking their echo to confirm reception), interpret the responses and also be on the lookout for URCs, which may come up at any time without any preceding command.

The function of the library is to allow the user to interact with the modem at a higher abstraction level than that of AT commands. For most of the tasks the user may want to do with the modem (send or read SMSs, open a TCP or UDP port, get GPS information, etc.), several commands need to be sent, analyzing their responses and any possible error messages. The intention is to hide this as much as possible from the user and present to him a streamlined interface.

## Abstraction layers

The library was designed with a strong emphasis on defining a number of abstraction layers and keeping a vertical-communication scheme as much as possible. That is, layers should ideally communicate only with the layer above and below it.

   +--------------+
   |              |
   | gsmInterface |
   |              |
   +--------------+
         \|
         \|
    +-----------+
    |           |
    | gsmEngine |
    |           |
    +-----------+

## Software structure

## Usage

## Acknowledgements
This was possible thanks to the Argentine Open Industrial Computer project (Proyecto CIAA). See more about the project at http://www.github.com/ciaa and http://www.proyecto-ciaa.com.ar.
