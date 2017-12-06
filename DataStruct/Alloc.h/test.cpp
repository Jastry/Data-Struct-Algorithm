#include "alloc.hpp"

int main()
{
	
	/*char* ret = (char*)SpatialConfigurator::__MallocAllocTemplate<0>::Allocate(10);
	SpatialConfigurator::__MallocAllocTemplate<0>::Deallocate(ret,0);
	ret = NULL;
	ret = (char*)SpatialConfigurator::__MallocAllocTemplate<0>::OOM_Malloc(10);
	SpatialConfigurator::__MallocAllocTemplate<0>::Deallocate(ret, 0);*/
	
	size_t a = 20;
	char *ret = SpatialConfigurator::__DefaultAllocTemplate<0, 0>::ChunkAlloc(20,a);
	ret = SpatialConfigurator::__DefaultAllocTemplate<0, 0>::Refill(20);
	ret = (char*)SpatialConfigurator::__DefaultAllocTemplate<0, 0>::Allocate(20);

	return 0;
}
