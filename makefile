all: mabe

<<<<<<< HEAD
mabe: objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_HumanBrain_HumanBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_HumanBrain_HumanBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o -o mabe
=======
mabe: objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_MarkovBrain_MarkovBrain.o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableFeedbackGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableDirectGate.o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o objectFiles/Brain_MarkovBrain_Gate_GPGate.o objectFiles/Brain_MarkovBrain_Gate_FeedbackGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableGate.o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o objectFiles/Brain_CGPBrain_CGPBrain.o objectFiles/Brain_HumanBrain_HumanBrain.o objectFiles/Brain_ANNBrain_ANNBrain.o objectFiles/Brain_BiLogBrain_BiLogBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread objectFiles/main.o objectFiles/Analyze_neurocorrelates.o objectFiles/Archivist_DefaultArchivist.o objectFiles/Brain_AbstractBrain.o objectFiles/Genome_AbstractGenome.o objectFiles/Global.o objectFiles/Group_Group.o objectFiles/Optimizer_AbstractOptimizer.o objectFiles/Organism_Organism.o objectFiles/Utilities_Data.o objectFiles/Utilities_MTree.o objectFiles/Utilities_Parameters.o objectFiles/Utilities_Loader.o objectFiles/Utilities_Filesystem.o objectFiles/Utilities_CSV.o objectFiles/World_AbstractWorld.o objectFiles/World_MapWorld_MapWorld.o objectFiles/Genome_CircularGenome_CircularGenome.o objectFiles/Genome_MultiGenome_MultiGenome.o objectFiles/Genome_MultiGenome_Chromosome_AbstractChromosome.o objectFiles/Genome_MultiGenome_Chromosome_TemplatedChromosome.o objectFiles/Brain_MarkovBrain_MarkovBrain.o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableFeedbackGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableDirectGate.o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o objectFiles/Brain_MarkovBrain_Gate_GPGate.o objectFiles/Brain_MarkovBrain_Gate_FeedbackGate.o objectFiles/Brain_MarkovBrain_Gate_DecomposableGate.o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o objectFiles/Brain_CGPBrain_CGPBrain.o objectFiles/Brain_HumanBrain_HumanBrain.o objectFiles/Brain_ANNBrain_ANNBrain.o objectFiles/Brain_BiLogBrain_BiLogBrain.o objectFiles/Optimizer_LexicaseOptimizer_LexicaseOptimizer.o objectFiles/Optimizer_SimpleOptimizer_SimpleOptimizer.o objectFiles/Archivist_LODwAPArchivist_LODwAPArchivist.o objectFiles/Archivist_SSwDArchivist_SSwDArchivist.o -o mabe
>>>>>>> 4625cf059184b59ed220f8571844fcb9a9b19eaa

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

<<<<<<< HEAD
objectFiles/Brain_HumanBrain_HumanBrain.o: ./Brain/HumanBrain/HumanBrain.cpp ./Brain/HumanBrain/HumanBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/HumanBrain/HumanBrain.cpp -o objectFiles/Brain_HumanBrain_HumanBrain.o

=======
objectFiles/Brain_MarkovBrain_MarkovBrain.o: ./Brain/MarkovBrain/MarkovBrain.cpp ./Brain/MarkovBrain/MarkovBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/MarkovBrain.cpp -o objectFiles/Brain_MarkovBrain_MarkovBrain.o

objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o: ./Brain/MarkovBrain/Gate/EpsilonGate.cpp ./Brain/MarkovBrain/Gate/EpsilonGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/EpsilonGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_EpsilonGate.o

objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o: ./Brain/MarkovBrain/Gate/ProbabilisticGate.cpp ./Brain/MarkovBrain/Gate/ProbabilisticGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/ProbabilisticGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_ProbabilisticGate.o

objectFiles/Brain_MarkovBrain_Gate_VoidGate.o: ./Brain/MarkovBrain/Gate/VoidGate.cpp ./Brain/MarkovBrain/Gate/VoidGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/VoidGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_VoidGate.o

objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o: ./Brain/MarkovBrain/Gate/NeuronGate.cpp ./Brain/MarkovBrain/Gate/NeuronGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/NeuronGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_NeuronGate.o

objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o: ./Brain/MarkovBrain/Gate/DeterministicGate.cpp ./Brain/MarkovBrain/Gate/DeterministicGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/DeterministicGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_DeterministicGate.o

objectFiles/Brain_MarkovBrain_Gate_DecomposableFeedbackGate.o: ./Brain/MarkovBrain/Gate/DecomposableFeedbackGate.cpp ./Brain/MarkovBrain/Gate/DecomposableFeedbackGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/DecomposableFeedbackGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_DecomposableFeedbackGate.o

objectFiles/Brain_MarkovBrain_Gate_DecomposableDirectGate.o: ./Brain/MarkovBrain/Gate/DecomposableDirectGate.cpp ./Brain/MarkovBrain/Gate/DecomposableDirectGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/DecomposableDirectGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_DecomposableDirectGate.o

objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o: ./Brain/MarkovBrain/Gate/AbstractGate.cpp ./Brain/MarkovBrain/Gate/AbstractGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/AbstractGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_AbstractGate.o

objectFiles/Brain_MarkovBrain_Gate_GPGate.o: ./Brain/MarkovBrain/Gate/GPGate.cpp ./Brain/MarkovBrain/Gate/GPGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/GPGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_GPGate.o

objectFiles/Brain_MarkovBrain_Gate_FeedbackGate.o: ./Brain/MarkovBrain/Gate/FeedbackGate.cpp ./Brain/MarkovBrain/Gate/FeedbackGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/FeedbackGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_FeedbackGate.o

objectFiles/Brain_MarkovBrain_Gate_DecomposableGate.o: ./Brain/MarkovBrain/Gate/DecomposableGate.cpp ./Brain/MarkovBrain/Gate/DecomposableGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/DecomposableGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_DecomposableGate.o

objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o: ./Brain/MarkovBrain/Gate/TritDeterministicGate.cpp ./Brain/MarkovBrain/Gate/TritDeterministicGate.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/Gate/TritDeterministicGate.cpp -o objectFiles/Brain_MarkovBrain_Gate_TritDeterministicGate.o

objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o: ./Brain/MarkovBrain/GateBuilder/GateBuilder.cpp ./Brain/MarkovBrain/GateBuilder/GateBuilder.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/GateBuilder/GateBuilder.cpp -o objectFiles/Brain_MarkovBrain_GateBuilder_GateBuilder.o

objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o: ./Brain/MarkovBrain/GateListBuilder/GateListBuilder.cpp ./Brain/MarkovBrain/GateListBuilder/GateListBuilder.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/MarkovBrain/GateListBuilder/GateListBuilder.cpp -o objectFiles/Brain_MarkovBrain_GateListBuilder_GateListBuilder.o

objectFiles/Brain_CGPBrain_CGPBrain.o: ./Brain/CGPBrain/CGPBrain.cpp ./Brain/CGPBrain/CGPBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/CGPBrain/CGPBrain.cpp -o objectFiles/Brain_CGPBrain_CGPBrain.o

objectFiles/Brain_HumanBrain_HumanBrain.o: ./Brain/HumanBrain/HumanBrain.cpp ./Brain/HumanBrain/HumanBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/HumanBrain/HumanBrain.cpp -o objectFiles/Brain_HumanBrain_HumanBrain.o

objectFiles/Brain_ANNBrain_ANNBrain.o: ./Brain/ANNBrain/ANNBrain.cpp ./Brain/ANNBrain/ANNBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/ANNBrain/ANNBrain.cpp -o objectFiles/Brain_ANNBrain_ANNBrain.o

objectFiles/Brain_BiLogBrain_BiLogBrain.o: ./Brain/BiLogBrain/BiLogBrain.cpp ./Brain/BiLogBrain/BiLogBrain.h
	c++ -Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread -c ./Brain/BiLogBrain/BiLogBrain.cpp -o objectFiles/Brain_BiLogBrain_BiLogBrain.o

>>>>>>> 4625cf059184b59ed220f8571844fcb9a9b19eaa
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
