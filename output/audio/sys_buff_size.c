#include<stdlib.h>

float sys_buff_size(float *size)
{
	static float buff_size=0;
	if ( size != NULL ){
		buff_size=*size;
	}

	return buff_size;
}
