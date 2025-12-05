// kernel.cpp - freestanding kernel with RAM shell and PS/2 keyboard
extern "C" {
    #include <stdint.h>
}

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// VGA text mode
volatile u16* vga = (u16*)0xB8000;
u8 cursor_x = 0, cursor_y = 0;

// Write a character to VGA
void putchar(char c) {
    if (c == '\n') { cursor_x = 0; cursor_y++; return; }
    vga[cursor_y * 80 + cursor_x] = (u16)c | (0x0F << 8); // white on black
    cursor_x++;
    if (cursor_x >= 80) { cursor_x = 0; cursor_y++; }
}

// Print string
void print(const char* s) {
    while (*s) putchar(*s++);
}

// Simple string compare
int strcmp(const char* a, const char* b) {
    while (*a && *b) { if (*a != *b) return 1; a++; b++; }
    return *a != *b;
}

// Simple string starts-with
int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 1;
        if (b[i] == 0) return 0;
    }
    return 0;
}

// ---------------- PS/2 Keyboard ----------------
#define PS2_DATA 0x60
#define PS2_STATUS 0x64

// Basic scancode -> ASCII table
const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0
};

// Read byte from I/O port
static inline u8 inb(u16 port) {
    u8 ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Polling keyboard input
char getkey() {
    u8 scancode = 0;
    while (1) {
        if (inb(PS2_STATUS) & 1) { // data available
            scancode = inb(PS2_DATA);
            break;
        }
    }
    if (scancode < 128) return scancode_to_ascii[scancode];
    return 0;
}

// ---------------- RAM Filesystem ----------------
struct FileEntry {
    const char* name;
    bool isDir;
};

FileEntry filesystem[64];
int filecount = 0;

// Commands
void makeF(const char* name) { filesystem[filecount++] = {name,false}; print("File created\n"); }
void makeD(const char* name) { filesystem[filecount++] = {name,true}; print("Directory created\n"); }
void show_files() {
    for (int i = 0; i < filecount; i++) {
        print(filesystem[i].isDir ? "[D] " : "[F] ");
        print(filesystem[i].name);
        print("\n");
    }
}

// ---------------- Shell ----------------
void shell() {
    char buffer[64];
    int pos = 0;
    print("> ");
    while (1) {
        char c = getkey();
        if (c == '\r') { // Enter
            buffer[pos] = 0;
            if (strcmp(buffer,"show")==0) show_files();
            else if (strncmp(buffer,"makeF ",6)==0) makeF(buffer+6);
            else if (strncmp(buffer,"makeD ",6)==0) makeD(buffer+6);
            else print("Unknown command\n");
            pos = 0;
            print("> ");
        } else if (c == '\b') { // Backspace
            if (pos > 0) { pos--; cursor_x--; putchar(' '); cursor_x--; }
        } else {
            buffer[pos++] = c;
            putchar(c);
        }
    }
}

// ---------------- Kernel Entry ----------------
extern "C" void kmain() {
    print("Bare-metal shell ready!\n");
    shell();
}
