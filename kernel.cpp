extern "C" {
    char* read_line();
    void write_line(const char*);
    void write_string(const char*);
}

// ===================================================
// MANUAL STRING HELPERS
// ===================================================
static int strncmp(const char* s1, const char* s2, int n) {
    for (int i=0;i<n;i++){
        if (s1[i]!=s2[i]) return s1[i]-s2[i];
        if (s1[i]==0) return 0;
    }
    return 0;
}

static int strcmp(const char* s1, const char* s2){
    int i=0;
    while (s1[i] && s2[i]){
        if (s1[i]!=s2[i]) return s1[i]-s2[i];
        i++;
    }
    return s1[i]-s2[i];
}

static char* strchr(char* s, char c){
    int i=0;
    while (s[i]){
        if (s[i]==c) return &s[i];
        i++;
    }
    return 0;
}

static void strcpy(char* dst, const char* src){
    int i=0;
    while (src[i]){
        dst[i]=src[i];
        i++;
    }
    dst[i]=0;
}

// ===================================================
// RAM FILESYSTEM
// ===================================================
struct File {
    char name[32];
    char data[512];
    int size;
    bool used;
    bool isDir;
};

static File fs[64];

static void fs_init() {
    for (int i=0;i<64;i++) fs[i].used=false;
    fs[0].used=true;
    fs[0].isDir=true;
    fs[0].name[0]='/'; fs[0].name[1]=0;
}

static File* find(const char* name) {
    for (int i=0;i<64;i++)
        if (fs[i].used){
            char* n=fs[i].name;
            int j=0;
            while(n[j] && name[j] && n[j]==name[j]) j++;
            if (n[j]==0 && name[j]==0) return &fs[i];
        }
    return 0;
}

static bool create_file(const char* name,bool dir){
    if (find(name)) return false;
    for (int i=1;i<64;i++){
        if (!fs[i].used){
            fs[i].used=true;
            fs[i].isDir=dir;
            fs[i].size=0;
            int j=0; while(name[j]){ fs[i].name[j]=name[j]; j++; }
            fs[i].name[j]=0;
            return true;
        }
    }
    return false;
}

static bool write_file(const char* name,const char* text){
    File* f=find(name);
    if (!f || f->isDir) return false;
    int i=0; while(text[i] && i<511){ f->data[i]=text[i]; i++; }
    f->data[i]=0; f->size=i;
    return true;
}

static bool read_file(const char* name){
    File* f=find(name);
    if (!f || f->isDir) return false;
    write_line(f->data);
    return true;
}

static bool delete_entry(const char* name){
    File* f=find(name);
    if (!f) return false;
    f->used=false;
    return true;
}

// ===================================================
// SHELL
// ===================================================
extern "C" void kernel_main() {
    fs_init();
    write_line("microOS v1.0");
    char line[128];

    while(true){
        write_string("prompt> ");
        char* input=read_line();

        if (!strncmp(input,"makeF ",6)){
            if (create_file(input+6,false)) write_line("file created");
            else write_line("failed");
        } else if (!strncmp(input,"makeD ",6)){
            if (create_file(input+6,true)) write_line("dir created");
            else write_line("failed");
        } else if (!strncmp(input,"del ",4)){
            if (delete_entry(input+4)) write_line("deleted");
            else write_line("failed");
        } else if (!strncmp(input,"write ",6)){
            char* name=input+6;
            char* space=strchr(name,' ');
            if (!space){ write_line("syntax: write file text"); continue; }
            *space=0;
            char* text=space+1;
            if (write_file(name,text)) write_line("written");
            else write_line("failed");
        } else if (!strncmp(input,"read ",5)){
            if (!read_file(input+5)) write_line("failed");
        } else if (!strcmp(input,"help")){
            write_line("Commands:");
            write_line(" makeF NAME      - create file");
            write_line(" makeD NAME      - create directory");
            write_line(" del NAME        - delete file/dir");
            write_line(" write FILE TXT  - write text to file");
            write_line(" read FILE       - print file contents");
        } else write_line('unknown command');
    }
}
