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
			for(auto& obj:component->lineArray)
			{
				auto linkobj=Cast<ULinkObject>(obj);
				int from=*mapId.FindKey(Cast<ABaseBlockActor>(linkobj->from->GetOwner()));
				int to=*mapId.FindKey(Cast<ABaseBlockActor>(linkobj->to->GetOwner()));
				G[to].Add(from);
			}
		}
	}
	return outDegree;
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
	for (auto& block : blockArray)
	{
		mapIdActor.Add(id, block);
		int outDeg = GetNodeComponents(block, G, mapIdActor);
		if (outDeg == 0)
		{
			bfsQueue.Enqueue(outDeg);
		}
		id++;
	}
	
	//找到出度为0的 开始生产bfs
}
