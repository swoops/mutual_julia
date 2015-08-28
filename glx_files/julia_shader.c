#include <stdlib.h>
#include <stdio.h>
#include "julia_shader.h"

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

void set_uniformi2(GLuint prog, const char *name, int x, int y, Bool debug) {

	GLint loc = glGetUniformLocation(prog, name);

  float cord[2];
  cord[0] = (float) x;
  cord[1] = (float) y;

  /*  getting rid of some clutter
  if (debug)
    printf("program: %d\n%s.x: %f\n%s.y: %f\nlocation: %d\n",
      prog, name, (float)x, name, (float)y, loc);
  */

	if(loc != -1) {
		glUniform2fv(loc, 1, cord);
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
