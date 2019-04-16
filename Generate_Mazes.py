# Code modified from: https://en.wikipedia.org/wiki/Maze_generation_algorithm#Python_code_example

import numpy
from numpy.random import randint as rand
import matplotlib.pyplot as pyplot


def maze(width=81, height=51, complexity=.75, density=.75):
	# Only odd shapes
	shape = ((height // 2) * 2 + 1, (width // 2) * 2 + 1)
	# Adjust complexity and density relative to maze size
	complexity = int(complexity * (5 * (shape[0] + shape[1]))) # number of components
	density = int(density * ((shape[0] // 2) * (shape[1] // 2))) # size of components

	# Build actual maze
	Z = numpy.zeros(shape, dtype=bool)
	# Fill borders
	Z[0, :] = Z[-1, :] = 1
	Z[:, 0] = Z[:, -1] = 1
	# Make aisles
	for i in range(density):
		x, y = rand(0, shape[1] // 2) * 2, rand(0, shape[0] // 2) * 2 # pick a random position
		Z[y, x] = 1
		for j in range(complexity):
			neighbours = []
			if x > 1:			  neighbours.append((y, x - 2))
			if x < shape[1] - 2:  neighbours.append((y, x + 2))
			if y > 1:			  neighbours.append((y - 2, x))
			if y < shape[0] - 2:  neighbours.append((y + 2, x))
			if len(neighbours):
				y_,x_ = neighbours[rand(0, len(neighbours) - 1)]
				if Z[y_, x_] == 0:
					Z[y_, x_] = 1
					Z[y_ + (y - y_) // 2, x_ + (x - x_) // 2] = 1
					x, y = x_, y_
	# print(Z)

	return Z

for i in range(10):
	Z = maze(13, 13)
	
	with open("Mazes/maze_{}.txt".format(i), 'w') as f:	
		for x in range(0, Z.shape[0]):
			for y in range(0, Z.shape[1]):
				f.write(str(1)) if Z[x,y] else f.write(str(0))
			f.write("\n")
	'''
	# Kepp 
	pyplot.figure(figsize=(11, 11))
	pyplot.imshow(Z, cmap=pyplot.cm.binary, interpolation='nearest')
	pyplot.xticks([]), pyplot.yticks([])
	pyplot.show()
	'''
	