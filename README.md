# microOS

microOS is a minimalist, educational operating‑system project. It’s designed from the ground up for learning — meant to be simple, transparent, and easy to extend. If you’re curious about OS internals, kernel development, or want a sandbox project to build features from scratch, microOS is for you.

## Why microOS?

- To experiment with OS design, boot process, and low‑level programming.  
- To understand how kernels, file systems, and basic shells work — without the bloat of a full operating system.  
- To provide a teaching / learning tool or base for hobby OS development.

## What microOS includes / supports

- Kernel (basic)  
- Bootable ISO image (see `microOS.iso`)  
- Basic file-system operations  
- Simple shell / command-line interface  
- GPL‑3.0 license (free / open source) :contentReference[oaicite:1]{index=1}

## Getting Started

### Prerequisites

- An x86 or x86_64 PC (or emulator)  
- Tools to build or test the ISO (e.g. `dd`, `qemu`, `grub-mkrescue`, or physical boot media like a flash drive)  
- Familiarity with low-level programming, or willingness to learn

### Running microOS

1. Download or build the ISO — current repo includes microOS.iso and the raw source code
2. Run in a virtual machine / emulator (for testing) — e.g.:

    ```sh
    qemu-system-x86_64 -cdrom microOS.iso
    ```

3. Boot and try out the shell / built‑in commands.
  
### Building from source  
1. To build from source, you can clone this repository and compile the kernel (raw/kernel.cpp) and link with ld using raw/linker.ld as the file to ld to follow.
2. Next, you create the following file structure:
    iso
     | - kernel.bin (compiled kernel)
     | - grub
     |    | - grub.cfg (from raw/grub.cfg)
     | - Any other files/programs you want in your build
3. You now use grub-mkrescue to compile the folder iso/ into a bootable iso
4. You are done
   
## License

microOS is licensed under the GNU GPL‑3.0. :contentReference[oaicite:3]{index=3}

---

## (Optional) Todo / Roadmap

- [ ] Document build-from-source instructions  
- [ ] Add basic I/O drivers or utilities  
- [ ] Add tutorial or comments explaining kernel code  
- [ ] Add automated testing (emulator boot + basic command test)  
- [ ] Improve shell, support for more commands  

---

**microOS** — barebones, open‑source OS sandbox. Build it, learn it, break it, improve it.
