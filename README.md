## Introduction
A simple program provides two functions: (1) pitch shifter (2) time stretcher.
## Build
```
make
```
## Usage
```
./process
./process --verbose --src in.wav --dest out.wav --mode pitch --factor 1.06 --size 2205
```
<table>
   <thead>
      <tr>
         <th colspan="3">Command line arguments</th>
      </tr>
   </thead>
   <tbody>
      <tr>
         <td>--src</td>
         <td colspan="2">The name of the input .wav file.</td>
      </tr>
      <tr>
         <td>--dest</td>
         <td colspan="2">The name of the output .wav file.</td>
      </tr>
      <tr>
         <td rowspan="2">--mode</td>
         <td>pitch</td>
         <td>Modify pitch, meanwhile keeping speed the same.</td>
      </tr>
      <tr>
         <td>speed</td>
         <td>Modify speed, meanwhile keeping pitch the same.</td>
      </tr>
      <tr>
         <td rowspan="2">--factor</td>
         <td colspan="2">In case of pitch, the value of 2 would yield 1 octave high.</td>
      </tr>
      <tr>
         <td colspan="2">In case of speed, the value of 2 would yield the doubled length.</td>
      </tr>
      <tr>
         <td>[--size]</td>
         <td colspan="2">Assign a specific grain size.</td>
      </tr>
      <tr>
         <td>[--verbose]</td>
         <td colspan="2">Show input .wav file metadata.</td>
      </tr>
   </tbody>
</table>