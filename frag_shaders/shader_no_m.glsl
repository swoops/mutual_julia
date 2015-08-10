uniform vec2 res;
uniform vec2 c;


void main() {
  vec2 pos = -1.0 + 2.0*gl_FragCoord.xy / res;
  vec2 temp;

  float i,iter = 100.0;
  int j,m;


  for(i=0.0; i<iter; i++) {
    temp.x = (pos.x * pos.x - pos.y * pos.y) + 0.001 * c.x;
    temp.y =  ( pos.x * pos.y + pos.x * pos.y )   + 0.001 * c.y;

    if(dot(temp, temp) > 4.0) break;
    pos = temp;
  }

  gl_FragColor = vec4(0.1*i/iter, 0.3*i/iter, i/iter, 1.0); 

}
