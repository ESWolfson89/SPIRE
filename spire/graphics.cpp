#include "graphics.h"

static SDL_Window *screen = NULL;

static SDL_Renderer *renderer = NULL;

static SDL_Texture *font_bitmap = NULL;

// initialize SDL2
bool initSDL()
{
    // initialize all of SDL2's utilities/mechanisms etc...
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return false;

    // initialize SDL_Window instance screen
    screen = SDL_CreateWindow("Spire", 10, 10,
                              SCREENWID, SCREENHGT, SDL_WINDOW_SHOWN);


    if(screen == NULL)
        return false;

    // initialize the renderer
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);

    if(renderer == NULL)
        return false;

    // initialize the font bitmap texture
    font_bitmap = IMG_LoadTexture(renderer, "font16x24.png");

    if(font_bitmap == NULL)
        return false;

    //SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN);

    return true;
}

// free/destroy graphics ptr objects
void freeSDL()
{
    SDL_DestroyTexture(font_bitmap);
    font_bitmap = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(screen);
    screen = NULL;

    SDL_Quit();
}
// print a string (sval) to the screen one bitmap character at a time
// at location (x,y) with color col.
void addGFXString(color_pair col_p, std::string sval, int lx, int ly)
{
    for (unsigned int i = 0; i < sval.size(); ++i)
    {
        addGFXObject(col_p, (int)sval[i],lx + i,ly);
    }
}

// overload to print non-keyboard ascii characters
// used for title screen for now...
void addGFXAltString(color_pair col_p, std::string sval, int lx, int ly, int alt_val)
{
    for (unsigned int i = 0; i < sval.size(); ++i)
    {
        if (sval[i] == ' ')
            addGFXObject(col_p, (int)sval[i],lx + i,ly);
        else
            addGFXObject(col_p, alt_val,lx + i,ly);
    }
}

// render portion of font_bitmap to screen with color col
void addGFXObject(color_pair col_p, int aval, int lx, int ly)
{
    // boundary checking
    if (aval < 0 || aval > 255)
        return;

    SDL_Rect loc;
    SDL_Rect crop;

    loc.x = lx * TILEWID;
    loc.y = ly * TILEHGT;

    // crop out bitmap tile given aval, TILEWID, and TILEHGT
    // BITMAPROWS and BITMAPCOLS should stay at 16
    crop.x = (aval % BITMAPCOLS) * TILEWID;
    crop.y = (int)(aval / BITMAPCOLS) * TILEHGT;

    crop.w = loc.w = TILEWID;
    crop.h = loc.h = TILEHGT;

    //Render background highlight to screen (via SDL2 rectangle function)
    SDL_SetRenderDrawColor(renderer,col_p.bg.r,col_p.bg.g,col_p.bg.b,0);
    SDL_RenderFillRect(renderer,&loc);

    //Render foreground character to screen (via parsing the bitmap font)
    SDL_SetTextureColorMod(font_bitmap,col_p.fg.r,col_p.fg.g,col_p.fg.b);
    SDL_RenderCopy(renderer,font_bitmap,&crop,&loc);
}


color_type getRandomVisibleColor()
{
    color_type c = {(Uint8)(ROLL(200) + 55), (Uint8)(ROLL(200) + 55), (Uint8)(ROLL(200) + 55)};
    return c;
}

// will return one of 12 different sets of colors
color_type getPresetColor(int roller)
{
    color_type c = {0,0,0};

    switch(roller)
    {
    // red/green/purple (blue is too dark)
    case(0):
        c.r = (Uint8)ROLL(100) + 155;
        break;
    case(1):
        c.g = (Uint8)ROLL(100) + 155;
        break;
    case(2):
        c.r = c.b = (Uint8)ROLL(100) + 155;
        break;
    // yellow
    case(3):
        c.r = c.g = (Uint8)ROLL(155) + 100;
        break;
    // cyan
    case(4):
        c.g = c.b = (Uint8)ROLL(100) + 155;
        break;
    // orange/brown/etc...
    case(5):
        c.r = (Uint8)ROLL(100) + 155;
        c.g = (Uint8)(c.r / 2);
        break;
    case(6):
        c.g = (Uint8)ROLL(100) + 155;
        c.r = (Uint8)(c.g / 2);
        break;
    // green/blue blue/green variants
    case(7):
        c.g = (Uint8)ROLL(100) + 155;
        c.b = (Uint8)(c.g / 2);
        break;
    case(8):
        c.b = (Uint8)ROLL(100) + 155;
        c.g = (Uint8)(c.b / 2);
        break;
    // reddish/brown
    case(9):
        c.r = (Uint8)ROLL(100) + 155;
        c.g = (Uint8)(c.r / 3);
        break;
    // pinkish color
    case(10):
        c.r = (Uint8)ROLL(100) + 155;
        c.g = c.b = (Uint8)(c.r / 2);
        break;
    // white/gray
    case(11):
        c.r = c.g = c.b = (Uint8)ROLL(155) + 100;
        break;
    }

    return c;
}

color_type getDimmedPresetColor(color_type c, Uint8 f, Uint8 o)
{
    if (f == 0)
        return c;

    c.r = (Uint8)(c.r / f) + o;
    c.g = (Uint8)(c.g / f) + o;
    c.b = (Uint8)(c.b / f) + o;

    return c;
}

color_pair getInvertedColorpair(color_pair pair)
{
    color_pair ret_val;
    ret_val.fg = pair.bg;
    ret_val.bg = pair.fg;
    return ret_val;
}

bool isColor(color_type c1, color_type c2)
{
    return(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

// clear screen to black
void clearScreen()
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
}

// update display
void updateScreen()
{
    SDL_RenderPresent(renderer);
}
