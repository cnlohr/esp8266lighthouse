#include "node.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

void Sigmoid( float x, float & y, float & dydx )
{
	const float alt = 2.;
	dydx = alt / ( exp( x ) + exp( -x ) );
	y = ( exp( x * alt ) - 1 ) / ( exp( x * alt ) + 1 );
	dydx = dydx * dydx;
}

void Linear( float x, float & y, float & dydx )
{
	y = x;
	dydx = 1;
}


#define NR_INPUTS 4
#define NR_MIDS 6
#define NR_MIDS2 4
#define ITERS 1000000

struct MarkElements
{
	uint32_t ootime;
	uint32_t swtime;
	double calc;
	float center;
	float ins[NR_INPUTS];
};

int main( int argc, char ** argv )
{
	srand( 4 );


	if( argc != 3 )
	{
		fprintf( stderr, "Error: Usage: [csv to read] [offset, in lines] to start subtracting at.\n" );
		exit( -1 );
	}

	Node Inputs[NR_INPUTS];
	Node Middle[NR_MIDS];
	Node Middle2[NR_MIDS2];
	Node Output( Sigmoid );

	float x, y, dydx;
	int i, j;

	for( i = 0; i < NR_INPUTS; i++ )
	{
		Inputs[i].Fixed = true;
		Inputs[i].SetFn( Linear );
		for( j = 0; j < NR_MIDS; j++ )
		{
			Inputs[i].AttachOutput( &Middle[j] );
			Middle[j].AttachInput( &Inputs[i] );
		}
	}


	for( i = 0; i < NR_MIDS; i++ )
	{
		if( i % 2 )
			Middle[i].SetFn( Sigmoid );
		else
			Middle[i].SetFn( Linear );

		for( j = 0; j < NR_MIDS2; j++ )
		{
			Middle[i].AttachOutput( &Middle2[j] );
			Middle2[j].AttachInput( &Middle[i] );
		}
	}

	for( i = 0; i < NR_MIDS2; i++ )
	{
		if( i%2 )
			Middle2[i].SetFn( Sigmoid );
		else
			Middle2[i].SetFn( Linear );


		Output.AttachInput( &Middle2[i] );
		Middle2[i].AttachOutput( &Output );
	}

	int skip = atoi(argv[2] );
	FILE * f = fopen( argv[1], "r" );
	if( !f )
	{
		fprintf( stderr, "Error: Cannot open %s for reading\n", argv[1] );
		exit( -2 );
	}

	int line = 0;
	MarkElements e;
	vector< MarkElements > elems;
	double calcavg = 0;

	while( !feof( f ) )
	{
		line++;

		int id;
		uint32_t first_transition;
		int len;
		float center;
		float frequency;
		int  strength;
		int  trans1;
		int  trans2;
		int trans3;
		int trans4; 
		int r = fscanf( f,"%d\t%d\t%d\t%f\t%f\t%d\t%d\t%d\t%d\t%d\n", &id, &first_transition, &len, &center, &frequency, &strength, &trans1, &trans2, &trans3, &trans4 );
		if( r != 10 )
		{
			printf( "Error reading line %d\n", line );
			break;
		}
		int l = line - skip - 1;
		if( l < 0 )
		{
			continue;
		}
		if( l & 2 )
		{
			if( l & 1 )
			{
				e.swtime = first_transition;
				e.center = center;
				e.calc = e.swtime - e.ootime + e.center;
				calcavg += e.calc;
				e.ins[2] = trans1;
				e.ins[3] = trans2;
				elems.push_back( e );
			}
			else
			{
				e.ootime = first_transition;
				e.ins[0] = trans1;
				e.ins[1] = trans2;
				printf( "%f\n", e.ins[3] );
			}
		}
	}

	calcavg /= elems.size();
	//printf( "Calculated average: %f\n", calcavg );
	for( i = 0; i < elems.size(); i++ )
	{
		struct MarkElements & e =  elems[i];
		e.calc -= calcavg;
		e.calc /= 40;
		//printf( "%f\n", e.calc );
	}

	float rmse = 0, zmse = 0;

	for( i = 0; i < ITERS; i++ )
	{
		int eid = rand()%elems.size();
		MarkElements &e  = elems[eid];

		for( j = 0; j < NR_INPUTS; j++ )
		{
			Inputs[j].Output = e.ins[j];
		}
		for( j = 0; j < NR_MIDS; j++ )
		{
			Middle[j].CalculateOutputs();
		}
		Output.CalculateOutputs();

		float weight = ((ITERS/100.0)/(i+ITERS/100.0));

		float Wrongness = e.calc - Output.Output;
		float MSE = fabs(Wrongness*Wrongness);
		float ZMSE = fabs( e.calc*e.calc );
		rmse = MSE * .01 + rmse * .99;
		zmse = ZMSE * .01 + zmse * .99;
		printf( "%14f %14f %14f %14f %14f\n", Output.Output, e.calc, rmse, zmse, weight );

		Output.Backprop( Wrongness, weight );
	}

	FILE * fs = fopen( "outdata.tsv", "w" );
	for( i = 0; i < elems.size(); i++ )
	{
		struct MarkElements & e =  elems[i];

		for( j = 0; j < NR_INPUTS; j++ )
		{
			Inputs[j].Output = e.ins[j];
		}
		for( j = 0; j < NR_MIDS; j++ )
		{
			Middle[j].CalculateOutputs();
		}
		Output.CalculateOutputs();

		fprintf( fs,"%f\t%f\t%f\n", e.calc * 40, Output.Output * 40, (e.calc - Output.Output)*40 );
	}
	fclose( fs );
}
//Copyright Charles Lohr 2009, Under the MIT/x11 or the NewBSD Licenses.

