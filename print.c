#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

extern char *LOGO_FILENAME;

void print_progress(size_t count, size_t max, char *message) {
    printf("\x1b[2K"); // Clear entire line
    printf("\x1b[1F"); // Move to beginning of previous line
    printf("\x1b[2K"); // Clear entire line
    const int bar_width = 100;
    float progress = (float) count / max;
    int bar_length = progress * bar_width;
    printf("Progress: [");
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%\n", progress * 100);
    printf(" ** %s ** ", message);
    //printf("\x1b[1E"); // Move to beginning of next line

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
