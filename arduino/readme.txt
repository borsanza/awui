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
		Menu:  0x02 or 0x03
		Play/Pause: 0x04 or 0x05
		Right: 0x06 or 0x07
		Left: 0x08 or 0x09
		Up: 0x0A or 0x0B
		Down:	0x0C or 0x0D
		Ok: 0x5C or 0x5D
		Play/Pause: 0x5E or 0x5F

		Pressing with Play/Pause
			Up: 0x0E or 0x0F
			Down: 0x10 or 0x11
			Right: 0x12 or 0x13
			Left: 0x14 or 0x15

		Pressing Menu
			Up: 0x16 or 0x17 -> Change resolution
			Down: 0x18 or 0x19 -> Reset

		Pressing with Play/Pause (gray remote)
			Menu: 0x60 or 0x61
			Ok: 0x62 or 0x63
			Up: 0x64 or 0x65
			Down: 0x66 or 0x67
			Left: 0x68 or 0x69
			Right: 0x6A or 0x6B
