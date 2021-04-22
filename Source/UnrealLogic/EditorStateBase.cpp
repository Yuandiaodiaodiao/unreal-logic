// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorStateBase.h"

int GetNodeComponents(ABaseBlockActor* actor, TMap<int, TArray<int>>& G, TMap<int, ABaseBlockActor*>& mapId)
{
	int outDegree = 0;
	TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
	actor->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);
	for (auto& component : componentArray)
	{
		if (component->type.Equals("output"))
		{
			outDegree += component->lineArray.Num();
		}
		else if (component->type.Equals("input"))
		{
			for (auto& obj : component->lineArray)
			{
				auto linkobj = Cast<ULinkObject>(obj);
				int from = *mapId.FindKey(Cast<ABaseBlockActor>(linkobj->from->GetOwner()));
				int to = *mapId.FindKey(Cast<ABaseBlockActor>(linkobj->to->GetOwner()));
				G.FindOrAdd(to).Add(from);
			}
		}
	}
	return outDegree;
}

void RefreshInput(ABaseBlockActor* actor)
{
	TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
	actor->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);
	for (auto& component : componentArray)
	{
		//刷新activate到当前tick 非门的输出 也要经过计算 所以就算没连也计算出是on? 刷回activate
		component->nowactivate = component->nextactivate;
		component->nextactivate = false;
	}
}

void SyncInput(ABaseBlockActor* actor, TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>>& componentArray)
{
	actor->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);
	for (auto& component : componentArray)
	{
		if (component->type.Equals("input"))
		{
			//拉取上一层的output (比如非门 上一层没连 那非门的输入(nextactivate)保持为什么? 输入应为false 但是是activate为false )
			//默认情况下 nextactivate应该为false吗?每tick刷新
			//未连线的都默认为false input没有output  output的input不连线确实是false
			if (component->lineArray.Num() > 0)
			{
				auto linkObj = Cast<ULinkObject>(component->lineArray.HeapTop());
				component->nextactivate = linkObj->from->nowactivate;
			}
		}
	}
}

void SolveOutput(ABaseBlockActor* actor, TArray<UNodeStaticMeshComponent*>& outputComponents,
                 TArray<UNodeStaticMeshComponent*>& inputComponents)
{
	auto type = actor->GateType;
	auto and = [](bool x, bool y)-> bool { return x & y; };
	auto or = [](bool x, bool y)-> bool { return x | y; };
	if (type.Equals("And"))
	{
		outputComponents.HeapTop()->nextactivate = and(inputComponents.Last()->nextactivate,
		                                               inputComponents.Last(1)->nextactivate);
	}
	else if (type.Equals("Or"))
	{
		outputComponents.HeapTop()->nextactivate = or(inputComponents.Last()->nextactivate,
		                                              inputComponents.Last(1)->nextactivate);
	}
	else if (type.Equals("input"))
	{
		//输入的输出 是始终同步的
		outputComponents.HeapTop()->nextactivate = outputComponents.HeapTop()->nowactivate;
	}
}

void AEditorStateBase::CollectDataToGraph()
{
	int id = 0;
	TMap<int, ABaseBlockActor*> mapIdActor;
	TQueue<int> bfsQueue;
	//反向图
	TMap<int, TArray<int>> G;
	TSet<int> bfsUsed;
	//建立id映射
	//找到出度为0的 开始生产bfs

	for (auto& block : blockArray)
	{
		mapIdActor.Add(id, block);
		int outDeg = GetNodeComponents(block, G, mapIdActor);
		if (outDeg == 0)
		{
			bfsQueue.Enqueue(outDeg);
			UE_LOG(LogTemp, Warning, TEXT("outdeg0 %s"), *mapIdActor[id]->GetName());
		}
		id++;
	}
	//跑一个bfs求出bfs序


	//按照bfs序进行input的拉取 和元件更新

	//暂时按照随机顺序拉取
}

void AEditorStateBase::SolveTickLogic()
{
	for (auto& block : blockArray)
	{
		//同步input
		TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
		SyncInput(block, componentArray);

		TArray<UNodeStaticMeshComponent*> outputComponentArray;
		TArray<UNodeStaticMeshComponent*> inputComponentArray;
		for (auto component : componentArray)
		{
			if (component->type.Equals("output"))
			{
				outputComponentArray.Add(component);
			}
			else if (component->type.Equals("input"))
			{
				inputComponentArray.Add(component);
			}
		}
		SolveOutput(block, outputComponentArray, inputComponentArray);
	}
	for (auto& block : blockArray)
	{
		RefreshInput(block);
	}
}
