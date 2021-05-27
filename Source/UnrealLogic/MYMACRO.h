#pragma once
#include "CoreMinimal.h"

#define LOGWARNING(Format,...)\
	{\
		UE_LOG(LogTemp,Warning,TEXT(Format),##__VA_ARGS__)\
	}
#define LOGERROR(Format,...) {UE_LOG(LogTemp,Error,TEXT(Format),##__VA_ARGS__);}
