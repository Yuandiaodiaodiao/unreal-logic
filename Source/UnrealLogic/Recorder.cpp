// Fill out your copyright notice in the Description page of Project Settings.


#include "Recorder.h"

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
	StatusArray.Empty();
}

void Recorder::RecordOnce(AEditorStateBase* gameState)
{
	TArray<BlockStatus> StatusTemp;

	for (auto& block : gameState->blockArray)
	{
		BlockStatus status = BlockStatus{0, 0};
		TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
		block->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);

		for (auto& component : componentArray)
		{
			//从右向左压入状态
			status.nodeStatus = (component->nowactivate & 1) | (status.nodeStatus << 1);
		}
		StatusTemp.Add(status);
	}
	//和上一个状态进行比较 如果不同就保留 相同就删除
	if (StatusArray.Num() == 0)
	{
		StatusArray.Add(StatusTemp);
	}
	else
	{
		bool same=true;
		auto& lastArray=StatusArray.Last();
		
		for (int index=0;index<lastArray.Num();++index)
		{
			bool thissame=lastArray[index].nodeStatus==StatusTemp[index].nodeStatus;
			bool same2=lastArray[index].insiderStatus==StatusTemp[index].insiderStatus;
			if(!(thissame&&same2))
			{
				same=false;
				break;
			}
		}
		if(same)
		{
			//不压入
		}else
		{
			StatusArray.Add(StatusTemp);
			LOGWARNING("当前状态数%d",StatusArray.Num())
		}
	}
	
}

void Recorder::ExeRecord(int indexw,AEditorStateBase* gameState)
{
	auto& temp=StatusArray[indexw];
	int index=0;
	for (auto& block : gameState->blockArray)
	{
		BlockStatus status = temp[index];
		index++;
		TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
		block->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);

		for (int indexq=componentArray.Num()-1;indexq>=0;--indexq)
		{
			componentArray[indexq]->nowactivate=status.nodeStatus&1;
			status.nodeStatus>>=1;
		}
	}
}
