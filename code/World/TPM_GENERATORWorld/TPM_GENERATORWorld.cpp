//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "TPM_GENERATORWorld.h"

// this is how you setup a parameter in MABE, the function Parameters::register_parameter()takes the
// name of the parameter (catagory-name), default value (which must conform with the type), a the useage message
shared_ptr<ParameterLink<int>> TPM_GENERATORWorld::numberOfHiddenPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-numberOfHidden", -1,
    "number of hidden in each organism?");

shared_ptr<ParameterLink<int>> TPM_GENERATORWorld::numberOfSamplesPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-numberOfSamples", 1,
    "how many times should each state be tested?");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::outputModePL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-outputMode", (std::string)"raw",
    "how should data be saved?\n"
    "  raw: output full time series for input, output, and hidden. This will result in 2 hidden entires for each input/output for the before and after hidden states\n"
    "  divided: output full time series for input, output, hidden before, and hidden after (if recurrentOutput output is divided into before and after)\n"
    "  joined: output two time series, one with input+hidden and the other with output+hidden (if recurrentOutput output will appear with input+hidden)\n"
    "  packed: output two time series, one with input+emptyOutput+hidden and the other with emptyInput+output+hidden (if recurrentOutput output will appear with input+hidden)");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::worldNamePL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-worldName", (std::string)"undefined", "name of the world the agent was evolved on");

shared_ptr<ParameterLink<int>> TPM_GENERATORWorld::missingSymbolPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-missingSymbol", -1, "value to be used when making filler samples (i.e. if outputMode is packed)");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::beforeStatesEndianPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-beforeStatesEndian", std::string("little"), "little vs. big, is little before states will have least signifigent bit on left (ie. 000,100,010,110,001,101,...,111)");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::discretizeRuleInputPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-discretizeRuleInput", std::string("BIT"), "rule to use when generating outputs to discretizing inputs\n"
    "options are INT, BIT, TRIT, NEAREST_INT, NEAREST_BIT, NEAREST_TRIT, MEDIAN, UNIQUE\n"
    "see  github.com/Hintzelab/MABE/wiki/TimeSeries  for explinations under remapToIntTimeSeries");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::discretizeRuleOutputPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-discretizeRuleOutput", std::string("BIT"), "rule to use when generating outputs to discretizing outputs");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::discretizeRuleHiddenPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-discretizeRuleHidden", std::string("BIT"), "rule to use when generating outputs to discretizing hidden");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::symbolsListInputPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-symbolsListInput", std::string("0,1"), "list of symbols to use when generating input patterns to be fed to brain");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::symbolsListOutputPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-symbolsListOutput", std::string("0,1"), "list of symbols to use when generating output patterns to be fed to brain");

shared_ptr<ParameterLink<std::string>> TPM_GENERATORWorld::symbolsListHiddenPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-symbolsListHidden", std::string("0,1"), "list of symbols to use when generating hidden patterns to be fed to brain");

shared_ptr<ParameterLink<bool>> TPM_GENERATORWorld::saveS2SPL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-saveS2S", false, "if true, save state to state plots");

shared_ptr<ParameterLink<bool>> TPM_GENERATORWorld::saveBrainStructurePL =
Parameters::register_parameter("WORLD_TPM_GENERATOR-saveBrainStructure", false, "if true, save a plot of brain structure");

// given a current TS sample and a list of symbols, update sample to the next
// return false is the sample is an over flow, i.e. we failed to get next
// littleEndian is either True or False (for bigEndian)
bool getNextInTS(std::vector<int>& sample, const int& symbolsCount, bool littleEndian) {
    int pos = 0; // point to first element
    int dir = 1; // more signifiget bits are first
    int last = sample.size(); // we know we are at the end when pos == last (i.e. overflow)
    int first = pos;

    if (littleEndian == false) {
        pos = sample.size() - 1; // point to last element
        dir = -1; // more signifigent bits first
        last = -1; // we know we are at the end when pos == last (i.e. overflow)
        first = pos;
    }

    while (pos != last && sample[pos] == symbolsCount) {
        pos += dir;
    }
    if (pos == last) { // overflow we can not get next...
        return(false);
    }
    else {
        sample[pos] += 1;
        for (int i = pos - dir; i != first - dir; i -= dir) {
            sample[i] = 0;
        }
    }
    return true;
}


TS::TimeSeries makeTSFromSymbols(int sampleSize, std::vector<double> symbols, bool littleEndian) {
    int symbolsCount = symbols.size()-1;
    std::vector<int> intSample(sampleSize, 0);
    std::vector<double> emptySample(sampleSize, 0);
    TS::TimeSeries newTS;

    bool gotNext;
    do {
        newTS.push_back(emptySample);
        for (int i = 0; i < sampleSize; i++) {
            newTS.back()[i] = symbols[intSample[i]];
        }
        gotNext = getNextInTS(intSample, symbolsCount, littleEndian);
    } while (gotNext == true);
    return newTS;
}

// the constructor gets called once when MABE starts up. use this to set things up
TPM_GENERATORWorld::TPM_GENERATORWorld(shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

    std::cout << "setting up TMP world...\n";

    auto tempWorldType = AbstractWorld::worldTypePL->get(PT);
    AbstractWorld::worldTypePL->set(worldNamePL->get(PT));
    auto subjectWorld = makeWorld(Parameters::root);
    AbstractWorld::worldTypePL->set(tempWorldType,PT);

    auto reqs = subjectWorld->requiredGroups();
    std::unordered_set<std::string> whichReqSet;

    for (auto elem : reqs["root::"]) {
        if ((elem[0] == 'B') && (elem[1] == ':') & (elem[2] == 'r')) { // is this the root brain requirement?
            std::vector<std::string> tempVect;
            convertCSVListToVector(elem, tempVect);
            convertString(tempVect[1], nrIn);
            convertString(tempVect[2], nrOut);
        }
    }

    nrHid = numberOfHiddenPL->get(PT);
    if (nrHid == -1) {
        std::cout << "  in TMP world setup, you have not provided a value for numberOfHidden.\n  exiting\n" << std::endl;
        exit(1);
    }
    
    missingSymbol = missingSymbolPL->get(PT);
    beforeStatesLittleEndian = (beforeStatesEndianPL->get(PT) == "little");
    outputMode = outputModePL->get(PT);

    discretizeRuleInputName = discretizeRuleInputPL->get(PT);
    discretizeRuleOutputName = discretizeRuleOutputPL->get(PT);
    discretizeRuleHiddenName = discretizeRuleHiddenPL->get(PT);
    
    if (discretizeRuleInputName == "BIT") discretizeRuleInput = TS::RemapRules::BIT;
    else if (discretizeRuleInputName == "INT") discretizeRuleInput = TS::RemapRules::INT;
    else if (discretizeRuleInputName == "TRIT") discretizeRuleInput = TS::RemapRules::TRIT;
    else if (discretizeRuleInputName == "NEAREST_INT") discretizeRuleInput = TS::RemapRules::NEAREST_INT;
    else if (discretizeRuleInputName == "NEAREST_BIT") discretizeRuleInput = TS::RemapRules::NEAREST_BIT;
    else if (discretizeRuleInputName == "NEAREST_TRIT") discretizeRuleInput = TS::RemapRules::NEAREST_TRIT;
    else if (discretizeRuleInputName == "MEDIAN") discretizeRuleInput = TS::RemapRules::MEDIAN;
    else if (discretizeRuleInputName == "UNIQUE") discretizeRuleInput = TS::RemapRules::UNIQUE;
    else {
        std::cout << "  in TPM_GENERATOR world :: found unknown discretizeRuleInput... please fix and try again. exiting." << std::endl;
        exit(1);
    }

    if (discretizeRuleOutputName == "BIT") discretizeRuleOutput = TS::RemapRules::BIT;
    else if (discretizeRuleOutputName == "INT") discretizeRuleOutput = TS::RemapRules::INT;
    else if (discretizeRuleOutputName == "TRIT") discretizeRuleOutput = TS::RemapRules::TRIT;
    else if (discretizeRuleOutputName == "NEAREST_INT") discretizeRuleOutput = TS::RemapRules::NEAREST_INT;
    else if (discretizeRuleOutputName == "NEAREST_BIT") discretizeRuleOutput = TS::RemapRules::NEAREST_BIT;
    else if (discretizeRuleOutputName == "NEAREST_TRIT") discretizeRuleOutput = TS::RemapRules::NEAREST_TRIT;
    else if (discretizeRuleOutputName == "MEDIAN") discretizeRuleOutput = TS::RemapRules::MEDIAN;
    else if (discretizeRuleOutputName == "UNIQUE") discretizeRuleOutput = TS::RemapRules::UNIQUE;
    else {
        std::cout << "  in TPM_GENERATOR world :: found unknown discretizeRuleOutput... please fix and try again. exiting." << std::endl;
        exit(1);
    }

    if (discretizeRuleHiddenName == "BIT") discretizeRuleHidden = TS::RemapRules::BIT;
    else if (discretizeRuleHiddenName == "INT") discretizeRuleHidden = TS::RemapRules::INT;
    else if (discretizeRuleHiddenName == "TRIT") discretizeRuleHidden = TS::RemapRules::TRIT;
    else if (discretizeRuleHiddenName == "NEAREST_INT") discretizeRuleHidden = TS::RemapRules::NEAREST_INT;
    else if (discretizeRuleHiddenName == "NEAREST_BIT") discretizeRuleHidden = TS::RemapRules::NEAREST_BIT;
    else if (discretizeRuleHiddenName == "NEAREST_TRIT") discretizeRuleHidden = TS::RemapRules::NEAREST_TRIT;
    else if (discretizeRuleHiddenName == "MEDIAN") discretizeRuleHidden = TS::RemapRules::MEDIAN;
    else if (discretizeRuleHiddenName == "UNIQUE") discretizeRuleHidden = TS::RemapRules::UNIQUE;
    else {
        std::cout << "  in TPM_GENERATOR world :: found unknown discretizeRuleOutput... please fix and try again. exiting." << std::endl;
        exit(1);
    }

    convertCSVListToVector(symbolsListInputPL->get(PT), symbolsListInput);
    convertCSVListToVector(symbolsListOutputPL->get(PT), symbolsListOutput);
    convertCSVListToVector(symbolsListHiddenPL->get(PT), symbolsListHidden);

    saveS2S = saveS2SPL->get(PT);
    saveBrainStructure = saveBrainStructurePL->get(PT);

    popFileColumns.clear();
    //popFileColumns.push_back("out0");
}

// the evaluate function gets called every generation. evaluate should set values on organisms datamaps
// that will be used by other parts of MABE for things like reproduction and archiving
auto TPM_GENERATORWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {
    std::cout << "In TPM_GENERATOR world:" << std::endl;
    std::cout << "\n  output mode = " << outputMode << std::endl;
    std::cout << "\n  descritize rules:" << std::endl;
    std::cout << "    input  = " << discretizeRuleInputName << std::endl;
    std::cout << "    output = " << discretizeRuleOutputName << std::endl;
    std::cout << "    hidden = " << discretizeRuleHiddenName << std::endl;
    std::cout << "\n  endian-ness = " << beforeStatesEndianPL->get(PT) << std::endl;

    int nrSamp = numberOfSamplesPL->get(PT);

    auto org = groups["root::"]->population[0];
    auto brain = org->brains["root::"];

    std::cout << "    this brain has " << nrIn << " inputs (" << std::pow(symbolsListInput.size(),nrIn) << " states)" << std::endl;
    int totalStates = std::pow(symbolsListInput.size(), nrIn);

    std::cout << "    this brain has " << nrOut << " outputs ";
    int maxOutValue = 1; // if no recurrentOutput then make sure we only iterate over one output value (which will not be used)
    if (brain->recurrentOutput) {
        std::cout << "... outputs are recurrent (" << std::pow(symbolsListOutput.size(), nrOut) << " states)" << std::endl;
        totalStates *= std::pow(symbolsListOutput.size(), nrOut);
    }
    else {
        std::cout << "... outputs are not recurrent" << std::endl;
    }

    std::cout << "    this brain has " << nrHid << " hidden (" << std::pow(symbolsListHidden.size(), nrHid) << " states)" << std::endl;
    totalStates *= std::pow(symbolsListHidden.size(), nrHid);

    std::cout << "\n    a total of " << totalStates << " input patterns will be evaluated" << std::endl;

    TS::TimeSeries Input_TS;
    TS::TimeSeries Output_TS;
    TS::TimeSeries Hidden_TS;

    Input_TS = makeTSFromSymbols(nrIn, symbolsListInput, beforeStatesLittleEndian);
    if (brain->recurrentOutput) {
        Output_TS = makeTSFromSymbols(nrOut, symbolsListOutput, beforeStatesLittleEndian);
    }
    else {
        Output_TS = { std::vector<double>(nrOut,missingSymbol) };
    }
    Hidden_TS = makeTSFromSymbols(nrHid, symbolsListHidden, beforeStatesLittleEndian);
    //std::cout << TS::TimeSeriesToString(Input_TS) << std::endl << std::endl;
    //std::cout << TS::TimeSeriesToString(Output_TS) << std::endl << std::endl;
    //std::cout << TS::TimeSeriesToString(Hidden_TS) << std::endl << std::endl;


    int popSize = groups["root::"]->population.size(); 

    // user may have loaded more then one org, itterate over all of them
    for (int i = 0; i < popSize; i++) {


        // create a shortcut to access the organism and organisms brain
        org = groups["root::"]->population[i];
        brain = org->brains["root::"];
        int orgID = org->ID;

        std::cout << "\n  working on agent with ID " << orgID << std::endl << std::endl;

        brain->setRecordActivity(true);
        
        for (auto const& inputState : Input_TS) { // for every input pattern
            for (auto const& outputState : Output_TS) { // for every output pattern (if not recurrent, there will only be 1)
                for (auto const& hiddenState : Hidden_TS) { // for every hidden pattern
                    
                    //for (auto s : { inputState,outputState,hiddenState }) {
                    //    std::cout << std::endl;
                    //    for (auto e : s) {
                    //        std::cout << " " << e;
                    //    }
                    //}
                    for (int samp = 0; samp < nrSamp; samp++) { // for some number of samples
                        brain->resetBrain();
                        brain->setInputVector(inputState);
                        if (brain->recurrentOutput) {
                            brain->setOutputVector(outputState);
                        }
                        brain->setHiddenState(hiddenState);
                        brain->update();
                    }
                }
            }
        }
        // now this brain should have all it's states, we just need to get them out!
        
        // get the time series from the brain

        auto discreetInput = TS::remapToIntTimeSeries(brain->getInputStates(), discretizeRuleInput);
        auto discreetOutput = TS::remapToIntTimeSeries(brain->getOutputStates(), discretizeRuleOutput);
        auto discreetHidden = TS::remapToIntTimeSeries(brain->getHiddenStates(), discretizeRuleHidden);
        auto lifeTimes = brain->getLifeTimes();


        std::string fileStr="";

        if (outputMode == "raw") {
            fileStr += "input,output,hidden\n";
            for (int i = 0; i < discreetHidden.size(); i++) {
                if (i < discreetInput.size()) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
                }
                else {
                    fileStr += "\"" + std::to_string(missingSymbol) + "\",";
                }
                if (i < discreetOutput.size()) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[i], ",") + "\",";
                }
                else {
                    fileStr += "\"" + std::to_string(missingSymbol) + "\",";
                }
                fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHidden[i]) + "\"\n";
            }
        }
        else if (outputModePL->get(PT) == "divided") {
            if (brain->recurrentOutput) {
                fileStr += "input,outputBefore,outputAfter,hiddenBefore,hiddenAfter\n";
                for (int i = 0; i < discreetInput.size(); i++) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[i * 2], ",") + "\","; // every other
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[(i * 2) + 1], ",") + "\","; // the other ones
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHidden[i * 2], ",") + "\","; // every other
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHidden[(i * 2) + 1], ",") + "\"\n"; // the other ones
                }
            }
            else {
                fileStr += "input,output,hiddenBefore,hiddenAfter\n";
                for (int i = 0; i < discreetInput.size(); i++) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHidden[i * 2], ",") + "\","; // every other
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHidden[(i * 2) + 1], ",") + "\"\n"; // the other ones
                }
            }
        }
        else if (outputMode == "joined") {
            TS::intTimeSeries beforeStates,afterStates;
            if (brain->recurrentOutput) {
                beforeStates = TS::Join({ discreetInput, TS::trimTimeSeries(discreetOutput, TS::Position::LAST, lifeTimes), TS::trimTimeSeries(discreetHidden, TS::Position::LAST, lifeTimes) });
                afterStates = TS::Join({ TS::trimTimeSeries(discreetOutput, TS::Position::FIRST, lifeTimes), TS::trimTimeSeries(discreetHidden, TS::Position::FIRST, lifeTimes) });
            }
            else {
                beforeStates = TS::Join(discreetInput, TS::trimTimeSeries(discreetHidden, TS::Position::LAST, lifeTimes));
                afterStates = TS::Join(discreetOutput, TS::trimTimeSeries(discreetHidden, TS::Position::FIRST, lifeTimes));
            }
            fileStr += "before,after\n";
            for (int i = 0; i < discreetInput.size(); i++) {
                fileStr += "\"" + TS::TimeSeriesSampleToString(beforeStates[i], ",") + "\",";
                fileStr += "\"" + TS::TimeSeriesSampleToString(afterStates[i], ",") + "\"\n";
            }
        }
        else if (outputMode == "packed") {
            
            std::vector<int> emptyIn(nrIn, missingSymbol);
            std::vector<int> emptyOut(nrOut, missingSymbol);
            int totalSamplesCount = discreetInput.size();
            TS::intTimeSeries emptyInTS(totalSamplesCount, emptyIn);
            TS::intTimeSeries emptyOutTS(totalSamplesCount, emptyOut);

            TS::intTimeSeries beforeStates, afterStates;

            if (brain->recurrentOutput) {
                beforeStates = TS::Join({ discreetInput, TS::trimTimeSeries(discreetOutput, TS::Position::LAST, lifeTimes), TS::trimTimeSeries(discreetHidden, TS::Position::LAST, lifeTimes) });
                afterStates = TS::Join({ emptyInTS, TS::trimTimeSeries(discreetOutput, TS::Position::FIRST, lifeTimes), TS::trimTimeSeries(discreetHidden, TS::Position::FIRST, lifeTimes) });
            }
            else {
                beforeStates = TS::Join({ discreetInput, emptyOutTS, TS::trimTimeSeries(discreetHidden, TS::Position::LAST, lifeTimes) });
                afterStates = TS::Join({ emptyInTS, discreetOutput, TS::trimTimeSeries(discreetHidden, TS::Position::FIRST, lifeTimes) });
            }

            fileStr += "before,after\n";
            for (int i = 0; i < discreetInput.size(); i++) {
                fileStr += "\"" + TS::TimeSeriesSampleToString(beforeStates[i], ",") + "\",";
                fileStr += "\"" + TS::TimeSeriesSampleToString(afterStates[i], ",") + "\"\n";
            }
        }
        FileManager::writeToFile("TPM_id_" + std::to_string(org->ID) + ".csv", fileStr);
        
        if (saveS2S) {
            std::string fileName = "StateToState.txt";
            if (brain->recurrentOutput) {
                S2S::saveStateToState({ discreetHidden, discreetOutput }, { discreetInput }, lifeTimes, "StateToState_H_O__I_id_" + std::to_string(org->ID) + ".dot");
                S2S::saveStateToState({ discreetHidden }, { discreetInput }, lifeTimes, "StateToState_H__I_id_" + std::to_string(org->ID) + ".dot");
            }
            else {
                S2S::saveStateToState({ discreetHidden, TS::extendTimeSeries(discreetOutput, lifeTimes, {0}, TS::Position::FIRST) }, { discreetInput }, lifeTimes, "StateToState_H_O__I_id_" + std::to_string(org->ID) + ".dot");
                //S2S::saveStateToState({ discreetHidden }, { discreetOutput, discreetInput }, lifeTimes, "StateToState_H__O_I_" + std::to_string(org->ID) + ".dot");
                S2S::saveStateToState({ discreetHidden }, { discreetInput }, lifeTimes, "StateToState_H__I_id_" + std::to_string(org->ID) + ".dot");
            }
        }

        if (saveBrainStructure) {
            brain->saveStructure("brainStructure_id_"+std::to_string(org->ID) + ".dot");
        }

    } // end of population loop
    std::cout << "\n\n  TPM_GENERATOR DONE." << std::endl;

}

// the requiredGroups function lets MABE know how to set up populations of organisms that this world needs
auto TPM_GENERATORWorld::requiredGroups() -> unordered_map<string,unordered_set<string>> {
	return { { "root::", { "B:root::," + std::to_string(nrIn) + "," +std::to_string(nrOut) } } };
}
