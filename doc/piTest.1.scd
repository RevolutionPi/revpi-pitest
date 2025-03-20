piTest(1)

# NAME

piTest - interact with in-kernel piControl process

# SYNOPSIS

*piTest* *-d*++
*piTest* *-v* _variablename_++
*piTest* [*-1q*] *-r* _variablename_[,_f_]++
*piTest* [*-1q*] *-r* _o_,_l_[,_f_]++
*piTest* *-w* _variablename_,_v_++
*piTest* *-w* _o_,_l_,_v_++
*piTest* *-g* _o_,_b_++
*piTest* *-s* _o_,_b_,(*0*|*1*)++
*piTest* *-R* _a_,_b_++
*piTest* *-C* _address_++
*piTest* *--module* _address_ [*--force*] *-f*++
*piTest* *-l*++
*piTest* *-S*++
*piTest* *-x*

# DESCRIPTION

*piTest* allows listing the I/O and gateway modules attached to a RevPi Core or
Connect, reading and writing values in the process image and resetting the
in-kernel piControl process.

Revolution Pi is a set of IoT products based on the Raspberry Pi and geared
towards industrial usage.

# OPTIONS

Multiple of these options may be given to execute a sequence of actions:

*-d*
	Get device list.

*-v* _variablename_
	Shows infos for a variable.

*-V*
	Shows the version info for this program.

*-1*
	Executes the following read command only once.

*-q*
	Executes the following read command quietly, print the value only. Can also
	be used to suppress the spinner output from a firmware update.

*-r* _variablename_[,_f_]
	Reads the value of a variable. It respects the length of variable as
	defined in PiCtory. The optional parameter _f_ defines the format: h for
	hex, d for decimal (default) and b for binary. The value is displayed
	cyclically every second until Ctrl-C is pressed.

*-r* _o_,_l_[,_f_]
	Reads _l_ bytes at offset _o_. The optional parameter _f_ defines the
	format: h for hext, d for decimal (default) and b for binary. The
	value is displayed cyclically every second until Ctrl-C is pressed.

*-w* _variablename_,_v_
	Writes value _v_ to the variable _variablename_. It respects the length of
	variable as defined in PiCtory.

*-w* _o_,_l_,_v_
	Write _l_ bytes with value _v_ (as dec) to offset _o_. Length should be one
	of 1|2|4.

*-g* _o_,_b_
	Gets bit _b_ (0-7) from byte at offset _o_. The value is displayed
	cyclically every second until Ctrl-C is pressed.

*-s* _o_,_b_,_0/1_
	Sets bit _b_ (0-7) of byte at offset _o_ to 0/1.

*-R* _a_,_b_
	Sets counter and encoder values to 0. _a_ is the address of a DIO or DI
	module as displayed in the device list (option -d). _b_ is a bitfield
	defining the counters/endcoders to reset.

*-C* _address_
	Retrieves the relay wear level counters of a RO module. _address_ is the
	address of a RO module as displayed in the device list (option -d).

*-f*
	Starts the firmware update process. When used the firmware will only be
	updated if the update is newer than the version running on the module. This
	behaviour can be changed with the *--force* flag.

	The option *--module* must be used to specify the address of the module that
	should be updated.

*--module* _address_
	Specify the module that the following flag should use with the _address_.
	This option can be used with the *-f* flag to specify a specific module to
	update.

	The value of _address_ must be greater than *0* as updating the Revolution
	Pi (which always has an address of *0*) is unsupported.

*--force*
	Force a module update even if the firmware version is equal to or smaller
	than the version of the firmware running on the target module. This option
	can be used with the *-f* flag and requires the address of the target module
	(option *--module*).

*--assume-yes*
	Don't ask for confirmation when updating the firmware with *-f*. In order to
	have an effect this needs to be given before the *-f* option.

*-l*
	Listen for events such as a piControl reset initiated by another process.
	With this flag the exit status of piTest no longer states whether the
	command failed or not but corresponds to an event that was caught. The exit
	codes are documents in *EXIT STATUS*.

*-S*
	Stop cyclic synchronization of the process image with attached I/O and
	gateway modules. It is then possible to manually set values in the process
	image using the _-w_ option and thereby simulate that these values were
	received from the attached modules. To restart synchronization with
	attached modules, invoke the program with _-S_ once more.

*-x*
	Reset control process.

*-h*
	Show summary of options.

# EXIT STATUS

piTest exits with an exit status 1 if an error occurred.

If the *-l* flag is given exit codes have the following meaning:

- 1: Reset

# EXAMPLES

Read the value of the variable *Input_001* and display it in hex format:

```
piTest -r Input_001,h
```

Read *16* bytes at offset *1188*:

```
piTest -r 1188,16
```

Write the value *23* to the variable *Output_001*:

```
piTest -w Output_001,23
```

Write the value *31224205* to the byte with offset *0* to *3*:

```
piTest -w 0,4,31224205
```

Get bit *5* from the byte at offset *0*:

```
piTest -b 0,5
```

Set bit *5* of the byte at offset *0* to *1*:

```
piTest -s 0,5,1
```

Reset the counters of the inputs *I2* and *I5* because the bits *2* and *5* are
set to *1*:

```
piTest -R 32,0x14
```

Retrieve the counters from a module with address *32*, i.e. the first module on
the right:

```
piTest -C 32
```

# SEE ALSO

*picontrol_ioctl*(4)

The source code of this program is available at
https://gitlab.com/revolutionpi/revpi-pitest++
For more information on Revolution Pi visit https://revolutionpi.com
