#include "./type.h"
#include <hls_math.h>


#ifndef ACTIVATION
#define ACTIVATION

DataType mysigmoid(DataType var){
#pragma HLS INLINE
//#pragma HLS PIPELINE II=1
	//return 1.0/(1 + hls::exp(-var));

    DataType a, b;

	if(var <= -8){ a = 0; b = 0 ;  }
	else if( var > -8 && var <= -7) { a = 0.006; b = 0.0048; }
	else if( var > -7 && var <= -6) { a = 0.0015; b = 0.0116; }
	else if( var > -6 && var <= -5) { a = 0.0042; b = 0.0271; }
	else if( var > -5 && var <= -4) { a = 0.0111; b = 0.0615; }
	else if( var > -4 && var <= -3) { a = 0.0291; b = 0.1322; }
	else if( var > -3 && var <= -2) { a = 0.0711; b = 0.2562; }
	else if( var > -2 && var <= -1) { a = 0.1495; b = 0.4107; }
	else if( var > -1 && var <= 0) { a = 0.2326; b = 0.4962; }
	else if( var > 0 && var <= 1) { a = 0.2326; b = 0.5038; }
	else if( var > 1 && var <= 2) { a = 0.1495; b = 0.5893; }
	else if( var > 2 && var <= 3) { a = 0.0711; b = 0.7438; }
	else if( var > 3 && var <= 4) { a = 0.0291; b = 0.8678; }
	else if( var > 4 && var <= 5) { a = 0.0111; b = 0.9385; }
	else if( var > 5 && var <= 6) { a = 0.0042; b = 0.9729; }
	else if( var > 6 && var <= 7) { a = 0.0015; b = 0.9884; }
	else if( var > 7 && var <= 8) { a = 0.0006; b = 0.9952; }
	else  { a = 0.0; b = 1.0; }

    return (a*var + b);
}



DataType mytanh(DataType var){
#pragma HLS INLINE
//#pragma HLS PIPELINE II=1
	//return (hls::exp(var) - hls::exp(-var))/(hls::exp(var) + hls::exp(-var));
    DataType a, b;

    

	if(var <= -6) { a = 0; b = -1 ; }
	else if( var > -6 && var <= -5) { a = 0.0001; b = -0.9996 ; }
	else if( var > -5 && var <= -4) { a = 0.0005; b = -0.9973 ; }
	else if( var > -4 && var <= -3) { a = 0.0040; b = -0.9838 ; }
	else if( var > -3 && var <= -2) { a = 0.0293; b = -0.9111 ; }
	else if( var > -2 && var <= -1) { a = 0.1940; b = -0.5999 ; }
	else if( var > -1 && var <= 0) { a = 0.7714; b = -0.0475 ; }
	else if( var > 0 && var <= 1) { a = 0.7714; b = 0.0475 ; }
	else if( var > 1 && var <= 2) { a = 0.1940; b = 0.5999 ; }
	else if( var > 2 && var <= 3) { a = 0.0293; b = 0.9111; }
	else if( var > 3 && var <= 4) { a = 0.0040; b = 0.9838; }
	else if( var > 4 && var <= 5) { a = 0.0005; b = 0.9973; }
	else if( var > 5 && var <= 6) { a = 0.0001; b = 0.9996; }
	else { a = 0.0; b = 1.0; }
		
    return (a*var + b);
    
}


#endif