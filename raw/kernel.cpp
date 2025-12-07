// kernel.cpp - freestanding kernel with screen abstraction + RAM shell + PS/2 keyboard
extern "C" {
    #include <stdint.h>
}

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// ================================================================
//  DISPLAY DRIVER SYSTEM
// ================================================================

struct DisplayDriver {
    void (*putchar)(char);
    void (*clear)();
    int width;
    int height;
};

DisplayDriver* display;   // global active display backend


// ================================================================
//  VGA TEXT MODE DRIVER
// ================================================================

volatile u16* vga_mem = (u16*)0xB8000;
u8 vga_x = 0, vga_y = 0;

void vga_putchar(char c) {
    if (c == '\n') { vga_x = 0; vga_y++; return; }

    vga_mem[vga_y * 80 + vga_x] = (u16)c | (0x0F << 8); // white on black
    vga_x++;

    if (vga_x >= 80) {
        vga_x = 0;
        vga_y++;
    }
}

void vga_clear() {
    for (int i = 0; i < 80 * 25; i++)
        vga_mem[i] = (u16)' ' | (0x0F << 8);
    vga_x = vga_y = 0;
}

DisplayDriver VGA_DRIVER = {
    vga_putchar,
    vga_clear,
    80,
    25
};


// ================================================================
//  PLACEHOLDER FRAMEBUFFER DRIVER (expands later)
// ================================================================

void fb_putchar(char c) {
    // No framebuffer font implemented yet.
    // This is intentionally empty so the kernel doesn't crash.
}

void fb_clear() {
    // No real framebuffer address or resolution yet.
}

DisplayDriver FRAMEBUFFER_DRIVER = {
    fb_putchar,
    fb_clear,
    1024,   // placeholder
    768     // placeholder
};


// ================================================================
//  GENERIC KERNEL PRINT USING SELECTED DISPLAY DRIVER
// ================================================================

void putchar(char c) {
    display->putchar(c);
}

void print(const char* s) {
    while (*s)
        display->putchar(*s++);
}


// ================================================================
//  STRING HELPERS
// ================================================================

int strcmp(const char* a, const char* b) {
    while (*a && *b) { if (*a != *b) return 1; a++; b++; }
    return *a != *b;
}

int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 1;
        if (b[i] == 0) return 0;
    }
    return 0;
}


// ================================================================
//  PS/2 KEYBOARD INPUT
// ================================================================

#define PS2_DATA 0x60
#define PS2_STATUS 0x64

const char scancode_to_ascii[128] = {
    0, 27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,
    '\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0
};

static inline u8 inb(u16 port) {
    u8 ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

char getkey() {
    u8 sc = 0;
    while (1) {
        if (inb(PS2_STATUS) & 1) {
            sc = inb(PS2_DATA);
            break;
        }
    }
    if (sc < 128) return scancode_to_ascii[sc];
    return 0;
}


// ================================================================
//  RAM FILESYSTEM
// ================================================================

struct FileEntry {
    const char* name;
    bool isDir;
};

FileEntry filesystem[64];
int filecount = 0;

void makeF(const char* name) {
    filesystem[filecount++] = {name,false};
    print("File created\n");
}

void makeD(const char* name) {
    filesystem[filecount++] = {name,true};
    print("Directory created\n");
}

void show_files() {
    for (int i = 0; i < filecount; i++) {
        print(filesystem[i].isDir ? "[D] " : "[F] ");
        print(filesystem[i].name);
        print("\n");
    }
}


// ================================================================
//  SHELL
// ================================================================

void shell() {
    char buffer[64];
    int pos = 0;

    print("> ");

    while (1) {
        char c = getkey();
        if (c == '\r') {
            buffer[pos] = 0;

            if (strcmp(buffer, "show") == 0) show_files();
            else if (strncmp(buffer,"makeF ",6) == 0) makeF(buffer+6);
            else if (strncmp(buffer,"makeD ",6) == 0) makeD(buffer+6);
            else print("Unknown command\n");

            pos = 0;
            print("> ");
        }
        else if (c == '\b') {
            if (pos > 0) {
                pos--;
                // simple backspace fix
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        }
        else {
            buffer[pos++] = c;
            putchar(c);
        }
    }
}


// ================================================================
//  KERNEL ENTRY
// ================================================================

extern "C" void kmain() {
    // Default to VGA backend until you add VESA detection
    display = &VGA_DRIVER;

    display->clear();

    print("go:\n");
    shell();
}
