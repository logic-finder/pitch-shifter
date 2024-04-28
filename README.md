## Introduction
A simple program provides two functions: (1) pitch shifter (2) time stretcher. (The quality of the result is not that good, I think.)
## Build
```
make
```
## Usage
```c
./process --help
./process --src in.wav --dest out.wav --pitch 0.84

/* -S is the abbreviation of --src;
   -D = --dest, -P = --pitch, -T = --speed */

./process -S in.wav -D out.wav -P 0.84
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
         <td>Display the manual that you are reading now.</td>
      </tr>
      <tr>
         <td>--src or -S</td>
         <td>The name of the input .wav file. <b>Required</b> to run.</td>
      </tr>
      <tr>
         <td>--dest or -D</td>
         <td>The name of the output .wav file. <b>Required</b> to run.</td>
      </tr>
      <tr>
         <td>--pitch or -P</td>
         <td>Modify pitch, meanwhile keeping speed the same. The value of 2 would yield 1 octave high. <b>Range: 0 ~ 3 (inclusive)</b>. <b>Required, but</b> can't be set with --speed together.</td>
      </tr>
      <tr>
         <td>--speed or -T</td>
         <td>Modify speed, meanwhile keeping pitch the same. The value of 2 would yield the doubled length. <b>Range: 0 ~ 3 (inclusive)</b>. <b>Required, but</b> can't be set with --pitch together.</td>
      </tr>
      <tr>
         <td>[--size]</td>
         <td>Assign a specific grain size: 2205 ~ 8820 (inclusive).</td>
      </tr>
      <tr>
         <td>[--verbose]</td>
         <td>Display the metadata of the input .wav file.</td>
      </tr>
      <tr>
         <td>--src* / -S*<br>--dest* / -D*</td>
         <td>Please refer to the below section for these asterisk commands.<br>(Environment variable suppression commands)</td>
      </tr>
   </tbody>
</table>

### About .env File
I found it inconvenient that I had to type the paths to .wav files all the time. From this reason, I've had the program read .env file where the pre-defined --src and --dest paths are written. Meanwhile, it would be helpful to use '*' character if it is desired to provide a full path manually.
```c
/* Suppose that .wav files are in ./src directory, but I do not want to type 'src/' every time in front of .wav files. */

/* without .env file */
./process --src src/music.wav --dest dest/result.wav --pitch 1.06

/* with .env file written as follows */
SRC_PATH    src/
DEST_PATH   dest/

./process --src music.wav --dest result.wav --pitch 1.06

/* If I want to search from a different directory, I can put the asterisk right after --src (= -S) command so that I can ignore the SRC_PATH. */
./process --src* other_dir/music.wav --dest result.wav --pitch 1.06

/* this 'suppression' can be applied to --dest (= -D) also. */
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
         <td>Lines starting with # character are comments and therefore will be ignored.</td>
      </tr>
      <tr>
         <td>SRC_PATH</td>
         <td>The path from where the program will look for input .wav files.</td>
      </tr>
      <tr>
         <td>DEST_PATH</td>
         <td>The path to where the program will save the result.</td>
      </tr>
      
   </tbody>
</table>