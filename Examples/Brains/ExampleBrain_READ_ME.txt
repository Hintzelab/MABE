A brains function is to convert input values into output values. Generally
the input values will be set and read by a world.

If you are new to building brains, you may find it helpful to read over
ExampleWorld_h and then ExampleWorld_cpp and also ExampleBrain_h  and
ExampleBrain_cpp before you get started.

If you know what you are doing already, use ExampleBrainClean.h and
ExampleBrainClean.h.

When you are ready to make your own world, you will:
	1) make a new Brain Direcotry, where the name of your brain 
		and the word "World" have capital letters (e.g. NewBrain)
	2) copy ExampleBrain_h and ExampleBrain_cpp to your directory
	2a) replace the '_' with '.' so they become *.h and *.cpp
	3) rename ExampleBrain.h and ExampleBrain.cpp to the name of your
		Brain, exactly as it appears in the direcory name
		(e.g. NewBrain.cpp and NewBrain.h)
	3) change all occurances of ExampleBrain in your code to the name of
		your brain, exactly as it appears in the direcory name.
	4) change the functions and parameters in the new .h and .cpp files
		as needed.
	5) edit builtOptions.txt and add your brain
	6) run MBuild
		6a) if you are using an IDE 'python PythonTools/MBuild -noCompile'
			then recompile in your IDE
		6b) if you are using command line compilation 'python PythonTools/MBuild'


About the ExampleBrain Files:

ExampleBrain_h and ExampleBrain_cpp are designed to help you build brains.
They collect information from various parts of the MABE documentation
(which can be found here: https://github.com/Hintzelab/MABE/wiki).

The example files are not complete explanations, rather they pull salient
points from the documentation and organize them in a condensed format.

To really understand MABE fully, you would need to read the docs. But these
files should get you started, and if all you actually want to do is get a new
brain up and running, these files may provide all the information
you need.

The example files provide a number of examples but, by no means cover the
gamut of what is possible.

Lastly, if you found these examples helpful, lacking or confusing,
let us know! Please send feedback to cliff@msu.edu

