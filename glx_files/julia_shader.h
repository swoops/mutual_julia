#include <GL/gl.h>
#define Bool int

extern void glGetShaderiv(GLuint, GLenum, GLint *); 
extern void glGetShaderInfoLog(GLuint,  GLsizei,  GLsizei *,  GLchar *); 
extern void glGetProgramiv(GLuint,  GLenum,  GLint *);
extern void glGetProgramInfoLog(GLuint,  GLsizei,  GLsizei *,  GLchar *);
extern GLuint glCreateShader(GLenum);
extern void glShaderSource( GLuint, GLsizei, const GLchar **, const GLint *);
extern void glCompileShader(GLuint);
extern GLuint glCreateProgram(void); 
extern void glAttachShader(GLuint, GLuint);
extern void glLinkProgram(GLuint);
extern void glUseProgram(GLuint); 
extern void glUniform1iv(GLint, GLsizei, const GLint *);
extern GLint glGetUniformLocation(GLuint, const GLchar *);
extern void glUniform2fv( GLint, GLsizei, const GLfloat *);

int make_shader(char *fname, Bool debug);
void printShaderInfoLog(GLuint obj);
void set_uniformi2(GLuint prog, const char *name, int x, int y, Bool debug);
void printProgramInfoLog(GLuint obj);
