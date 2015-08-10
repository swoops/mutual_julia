uniform vec2 res;
uniform int que[6];
uniform int ans[6];

void main() {
  vec2 pos = -1.0 + 2.0*gl_FragCoord.xy / res;
  vec3 col;
  int j, m;
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
  if (ans[0] == 9){
    // first number is 9
    if (mod(9,5) == 4.0){
      // 9 mod 5 is 4... right?
      if (int(mod(ans[0],5)) == 4){
        // 9 mod 5 is 4
        gl_FragColor = vec4(vec3(0.0), 1.0); 
        return;
      }else{
        // 9 mod 5 is not 4
        gl_FragColor = vec4(vec3(0.5), 1.0); 
        return;
      }
    }
  }


  if ( pos.x < 0.1 && pos.x > -0.1  ||
     pos.y < 0.1 && pos.y > -0.1) 
    col = vec3(res * .001 , 0.0);
  else if ( abs( pos.x ) < 0.6 && abs( pos.x ) > 0.4 )
    col = vec3(res * .105 , 0.0);
  else if ( abs( pos.y ) < 0.6 && abs( pos.y ) > 0.4 )
    col = vec3(res * .105 , 0.0);
  else
    col = vec3(0.0, 0.0, 0.0);

  gl_FragColor = vec4(col, 1.0); 
}

