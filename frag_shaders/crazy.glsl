/*
*    crazy.glsl, Copyright (c) 2015 Dennis Goodlett <dennis@hurricanelabs.com>
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
uniform vec2 res;
uniform int totp[6];


void main() {
  vec2 pos = -1.0 + 2.0*gl_FragCoord.xy / res;
  vec2 temp;

  float iter = 100.0;
  float i;

  vec2 c;
  c.x = (float( totp[0]*100 + totp[2] * 10 + totp[4] ) *  .001f) * pow(-1.0f, totp[4]);
  c.y = (float( totp[1]*100 + totp[3] * 10 + totp[5] ) *  .001f)* pow(-1.0f, totp[5]);


  float trap1 = 1.0f; 
  float trap2 = 1.0f; 

  for(i=0.0; i<iter; i++) {
    temp.x = pos.x * pos.x - pos.y * pos.y;
    temp.y = pos.x * pos.y + pos.x * pos.y;

    if ( mod(totp[5], 2) == 0.0 ){
        // made mod3 and temp.x+= also
        temp.x += temp.x * pos.x - temp.y * pos.y;
       	temp.y += temp.x * pos.y + pos.x * temp.y;
    }
    if ( mod(totp[4], 2) == 0.0 ){
        // made mod3 and temp.x+= also
        temp.x = temp.x * pos.x - temp.y * pos.y;
       	temp.y = temp.x * pos.y + pos.x * temp.y;
    }
	  temp += c;

    trap1 = min( trap1, abs(dot(temp, temp)) );
    trap2 = min( trap1, abs(temp.y - temp.x) );

    if(dot(temp, temp) > 4.0){
      break;
    }

    pos = temp;
  }
    
  // change prominence of traps bassed uppon TOTP
  trap1 *= float(totp[3]) * 0.1;
  trap2 *= float(totp[2]) * 0.1;

  vec3 color = vec3(i/( iter * trap2), trap2*i, 0.1*trap1);

  if ( totp[5] == 0 )
    gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
  else if ( totp[5] == 1 )
    gl_FragColor = vec4(color.x, color.z, color.y, 1.0);
  else if ( totp[5] == 2 )
    gl_FragColor = vec4(color.y, color.x, color.z, 1.0);
  else if ( totp[5] == 3 )
    gl_FragColor = vec4(color.y, color.z, color.x, 1.0);
  else if ( totp[5] == 4 )
    gl_FragColor = vec4(color.z, color.x, color.y, 1.0);
  else if ( totp[5] == 4 )
    gl_FragColor = vec4(color.z, color.y, color.x, 1.0);
  else
    if ( pos.x >= 0.0 )
      if ( pos.y >= 0.0 )
        gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
      else
        gl_FragColor = vec4(color.x, color.z, color.y, 1.0);
    else
      if ( pos.y >= 0.0 )
        gl_FragColor = vec4(color.y, color.x, color.z, 1.0);
      else
        gl_FragColor = vec4(color.y, color.z, color.x, 1.0);
      

  // set brightness
  gl_FragColor *= vec4(vec3(0.3f),1.0f);

}
