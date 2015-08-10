uniform vec2 res;
uniform vec2 c;
uniform int que[6];
uniform int ans[6];


void main() {
  vec2 pos = -1.0 + 2.0*gl_FragCoord.xy / res;
  vec2 temp;

  float i,iter = 100.0;
  int j,m;

  // question section
  for (j=5; j>=0; j--){
    m = int(mod(que[j], 5)); // only 5 maniuplative function (duplicate too), once there are 10 this won't be needed
  
    if ( m == 1)
      pos.y -= sin(pos.x*3.14*3.0)*0.1;
    else if ( m == 2)
      pos.x -= sin(pos.y*3.14*3.0)*0.1;
    else if ( m == 3)
      pos.y -= tan(pos.x*0.5*3.14)*0.1;
    else if ( m == 4)
      pos.x -= tan(pos.y*0.5*3.14)*0.1;
    else if ( m == 0){
      if ( mod(pos.y, 0.5) >= 0.25)
        pos.x -= 0.1;
      else
        pos.x += 0.1;
      if ( mod(pos.x, 0.5) >= 0.25)
        pos.y -= 0.1;
      else
        pos.y += 0.1;
    }
  }
  // answer section
  for (j=0; j<6; j++){
    if (ans[j] == 100) break;
    m = int(mod(ans[j], 5.0));

    if ( m == 1)
      pos.y += sin(pos.x*3.14*3.0)*0.1;
    else if ( m == 2)
      pos.x += sin(pos.y*3.14*3.0)*0.1;
    else if ( m == 3)
      pos.y += tan(pos.x*0.5*3.14)*0.1;
    else if ( m == 4)
      pos.x += tan(pos.y*0.5*3.14)*0.1;
    else if ( m == 0){
      if ( mod(pos.x, 0.5) >= 0.25)
        pos.y += 0.1;
      else
        pos.y -= 0.1;
      if ( mod(pos.y, 0.5) >= 0.25)
        pos.x += 0.1;
      else
        pos.x -= 0.1;
    }
  }

  for(i=0.0; i<iter; i++) {
    temp.x = (pos.x * pos.x - pos.y * pos.y) + 0.001 * c.x;
    temp.y =  ( pos.x * pos.y + pos.x * pos.y )   + 0.001 * c.y;

    if(dot(temp, temp) > 4.0) break;
    pos = temp;
  }

  gl_FragColor = vec4(0.1*i/iter, 0.3*i/iter, i/iter, 1.0); 

}
