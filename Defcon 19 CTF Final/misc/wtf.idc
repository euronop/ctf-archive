#include <idc.idc>


static main(void)
{
	auto i;

	i = 0x15;

	while(1)
	{
		PatchDword(i, Dword(i) ^ 0x5F457BE3);
		i = i + 4;
		if( i > 0x404 )
			break;
	}
}