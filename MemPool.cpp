/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file MemPool.cpp
 * @author liujun05(com@baidu.com)
 * @date 2014/02/14 15:11:20
 * @brief 
 *  
 **/

#include "MemPool.h"


//构造函数，就是传递参数
MemPool::MemPool(uint32 block_size = BLOCK_SIZE + sizeof(block_t) , uint32 block_num= BLOCK_NUM, uint32 slice_size=SLICE_SIZE)
{
    this->m_max_block_size = block_size;
    this->m_max_slice_size = slice_size;
    this->m_max_block_num = block_num;
    //最大的片数目
    this->m_max_slice_num = ( block_size - sizeof(block_t) ) / slice_size;
}

mempool_key_t MemPool::alloc_mem(uint32 size)
{
    mempool_key_t key;
    //如果大于最大block_size，则不允许分配(可以改进成分配多个block)
    if( size + sizeof(block_t) > this->m_max_block_size ){
        return INVALID_MEM_KEY; 
    }

    //如果没有，那么重新申请
    uint32 slice_num = (size % this->m_max_slice_size) ? size/this->m_max_slice_size + 1 : size/this->m_max_slice_size;
    
    //首先从free_blocks上分配
    vector <mempool_key_t*>::iterator iter;
    for(iter = m_free_block_nodes.begin(); iter != m_free_block_nodes.end(); iter++)
    {
        block_t* block = this->m_blocks[(*iter)->block_index];
        //如果大小小于block的size，就可以直接使用这个block
        if( block-> block_size > size + sizeof(block_t))
        {
            //已经分配大小
            block->slice_alloc = slice_num; 
            //加入到work链表
            m_work_blocks.push_back(block);
            //从free链表删除
            m_free_block_nodes.erase(iter);
            //key的index
            key.block_index = block->block_index;
            key.slice_index = 0;
            return key;
        }
    }

    //在从work_list上找空间
    for(int i=0; i<m_work_blocks.size() ; i++){
        //如果还有额外空间
        if(m_work_blocks[i]-> block_size - m_work_blocks[i]->slice_alloc * this->m_max_slice_size > slice_num * this->m_max_slice_size)
        {
            //block_index
            key.block_index = m_work_blocks[i]->block_index;
            //slice_index恰好是已经分配的slice_alloc
            key.slice_index = m_work_blocks[i]->slice_alloc;
            //已经分配的需要加上slice_num
            m_work_blocks[i]->slice_alloc += slice_num;
            return key;
        }
    }

    //分配一个新的block,全部用完
    block_t* block = (block_t*)malloc(sizeof(block_t) + slice_num * this->m_max_slice_size );
    block->block_size = slice_num * this->m_max_slice_size + sizeof(block_t) ;
    block->slice_num = slice_num;
    block->slice_alloc = slice_num;
    block->block_index = m_blocks.size();
    m_blocks.push_back(block);
    m_work_blocks.push_back(block);
    key.block_index = block->block_index;
    key.slice_index = 0;
}


int MemPool::free_mem(mempool_key_t key)
{
    //删除都是直接删除一个block(其他数据要拷贝到其他block中)
    
    vector <block_t*>::iterator iter;
    for(iter = m_work_blocks.begin(); iter != m_work_blocks.end(); iter++)
    {
        if( (*iter) -> block_index == key.block_index)
        {
            m_work_blocks.erase(iter);
            break;
        }
    }

    m_free_block_nodes.push_back(&key);
    return 0;
}


void* MemPool::addr(mempool_key_t key)
{
    block_t* block = m_blocks[key.block_index];
    void* ptr =  (char*)block + sizeof(block_t) + this->m_max_slice_size * key.slice_index;
    return ptr;
}


MemPool::~MemPool()
{
   //删除所有数据即可
}


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
