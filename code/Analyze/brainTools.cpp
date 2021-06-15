#include "brainTools.h"


void BRAINTOOLS::saveStateToState(std::shared_ptr<AbstractBrain> brain, std::string(fileName), TS::RemapRules remapRule, std::vector<double> ruleParameter) {
    if (brain->recurrentOutput) {
        S2S::saveStateToState({ TS::remapToIntTimeSeries(brain->getHiddenStates(),remapRule,ruleParameter), TS::remapToIntTimeSeries(brain->getOutputStates(),remapRule,ruleParameter) },
            { TS::remapToIntTimeSeries(brain->getInputStates(),remapRule,ruleParameter) }, brain->getLifeTimes(), fileName);
    }
    else {
        auto outputStates = TS::remapToIntTimeSeries(brain->getOutputStates(), remapRule, ruleParameter);
        auto inputStates = TS::remapToIntTimeSeries(brain->getInputStates(), remapRule, ruleParameter);
        auto hiddenStates = TS::remapToIntTimeSeries(brain->getHiddenStates(), remapRule, ruleParameter);
        auto lifeTimes = brain->getLifeTimes();
        S2S::saveStateToState({ hiddenStates, TS::extendTimeSeries(outputStates, lifeTimes, {0}, TS::Position::FIRST) }, { inputStates }, lifeTimes, "H_O__I_" + fileName);
        S2S::saveStateToState({ hiddenStates }, { outputStates, inputStates }, lifeTimes, "H__O_I_" + fileName);
        S2S::saveStateToState({ hiddenStates }, { inputStates }, lifeTimes, "H_I_" + fileName);
    }
}

double BRAINTOOLS::getR(std::shared_ptr<AbstractBrain> brain, TS::intTimeSeries worldFeatures, TS::RemapRules remapRule, std::vector<double> ruleParameter) {
    return ENT::ConditionalMutualEntropy(
        worldFeatures,
        TS::trimTimeSeries(TS::remapToIntTimeSeries(brain->getHiddenStates(), remapRule, ruleParameter), TS::Position::FIRST, brain->getLifeTimes()),
        TS::remapToIntTimeSeries(brain->getHiddenStates(), remapRule, ruleParameter)
    );
}
