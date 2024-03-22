## Introduction
A simple program provides two functions: (1) pitch shifter (2) time stretcher.
## Build
```
make
```
## Usage
```
./process --help
./process --src in.wav --dest out.wav --pitch 0.8
```
<i>Note. in order to calculate a frequency: 2^(n/12).</i>
<table>
   <thead>
      <tr>
         <th colspan="3">Command line arguments</th>
      </tr>
   </thead>
   <tbody>
      <tr>
         <td>--help</td>
         <td>Display the manual that you are reading now.</td>
      </tr>
      <tr>
         <td>--src</td>
         <td>The name of the input .wav file.</td>
      </tr>
      <tr>
         <td>--dest</td>
         <td>The name of the output .wav file.</td>
      </tr>
      <tr>
         <td>--pitch</td>
         <td>Modify pitch, meanwhile keeping speed the same. The value of 2 would yield 1 octave high.</td>
      </tr>
      <tr>
         <td>--speed</td>
         <td>Modify speed, meanwhile keeping pitch the same. The value of 2 would yield the doubled length.</td>
      </tr>
      <tr>
         <td>[--size]</td>
         <td>Assign a specific grain size.</td>
      </tr>
      <tr>
         <td>[--verbose]</td>
         <td>Display the metadata of the input .wav file.</td>
      </tr>
   </tbody>
</table>