// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"MYMACRO.h"
#include "EditorStateBase.h"

struct BlockStatus
{
	int nodeStatus;
	int insiderStatus;
};

/**
 * 
 */
class UNREALLOGIC_API Recorder
{
public:
	Recorder();
	~Recorder();
	void RecordOnce(AEditorStateBase* gameState);
	TArray<TArray<BlockStatus>> StatusArray;
	void ExeRecord(int index,AEditorStateBase* gameState);
};
