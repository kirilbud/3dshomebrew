//modified version of the sterioscopic_2d example from devikitpro example repo

#include <citro2d.h>
#include <3ds.h>

#include "blue.h"
#include "orange.h"

int main(){
    /// screen target variables
    C3D_RenderTarget* left;
    C3D_RenderTarget* right;

    C2D_SpriteSheet blue_sheet;
    C2D_Image blue;

    C2D_SpriteSheet orange_sheet;
    C2D_Image orange;

    int keysD;
    int keysH;
    float slider;

    //per-eye offsets dont think i need these but maybe
    int offsetUpper = 0;
    int offsetLower = 0;

    //init libraries
    romfsInit();
    gfxInitDefault();
    gfxSet3D(true); //3d needs to be enabled as its off by defualt
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS); //the 3ds can only handle 4096 sprites at once
    C2D_Prepare(); //sets the gpu into 2D mode
    consoleInit(GFX_BOTTOM, NULL);//print stuff to the bottom consol
    
    printf("\x1b[1;1H finally me and my wife can play\n both orange and blue for 3ds!\n");

    //make targets for both eyes on the top screen
    left = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);

    //load both images
    orange_sheet = C2D_SpriteSheetLoad("romfs:/gfx/orange.t3x");
    orange = C2D_SpriteSheetGetImage(orange_sheet, 0);

    blue_sheet = C2D_SpriteSheetLoad("romfs:/gfx/blue.t3x");
    blue = C2D_SpriteSheetGetImage(blue_sheet, 0);

    //game loop
    while (aptMainLoop()) {
        //get input (might get rid of later)
        hidScanInput();
        keysD = hidKeysDown();
        keysH = hidKeysHeld();

        if (keysD & KEY_START)
        {
            break; //exit on start key
        }

        //dpad controls
        if (keysD & KEY_DRIGHT || keysH & KEY_DUP)
        {
            offsetUpper++;
        }
        if (keysD & KEY_DLEFT || keysH & KEY_DDOWN)
        {
            offsetUpper--;
        }
        
        //button controls
        if (keysD & KEY_A || keysH & KEY_X)
        {
            offsetUpper++;
        }
        if (keysD & KEY_Y || keysH & KEY_B)
        {
            offsetUpper--;
        }
        
        slider = osGet3DSliderState();

        //print info
        printf("\x1b[5;1H %.2f | %i | %i          \n", slider, offsetUpper, offsetLower);

        //render scene

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        //render left eye

        {
            C2D_TargetClear(left, C2D_Color32(0xff, 0xff, 0xff, 0xff));
            C2D_SceneBegin(left);
            
            //i think documentation is wrong according to example
            C2D_DrawImageAt(orange, 100 + offsetUpper * slider, 0, 0);	
        }

        //render right eye
        {
            C2D_TargetClear(right, C2D_Color32(0xff, 0xff, 0xff, 0xff));
            C2D_SceneBegin(right);

            //again documentation is maybe wrong
            C2D_DrawImageAt(blue, 100 + offsetUpper * slider, 0, 0);	
        }

        C3D_FrameEnd(0); // not sure what this is
        //todo figure this out^
    }
    // free sprite sheets
    C2D_SpriteSheetFree(orange_sheet);
    C2D_SpriteSheetFree(blue_sheet);

    //de initilaize libraries
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
}