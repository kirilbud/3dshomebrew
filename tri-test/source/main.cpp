// yet again steeling this from the example repo of devkit pro this one is simple_tri

#include <3ds.h>
#include <citro3d.h>
#include <string.h>
#include "vshader_shbin.h"

#define CLEAR_COLOR 0x68B0D8FF // background color I think
//should be a nice blue

#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))
//TODO figure out like all of this^

typedef struct { float x, y, z;} vertex;
// how we will define a vertex

//3ds top screen resolution 400 x 240
static const vertex vertex_list[] = 
{
    { 200.0f, 200.0f , 0.5f }, //bottom right
    { 100.0f, 40.0f, 0.5f }, //right up i think
    { 300.0f, 40.0f, 0.5f }, //left up i think
};

#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))
//TODO figure out how to do something like this for multiple models

static DVLB_s* vshader_dvlb; //shader binarry
static shaderProgram_s program; //this is the compiled shader program
static int uLoc_projection; //coation of the projection matrix in the shader
static C3D_Mtx projection; //just a 4x4 matrix


static void* vbo_data;

static void sceneInit(void){
    //parse the shader binary
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
    //initialize the shader program
    shaderProgramInit(&program);
    //set vertex shader
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    //TODO find documentation for citro 2d/3d
    C3D_BindProgram(&program);

    //getting memory location of uniforms
    uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");

    // configure attributes for use with the vertex shader
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // set register 0 to be position
    AttrInfo_AddFixed(attrInfo, 1); // register 1 will be the color

    //set the fixed addribute in register 1 to a solid white
    C3D_FixedAttribSet(1, 1.0, 1.0, 1.0, 1.0);

    //compute the projection matrix
    Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);

    //make the Vertex buffer object
    vbo_data = linearAlloc(sizeof(vertex_list));
    memcpy(vbo_data, vertex_list, sizeof(vertex_list));

    //configure buffers
    //TODO figure out what info its getting
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 1, 0x0);

    //configure first fragment shading substage(no idea) to just pass through the vertex color
    // see https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight apperently
    //TODO read into this more^ as well as bellow
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, static_cast<GPU_TEVSRC>(0) , static_cast<GPU_TEVSRC>(0));
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

static void sceneRender(void){
    //update uniformes passing in the uniform memory location as well as the projection matrix
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

    //draw vertex buffer object in this case the triangel
    C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);

}

//free all alocated memory to avoid mem leaks
static void sceneExit(void){
    //free vertext buffer object
    linearFree(vbo_data);
    
    // free the shader program
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
}

int main(){
    //init the graphics
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    //initialize the render target
    C3D_RenderTarget* target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    //init scene and alocate the memory needed
    sceneInit();

    while (aptMainLoop())
    {
        //scan for input
        hidScanInput();

        //get user unput and exit if start is pressed
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
        {
            break;
        }
        
        //render scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(target); // <---- right here is where I think we could do 3d
        sceneRender();
        C3D_FrameEnd(0);
    }
    

    //free initialized scene/memory
    sceneExit();

    //Deinitialize graphics
    C3D_Fini();
    gfxExit();
    return 0;
}