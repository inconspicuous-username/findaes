# Findaes
This is a copy of the project by Jesse Kornblum hosted on [Sourceforge](https://sourceforge.net/projects/findaes/).
Since usefull stuff has the habbit of dissapearing from the internet, created this repo to have a personal copy for me and others whomight find this tool useful.

I am in no way claiming authorship to these files. The Sourceforge project does not contain any license, since it is not my place to relicense this code I cannot add a license on this repo either.

## Original description
Searches for AES keys by searching for their key schedules. Able to find 128. 192, and 256 bit keys, such as those used by TrueCrypt and BitLocker. Originally intended for memory images, can use arbitrary data.

## Additional description.
The program can be ran on any binary data format. It does not produce any output while running except when it found a keyschedule, so there is no indication of progress.
When running on large files this can be a bit annoying, but hey never look a givien horse in the mouth🤷‍♀️

Example output:
```
searching /home/memorypdump_of_device.raw
Found AES-256 key schedule at offset 0x1152292a0: 
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
Found AES-128 key schedule at offset 0x150a5e510: 
76 97 e0 2a 78 72 a4 f2 51 8b f8 96 ce 81 70 94
```
