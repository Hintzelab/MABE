import posixpath

# Handles writing modules.h
# TODO: Make this use a template file
# Since a lot of the parts are the same regardless of options, 
# it'd be better to have a "modules.h.template", with a couple keywords
# (ex: TEMPLATE_INCLUDES_EXTRAS) that could then be replaced via sub, sed, etc
def write_modules_h(pathToMABE, options):
    # Create modules.h
    outFile = open(posixpath.join(pathToMABE,"modules.h"), 'w')
    outFile.write('//  MABE is a product of The Hintza Lab @ MSU\n')
    outFile.write('//     for general research information:\n')
    outFile.write('//         http://hintzelab.msu.edu/\n')
    outFile.write('//     for MABE documentation:\n')
    outFile.write('//         github.com/Hintzelab/MABE/wiki\n')
    outFile.write('//\n')
    outFile.write('//  Copyright (c) 2015 Michigan State University. All rights reserved.\n')
    outFile.write('//     to view the full license, visit:\n')
    outFile.write('//          github.com/Hintzelab/MABE/wiki\n\n')
    outFile.write('//  This file was auto-generated with MBuilder.py\n\n')

    # modules.h:includes

    outFile.write('#ifndef __AutoBuild__Modules__\n')
    outFile.write('#define __AutoBuild__Modules__\n')
    for option in options["World"]:
        outFile.write('#include "World/'+option+'World/'+option+'World.h"\n')
    for option in options["Genome"]:
        outFile.write('#include "Genome/'+option+'Genome/'+option+'Genome.h"\n')
    for option in options["Brain"]:
        outFile.write('#include "Brain/'+option+'Brain/'+option+'Brain.h"\n')
    for option in options["Optimizer"]:
        outFile.write('#include "Optimizer/'+option+'Optimizer/'+option+'Optimizer.h"\n')
    outFile.write('\n#include "Archivist/DefaultArchivist.h"\n')
    for option in options["Archivist"]:
        if option != "Default":
            outFile.write('#include "Archivist/'+option+'Archivist/'+option+'Archivist.h"\n')

    # modules.h:makeWorld

    outFile.write('\n\n//create a world\n')
    outFile.write('std::shared_ptr<AbstractWorld> makeWorld(std::shared_ptr<ParametersTable> PT){\n')
    outFile.write('  std::shared_ptr<AbstractWorld> newWorld;\n')
    outFile.write('  bool found = false;\n')
    outFile.write('  std::string worldType = AbstractWorld::worldTypePL->get(PT);\n')
    for option in options["World"]:
        outFile.write('  if (worldType == "'+option+'") {\n')
        outFile.write('    newWorld = std::make_shared<'+option+'World>(PT);\n')
        outFile.write('    found = true;\n')    
        outFile.write('    }\n')
    outFile.write('  if (!found){\n')
    outFile.write('    std::cout << "  ERROR! could not find WORLD-worldType \\"" << worldType << "\\".\\n  Exiting." << std::endl;\n')
    outFile.write('    exit(1);\n')
    outFile.write('    }\n')
    outFile.write('  return newWorld;\n')
    outFile.write('}\n')

    # modules.h:makeOptimizer

    outFile.write('\n\n//create an optimizer\n')
    outFile.write('std::shared_ptr<AbstractOptimizer> makeOptimizer(std::shared_ptr<ParametersTable> PT){\n')
    outFile.write('  std::shared_ptr<AbstractOptimizer> newOptimizer;\n')
    outFile.write('  bool found = false;\n')
    outFile.write('  std::string optimizerType = AbstractOptimizer::Optimizer_MethodStrPL->get(PT);\n')
    for option in options["Optimizer"]:
        outFile.write('  if (optimizerType == "'+option+'") {\n')
        outFile.write('    newOptimizer = std::make_shared<'+option+'Optimizer>(PT);\n')
        outFile.write('    found = true;\n')    
        outFile.write('    }\n')
    outFile.write('  if (!found){\n')
    outFile.write('    std::cout << "  ERROR! could not find OPTIMIZER-optimizer \\"" << optimizerType << "\\".\\n  Exiting." << std::endl;\n')
    outFile.write('    exit(1);\n')
    outFile.write('    }\n')
    outFile.write('  return newOptimizer;\n')
    outFile.write('}\n')

    # modules.h:makeArchivist

    outFile.write('\n\n//create an archivist\n')
    outFile.write('std::shared_ptr<DefaultArchivist> makeArchivist(std::vector<std::string> popFileColumns, std::shared_ptr<Abstract_MTree> _maxFormula, std::shared_ptr<ParametersTable> PT, std::string groupPrefix = ""){\n')
    outFile.write('  std::shared_ptr<DefaultArchivist> newArchivist;\n')
    outFile.write('  bool found = false;\n')
    outFile.write('  std::string archivistType = DefaultArchivist::Arch_outputMethodStrPL->get(PT);\n')
    for option in options["Archivist"]:
        outFile.write('  if (archivistType == "'+option+'") {\n')
        outFile.write('    newArchivist = std::make_shared<'+option+'Archivist>(popFileColumns, _maxFormula, PT, groupPrefix);\n')
        outFile.write('    found = true;\n')    
        outFile.write('    }\n')
    outFile.write('  if (!found){\n')
    outFile.write('    std::cout << "  ERROR! could not find ARCHIVIST-outputMethod \\"" << archivistType << "\\".\\n  Exiting." << std::endl;\n')
    outFile.write('    exit(1);\n')
    outFile.write('    }\n')
    outFile.write('  return newArchivist;\n')
    outFile.write('}\n')

    # modules.h:makeTemplateGenome

    outFile.write('\n\n//create a template genome\n')
    outFile.write('std::shared_ptr<AbstractGenome> makeTemplateGenome(std::shared_ptr<ParametersTable> PT){\n')
    outFile.write('  std::shared_ptr<AbstractGenome> newGenome;\n')
    outFile.write('  bool found = false;\n')
    outFile.write('  std::string genomeType = AbstractGenome::genomeTypeStrPL->get(PT);\n')
    for option in options["Genome"]:
        outFile.write('  if (genomeType == "'+option+'") {\n')
        outFile.write('    newGenome = '+option+'Genome_genomeFactory(PT);\n')
        outFile.write('    found = true;\n')    
        outFile.write('    }\n')
    outFile.write('  if (found == false){\n')
    outFile.write('    std::cout << "  ERROR! could not find GENOME-genomeType \\"" << genomeType << "\\".\\n  Exiting." << std::endl;\n')
    outFile.write('    exit(1);\n')
    outFile.write('    }\n')
    outFile.write('  return newGenome;\n')
    outFile.write('}\n')

    # modules.h:makeTemplateBrain

    outFile.write('\n\n//create a template brain\n')
    outFile.write('std::shared_ptr<AbstractBrain> makeTemplateBrain(int inputs, int outputs, std::shared_ptr<ParametersTable> PT){\n')
    outFile.write('  std::shared_ptr<AbstractBrain> newBrain;\n')
    outFile.write('  bool found = false;\n')
    outFile.write('  std::string brainType = AbstractBrain::brainTypeStrPL->get(PT);\n')
    for option in options["Brain"]:
        outFile.write('  if (brainType == "'+option+'") {\n')
        outFile.write('    newBrain = '+option+'Brain_brainFactory(inputs, outputs, PT);\n')
        outFile.write('    found = true;\n')    
        outFile.write('    }\n')
    outFile.write('  if (found == false){\n')
    outFile.write('    std::cout << "  ERROR! could not find BRAIN-brainType \\"" << brainType << "\\".\\n  Exiting." << std::endl;\n')
    outFile.write('    exit(1);\n')
    outFile.write('    }\n')
    outFile.write('  return newBrain;\n')
    outFile.write('}\n')

    # modules.h:configure Defaults and Documentation

    outFile.write('\n\n//configure Defaults and Documentation\n')
    outFile.write('void configureDefaultsAndDocumentation(){\n')


    outFile.write('  Parameters::root->setParameter("BRAIN-brainType", (std::string)"' + options["Brain"][0] + '");\n')
    optionsList = ''
    for t in options["Brain"]:
        optionsList += t + ', '
    optionsList = optionsList[:-2]
    outFile.write('  Parameters::root->setDocumentation("BRAIN-brainType", "brain to be used, [' + optionsList + ']");\n\n')

    outFile.write('  Parameters::root->setParameter("GENOME-genomeType", (std::string)"' + options["Genome"][0] + '");\n')
    optionsList = ''
    for t in options["Genome"]:
        optionsList += t + ', '
    optionsList = optionsList[:-2]
    outFile.write('  Parameters::root->setDocumentation("GENOME-genomeType", "genome to be used, [' + optionsList + ']");\n\n')

    outFile.write('  Parameters::root->setParameter("ARCHIVIST-outputMethod", (std::string)"' + options["Archivist"][0] + '");\n')
    optionsList = ''
    for t in options["Archivist"]:
        optionsList += t + ', '
    optionsList = optionsList[:-2]
    outFile.write('  Parameters::root->setDocumentation("ARCHIVIST-outputMethod", "output method, [' + optionsList + ']");\n\n')

    outFile.write('  Parameters::root->setParameter("OPTIMIZER-optimizer", (std::string)"' + options["Optimizer"][0] + '");\n')
    optionsList = ''
    for t in options["Optimizer"]:
        optionsList += t + ', '
    optionsList = optionsList[:-2]
    outFile.write('  Parameters::root->setDocumentation("OPTIMIZER-optimizer", "optimizer to be used, [' + optionsList + ']");\n\n')

    outFile.write('  Parameters::root->setParameter("WORLD-worldType", (std::string)"' + options["World"][0] + '");\n')
    optionsList = ''
    for t in options["World"]:
        optionsList += t + ', '
    optionsList = optionsList[:-2]
    outFile.write('  Parameters::root->setDocumentation("WORLD-worldType","world to be used, [' + optionsList + ']");\n')
    outFile.write('}\n')

    outFile.write('\n\n#endif /* __AutoBuild__Modules__ */\n')

    outFile.close()