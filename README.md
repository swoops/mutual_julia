# mutual_julia
PoC for a mutual authentication screen saver.  The code is ugly, I am just 
trying to get something that works for now.

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
Get the source for Xscreensaver and run: 
```
    ./configure 
```
and copy these files (except the README) to the hacks/glx directory.  Then run 
```
    make julia_gl
```
Running the program in windowed mode:
```
    ./julia_gl
```
will cause a window to pop up with a distorted Julia set.  Here is what the keys do:
```
    w       print TOTP token
    a       print how much of the token you have typed
    0-9     input the TOTP token
    r       reload the shader
```
You can also reference the executable in your ~/.xscreensaver file and use it as a 
screensaver but it will not accept any key presses.  If you do that make sure you
reference the shader file properly with the -shader flag.  

If you just want to have this as a Julia fractal screen saver and not the TOTP 
stuff then reference the shader_no_m.glsl file with the -shader flag.


