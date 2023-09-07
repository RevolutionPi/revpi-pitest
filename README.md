<details>
<summary>We have moved to GitLab! Read this for more information.</summary>

We have recently moved our repositories to GitLab. You can find revpi-pitest
here: https://gitlab.com/revolutionpi/revpi-pitest  
All repositories on GitHub will stay up-to-date by being synchronised from
GitLab.

We still maintain a presence on GitHub but our work happens over at GitLab. If
you want to contribute to any of our projects we would prefer this contribution
to happen on GitLab, but we also still accept contributions on GitHub if you
prefer that.
</details>

<!--
SPDX-FileCopyrightText: 2023 KUNBUS GmbH

SPDX-License-Identifier: MIT
-->

# piTest

```
Usage: piTest [OPTION]
- Shows infos from RevPiCore control process
- Reads values of RevPiCore process image
- Writes values to RevPiCore process image

Options:
                 -d: Get device list.

      -v <var_name>: Shows infos for a variable.

                 -1: execute the following read only once.

                 -q: execute the following read quietly, print only the value.

-r <var_name>[,<f>]: Reads value of a variable.
                     <f> defines the format: h for hex, d for decimal (default), b for binary
                     E.g.: -r Input_001,h
                     Read value from variable 'Input_001'.
                     Shows values cyclically every second.
                     Break with Ctrl-C.

   -r <o>,<l>[,<f>]: Reads <l> bytes at offset <o>.
                     <f> defines the format: h for hex, d for decimal (default), b for binary
                     E.g.: -r 1188,16
                     Read 16 bytes at offset 1188.
                     Shows values cyclically every second.
                     Break with Ctrl-C.

  -w <var_name>,<v>: Writes value <v> to variable.
                     E.g.: -w Output_001,23:
                     Write value 23 dez (=17 hex) to variable 'Output_001'.

     -w <o>,<l>,<v>: Writes <l> bytes with value <v> (as hex) to offset <o>.
                     length should be one of 1|2|4.
                     E.g.: -w 0,4,31224205:
                     Write value 31224205 hex (=824328709 dez) to offset 0.

         -g <o>,<b>: Gets bit number <b> (0-7) from byte at offset <o>.
                     E.g.: -b 0,5:
                     Get bit 5 from byte at offset 0.

   -s <o>,<b>,<0|1>: Sets 0|1 to bit <b> (0-7) of byte at offset <o>.
                     E.g.: -b 0,5,1:
                     Set bit 5 to 1 of byte at offset 0.

     -R <addr>,<bs>: Reset counters/encoders in a digital input module like DIO or DI.
                     <addr> is the address of module as displayed with option -d.
                     <bs> is a bitset. If the counter on input pin n must be reset,
                     the n-th bit must be set to 1.
                     E.g.: -R 32,0x0014:
                     Reset the counters on input pin I_3 and I_5.

                 -S: Stop/Restart I/O update.

                 -x: Reset piControl process.

                 -l: Wait for reset of piControl process.

                 -f: Update firmware. (see tutorials on website for more info) 

  -c <addr>,<c>,<m>,<x>,<y>: Do the calibration. (see tutorials on website for more info)
                     <addr> is the address of module as displayed with option -d.
                     <c> is the bitmap of channels
                     <m> is the mode
                     <x> is the check point on x axix
                     <y> is the check point on y axis
```
