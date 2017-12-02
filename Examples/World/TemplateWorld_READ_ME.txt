In MABE, a world defines a problem (or task or question...) which an organism (or
collection of organisms) is faced with. This problem may be finding the sum of
two numbers, fitting a curve, navigating a maze, finding a stratagy to harvest the
most resources in a limited time, etc. Worlds can be written with more abstract 
problems (perhaps produce offspring who survive...).

Generally speaking though, a worlds function is to evaluate organisms and either:

	a) to assign values to ortanisms so that an Optimizer can manage reproduction

	or

	b) to evaluate organisms and manage reproduction themselvs.

If you are new to building worlds, you may find it helpful to read over
ExampleWorld_h and then ExampleWorld_cpp before you get started.

If you know what you are doing already, use ExampleWorldClean_h and
ExampleWorldClean_h.

When you are ready to make your own world, you will:
	1) make a new World Direcotry, where the name of your world 
		and the word "World" have capital letters (e.g. NewWorld)
	2) copy ExampleWorld_h and ExampleWorld_cpp to your directory
	2a) replace the '_' with '.' so they become *.h and *.cpp
	3) rename ExampleWorld.h and ExampleWorld.cpp to the name of your
		world, exactly as it appears in the direcory name
		(e.g. NewWorld.cpp and NewWorld.h)
	3) change all occurances of ExampleWorld in your code to the name of
		your world, exactly as it appears in the direcory name.
	4) change the functions and parameters in the new .h and .cpp files
		as needed.
	5) edit builtOptions.txt and add your world
	6) run MBuild
		6a) if you are using an IDE 'python PythonTools/MBuild -noCompile'
			then recompile in your IDE
		6b) if you are using command line compilation 'python PythonTools/MBuild'


About the ExampleWorld Files:

ExampleWorld_h and ExampleWorld_cpp are designed to help you build worlds.
They collect information from various parts of the MABE documentation
(which can be found here: https://github.com/Hintzelab/MABE/wiki).

The example files are not complete explanations, rather they pull salient
points from the documentation and organize them in a condensed format.

To really understand MABE fully, you would need to read the docs. But these
files should get you started, and if all you actually want to do is get a new
world up and running, these files may provide all the information
you need.

The example files provide a number of examples but, by no means cover the
gamut of what is possible.

Lastly, if you found these examples helpful, lacking or confusing,
let us know! Please send feedback to cliff@msu.edu

