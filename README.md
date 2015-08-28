# mutual_julia --mutate--
PoC for a mutual authentication screen saver.  This is a branch for using
the mutation of an image to indicate authentication rather then the 
duplication of the image, which is now the master branch.

# Begin Rant
When a colleague returned from coffee he found a laptop exactly where he left 
his laptop, with apparently the same operating system and even the same screen 
saver?  Must be his computer right?  So he typed his password into my computer 
and I was later able to make his computer talk to him.  There is more to the 
attack but I won't bore you.

This is an xscreensaver hack that I created to display a distorted Julia set 
fractal, typing in the correct two factor token causes the image to become 
undistorted, showing the screen saver had the same knowledge of the TOTP secret 
as you.  Why?  Because it was able to distort the image in a way that your 
your TOTP token fix.  This would prove the computer is your computer.  As long
as the distortion functions were easy to follow.

This is a proof of concept, it does not really work.  First of all, I am not a 
cryptographer and so there are likely attacks on this approach.  Second, it does
not work as an actual screen saver.  It will display the image but typing in 
your TOTP token will just bring up the normal PAM auth.  Xscreensaver works 
hard to not let any keys go to any other program, and I don't know that I want 
to risk changing that behavior just to have my fancy cryptographic mutual 
authentication screen saver.

Why make it public?  I have not put any good work into getting key presses to
go to the hack.  I think it might be possible with PAM but I am not sure.  I
am also interested in adding this to slock instead, because the code is 
simpler but at the same time I would rather use Xscreensaver.  So I am kinda
stuck and figured it would be easier to share my concept and get help if I 
could give out the code.  Currently there is a better version of this that I
use, without the distortion functions, and with fancy geometric traps.  I 
don't want to publish that just yet because my colleagues could use it to 
steal my password.

# Building and running
This assumes the Xscreensaver source is inside /tmp/xscreensaver-5.33/:
```
	cd /tmp
	git clone https://github.com/swoops/mutual_julia.git # get source
	# copy source files into OpenGL hacks directory
	cp /tmp/mutual_julia/glx_files/* /tmp/xscreensaver-5.33/hacks/glx/  
	# copy the settings XML file
	cp /tmp/mutual_julia/hack_config/* /tmp/xscreensaver-5.33/hacks/config/ 
	cd xscreensaver-5.33/hacks/config/
	./configure
```
If you don't want to install everything and just want to play with the binary
```
cd /tmp/xscreensaver-5.33/hacks/glx/
make julia_gl
./julia_gl -shader /tmp/mutual_julia/frag_shaders/shader.glsl

```
will cause a window to pop up with a distorted Julia set.  Here is what the keys do:
```
    w       print TOTP token
    a       print how much of the token you have typed
    0-9     input the TOTP token
    r       reload the shader
```
If you want to install this and actually use it as a screensaver:
```
cd /tmp/xscreensaver-5.33/
make
sudo make install

```
From here place the shader files somewhere you like.  Then run:
```
xscreensaver-demo
```
Select julia_gl and click the settings button.  From there change the shader file to 
the file of your choice and you are ready to go.

If you just want to have this as a Julia fractal screen saver and not the TOTP 
stuff then reference the shader_no_m.glsl fragment shader instead of the shader.glsl.


