stty -F /dev/ttyUSB0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
cat /dev/ttyUSB0

# Protocol
Id:Command:RemoteId:isDown: Text

Commands:
  with Id = 0x87E0
		Menu + Ok: 0x02 or 0x03 -> Remote Linked
		Menu + Right: 0x02 or 0x03 -> Remote Linked
		Menu + Left: 0x04 or 0x05 -> Accept all remotes

  with Id = 0x87EE
		Play/Pause: 0x5F or 0x5E or 0x04 or 0x05
		Menu:  0x03
		Ok: 0x5C or 0x5D
		Right: 0x07 or 0x06
		Left: 0x08 or 0x09
		Up: 0x0B or 0x0A
		Down:	0x0D or 0x0C

		Pressing with Play/Pause
			Right: 0x13
			Left: 0x15
			Up: 0x0E
			Down: 0x10

		Pressing with Play/Pause (gray remote)
			Right: 0x6A
			Left: 0x69
			Up: 0x65
			Down: 0x66
			Ok: 0x63

		Pressing Menu
			Up: 0x16 or 0x17 -> Change resolution
			Down: 0x18 or 0x19 -> Reset
			Play/Pause: 0x60
