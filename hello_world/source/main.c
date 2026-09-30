//basically stolen from the devkit pro 3ds examples page but with my own comments/modifications

#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv){
    //Initialize LCD frame buffers whatever that means
    gfxInitDefault();

    //initialize the top screen
    consoleInit(GFX_TOP,NULL);
    

    printf("\x1b[16;12HComputer make 3ds homebrew");
    //ok so \x1b is a thing called a Format Specifier where it specifies the type of data that should be displayed
    //in this case \x1b is formated with "\x1b[r:cH" with r is rows and c collums
    //this format specifier moves the cursor to that position
    //top screen has 30 rows and 50 colums
    //bottom has 30 rows and 40 colums

    printf("\x1b[30;16HPress Start to exit.");

    //main game loop called every frame
    //returns false if the game should not be running
    while (aptMainLoop())
    {
        hidScanInput();//scan all inputs

        u32 kDown = hidKeysDown();
        //get the newly pressed keys as an unsighned integer
        //just use an bitwise and function to check if a key is pressed :D

        //like so
        if (kDown & KEY_START) break; //exit game back to homebrew menu

        // flush and swap frame buffers whatever that means
        gfxFlushBuffers();
        gfxSwapBuffers();

        //wait for VBlank unsure what this means
        gspWaitForVBlank();
    }
    
    //free the frame buffers
    gfxExit();
    return 0;
}