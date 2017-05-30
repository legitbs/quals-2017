typedef unsigned int size_t;

void *memcpy( void * dest, const void *src, size_t num )
{
	if ( num >= 4 )
	{
		size_t pos = 0;
		
		for ( pos = 0; pos < (num&0xFFFFFFFC0); pos+=4 )
		{
			*((unsigned int *)(dest+pos)) = *((unsigned int *)(src+pos));
		}

		for ( ; pos < num; pos++ )
		{
			((char*)dest)[pos] = ((char*)src)[pos];
		}
	}
	else
	{
		size_t pos = 0;
		for ( pos = 0; pos < num; pos++ )
		{
			((char*)dest)[pos] = ((char*)src)[pos];
		}
	}

	return dest;
}
