#ifndef __Job__	
#define __Job__


class Job
{
	uint32_t source;
	uint32_t target;
	bool kind;
public:
	Job(uint32_t source_ , uint32_t target_,bool k);
	
};