/* julia_gl, Copyright (c) 2014-2015 Dennis Goodlett <dennis@hurricanelabs.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */


#define DEFAULTS	"*delay:	30000       \n" \
			"*count:        30          \n" \
			"*showFPS:      False       \n" \

# define refresh_julia 0
#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))
#define STEP 30
#define SEC_SIZE 10
#define SEC_b32_SIZE 16

#include "xlockmore.h"
#include <ctype.h>
#include "julia_gl.h"
#include "julia_shader.h"

#ifdef USE_GL /* whole file */


typedef struct {
  GLXContext *glx_context;
  Bool button_down_p;
  Bool pause;
  Bool update;
  GLuint p;
  GLuint error;
  GLuint step;
  GLint que[6];
  GLint ans[6];
  char secret[SEC_SIZE];
  size_t i;
} julia_configuration;

static julia_configuration *bps = NULL;

static char *shaderf;
static char *secf;
static Bool debug;
static Bool use_random;

static XrmOptionDescRec opts[] = {
  { "-random",   ".random",   XrmoptionNoArg, "True" },
  { "-secret",   ".secret", XrmoptionSepArg, 0 },
  { "-shader",   ".shader", XrmoptionSepArg, 0 },
  { "-debug",   ".debug",   XrmoptionNoArg, "True" },
};

static argtype vars[] = { 
  {&use_random,   "random",   "TOTP() vs RANDOM()",   "False",   t_Bool},
  {&secf, "secret", "TOTP secret file", "./.julia_secret", t_String },
  {&shaderf, "shader", "File Name", "shader.glsl", t_String },
  {&debug,   "debug",   "Debuging on/off",   "False",   t_Bool},
};
ENTRYPOINT ModeSpecOpt julia_opts = {countof(opts), opts, countof(vars), vars, NULL};



/* called on window changes: size, focus */
ENTRYPOINT void
reshape_julia (ModeInfo *mi, int width, int height) {
  julia_configuration *bp = &bps[MI_SCREEN(mi)];

  set_uniformi2(bp->p, "res", width, height, debug);

  /*  Set to 2D orthographic projection with the specified clipping area */
  glMatrixMode(GL_PROJECTION);      /*  Select the Projection matrix for operation */
  glLoadIdentity();                 /*  Reset Projection matrix */
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0); /*  Set clipping area's left, right, bottom, top */
  glViewport(0,0, width, height);
}

void zero6(GLint *aq_array){
  size_t i;
  for(i=0; i < 6; i++)
    aq_array[i] = 100;
  /*
    why 100? this is in preperation for animating the correcing manipulation
    ans[i] % 10 will be the manipulation, (ans[i] % 100)/10  (tens place) will 
    be the amplitude of the manipulation.  100 will be no manipulation and a break;
  */
}


ENTRYPOINT Bool
julia_handle_event (ModeInfo *mi, XEvent *event) {
  size_t i;
  julia_configuration *bp = &bps[MI_SCREEN(mi)];
  bp->update=!bp->update;

  /* key down only please */
  if (bp->button_down_p == True){
    bp->button_down_p = False;
    return False;
  }

  bp->button_down_p = True;
  /* num pad key */
  if (event->xbutton.button >= 0x0a && event->xbutton.button <= 0x13){
        int key = event->xbutton.button - 0x09;
        if (debug) printf("key %d\n", key % 10);
        bp->ans[bp->i] = key % 10;
        distort(bp->p, "ans", bp->ans, debug);
        bp->i = ( bp->i+1 ) % 6;
        if (bp->i % 6 == 0 ){
          for (i=0;i<6;i++){
            if (bp->ans[i] != bp->que[i]) break;
            else if (i == 5) printf("===========================\n"
                        "YAY You Know that Answer!!!"
                        "\n===========================\n");
        }

  }
  }else{
    /* non num pad key */
    switch(event->xbutton.button) {
      case 0x19: /* w key, print question */
        printf("\n=================\nquestion: ");
        for(i=0;i<6;i++) printf("%x", bp->que[i]%10);
        printf("\n=================\n");
        break;
      case 0x26: /* a key, print answer */
        printf("\n=================\nanswer: ");
        for(i=0;i<6;i++) printf("%x", bp->ans[i]%10);
        printf("\n=================\n");
        break;
      case 0x1b: /* r key, reload shader */
        if ( ( bp->p = make_shader(shaderf, debug )) ) bp->error = 1;
        set_uniformi2(bp->p, "c", random()%2000 - 1000, random()%2000 - 1000, debug);
        break;
      case 0x31:              /* ` key, reset*/
        zero6(bp->ans);
        bp->i = 0;
        distort(bp->p, "ans", bp->ans, debug);
        break;
      case 0x16: /* backspace */
        if ( bp->i == 0) break;
        bp->ans[--bp->i] = 10;
        distort(bp->p, "ans", bp->ans, debug);
        break;
      case 0x39: /* n key, new fractal*/
        set_uniformi2(bp->p, "c", random()%2000 - 1000, random()%2000 - 1000, debug);
        break;
      case 0x41: /*space key, pause */
        if (debug && bp->pause) printf("playing...");
        else if (debug ) printf("pausing...");
        bp->pause = !bp->pause;
        break;
      default:
        /* if (debug) printf("key: %x\n", event->xbutton.button); */
        break;
    }
  }

  return False;
}

ENTRYPOINT void
release_julia (ModeInfo *mi) {
  if (bps) {
    free (bps);
    bps = 0;
  }
  FreeAllGL(mi);
}

ENTRYPOINT void 
init_julia (ModeInfo *mi) {
  julia_configuration *bp;

  /* if (mi->) do_fps (mi); */
  if (!bps) {
    bps = (julia_configuration *)
      calloc (MI_NUM_SCREENS(mi), sizeof (julia_configuration));
    if (!bps) {
      fprintf(stderr, "%s: out of memory\n", progname);
      exit(1);
    }
  }
	glEnable(GL_DEPTH_TEST);
  bp = &bps[MI_SCREEN(mi)];
  /* go ahead and get secret*/
  if ( ! use_random ) get_secret(bp->secret);
  zero6(bp->que);
  zero6(bp->ans);
  bp->i = 0;
  bp->glx_context = init_GL(mi);
  bp->step = time(NULL) / STEP;
  if (! bp->update) bp->update = !bp->update; /* update right away */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); /* Black and opaque */

  if ( ( bp->p = make_shader(shaderf, debug )) ) bp->error = 1;
  if (debug) printf("bp->p: %d", bp->p);

  new_totp(bp->secret, bp->que, bp->p, debug);
  set_uniformi2(bp->p, "c", random()%2000 - 1000, random()%2000 - 1000, debug);


  reshape_julia (mi, MI_WIDTH(mi), MI_HEIGHT(mi));
  /* clear_gl_error();  */
}


ENTRYPOINT void
draw_julia (ModeInfo *mi) {
  GLuint step = time(NULL) / STEP;
  julia_configuration *bp = &bps[MI_SCREEN(mi)];
	glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);   /*  Clear the color buffer with current clearing color */
  glUseProgram(bp->p);
  
  if (step > bp->step && !bp->pause){
    if (debug) printf("\nstep: %d      bp->step: %d\n", step, bp->step);
    set_uniformi2(bp->p, "c", random()%2000 - 1000, random()%2000 - 1000, debug);
    new_totp(bp->secret, bp->que, bp->p, debug);
    bp->step =  step;
  }else if ( ! bp->update ){ /* should I update? */
    return;
  }else
    bp->update = !bp->update;

  glBegin(GL_QUADS);            
    glVertex2f(1.0, 1.0); 
    glVertex2f(1.0, 1.0 * -1.0);
    glVertex2f(1.0 * -1.0, 1.0 * -1.0);
    glVertex2f(1.0 * -1.0, 1.0);
  glEnd();

  if (mi->fps_p) do_fps (mi);
  glXSwapBuffers(MI_DISPLAY (mi), MI_WINDOW(mi));
}


void distort(GLuint prog, const char *name, GLint *totp, Bool debug) {
	GLint loc = glGetUniformLocation(prog, name);
  if (debug){
    printf("program: %d location: %d\n"
          "\t%s[0] = %d\n"
          "\t%s[1] = %d\n"
          "\t%s[2] = %d\n"
          "\t%s[3] = %d\n"
          "\t%s[4] = %d\n"
          "\t%s[5] = %d\n",
        prog, loc,
        name, totp[0],
        name, totp[1],
        name, totp[2],
        name, totp[3],
        name, totp[4],
        name, totp[5]);
  }

	if(loc != -1) {
		glUniform1iv(loc, 6, totp);
	}
}

static unsigned int pow10(int pow){
  size_t i;
  unsigned int ans = 10;
  if (pow <= 0) return 1;
  for (i=1; i<pow; i++)
    ans *= 10; 
  return ans;
}

static void get_secret(char *secret){
  char base32[SEC_b32_SIZE + 1];
  FILE *fp = fopen(secf, "r");
  int ret;
  if (fp == NULL ){
    /* printf("Using random() could not open %s\n", secf); */
    use_random = 1;
    return;
  }
  fseek(fp, 0, SEEK_END);
  if ( ftell(fp) <= SEC_b32_SIZE ){
    fclose(fp);
    /* printf("File too small using random()\n"); */
    use_random = 1;
    return;
  }
  rewind(fp);
  fread(base32, 1, SEC_b32_SIZE, fp);
  base32[SEC_b32_SIZE] = 0x00;  /*double check*/
  ret = base32_convert(base32, secret);
  fclose(fp);
  if ( ret != 0 ) use_random = 1;
}

/* made for 16 char only!!! */
static int base32_convert(char *string, char *result){
  unsigned short buff = 0;        /* hold addition */
  unsigned short count = 0;      /* number of times dumped  */
  size_t i;                     /* 44loops */
  short int bitsin = 0;

  for (i=0; i<SEC_b32_SIZE; i++){
    buff <<= 5;
    if ((string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= 'a' && string[i] <= 'z')) {
      buff |= (string[i] & 0x1F) - 1 ;
    } else if (string[i] >= '2' && string[i] <= '7') {
      buff |= string[i] - 50 + 26  ;
    } else {
      return -1;
    }

    bitsin += 5;

    if (bitsin >= 8) {
      result[count++] = buff >> (bitsin - 8);
      bitsin -= 8;
    }
  }
  return 0;
}

static unsigned long byte_reverse_32(unsigned num) {
  unsigned long res = num & 0xFF;
  size_t i;
  for (i=0; i < 7; i++){
    num >>= 8; 
    res <<= 8;
    res += num & 0xFF; 
  }
  return res;
}

static void new_totp(char* secret, GLint *totp, GLuint program, Bool debug){
  int i;
  if ( use_random ){
    for(i=0; i < 6; i++)
      totp[i] =  random() % 10; /* probbaly would be better to call random once... */
    distort(program, "que", totp, debug);
    return;
  }
  unsigned long epoch_steps = byte_reverse_32((time(NULL) / 30));
  unsigned char* digest =  HMAC(EVP_sha1(), secret, 10, (unsigned char*)&epoch_steps, sizeof(epoch_steps) , NULL, NULL);    

  int short offset = digest[19] & 0xf;
  unsigned long int code = ((digest[offset] & 0x7f) << 24 |
          (digest[offset + 1] & 0xff) << 16 |
          (digest[offset + 2] & 0xff) << 8 |
          (digest[offset + 3] & 0xff)) % 1000000;

  for(i=0; i < 6; i++)
    totp[i] =  ( code % pow10(6-i) ) / pow10(5-i);
  

  distort(program, "que", totp, debug);
}


XSCREENSAVER_MODULE_2 ("Julia_GL", julia_gl, julia)



#endif /* USE_GL */
