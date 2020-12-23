#! /usr/bin/python3

f=open("1mb.txt", 'w')
text = ''
i = 0
line = str(i)
while len(text + line) <= (1024*1024):
    text += line
    i += 1
    line = '\r\n' + str(i)
f.write(text)
f.close()