# Binject
Binject is a simple payload injector for 32-bit and 64-bit ELF binaries.

## Features
Binject allows the user to inject an existing elf binary with a binary payload, 
which will be executed at the start when executing the binary. After which it can jump 
back the original entry of the executable.

Binject will automatically find a suitable code cave to write the payload into and edit the program headers accordingly. It won't touch the section headers so a simple `objdump` won't show the injected code.

## Using
Running Binject takes a target elf binary and a payload to inject.


The payload is just a binary file with the instructions to inject and will start execution on the first byte. Returning to the original program is as easy as adding a `ret` instruction. You can find a couple of example payloads in the [examples directory](examples/).

Once its done injecting the payload it will create a new file ending with 
`.infected` so you don't have to worry about overwriting your binaries.


Below is an example of using Binject:
```bash
nasm examples/hello_world_payload.s -o payload
./binject /bin/ls payload
    Created: ls.infected
./ls.infected
    Hello, World!
    binject  examples  ls.infected	Makefile  obj  payload	README.md  src
```

### Building
Building is as simple as running make:
```bash
git clone https://github.com/Chimplement/binject.git binject
cd binject
make
```

## Why
I made Binject because I was learning about the executable and linkable format (ELF), and wanted to put what I learned into practice. This seemed like a useful project while getting into cybersecurity.

## Disclaimer
This project was made for educational purposes.