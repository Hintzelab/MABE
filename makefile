all: mabe

mabe: objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_CGPBrain_CGPBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_CGPBrain_CGPBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o -o mabe

objectFiles/main.o: ./main.cpp
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./main.cpp -o objectFiles/main.o

objectFiles/Analyze_neurocorrelates.o: ./Analyze/neurocorrelates.cpp ./Analyze/neurocorrelates.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Analyze/neurocorrelates.cpp -o objectFiles/Analyze_neurocorrelates.o

objectFiles/Archivist_DefaultArchivist.o: ./Archivist/DefaultArchivist.cpp ./Archivist/DefaultArchivist.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Archivist/DefaultArchivist.cpp -o objectFiles/Archivist_DefaultArchivist.o

objectFiles/Brain_AbstractBrain.o: ./Brain/AbstractBrain.cpp ./Brain/AbstractBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/AbstractBrain.cpp -o objectFiles/Brain_AbstractBrain.o

objectFiles/Genome_AbstractGenome.o: ./Genome/AbstractGenome.cpp ./Genome/AbstractGenome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/AbstractGenome.cpp -o objectFiles/Genome_AbstractGenome.o

objectFiles/Global.o: ./Global.cpp ./Global.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Global.cpp -o objectFiles/Global.o

objectFiles/Group_Group.o: ./Group/Group.cpp ./Group/Group.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Group/Group.cpp -o objectFiles/Group_Group.o

objectFiles/Optimizer_AbstractOptimizer.o: ./Optimizer/AbstractOptimizer.cpp ./Optimizer/AbstractOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/AbstractOptimizer.cpp -o objectFiles/Optimizer_AbstractOptimizer.o

objectFiles/Organism_Organism.o: ./Organism/Organism.cpp ./Organism/Organism.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Organism/Organism.cpp -o objectFiles/Organism_Organism.o

objectFiles/Utilities_Data.o: ./Utilities/Data.cpp ./Utilities/Data.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Data.cpp -o objectFiles/Utilities_Data.o

objectFiles/Utilities_MTree.o: ./Utilities/MTree.cpp ./Utilities/MTree.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/MTree.cpp -o objectFiles/Utilities_MTree.o

objectFiles/Utilities_Parameters.o: ./Utilities/Parameters.cpp ./Utilities/Parameters.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Parameters.cpp -o objectFiles/Utilities_Parameters.o

objectFiles/Utilities_Loader.o: ./Utilities/Loader.cpp ./Utilities/Loader.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Loader.cpp -o objectFiles/Utilities_Loader.o

objectFiles/Utilities_Filesystem.o: ./Utilities/Filesystem.cpp ./Utilities/Filesystem.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/Filesystem.cpp -o objectFiles/Utilities_Filesystem.o

objectFiles/Utilities_CSV.o: ./Utilities/CSV.cpp ./Utilities/CSV.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Utilities/CSV.cpp -o objectFiles/Utilities_CSV.o

objectFiles/World_AbstractWorld.o: ./World/AbstractWorld.cpp ./World/AbstractWorld.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./World/AbstractWorld.cpp -o objectFiles/World_AbstractWorld.o

objectFiles/World_MapWorld_MapWorld.o: ./World/MapWorld/MapWorld.cpp ./World/MapWorld/MapWorld.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./World/MapWorld/MapWorld.cpp -o objectFiles/World_MapWorld_MapWorld.o

objectFiles/Genome_CircularGenome_CircularGenome.o: ./Genome/CircularGenome/CircularGenome.cpp ./Genome/CircularGenome/CircularGenome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/CircularGenome/CircularGenome.cpp -o objectFiles/Genome_CircularGenome_CircularGenome.o

objectFiles/Genome_MultiGenome_MultiGenome.o: ./Genome/MultiGenome/MultiGenome.cpp ./Genome/MultiGenome/MultiGenome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/MultiGenome/MultiGenome.cpp -o objectFiles/Genome_MultiGenome_MultiGenome.o

objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o: ./Genome/MultiGenome/Chromosome/AbstractChromosome.cpp ./Genome/MultiGenome/Chromosome/AbstractChromosome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/MultiGenome/Chromosome/AbstractChromosome.cpp -o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o

objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o: ./Genome/MultiGenome/Chromosome/TemplatedChromosome.cpp ./Genome/MultiGenome/Chromosome/TemplatedChromosome.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Genome/MultiGenome/Chromosome/TemplatedChromosome.cpp -o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o

objectFiles/Brain_CGPBrain_CGPBrain.o: ./Brain/CGPBrain/CGPBrain.cpp ./Brain/CGPBrain/CGPBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/CGPBrain/CGPBrain.cpp -o objectFiles/Brain_CGPBrain_CGPBrain.o

objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o: ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.cpp ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/LexicaseOptimizer/LexicaseOptimizer.cpp -o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o

objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o: ./Optimizer/SimpleOptimizer/SimpleOptimizer.cpp ./Optimizer/SimpleOptimizer/SimpleOptimizer.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Optimizer/SimpleOptimizer/SimpleOptimizer.cpp -o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o

objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o: ./Archivist/LODwAPArchivist/LODwAPArchivist.cpp ./Archivist/LODwAPArchivist/LODwAPArchivist.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Archivist/LODwAPArchivist/LODwAPArchivist.cpp -o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o

objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o: ./Archivist/SSwDArchivist/SSwDArchivist.cpp ./Archivist/SSwDArchivist/SSwDArchivist.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Archivist/SSwDArchivist/SSwDArchivist.cpp -o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o

clean:
	rm -r objectFiles/* mabe

cleanup:
	rm -r objectFiles/*
