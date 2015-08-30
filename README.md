![Pic of crazy.glsl fragment shader](./mutual_julia.png?raw=true "Crazy Shader")

# mutual_julia
PoC for a mutual authentication screen saver using a Julia Set and TOTP.  

For now it just makes pretty pictures based off a TOTP token instead of 
randomness.  Eventually I hope to have an Android app that produces the same 
images at the same time.  If your phone shows the same image as *your* computer 
then you have reason to trust that it is your computer.

# Mutual Authentication?
Authentication is a process that is used to prove your identity.  When you 
log into your lock screen (you do lock your computer I hope) you are providing
a secret that proves your identity.  Since you and your computer are the only
two entities that know your secret (right?) your computer can assume it must 
be you and grant you access.  Mutual authentication is when both parties prove
their identities to each other.  Your password does not prove the computer is
your computer.  I could make a computer that looks like yours and lets you in
regardless of the password you type.

This is an Xscreensaver hack (pretty picture/animation) that is intended to
provide verification that no one has switched your computer for a malicious
one when you were not looking.

# Malicious Computer Switching... Really?
Yup... It worked for me.  

When a colleague returned from getting his coffee he found *a* laptop exactly 
where he left *his* laptop.  This laptop looked exactly like his laptop, it 
smelled like his laptop, it even sounded like his laptop.  Therefore it must be 
his laptop and thus he should definitely type in one of his most important 
passwords (QED).

The physical laptop was actually his, but had been rebooted into a malicious OS.
The OS was running entirely in RAM and did not change the disk, and it showed a 
screen saver that looked exactly like his screen saver.  When he typed in his 
password, I received it on my computer.  Then *his* computer appeared to crash, 
causing him to reboot it into his real computer.

This guy was no slouch either.  He had full disk encryption, a very complicated 
password and was blocking DMA.  Upon reboot logs would only show someone had 
hard rebooted his computer.  A time discrepancy is the only sign of something 
going wrong. However, who would have checked the logs?  Who would have 
scrutinised the times of all the log messages and realized his password was 
compromised?

Later I made his computer talk to him :)

# Will this really stop that attack?
Probably not, but it has been fun making it thus far.  I am no cryptographer but
this should make attacks much more difficult.  

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
If you don't want to install everything and just want to play with the binary:
```
cd /tmp/xscreensaver-5.33/hacks/glx/
make julia_gl
./julia_gl -shader /tmp/mutual_julia/frag_shaders/shader.glsl

```
...will cause a window to pop up with a distorted Julia set.  

Here are what the keys do:
```
    w       	print TOTP token
    r       	reload the shader
	<space> 	pause the screen
	n			new picture (enforces random() instead of TOTP)
```
If you want to install this and actually use it as a screensaver:
```
cd /tmp/xscreensaver-5.33/
make
sudo make install

```
From here, place the shader files somewhere you like.  Then run:
```
xscreensaver-demo
```
Select julia_gl and click the settings button.  From there change the shader file to 
the file of your choice and you are ready to go.
