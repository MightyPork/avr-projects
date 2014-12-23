#!/bin/env python3

import sys
import re

file = open(sys.argv[1], 'r')

src = file.read()

matches = re.search(r'static unsigned int width = (\d*);', src)
width = int(matches.groups(1)[0])

matches = re.search(r'static unsigned int height = (\d*);', src)
height = int(matches.groups(1)[0])


matches = re.search(r'static char header_data_cmap\[\d+\]\[\d+\] = \{\n\s*(\{.*?\}),\s*(\{.*?\})', src, flags=re.S)
s = matches.groups(1)[0]
t = matches.groups(1)[1]

if s == '{255,255,255}' and t == '{  0,  0,  0}':
	inverted=True
elif s == '{  0,  0,  0}' and t == '{255,255,255}':
	inverted=False
else:
	raise Exception('Wrong colors')




matches = re.search(r'static char header_data\[\] = \{(.*?)\};', src, flags=re.S)
s = matches.groups(1)[0]

# clean up - leave only numbers
s = re.sub(r'[\s,]', '', s).strip()

s = [s[x:x+width] for x in range(0,width*height,width)]

cols = [''.join(i) for i in zip(*s)]

bc = 0

print("""
#define ROWS {r}
#define COLS {c}

const uint8_t image[COLS][ROWS] PROGMEM = {{""".format(r=int(height/8), c=width))

for c in cols:

	# convert colors based on pallete
	if inverted:
		c=c.translate({
			ord('0'): ord('1'),
			ord('1'): ord('0')
		})


	bytz = ['0b{}'.format(c[x:x+8]) for x in range(0,len(c),8)]

	print('\t{ ', end="")

	print(', '.join(bytz), end="")

	print(' }, //', end="")

	print(c.translate({
			ord('0'): ord(' '),
			ord('1'): ord('█')
		}))

print('};\n')

