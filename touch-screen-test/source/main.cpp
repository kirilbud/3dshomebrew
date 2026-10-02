//more or less copied from 3ds examples for devkitpro

#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv){
    //Initialize lcd frame buffers
    gfxInitDefault();

    //just initialize the top screen 
    consoleInit(GFX_TOP, NULL);

    printf("\x1b[1;0HPress Start to exit");
    printf("\x1b[2;0HTouch Screen pos:");

    //game loop
    while (aptMainLoop())
    {
        //grab all inputs at the start of the frame
        hidScanInput();

        //information on keys just pressed
        u32 kDown = hidKeysDown();
        
        //break if player pressed start
        if (kDown & KEY_START)
        {
            break; //exit the loop and therefor the program
        }

        touchPosition touch; // what we will use to store the touch position

        //Read touch screen cordnates and store it in touch
        hidTouchRead(&touch);

        //print the cords to the screen
        printf("\x1b[3;0H%04d; %04d", touch.px, touch.py);


        //swap frame buffers (needs to be done every frame)
        gfxFlushBuffers();
        gfxSwapBuffers();

        //wait for VBlank 
        //todo learn what a VBlank is
        gspWaitForVBlank();
    }
    

    //exit game
    gfxExit();
    return 0;
}