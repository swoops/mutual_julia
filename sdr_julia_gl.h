#include <GL/gl.h>
#include <openssl/hmac.h>


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

void zero6(GLint *totp);
void printProgramInfoLog(GLuint obj);
void printShaderInfoLog(GLuint obj);
int make_shader(char *fname, Bool debug);
void set_uniformi2(GLuint prog, const char *name, int x, int y, Bool debug);
void distort(GLuint prog, const char *name, GLint *totp, Bool debug);
static unsigned int pow10(int pow);
static unsigned long byte_reverse_32(unsigned num);
static void new_totp(GLint *totp, GLuint program, Bool debug);
