#include <GL/gl.h>
#include <openssl/hmac.h>


void zero6(GLint *totp);
void distort(GLuint prog, const char *name, GLint *totp, Bool debug);
static unsigned int pow10(int pow);
static unsigned long byte_reverse_32(unsigned num);
static void new_totp(GLint *totp, GLuint program, Bool debug);
