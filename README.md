# Zephyr 1Wire application
This simple application lets you read out and copy 1Wire-based keys, for example those with iButtons. These very simple (and by today's standars insecure) keys are simply read out by the lock, and their serial is compared against an internal memory of the lock to decide if access is allowed or not.

To use this application, you need a serial terminal to interact with the application, as well as a very simple hardware setup. The 1Wire bus is driven via serial. You need to connect RX and TX using a 3k3 to 4k7 pullup resistor and connect its RX side to the inner ring of the iButton. The outer ring gets connected to GND.

## Setup
```bash
west init -m https://github.com/BeckmaR/onewire onewire
cd onewire
west update
```

## Building
From the inner onewire directory:
```bash
west build -b nrf52840dk_nrf52840 app
```
If you want to use a different board, replace it as you see fit. You will need an overlay file in `app/boards`.

## Usage
This is a shell-based application. In your serial terminal, type `help` to get a list of all commands.
- `w1 read`: Read out an attached 1Wire ROM. This should print out a 8-byte serial in hex.
- `w1 store`: Store this serial for later writing.
- `w1 write`: Write a stored serial to another key.
