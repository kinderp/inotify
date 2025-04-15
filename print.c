#include<stdio.h>
#include <unistd.h>

extern char *LOGO_FILENAME;

void print_progress(size_t count, size_t max, char *message) {
    const int bar_width = 50;

    float progress = (float) count / max;
    int bar_length = progress * bar_width;

    printf("\rProgress: [");
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%", progress * 100);
    printf("\t ** %s **", message);

    fflush(stdout);
}

void print_logo(){
        FILE *LOGO;
        char ch;
        LOGO = fopen(LOGO_FILENAME, "r");
        while( (ch = getc(LOGO) ) != EOF){
                printf("%c",ch);
                sleep(0.8);
        }
        fclose(LOGO);
}
