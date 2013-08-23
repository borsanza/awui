Diodo usado:
  - TK1833 (FD019)

stty -F /dev/ttyUSB0 115200 min 1 -parenb -parodd cs8 hupcl -cstopb cread clocal -crtscts -ignbrk -brkint -ignpar -parmrk -inpck -istrip -igncr -icrnl ixon -ixoff -iuclc -ixany -imaxbel -iutf8 opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke
cat /dev/ttyUSB0

# Protocol
LibraryCode:isDown:Id:Command:RemoteId:Count: Text

Commands:
  with Id = 0x87E0
Code
51		Menu + Ok: 0x02 or 0x03 -> Remote Linked
51		Menu + Right: 0x02 or 0x03 -> Remote Linked
52		Menu + Left: 0x04 or 0x05 -> Accept all remotes

  with Id = 0x87EE
10		Menu:  0x02 or 0x03
11		Play/Pause: 0x04 or 0x05 // white
12		Right: 0x06 or 0x07
13		Left: 0x08 or 0x09
14		Up: 0x0A or 0x0B
15		Down:	0x0C or 0x0D
16		Ok: 0x5C or 0x5D // gray
17		Play/Pause: 0x5E or 0x5F // gray

		Pressing with Play/Pause
21		Up: 0x0E or 0x0F
22		Down: 0x10 or 0x11
23		Right: 0x12 or 0x13
24		Left: 0x14 or 0x15

		Pressing Menu
31		Up: 0x16 or 0x17 -> Change resolution
32		Down: 0x18 or 0x19 -> Reset

		Pressing with Play/Pause (gray remote)
41		Menu: 0x60 or 0x61
42		Ok: 0x62 or 0x63
43		Up: 0x64 or 0x65
44		Down: 0x66 or 0x67
45		Left: 0x68 or 0x69
46		Right: 0x6A or 0x6B
