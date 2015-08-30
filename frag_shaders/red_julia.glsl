uniform vec2 res;
uniform int totp[6];


void main() {
  vec2 pos = -1.0 + 2.0*gl_FragCoord.xy / res;
  vec2 temp;

  float i,iter = 100.0;
  int j,m;

  vec2 c;
  c.x = (float( totp[0]*100 + totp[2] * 10 + totp[4] ) *  .001f) * pow(-1.0f, totp[4]);
  c.y = (float( totp[1]*100 + totp[3] * 10 + totp[5] ) *  .001f)* pow(-1.0f, totp[5]);

  for(i=0.0; i<iter; i++) {
    temp.x = (pos.x * pos.x - pos.y * pos.y) + c.x;
    temp.y =  ( pos.x * pos.y + pos.x * pos.y ) + c.y;

    if(dot(temp, temp) > 4.0) break;
    pos = temp;
  }

  float color = i/( iter * 0.1 );
  gl_FragColor = vec4( color, 0.2 * color, 0.2 * color, 1.0); 

}
