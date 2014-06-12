#! /usr/bin/env python

from random import random

ofile = open("rossiniColors.h" , 'w')
numColors =87360
ofile.write("static GLfloat COLORS[] = {");
for i in range(numColors/2):
	#~ ofile.write("1.0f, 0.0f, 0.0f, 1.0f, \n");
	#~ ofile.write("1.0f, 1.0f, 0.0f, 1.0f, \n");
	#~ ofile.write("1.0f, 0.5f, 0.1f, 1.0f, \n");
	#~ ofile.write("%gf, %gf, %gf, 1.0f, \n" %(0.8 + 0.2*random(), 0.4 + 0.2*random(), random() ));
	ofile.write("{%g, %g, %g, 1.0}, \n" %(0,1,0))#%(0.8 + 0.2*random(), 0.4 + 0.2*random(), random() ));

for i in range(numColors/2):
	#~ ofile.write("1.0f, 0.0f, 0.0f, 1.0f, \n");
	#~ ofile.write("1.0f, 1.0f, 0.0f, 1.0f, \n");
	#~ ofile.write("1.0f, 0.5f, 0.1f, 1.0f, \n");
	#~ ofile.write("%gf, %gf, %gf, 1.0f, \n" %(0.8 + 0.2*random(), 0.4 + 0.2*random(), random() ));
	ofile.write("{%g, %g, %g, 1.0}, \n" %(1,0,1))#%(0.8 + 0.2*random(), 0.4 + 0.2*random(), random() ));

ofile.write("};")
