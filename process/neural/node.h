#ifndef _NODE_H
#define _NODE_H

#include <vector>

using namespace std;


class Node
{
public:
	Node();
	Node( void (*fn)(float, float &, float & ) );
	void SetFn( void (*fn)(float, float &, float & ) );

	void CalculateOutputs();

	void PrintTree( int Depth = 0 );

	//Returns /this/
	Node * AttachOutput( Node * n );

	//Returns /this/
	Node * AttachInput( Node * n );

	void Backprop( float wrongness, float training = 0.05 );

	std::vector< Node * > Outputs;

	std::vector< float >  InputWeights;
	std::vector< Node * > Inputs;

//	float Sigma;

//	float Offset;
	float Output;
	float Delta;
	float LastSum;

	bool Fixed;

	void (*Function)(float x, float & y, float & dydx );
};

#endif

//Copyright Charles Lohr 2009, Under the MIT/x11 or the NewBSD Licenses.

