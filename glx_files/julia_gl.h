#include <GL/gl.h>
#include <openssl/hmac.h>


void zero6(GLint *totp);
void distort(GLuint prog, const char *name, GLint *totp, Bool debug);
static unsigned int pow10(int pow);
static unsigned long byte_reverse_32(unsigned num);
static void new_totp(char* secret, GLint *totp, GLuint program);
/*
* this function will pass the first 16 characters of the secret file to
* a base32 decoding function and retrieve 10 raw bytes to be used as the
* TOTP secret
* If the file has less then 16 bytes or does not contain valid base32 characters
* in the first 16 bytes random() will be used instead of TOTP()
*/
static void get_secret(char *sec);
static int base32_convert(char *string, char *result);
