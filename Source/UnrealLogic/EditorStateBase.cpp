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
	auto and = [](bool x, bool y)-> bool { return x && y; };
	auto or = [](bool x, bool y)-> bool { return x || y; };
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
	else if (type.Equals("Input"))
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
	/*cuda计算
	 *
	 *
	 *
	 */

	UE_LOG(LogTemp, Warning, TEXT("block num=%d"), blockArray.Num());
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
	//这个位置可以根据nowactivate 和nextactivate算一次是否变化
	//变化了就分配内存压入变化数组 打时间戳
	//n个器件 每个器件有n个节点 每个节点有状态 同时对于d触发器 还要有器件本身的储存状态(用一个int32存32个比特位) 
	/* 考虑到相连的节点之间 状态共享 可以存入边作为状态   
	 * 有n个器件 其中两两可能有n条边 所以邻接矩阵不适用
	 * 最终存入的还是 器件id + 接口状态[] + 内部状态
	 * 接口状态可以按in out的find顺序 组成状态数组
	 * 考虑到状态是1 0 则可以压比特 压int64
	 * 器件id int32 + int64 +int32  16字节一组
	 * 则最终的空间占用就是16字节*元件个数
	 * 器件id其实可以忽略
	 * 也可以int32+int32 8字节*元件个数
	 * 
	 * 
	 */

	for (auto& block : blockArray)
	{
		RefreshInput(block);
	}
}

struct tempop
{
	FString op;
	TArray<int> inputwireId;
	TArray<TArray<int>> outputwireId;
	TArray<FString> inputwireIdF;
	TArray<TArray<FString>> outputwireIdF;
};

void linkassign(FString& fs, TArray<FString>& fsa)
{
	if (fsa.Num() > 1)
	{
		FString first = fsa.HeapTop();
		for (auto x : fsa)
		{
			if (first.Equals(x))continue;
			fs += "\n assign " + x + "=" + first + ";";
		}
	}
}

void AEditorStateBase::SaveVerilog()
{
	//对边建立id
	//lineArray 直接按index做id
	//wire

	TArray<tempop> tempopArray;
	for (auto& block : blockArray)
	{
		TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
		block->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);
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
		auto temp = tempop();
		temp.op = block->GateType;
		for (auto input : inputComponentArray)
		{
			if (input->lineArray.Num() == 0)
			{
				temp.inputwireId.Add(-1);
			}
			else
			{
				auto linkObj = Cast<ULinkObject>(input->lineArray.HeapTop());
				auto index = lineArray.Find(linkObj);
				temp.inputwireId.Add(index);
			}
		}
		int outputline = 0;
		for (auto output : outputComponentArray)
		{
			TArray<int> outputGroup;
			for (auto obj : output->lineArray)
			{
				outputline++;
				auto linkObj = Cast<ULinkObject>(obj);
				auto index = lineArray.Find(linkObj);
				outputGroup.Add(index);
			}
			temp.outputwireId.Add(outputGroup);
		}
		if (outputline > 0 || block->GateType.Equals("Output"))
		{
			tempopArray.Add(temp);
		}
	}
	// op [input...] [ [output...],[output...]]
	// wire input,...;
	// wire outputgroup1,outputgroup2;
	// 对于只有一个outputgroupt的 直接写assign
	// 对于有多个outputgroup的 先建立automatic task 然后output是reg 在外部再进行assign
	//中间代码生成完毕

	//目标代码生成
	FString modelInitCode = "";
	modelInitCode += "module mmm(";
	FString WireAll = "";
	
	for (int a = 0; a < lineArray.Num(); ++a)
	{
		WireAll += "\nwire gatewire";
		WireAll += FString::FromInt(a);
		WireAll += ";";
	}
	
	FString InputWire = "";
	//收集输入输出的wire
	TArray<tempop> inputWire;
	TArray<int> outputWire;
	int inputid = 0;
	for (auto& op : tempopArray)
	{
		if (op.op.Equals("Input"))
		{
			inputid++;

			modelInitCode += "\ninput input";
			modelInitCode += FString::FromInt(inputid);
			modelInitCode += ",";

			for (auto out : op.outputwireId.HeapTop())
			{
				InputWire += "\nassign ";
				InputWire += "gatewire";
				InputWire += FString::FromInt(out);
				InputWire += "=";
				InputWire += "input";
				InputWire += FString::FromInt(inputid);
				InputWire += ";";
			}
		}
	}
	FString OutputWire = "";
	for (auto& op : tempopArray)
	{
		if (op.op.Equals("Output") && op.inputwireId.Num() > 0 && op.inputwireId.HeapTop() >= 0)
		{
			modelInitCode += "\noutput output";
			modelInitCode += FString::FromInt(op.inputwireId.HeapTop());
			modelInitCode += ",";
			OutputWire += "\nassign output";
			OutputWire += FString::FromInt(op.inputwireId.HeapTop());
			OutputWire += "=gatewire";
			OutputWire += FString::FromInt(op.inputwireId.HeapTop());
			OutputWire += ";";
		}
	}
	
	if (modelInitCode.EndsWith(","))
	{
		modelInitCode.RemoveFromEnd(",");
	}
	modelInitCode += "\n);";
	for (auto& op : tempopArray)
	{
		for (auto input : op.inputwireId)
		{
			if (input >= 0)
			{
				op.inputwireIdF.Add(FString("gatewire") + FString::FromInt(input));
			}
			else
			{
				op.inputwireIdF.Add("0");
			}
		}
		for (auto& ouputgroup : op.outputwireId)
		{
			TArray<FString> group;
			for (auto output : ouputgroup)
			{
				if (output >= 0)
				{
					group.Add(FString("gatewire") + FString::FromInt(output));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("警告 输出为空"));
					group.Add("0");
				}
			}
			op.outputwireIdF.Add(group);
		}
	}
	FString MainCode="";
	for (auto& op : tempopArray)
	{
		if (op.outputwireId.Num() <= 1)
		{
			//单路输出逻辑门
			if (op.op.Equals("And"))
			{
				MainCode += "\nassign " + op.outputwireIdF.HeapTop().HeapTop() + "=" + op.inputwireIdF.HeapTop() + "&"
					+
					op.inputwireIdF.Last() + ";";
				//链式赋值
				linkassign(MainCode, op.outputwireIdF.HeapTop());
			}
			else if (op.op.Equals("Or"))
			{
				MainCode += "\nassign " + op.outputwireIdF.HeapTop().HeapTop() + "=" + op.inputwireIdF.HeapTop() + "|"
					+
					op.inputwireIdF.Last() + ";";
				//链式赋值
				linkassign(MainCode, op.outputwireIdF.HeapTop());
			}
		}
	}

	//拼接
	FString outputString = modelInitCode;
	outputString += WireAll;
	outputString+=InputWire;
	outputString += OutputWire;
	outputString += MainCode;
	outputString += "\nendmodule";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *outputString);
	FString gameDir = FPaths::ProjectDir() + FString("verilog.v");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *gameDir);
	FFileHelper::SaveStringToFile(outputString, *gameDir);
	
}
