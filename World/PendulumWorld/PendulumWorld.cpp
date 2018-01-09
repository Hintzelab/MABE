//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "PendulumWorld.h"
#include "limits.h"
#define PI 3.14159265359
#include <cmath>

shared_ptr<ParameterLink<string>> PendulumWorld::groupNamePL = Parameters::register_parameter("WORLD_PENDULUM_NAMES-groupName", (string)"root::", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> PendulumWorld::brainNamePL = Parameters::register_parameter("WORLD_PENDULUM_NAMES-brainName", (string)"root::", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<int>> PendulumWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_PENDULUM-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");

class Pendulum{
public:
	double gravity,masscart,masspole,total_mass,length,polemass_length,force_mag,tau;
	double state[4],high[4];
	double theta_threshold_radians;
	double x_threshold;
	
	Pendulum(){
		gravity = 9.8;
		masscart = 1.0;
		masspole = 0.1;
		total_mass = masspole +masscart;
		length = 0.5;// # actually half the pole's length
		polemass_length = masspole * length;
		force_mag = 10.0;
		tau = 0.02;//  # seconds between state updates
		theta_threshold_radians = 12.0 * 2.0 * PI / 360.0;
		x_threshold = 2.4;
		// Angle limit set to 2 * theta_threshold_radians so failing observation is still within bounds
		high[0]=x_threshold*2.0;
		high[1]=numeric_limits<double>::max();
		high[2]=theta_threshold_radians * 2.0;
		high[3]=high[1];

	}
	~Pendulum(){}
	
	void resetState(){
		for(int i=0;i<4;i++)
			state[i]=Random::getDouble(-0.05, 0.05);
	}
	void step(double action){
		double x, x_dot, theta, theta_dot;
		x=state[0]; x_dot=state[1]; theta=state[2]; theta_dot=state[3];
		double force=force_mag*action;
		double costheta = cos(theta);
		double sintheta = sin(theta);
		double temp = (force + polemass_length * theta_dot * theta_dot * sintheta) / total_mass;
		double thetaacc = (gravity * sintheta - costheta* temp) / (length * (4.0/3.0 - masspole * costheta * costheta / total_mass));
		double xacc  = temp - polemass_length * thetaacc * costheta / total_mass;
		x  = x + tau * x_dot;
		x_dot = x_dot + tau * xacc;
		theta = theta + tau * theta_dot;
		theta_dot = theta_dot + tau * thetaacc;
		state[0] = x; state[1]=x_dot; state[2]=theta; state[3]=theta_dot;
	}
	bool done(){
		if ((state[0] < -x_threshold) || (state[0] > x_threshold) || (state[2]<-theta_threshold_radians) || (state[2]>theta_threshold_radians))
			return true;
		return false;
	}
};

PendulumWorld::PendulumWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	groupName = groupNamePL->get(_PT);
	brainName = brainNamePL->get(_PT);

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically because _VAR)
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void PendulumWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	//cout << "in test world evalSolo" << endl;
	auto brain = org->brains[brainName];
	Pendulum *P=new Pendulum();
	vector<double> answers;
	double score=0.0;
	for(int reps=evaluationsPerGenerationPL->get(PT); reps>=0; --reps){
		P->resetState();
		int t=0;
		brain->resetBrain();
		answers.clear();
		while((t<500)&&(!P->done())){
			for(int i=0;i<8;i++){
				for(int j=0;j<4;j++)
					brain->setInput(j, P->state[j]);
				brain->update();
			}
			double action=brain->readOutput(0);
			if((isnan(action))||(isinf(action)))
			   P->step(0.0);
		   else
			   P->step(action);
			t++;
			answers.push_back(action);
		}
		score+=(double)t;
	}
    org->dataMap.set("score",score);
	org->dataMap.set("score",score/evaluationsPerGenerationPL->get(PT));
}
