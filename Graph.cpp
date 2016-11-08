#include "Graph.h"
#include "list.h"
///////////////////////////////////////////////////////////////////////////////
/* GRAPH */
///////////////////////////////////////////////////////////////////////////////
NodeIndex* Graph::getOutIndex()
{
	return &out_index;
}
///////////////////////////////////////////////////////////////////////////////
NodeIndex* Graph::getIncIndex()
{
	return &inc_index;
}
//////////////////////////////////////////////////////////////////////////////
Buffer* Graph::getOutBuffer()
{
	return &out_buffer;
}
//////////////////////////////////////////////////////////////////////////////
Buffer* Graph::getIncBuffer()
{
	return &inc_buffer;
}
//////////////////////////////////////////////////////////////////////////////
bool Graph::Insert(NodeIndex *index,Buffer *buffer, uint32_t id,uint32_t id2)
{	

	while(id>=index->getSize()) {
		index->reallocation();
	}
	index->setCount(id);
	unsigned int last=buffer->getLast(); 
	if(index->getPosition(id)==-1)
	{
		if(last>=buffer->getSize())
			buffer->reallocation();
		int offset=buffer->InsertFirstNode(id2);
		index->InsertNode(id,offset);
	}
	else
	{
		int position = index->getPosition(id);
		bool res=buffer->getListNode(position)->Insert(id2);
		if(res==false)
			if(index->getLastBucket(id)!=0)
			{
				res=buffer->getListNode(index->getLastBucket(id))->Insert(id2);
			}
		if(res==false)
		{
			if(last>=buffer->getSize()) {
				buffer->reallocation();
			}

			if(buffer->getListNode(position)->getOffset()==-1)
				buffer->getListNode(position)->setOffset(last);
			else if(buffer->getListNode(index->getLastBucket(id))->getOffset()==-1)
				buffer->getListNode(index->getLastBucket(id))->setOffset(last);
			index->setLastBucket(id,last);
			buffer->getListNode(last)->Insert(id2);
			buffer->setLast(last+1);
		}
	}
	return true;
}

bool Graph::search(uint32_t id,uint32_t id2)
{
	int count1=out_index.getCount(id);
	int count2=inc_index.getCount(id2);
	if(count1<count2)
	{
		int offset = out_index.getPosition(id);
		if(offset==-1)
			return false;
		uint32_t* neighbors = out_buffer.getListNode(offset)->getNeighbors();
		int offset2 = out_buffer.getListNode(offset)->getOffset();		
		for(int j = 0; j<out_buffer.getListNode(offset)->getLastNeighbor(); j++)
		{
			if(neighbors[j]==id2)
				return true;
		}
		while(offset2!=-1) {
			neighbors = out_buffer.getListNode(offset2)->getNeighbors();
			for(int j = 0; j<out_buffer.getListNode(offset2)->getLastNeighbor(); j++)
			{
				if(neighbors[j]==id2)
					return true;
			}
			offset2 = out_buffer.getListNode(offset2)->getOffset();
		}

	}
	else
	{
		int offset = inc_index.getPosition(id);
		if(offset==-1)
			return false;
		uint32_t* neighbors = inc_buffer.getListNode(offset)->getNeighbors();
		int offset2 = inc_buffer.getListNode(offset)->getOffset();		
		for(int j = 0; j<inc_buffer.getListNode(offset)->getLastNeighbor(); j++)
		{
			if(neighbors[j]==id)
				return true;
		}
		while(offset2!=-1) {
			neighbors = inc_buffer.getListNode(offset2)->getNeighbors();
			for(int j = 0; j<inc_buffer.getListNode(offset2)->getLastNeighbor(); j++)
			{
				if(neighbors[j]==id)
					return true;
			}
			offset2 = inc_buffer.getListNode(offset2)->getOffset();
		}
	}
	return false;
}

int Graph::BBFS(uint32_t start , uint32_t target)
{
	List out_oura;
	List inc_oura;
	int count=0;
	bool result;
	int bigger;
	if(out_index.getSize()>inc_index.getSize())
		bigger=out_index.getSize();
	else
		bigger=inc_index.getSize();
	int * visited=(int *)malloc(sizeof(int)*bigger);
	for(int i=0;i<out_index.getSize();i++)
		visited[i]=-1;
	if(start==target)
	{
		free (visited);
		return 0;
	}
	out_oura.push(start);
	inc_oura.push(target);
	while(!out_oura.empty() && !inc_oura.empty())
	{
		if(Update(out_index,out_buffer,count,out_oura,visited,0) || Update(inc_index,inc_buffer,count,inc_oura,visited,1))
		{
			free(visited);
			return count;
		}
	}	
	free (visited);
	return -1;
}

bool Graph::Update(NodeIndex &index,Buffer &buffer,int &count,List &oura,int *visited,int situation)
{
	unsigned int off;
	list_node * cells;
	uint32_t* neigh;
	int id;
	if(!oura.empty())
	{
		int size=oura.get_size();
		count++;
		for(int i=0;i<size;i++)
		{
			id=oura.remove();
			off=index.getPosition(id);
			while(off!=-1)
			{
				cells=buffer.getListNode(off);
				neigh=cells->getNeighbors();
				for(int i=0;i<cells->getLastNeighbor();i++)
				{
						if(visited[neigh[i]]==situation)
							return true;
						if(visited[neigh[i]]!=1-situation)
						{
							oura.push(neigh[i]);
							visited[neigh[i]]=1-situation;
						}
				}
				off=cells->getOffset();
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
