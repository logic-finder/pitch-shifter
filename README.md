## Introduction
`pitsh` is a simple program provides two functions: (1) pitch shifter (2) time stretcher. Meanwhile, this program is developed by a beginner and the results which this program produce are somewhat unsatisfactory in that there are some perceivable noises in those.
## Build
Executing `make` command in the root directory will produce `pitsh`, the executable. Meanwhile, one may find it helpful to type `make help` to find the effect of `make clean` command.
## Usage
```c
./pitsh --help

./pitsh --src in.wav --dest out.wav --pitch 0.84
./pitsh  -S   in.wav  -D    out.wav  -P 0.84   // abbreviated

./pitsh ... --speed 1.2
./pitsh ...  -T     1.2   // abbreviated
```
<i>Note. It would be helpful to use the following formula to get values for --pitch command: 2^(n/12).<br>Example: 3 half tones down = 2^(-3/12) = 0.84</i>
<table>
   <thead>
      <tr>
         <th colspan="3">Command Line Arguments</th>
      </tr>
   </thead>
   <tbody>
      <tr>
         <td>--help</td>
         <td>displays the manual that you are reading now.</td>
      </tr>
      <tr>
         <td>--src <em>or</em> -S</td>
         <td>specifies the name of the input .wav file. <b>Required</b> to run.</td>
      </tr>
      <tr>
         <td>--dest <em>or</em> -D</td>
         <td>specifies the name of the output .wav file. <b>Required</b> to run.</td>
      </tr>
      <tr>
         <td>--pitch <em>or</em> -P</td>
         <td>modifies pitch, meanwhile keeping speed the same. The value of 2 would yield 1 octave high. <b>Range: 0 ~ 3 (inclusive)</b>. <b>Required, but</b> can't be set with --speed together.</td>
      </tr>
      <tr>
         <td>--speed <em>or</em> -T</td>
         <td>modifies speed, meanwhile keeping pitch the same. The value of 2 would yield the doubled length. <b>Range: 0 ~ 3 (inclusive)</b>. <b>Required, but</b> can't be set with --pitch together.</td>
      </tr>
      <tr>
         <td>[--size]</td>
         <td>assigns a specific grain size: 2205 ~ 8820 (inclusive). Optional.</td>
      </tr>
      <tr>
         <td>[--verbose]</td>
         <td>displays the metadata of the input .wav file. Optional.</td>
      </tr>
      <tr>
         <td>--src* / -S*<br>--dest* / -D*</td>
         <td>These are what I call <em>environment variable suppression commands</em>.<br>Please refer to the below section for the detailed description.</td>
      </tr>
   </tbody>
</table>

### About `.env` File
I found it inconvenient that I had to type the paths to .wav files all the time. From this reason, I've had the program read .env file where the pre-defined --src and --dest paths are written. Meanwhile, it would be helpful to use the `*` character if it is desired to provide a full path manually.
```c
/* Suppose that .wav files are in ./src directory,
   but I do not want to type 'src/' every time in
   front of the names of the .wav files. */

/* Without the .env file, */
./pitsh --src src/music.wav --dest dest/result.wav --pitch 1.06

/* With .env file written as follows, */
SRC_PATH    src/
DEST_PATH   dest/

./pitsh --src music.wav --dest result.wav --pitch 1.06

/* If I want to search from a different directory,
   I can put the asterisk right after --src (= -S)
   command so that I can ignore the SRC_PATH. */
./pitsh --src* other_dir/music.wav --dest result.wav --pitch 1.06

/* This 'suppression' can be applied to --dest (= -D) also. */
```
<table>
   <thead>
      <tr>
         <th colspan="3">.env File Structure</th>
      </tr>
   </thead>
   <tbody>
      <tr>
         <td>#</td>
         <td>represents a comment line, if and only if the line starts with the '#' character. Comment lines are ignored.</td>
      </tr>
      <tr>
         <td>SRC_PATH</td>
         <td>represents the path from which the program will look for input .wav files.</td>
      </tr>
      <tr>
         <td>DEST_PATH</td>
         <td>represents the path to which the program will save the result.</td>
      </tr>
   </tbody>
</table>