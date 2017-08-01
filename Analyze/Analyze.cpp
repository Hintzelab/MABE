//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full licence, visit:
//         github.com/Hintzelab/MABE/wiki/License

//#include "Analyze.h"
//
//double Analyse::mutualInformation(vector<int> A, vector<int> B) {
//	set<int> nrA, nrB;
//	set<int>::iterator aI, bI;
//	map<int, map<int, double> > pXY;
//	map<int, double> pX, pY;
//	size_t i;
//	double c = 1.0 / (double) A.size();
//	double I = 0.0;
//	for (i = 0; i < A.size(); i++) {
//		nrA.insert(A[i]);
//		nrB.insert(B[i]);
//		pX[A[i]] = 0.0;
//		pY[B[i]] = 0.0;
//	}
//	for (aI = nrA.begin(); aI != nrA.end(); aI++)
//		for (bI = nrB.begin(); bI != nrB.end(); bI++) {
//			pXY[*aI][*bI] = 0.0;
//		}
//	for (i = 0; i < A.size(); i++) {
//		pXY[A[i]][B[i]] += c;
//		pX[A[i]] += c;
//		pY[B[i]] += c;
//	}
//	for (aI = nrA.begin(); aI != nrA.end(); aI++)
//		for (bI = nrB.begin(); bI != nrB.end(); bI++)
//			if ((pX[*aI] != 0.0) && (pY[*bI] != 0.0) && (pXY[*aI][*bI] != 0.0))
//				I += pXY[*aI][*bI] * log2(pXY[*aI][*bI] / (pX[*aI] * pY[*bI]));
//	return I;
//}
//
//double Analyse::ei(vector<int> A, vector<int> B, int theMask) {
//	set<int> nrA, nrB;
//	set<int>::iterator aI, bI;
//	map<int, map<int, double> > pXY;
//	map<int, double> pX, pY;
//	size_t i;
//	double c = 1.0 / (double) A.size();
//	double I = 0.0;
//	for (i = 0; i < A.size(); i++) {
//		nrA.insert(A[i] & theMask);
//		nrB.insert(B[i] & theMask);
//		pX[A[i] & theMask] = 0.0;
//		pY[B[i] & theMask] = 0.0;
//	}
//	for (aI = nrA.begin(); aI != nrA.end(); aI++)
//		for (bI = nrB.begin(); bI != nrB.end(); bI++) {
//			pXY[*aI][*bI] = 0.0;
//		}
//	for (i = 0; i < A.size(); i++) {
//		pXY[A[i] & theMask][B[i] & theMask] += c;
//		pX[A[i] & theMask] += c;
//		pY[B[i] & theMask] += c;
//	}
//	for (aI = nrA.begin(); aI != nrA.end(); aI++)
//		for (bI = nrB.begin(); bI != nrB.end(); bI++)
//			if ((pX[*aI] != 0.0) && (pY[*bI] != 0.0) && (pXY[*aI][*bI] != 0.0))
//				I += pXY[*aI][*bI] * log2(pXY[*aI][*bI] / (pY[*bI]));
//	return -I;
//}
//
//double Analyse::computeAtomicPhi(vector<int> A, int states) {
//	int i;
//	double P, EIsystem;
//	vector<int> T0, T1;
//	T0 = A;
//	T1 = A;
//	T0.erase(T0.begin() + T0.size() - 1);
//	T1.erase(T1.begin());
//	EIsystem = ei(T0, T1, (1 << states) - 1);
//	P = 0.0;
//	for (i = 0; i < states; i++) {
//		double EIP = ei(T0, T1, 1 << i);
//		//		cout<<EIP<<endl;
//		P += EIP;
//	}
//	//	cout<<-EIsystem+P<<" "<<EIsystem<<" "<<P<<" "<<T0.size()<<" "<<T1.size()<<endl;
//	return -EIsystem + P;
//}
//
//double Analyse::computeR(vector<int> sensor, vector<int> brain, vector<int> environment, vector<int> total) {
//	double Iwh = mutualInformation(environment, brain);
//	double Iws = mutualInformation(environment, sensor);
//	double Ish = mutualInformation(sensor, brain);
//	double Hh = entropy(brain);
//	double Hs = entropy(sensor);
//	double Hw = entropy(environment);
//	double Hhws = entropy(total);
//	double delta = Hhws + Iwh + Iws + Ish - Hh - Hs - Hw;
//	double R = Iwh - delta;
//	return R;
//}
//
//double Analyse::entropy(vector<int> list) {
//	map<int, double> p;
//	map<int, double>::iterator pI;
//	size_t i;
//	double H = 0.0;
//	double c = 1.0 / (double) list.size();
//	for (i = 0; i < list.size(); i++)
//		p[list[i]] += c;
//	for (pI = p.begin(); pI != p.end(); pI++) {
//		H += p[pI->first] * log2(p[pI->first]);
//	}
//	return -1.0 * H;
//}
//
//int Analyse::getMaxConnectivityDensity(vector<vector<int>> M) {
//	int m = M[0][0];
//	for (auto x : M)
//		for (int v : x)
//			if (v > m)
//				m = v;
//	return m;
//}
//
//double Analyse::getMeanConnectivityDensity(vector<vector<int>> M) {
//	double m;
//	int n;
//	for (auto x : M)
//		for (int v : x) {
//			m += (double) v;
//			n++;
//		}
//	return m / (double) n;
//}
//
//double Analyse::getDensity(vector<vector<int>> M) {
//	double m;
//	int n;
//	for (auto x : M)
//		for (int v : x) {
//			if (v > 0)
//				m++;
//			n++;
//		}
//	return m / (double) n;
//}

