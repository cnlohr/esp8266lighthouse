#include "node.h"
#include <stdlib.h>
#include <stdio.h>

Node::Node()
{
	Fixed = false;
}

void Node::SetFn( void (*fn)(float x, float & y, float & dydx ) )
{
	Function = fn;
	Function( 0, Output, Delta );
	InputWeights.push_back( ((float)(rand()%1000))/500. - 1. );
}


Node::Node( void (*fn)(float x, float & y, float & dydx ) )
{
	Function = fn;
	Function( 0, Output, Delta );
//	Offset = ((float)(rand()%1000))/500. - 1.;

	Fixed = false;

	InputWeights.push_back( ((float)(rand()%1000))/500. - 1. );
}

void Node::PrintTree( int Depth )
{
	//printf( "%*c(%f + %f) -> (%f %f))\n", Depth+1, 0, LastSum - Offset, Offset, Output, Delta );
}

void Node::CalculateOutputs()
{
	if( Fixed )
		return;

//	float iTotal = 0;
//
//	for( unsigned i = 0; i < InputWeights.size(); i++ )
//		iTotal += (InputWeights[i] > 0)?(InputWeights[i]):(-InputWeights[i]);
//
//	for( unsigned i = 0; i < InputWeights.size(); i++ )
//		InputWeights[i] = InputWeights[i] / (iTotal/2.);

	LastSum = 0;

	LastSum += InputWeights[0];

//	printf( "\nCalc: " );
	for( unsigned i = 0; i < Inputs.size(); i++ )
	{
		LastSum += Inputs[i]->Output * InputWeights[i+1];
//		printf( "%f * %f + %f = %f;", Inputs[i]->Output, InputWeights[i+1], Offset, Inputs[i]->Output * InputWeights[i+1] + Offset  );
	}
	Function( LastSum, Output, Delta );
//	printf( "; Total: %f (%f / %f)\n", LastSum, Output, Delta );
}

void Node::Backprop( float wrongness, float training )
{
	if( Fixed )
		return;


	InputWeights[0] += wrongness * Delta * training;
//	printf( "[o] %f += %f * %f\n", InputWeights[0], wrongness, Delta );

	for( unsigned i = 0; i < Inputs.size(); i++ )
	{
		InputWeights[i+1] += wrongness * Delta * training * Inputs[i]->Output;
//		printf( "[%d] %f += %f * %f\n", i, InputWeights[i+1], wrongness, Delta );
		Inputs[i]->Backprop( wrongness * InputWeights[i+1], training );
	}
}

Node * Node::AttachOutput( Node * n )
{
	Outputs.push_back( n );
	n->AttachInput( this );
	return this;
}

Node * Node::AttachInput( Node * n )
{
//	printf( "%p  adding %p as input\n", this, n );
	Inputs.push_back( n );
	InputWeights.push_back( ((float)(rand()%1000))/500. - 1. );
	return this;
}


//Copyright Charles Lohr 2009, Under the MIT/x11 or the NewBSD Licenses.

