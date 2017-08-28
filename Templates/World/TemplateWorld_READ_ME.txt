

If you are new to building worlds, you may find it helpful to read over
TemplateWorld.h and then TemplateWorld.cpp before you get started.

If you know what you are doing already, use TemplateWorldClean.h and
TemplateWorldClean.h.

When you are ready to make your own world, you will:
1) make a new World Direcotry, where the name of your world 
	and the word "World" have capital letters (e.g. NewWorld)
2) copy TemplateWorld.h and TemplateWorld.cpp to your directory
3) rename TemplateWorld.h and TemplateWorld.cpp to the name of your
	world, exactly as it appears in the direcory name (e.g. NewWorld.cpp
	and NewWorld.h)
3) change all occurances of TemplateWorld in your code to the name of
	your world, exactly as it appears in the direcory name.
4) change the contents of the new .h and .cpp files as needed.
5) edit builtOptions.txt and add your world
6) run MBuild
	6a) if you are using an IDE 'python PythonTools/MBuild -noCompile'
		then recompile in your IDE
	6b) if you are using command line compilation 'python PythonTools/MBuild'


About the Template Files:

TemplateWorld.h and TemplateWorld.cpp are designed to help you build worlds.
They collect information from various parts of the MABE documentation
(which can be found here: https://github.com/Hintzelab/MABE/wiki).

The template files are not designed to be complete explanations, rather
these files pull salient points from the documentation and organize
them in a useful format.

To really understand MABE fully, you would need to read the docs. But these
files should get you started, and if you actually want to do is get a new
world up and running, these template files may provide all the information
you need.

Lastly, in the simplest terms, a worlds function is to evaluate organisms
and either:
a) to assign values to ortanisms so that an Optimizer can manage reproduction
or
b) to manage reproduction themselvs.

These template files provide a number of examples but, by no means cover the
gamut of what is possible.


