#include <SDL2/SDL.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== DATA STRUCTURES ====================

// Tracks the state of mouse buttons
typedef struct { 
    int left, middle, right; 
} MouseState;

// Linked list node to track keyboard key state
typedef struct KeyNode {
    SDL_Keycode sdl_key;
    int pressed;
    struct KeyNode *next;
} KeyNode;

// Keyboard state as a linked list of KeyNodes
typedef struct { 
    KeyNode *head; 
} KeyboardState;

// ==================== KEYBOARD FUNCTIONS ====================

// Initialize keyboard state
void kb_init(KeyboardState *kb) { 
    kb->head = NULL; 
}

// Find a key node in the linked list
KeyNode* kb_find(KeyboardState *kb, SDL_Keycode k) { 
    KeyNode *cur = kb->head;
    while(cur) { 
        if(cur->sdl_key == k) return cur; 
        cur = cur->next; 
    }
    return NULL;
}

// Set a key state (pressed/released)
void kb_set(KeyboardState *kb, SDL_Keycode k, int pressed){
    KeyNode *node = kb_find(kb,k);
    if(node) node->pressed = pressed;
    else {
        node = malloc(sizeof(KeyNode));
        node->sdl_key = k;
        node->pressed = pressed;
        node->next = kb->head;
        kb->head = node;
    }
}

// Get a key state (pressed/released)
int kb_get(KeyboardState *kb, SDL_Keycode k){
    KeyNode *node = kb_find(kb,k);
    return node ? node->pressed : 0;
}

// Free all nodes in keyboard state
void kb_free(KeyboardState *kb){
    KeyNode *cur = kb->head;
    while(cur){ 
        KeyNode *next = cur->next; 
        free(cur); 
        cur = next; 
    }
}

// Map SDL keycode to X11 keycode
int sdlkey_to_xkey(SDL_Keycode k, Display *dpy){
    if(!dpy) return 0;
    switch(k){
        case SDLK_a: return XKeysymToKeycode(dpy,XK_a);
        case SDLK_b: return XKeysymToKeycode(dpy,XK_b);
        case SDLK_c: return XKeysymToKeycode(dpy,XK_c);
        case SDLK_d: return XKeysymToKeycode(dpy,XK_d);
        case SDLK_e: return XKeysymToKeycode(dpy,XK_e);
        case SDLK_f: return XKeysymToKeycode(dpy,XK_f);
        case SDLK_g: return XKeysymToKeycode(dpy,XK_g);
        case SDLK_h: return XKeysymToKeycode(dpy,XK_h);
        case SDLK_i: return XKeysymToKeycode(dpy,XK_i);
        case SDLK_j: return XKeysymToKeycode(dpy,XK_j);
        case SDLK_k: return XKeysymToKeycode(dpy,XK_k);
        case SDLK_l: return XKeysymToKeycode(dpy,XK_l);
        case SDLK_m: return XKeysymToKeycode(dpy,XK_m);
        case SDLK_n: return XKeysymToKeycode(dpy,XK_n);
        case SDLK_o: return XKeysymToKeycode(dpy,XK_o);
        case SDLK_p: return XKeysymToKeycode(dpy,XK_p);
        case SDLK_q: return XKeysymToKeycode(dpy,XK_q);
        case SDLK_r: return XKeysymToKeycode(dpy,XK_r);
        case SDLK_s: return XKeysymToKeycode(dpy,XK_s);
        case SDLK_t: return XKeysymToKeycode(dpy,XK_t);
        case SDLK_u: return XKeysymToKeycode(dpy,XK_u);
        case SDLK_v: return XKeysymToKeycode(dpy,XK_v);
        case SDLK_w: return XKeysymToKeycode(dpy,XK_w);
        case SDLK_x: return XKeysymToKeycode(dpy,XK_x);
        case SDLK_y: return XKeysymToKeycode(dpy,XK_y);
        case SDLK_z: return XKeysymToKeycode(dpy,XK_z);
        case SDLK_0: return XKeysymToKeycode(dpy,XK_0);
        case SDLK_1: return XKeysymToKeycode(dpy,XK_1);
        case SDLK_2: return XKeysymToKeycode(dpy,XK_2);
        case SDLK_3: return XKeysymToKeycode(dpy,XK_3);
        case SDLK_4: return XKeysymToKeycode(dpy,XK_4);
        case SDLK_5: return XKeysymToKeycode(dpy,XK_5);
        case SDLK_6: return XKeysymToKeycode(dpy,XK_6);
        case SDLK_7: return XKeysymToKeycode(dpy,XK_7);
        case SDLK_8: return XKeysymToKeycode(dpy,XK_8);
        case SDLK_9: return XKeysymToKeycode(dpy,XK_9);
        case SDLK_RETURN: return XKeysymToKeycode(dpy,XK_Return);
        case SDLK_ESCAPE: return XKeysymToKeycode(dpy,XK_Escape);
        case SDLK_BACKSPACE: return XKeysymToKeycode(dpy,XK_BackSpace);
        case SDLK_DELETE: return XKeysymToKeycode(dpy,XK_Delete);
        case SDLK_TAB: return XKeysymToKeycode(dpy,XK_Tab);
        case SDLK_SPACE: return XKeysymToKeycode(dpy,XK_space);
        case SDLK_LCTRL: return XKeysymToKeycode(dpy,XK_Control_L);
        case SDLK_RCTRL: return XKeysymToKeycode(dpy,XK_Control_R);
        case SDLK_LSHIFT: return XKeysymToKeycode(dpy,XK_Shift_L);
        case SDLK_RSHIFT: return XKeysymToKeycode(dpy,XK_Shift_R);
        case SDLK_LALT: return XKeysymToKeycode(dpy,XK_Alt_L);
        case SDLK_RALT: return XKeysymToKeycode(dpy,XK_Alt_R);
        default: return 0;
    }
}

// Handle keyboard events and send them to X11
void handle_keyboard_event(SDL_Event *e, Display *dpy, KeyboardState *kb_state){
    int xkey = sdlkey_to_xkey(e->key.keysym.sym,dpy);
    if(!xkey) return;

    if(e->type==SDL_KEYDOWN && !kb_get(kb_state,e->key.keysym.sym)){
        XTestFakeKeyEvent(dpy,xkey,True,CurrentTime);
        kb_set(kb_state,e->key.keysym.sym,1);
    } else if(e->type==SDL_KEYUP && kb_get(kb_state,e->key.keysym.sym)){
        XTestFakeKeyEvent(dpy,xkey,False,CurrentTime);
        kb_set(kb_state,e->key.keysym.sym,0);
    }
    XFlush(dpy);
}

// Print active key combinations like CTRL+SHIFT+Key
void print_active_combos(KeyboardState *kb_state){
    int ctrl = kb_get(kb_state,SDLK_LCTRL) || kb_get(kb_state,SDLK_RCTRL);
    int shift= kb_get(kb_state,SDLK_LSHIFT) || kb_get(kb_state,SDLK_RSHIFT);
    int alt  = kb_get(kb_state,SDLK_LALT) || kb_get(kb_state,SDLK_RALT);

    KeyNode *cur = kb_state->head;
    while(cur){
        if(cur->pressed &&
           cur->sdl_key!=SDLK_LCTRL && cur->sdl_key!=SDLK_RCTRL &&
           cur->sdl_key!=SDLK_LSHIFT && cur->sdl_key!=SDLK_RSHIFT &&
           cur->sdl_key!=SDLK_LALT && cur->sdl_key!=SDLK_RALT){

            if(ctrl && shift){
                printf("Combo: CTRL+SHIFT+SDL_%d\n",(int)cur->sdl_key);
            }
            if(ctrl && alt){
                printf("Combo: CTRL+ALT+SDL_%d\n",(int)cur->sdl_key);
            }
        }
        cur=cur->next;
    }
}

// ==================== MOUSE FUNCTIONS ====================

// Handle mouse motion, clicks, and wheel, sending events to X11
void handle_mouse_event(SDL_Event *e, Display *dpy, Window root,
                        int win_w,int win_h,int screen_w,int screen_h,
                        MouseState *state){
    int dx,dy;

    if(e->type==SDL_MOUSEMOTION){
        dx = e->motion.x*screen_w/win_w;
        dy = e->motion.y*screen_h/win_h;
        XWarpPointer(dpy,None,root,0,0,0,0,dx,dy);
        XFlush(dpy);
    } else if(e->type==SDL_MOUSEBUTTONDOWN || e->type==SDL_MOUSEBUTTONUP){
        dx = e->button.x*screen_w/win_w;
        dy = e->button.y*screen_h/win_h;
        int button = (e->button.button==SDL_BUTTON_LEFT?1:
                      e->button.button==SDL_BUTTON_MIDDLE?2:
                      e->button.button==SDL_BUTTON_RIGHT?3:0);
        int pressed = (e->type==SDL_MOUSEBUTTONDOWN);
        if(button==1) state->left=pressed;
        if(button==2) state->middle=pressed;
        if(button==3) state->right=pressed;
        XWarpPointer(dpy,None,root,0,0,0,0,dx,dy);
        XTestFakeButtonEvent(dpy,button,pressed,CurrentTime);
        XFlush(dpy);
    } else if(e->type==SDL_MOUSEWHEEL){
        int i;
        if(e->wheel.y>0) for(i=0;i<e->wheel.y;i++){ XTestFakeButtonEvent(dpy,4,1,CurrentTime); XTestFakeButtonEvent(dpy,4,0,CurrentTime);}
        if(e->wheel.y<0) for(i=0;i<-e->wheel.y;i++){ XTestFakeButtonEvent(dpy,5,1,CurrentTime); XTestFakeButtonEvent(dpy,5,0,CurrentTime);}
        XFlush(dpy);
    }

    // Keep cursor position updated if any button pressed
    if(state->left||state->middle||state->right){
        int x,y; SDL_GetMouseState(&x,&y);
        dx = x*screen_w/win_w; 
        dy = y*screen_h/win_h;
        XWarpPointer(dpy,None,root,0,0,0,0,dx,dy);
        XFlush(dpy);
    }
}

// ==================== MAIN ====================

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <DISPLAY>\n", argv[0]);
        return 1;
    }

    const char *display_name = argv[1];   // e.g. ":0"
           // e.g. 6300

    Display *dpy = XOpenDisplay(display_name);
    if (!dpy) {
        fprintf(stderr, "Cannot open X display %s\n", display_name);
        return 1;
    }

    Window root = DefaultRootWindow(dpy);
    int screen_w = DisplayWidth(dpy, DefaultScreen(dpy));
    int screen_h = DisplayHeight(dpy, DefaultScreen(dpy));

    // Initialize SDL2
    if(SDL_Init(SDL_INIT_VIDEO)!=0){ 
        fprintf(stderr,"SDL_Init Error: %s\n",SDL_GetError()); 
        return 1; 
    }

    // Create SDL window and renderer
    SDL_Window *win = SDL_CreateWindow("SDL2 Display :99 Capture + Mouse + Keyboard",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       screen_w/2,screen_h/2,
                                       SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

    // Initialize state
    int running=1, win_w, win_h;
    MouseState mouse_state = {0,0,0};
    KeyboardState kb_state; 
    kb_init(&kb_state);
    SDL_Event e;

    // ==================== MAIN LOOP ====================
    while(running){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT: running=0; break;

                case SDL_KEYDOWN: case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_ESCAPE && e.type==SDL_KEYDOWN) running=0;
                    handle_keyboard_event(&e, dpy, &kb_state);
                    print_active_combos(&kb_state);
                    break;

                case SDL_WINDOWEVENT:
                    if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                        win_w = e.window.data1; 
                        win_h = e.window.data2;
                    }
                    break;

                case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: case SDL_MOUSEWHEEL:
                    SDL_GetWindowSize(win,&win_w,&win_h);
                    handle_mouse_event(&e,dpy,root,win_w,win_h,screen_w,screen_h,&mouse_state);
                    break;
            }
        }

        SDL_GetWindowSize(win,&win_w,&win_h);

        // Capture screen using X11
        XImage *img = XGetImage(dpy, root, 0, 0, screen_w, screen_h, AllPlanes, ZPixmap);
        if(!img || !img->data) continue;

        // Determine pixel masks based on bits per pixel
        int bpp = img->bits_per_pixel;
        Uint32 rmask,gmask,bmask,amask;
        if(bpp==32){ rmask=0x00FF0000; gmask=0x0000FF00; bmask=0x000000FF; amask=0xFF000000; }
        else if(bpp==24){ rmask=0x00FF0000; gmask=0x0000FF00; bmask=0x000000FF; amask=0; }
        else if(bpp==16){ rmask=0xF800; gmask=0x07E0; bmask=0x001F; amask=0; }
        else{ rmask=0x00FF0000; gmask=0x0000FF00; bmask=0x000000FF; amask=0xFF000000; }

        // Convert XImage to SDL_Surface
        SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(img->data,img->width,img->height,bpp,img->bytes_per_line,
                                                     rmask,gmask,bmask,amask);
        if(!surf){ XDestroyImage(img); continue; }

        // Render surface to SDL window
        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
        if(tex){
            SDL_RenderClear(ren);
            SDL_Rect dest = {0,0,win_w,win_h};
            SDL_RenderCopy(ren, tex, NULL, &dest);
            SDL_RenderPresent(ren);
            SDL_DestroyTexture(tex);
        }

        SDL_FreeSurface(surf);
        XDestroyImage(img);

        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    kb_free(&kb_state);
    SDL_DestroyRenderer(ren); 
    SDL_DestroyWindow(win);
    SDL_Quit(); 
    XCloseDisplay(dpy);

    return 0;
}

