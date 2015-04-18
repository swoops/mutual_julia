/* dangerball, Copyright (c) 2001-2008 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/*  TODO
 *  It would appear I can't register events for the 0-9 keys to do 
 *  catch events, not by default at least, maybe I can register my own
 *  event catching function?
*/

#define DEFAULTS	"*delay:	30000       \n" \
			"*count:        30          \n" \
			"*showFPS:      False       \n" \

# define refresh_julia 0
#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))
#define STEP 30

#include "xlockmore.h"
#include "colors.h"
#include "gltrackball.h"  /*trackball state*/
#include <ctype.h>
#include "sdr_julia_gl.h"

#ifdef USE_GL /* whole file */


#define DEF_DEBUG       "False"
#define DEF_FNAME       "shader.glsl"

#define SPIKE_FACES   12  /* how densely to render spikes */
#define SMOOTH_SPIKES True
#define SPHERE_SLICES 32  /* how densely to render spheres */
#define SPHERE_STACKS 16


typedef struct {
  GLXContext *glx_context;
  trackball_state *trackball;
  Bool button_down_p;
  Bool pause;
  Bool update;
  GLuint p;
  GLuint error;
  GLuint step;
  GLint que[6];
  GLint ans[6];
  size_t i;
} julia_configuration;

static julia_configuration *bps = NULL;

static char *shaderf;
static Bool debug;

static XrmOptionDescRec opts[] = {
  { "-shader",   ".shader", XrmoptionSepArg, 0 },
  { "-debug",   ".debug",   XrmoptionNoArg, "True" },
  { "+debug",   ".debug",   XrmoptionNoArg, "False" },
};

/* seems to list commands to change stuff, like -speed 1 makes it go fast -speed .01 slow */
static argtype vars[] = { 
  {&shaderf, "shader", "File Name", DEF_FNAME, t_String },
  {&debug,   "debug",   "Debuging on/off",   DEF_DEBUG,   t_Bool},
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
  zero6(bp->que);
  zero6(bp->ans);
  bp->i = 0;
  bp->glx_context = init_GL(mi);
  bp->trackball = gltrackball_init ();
  bp->step = time(NULL) / STEP;
  if (! bp->update) bp->update = !bp->update; /* update right away */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); /* Black and opaque */

  if ( ( bp->p = make_shader(shaderf, debug )) ) bp->error = 1;
  if (debug) printf("bp->p: %d", bp->p);

  new_totp(bp->que, bp->p, debug);
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
    new_totp(bp->que, bp->p, debug);
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
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}


void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}


int make_shader(char *fname, Bool debug){
  GLuint f,program;
  unsigned int len;
  char *shader;
  FILE *fp;

	if(!(fp = fopen(fname, "r"))) {
		fprintf(stderr, "failed to open shader: %s\n", fname);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	shader = malloc(len + 1);

	fread(shader, 1, len, fp);  /* put 1*len byptes from fp into shader */
	shader[len] = 0;  /*  null_terminate :) */

  if (debug) printf("=====Shaer Source=====\n%s\n=======================\n", shader);
  f = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(f, 1, (const char**)&shader, NULL);
  free(shader);

	glCompileShader(f);
	if (debug) printShaderInfoLog(f);

	program = glCreateProgram();
	glAttachShader(program,f);
	glLinkProgram(program);
	if (debug) printProgramInfoLog(program);
	glUseProgram(program);
  return program;
}

void set_uniformi2(GLuint prog, const char *name, int x, int y, Bool debug) {

	GLint loc = glGetUniformLocation(prog, name);

  float cord[2];
  cord[0] = (float) x;
  cord[1] = (float) y;

  if (debug)
    printf("program: %d\n%s.x: %f\n%s.y: %f\nlocation: %d\n",
      prog, name, (float)x, name, (float)y, loc);

	if(loc != -1) {
		glUniform2fv(loc, 1, cord);
	}
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
/*
static void new_totp(GLint *totp, GLuint program, Bool debug){
  size_t i;
  for(i=0; i<6; i++)
    totp[i] =  random() % 2 +1;
  distort(program, "que", totp, debug);
}
*/

static unsigned int pow10(int pow){
  size_t i;
  unsigned int ans = 10;
  if (pow <= 0) return 1;
  for (i=1; i<pow; i++)
    ans *= 10; 
  return ans;
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

static void new_totp(GLint *totp, GLuint program, Bool debug){
  int i;
  char secret[] = "\x60\x98\x09\x4c\x91\x53\x5d\x69\x04\x92"; 
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
