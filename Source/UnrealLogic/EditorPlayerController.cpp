// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorPlayerController.h"


#include "BaseBlockActor.h"
#include "DrawDebugHelpers.h"
#include "EditorStateBase.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerInput.h"
#include "LinkObject.h"
#include "MyUserWidget.h"
#include "Blueprint/UserWidget.h"

AEditorPlayerController::AEditorPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	mouseState.Append("release");
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/AndGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/OrGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/NotGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/Input")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/Output")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/NAndGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/XorGate")).Class);
}

void AEditorPlayerController::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("controller begin"));
}

ALinkStaticMeshActor* AEditorPlayerController::createLinkingMesh(FVector startPosition)
{
	auto mesh = GetWorld()->SpawnActor<ALinkStaticMeshActor>(ALinkStaticMeshActor::StaticClass(), startPosition,
	                                                         FRotator(0, 0, 90), FActorSpawnParameters());
	return mesh;
}

void AEditorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	EnableInput(this);
	InputComponent->BindAction("Mouse_Left", IE_Pressed, this, &AEditorPlayerController::MouseLeftClick);
	InputComponent->BindAction("Mouse_Right", IE_Pressed, this, &AEditorPlayerController::MouseRightClick);
	InputComponent->BindAction("Mouse_Right", IE_Released, this, &AEditorPlayerController::MouseRightRelease);
	InputComponent->BindAction("Put_Menu", IE_Pressed, this, &AEditorPlayerController::MenuOn);
	InputComponent->BindAction("Next", IE_Pressed, this, &AEditorPlayerController::Next);
	InputComponent->BindAction("Start_Sim", IE_Pressed, this, &AEditorPlayerController::StartSim);
	InputComponent->BindAction("Always_Sim", IE_Pressed, this, &AEditorPlayerController::AlwaysSim);
	InputComponent->BindAction("SaveVerilog", IE_Pressed, this, &AEditorPlayerController::SaveVerilog);
	InputComponent->BindAction("Record", IE_Pressed, this, &AEditorPlayerController::Record);
	InputComponent->BindAction("Test", IE_Pressed, this, &AEditorPlayerController::PerformanceTest);
}

void AEditorPlayerController::ChangeNodeState(UNodeStaticMeshComponent* component, FVector startNormal)
{
	auto location = component->GetComponentTransform().GetLocation();
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *(location.ToString()));
	static TArray<UNodeStaticMeshComponent*> componentArray;
	if (mouseState.Equals("linking"))
	{
		//????????????
		mouseState = "release";

		//????????????
		componentArray.Add(component);
		UE_LOG(LogTemp, Warning, TEXT("???????????? %s"), *component->GetOuter()->GetName());
		auto endPosition = component->GetComponentTransform().GetLocation();
		//???????????? ????????????
		endPosition.Z -= 10;
		linkingPositionTemp.Add(endPosition);
		//todo-?????????????????? ??????????????????
		//n????????? n+1???position
		//???????????????????????????????????????
		if (linkingMeshTemp.Num() > 1)
		{
			auto& mesh = linkingMeshTemp.Last();
			auto& endPos = linkingPositionTemp.Last();
			auto& startPos = linkingPositionTemp.Last(1);
			if (linkDirection.X == 0)
			{
				startPos.X = endPos.X;
			}
			else
			{
				startPos.Y = endPos.Y;
			}
			ChangeMesh(startPos, endPos, mesh);
			auto& mesh1 = linkingMeshTemp.Last(1);
			auto& startPos2 = linkingPositionTemp.Last(2);
			ChangeMesh(startPos2, startPos, mesh1);
		}
		else
		{
		}
		auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
		auto lineObject = NewObject<ULinkObject>();
		lineObject->init(linkingPositionTemp, linkingMeshTemp, componentArray.HeapTop(), componentArray.Last());
		gameState->lineArray.Add(lineObject);
		componentArray.HeapTop()->lineArray.Add(lineObject);
		componentArray.Last()->lineArray.Add(lineObject);
		linkingMeshTemp.Reset();
		linkingPositionTemp.Reset();
	}
	else if (mouseState.Equals("release"))
	{
		lock = true;
		linkDirection = startNormal;
		//????????????
		mouseState = "linking";
		componentArray.Reset();
		componentArray.Add(component);
		UE_LOG(LogTemp, Warning, TEXT("???????????? %s"), *component->GetOuter()->GetName());

		auto startPosition = component->GetComponentTransform().GetLocation();
		//???????????? ????????????
		startPosition.Z -= 10;
		linkingPositionTemp.Add(startPosition);
		//???????????????????????????
		auto mesh = createLinkingMesh(startPosition);
		linkingMeshTemp.Add(mesh);
		FVector2D mousePosition;
		if (GetMousePosition(mousePosition.X, mousePosition.Y))
		{
			ChangeLinkShape(mousePosition);
		}
	}
}

void AEditorPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (simOn)
	{
		StartSim();
	}
	lock = false;
	FVector2D mouseDelta;
	GetInputMouseDelta(mouseDelta.X, mouseDelta.Y);
	static FVector2D lastMousePosition;

	//camera??????
	if (mouseDelta.GetAbsMax() > 0)
	{
		//??????????????????
		if(rightMouseStatus)
		{
			mouseDelta *= -50;
			if (UWorld* World = GetWorld())
			{
				for (FConstCameraActorIterator Iterator = World->GetAutoActivateCameraIterator(); Iterator; ++Iterator)
				{
					ACameraActor* PlayerController = Iterator->Get();
					PlayerController->SetActorLocation(
                        PlayerController->GetActorLocation() + FVector(mouseDelta.Y, mouseDelta.X, 0));
				}
			}
		}
		
	}

	//????????????????????????
	FVector2D mousePosition;
	if (GetMousePosition(mousePosition.X, mousePosition.Y))
	{
		if (!lastMousePosition.Equals(mousePosition))
		{
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *mousePosition.ToString())
			lastMousePosition = mousePosition;
			OnMouseMove(mousePosition);
			if (menuOn)
			{
				ReFreshPut();
			}
		}
	}

	if(recordOn==false && WidgetInstance)
	{
		//?????????????????????
		static int index=-1;
		int indexnew=floor(WidgetInstance->sliderValue);
		if(index!=indexnew)
		{
			//?????????????????????
			if(indexnew>=0 && indexnew<record->StatusArray.Num())
			{
				index=indexnew;
				auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
				record->ExeRecord(indexnew,gameState);
				LOGWARNING("??????%d",indexnew)
			}
		}
	}
	
	//????????????
	this->Refresh();
	if(simOn)
	{
		if (recordOn)
		{
			auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
			if (!record)
			{
				record = new Recorder();
			}
			record->RecordOnce(gameState);
			//???????????? ????????????
		}
	}
}

void AEditorPlayerController::OnMouseMove(FVector2D mousePosition)
{
	if (mouseState.Equals("linking"))
	{
		ChangeLinkShape(mousePosition);
	}
}


void AEditorPlayerController::ChangeLinkShape(FVector2D mousePosition)
{
	auto mesh = linkingMeshTemp.Last();
	auto startPosition = linkingPositionTemp.Last();
	FVector Start, Dir, End;
	//?????????????????? ?????????????????????????????????????????????
	auto bDirectionGet = DeprojectMousePositionToWorld(Start, Dir); //???????????????????????????
	End = Start + (Dir * 8e9f);
	if (bDirectionGet)
	{
		DrawDebugLine(GetWorld(), FVector(0), startPosition, FColor::Red, false, 1, 1, 10);
		// DrawDebugBox(GetWorld(),FVector(startPosition),FVector(100, 100, 100),FColor::Blue);
		FVector pos = FMath::LinePlaneIntersection(Start, End,
		                                           startPosition,
		                                           FVector(0, 0, 1));
		DrawDebugLine(GetWorld(), FVector(0), pos, FColor::Blue, false, 1, 1, 10);
		// UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString())
		if (linkDirection.X == 0)
		{
			pos.X = startPosition.X;
			mesh->SetActorRotation(FRotator(90, 90, 0));
		}
		else
		{
			pos.Y = startPosition.Y;
			mesh->SetActorRotation(FRotator(0, 90, 90));
		}
		ChangeMesh(startPosition, pos, mesh);
		// float distance = FVector::Distance(pos, startPosition);
		// mesh->SetActorScale3D(FVector(0.25, 0.25, distance / 100));
		//
		// lastEndPosition=pos;
		// FVector centerPos = (pos + startPosition) / 2;
		// mesh->SetActorLocation(centerPos);
	}
}

void AEditorPlayerController::MouseLeftClick()
{
	UE_LOG(LogTemp, Warning, TEXT("leftclick"))

	if (mouseState.Equals("linking") && lock == false)
	{
		float temp = linkDirection.X;
		linkDirection.X = linkDirection.Y;
		linkDirection.Y = temp;
		//????????????

		linkingPositionTemp.Add(lastEndPosition);
		//???????????????????????????
		auto mesh = createLinkingMesh(lastEndPosition);
		linkingMeshTemp.Add(mesh);
		FVector2D mousePosition;
		if (GetMousePosition(mousePosition.X, mousePosition.Y))
		{
			ChangeLinkShape(mousePosition);
		}
	}
	if (menuOn)
	{
		menuOn = false;
		actorShow = nullptr;
	}
}

void AEditorPlayerController::MouseRightClick()
{
	rightMouseStatus=true;
	if (mouseState.Equals("linking"))
	{
		float temp = linkDirection.X;
		linkDirection.X = linkDirection.Y;
		linkDirection.Y = temp;
		linkingPositionTemp.Pop();
		linkingMeshTemp.Pop()->Destroy();
		if (linkingMeshTemp.Num() == 0)
		{
			mouseState = "release";
			return;
		}
		FVector2D mousePosition;
		if (GetMousePosition(mousePosition.X, mousePosition.Y))
		{
			ChangeLinkShape(mousePosition);
		}
	}
	else
	{
		FVector Start, Dir, End;
		if (DeprojectMousePositionToWorld(Start, Dir))
		{
			// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 10, 1, 10);
			End = (Dir * 1e9f);
			End += Start;
			//?????????????????? ?????????????????????????????????????????????

			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
			auto actor = HitResult.Actor.Get();
			if (actor)
			{
				UClass* ca = actor->GetClass();
				UClass* cb = ABaseBlockActor::StaticClass();


				auto block = Cast<ABaseBlockActor>(actor);
				if (block && block->GateType == "Input")
				{
					TArray<UNodeStaticMeshComponent*, TInlineAllocator<8>> componentArray;
					actor->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<8>>(componentArray);
					auto component = componentArray.HeapTop();
					component->nowactivate = !component->nowactivate;
				}
			}
		}
	}
}

void AEditorPlayerController::MouseRightRelease()
{
	rightMouseStatus=false;
}

void AEditorPlayerController::MenuOn()
{
	menuOn = !menuOn;
	if (menuOn == false)
	{
		if (actorShow)
		{
			actorShow->Destroy();
			auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
			gameState->blockArray.Remove(actorShow);
		}
	}
	Next(-1);
}

void AEditorPlayerController::Next(int idDelta)
{
	if (menuOn == false)return;
	static int buildId = 0;
	buildId += idDelta;
	buildId++;
	buildId %= PutList.Num();
	FVector Start, Dir, End;
	if (DeprojectMousePositionToWorld(Start, Dir))
	{
		if (actorShow)
		{
			actorShow->Destroy();
			auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
			gameState->blockArray.Remove(actorShow);
		}

		//?????????????????? ?????????????????????????????????????????????
		End = Start + (Dir * 1e9f);
		FVector pos = FMath::LinePlaneIntersection(Start, End,
		                                           FVector(0, 0, 80),
		                                           FVector(0, 0, 1));
		actorShow = GetWorld()->SpawnActor<ABaseBlockActor
		>(PutList[buildId], pos, FRotator(0), FActorSpawnParameters());
	}
}

void AEditorPlayerController::Next()
{
	Next(0);
}


void AEditorPlayerController::ReFreshPut()
{
	FVector Start, Dir, End;
	if (DeprojectMousePositionToWorld(Start, Dir))
	{
		//?????????????????? ?????????????????????????????????????????????
		End = Start + (Dir * 8e9f);
		FVector pos = FMath::LinePlaneIntersection(Start, End,
		                                           FVector(0, 0, 80),
		                                           FVector(0, 0, 1));
		// UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString())

		actorShow->SetActorLocation(pos);
	}
}

void AEditorPlayerController::StartSim()
{
	double timestart = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
	auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
	//cuda??????
	// gameState->LoadDLL();

	gameState->SolveTickLogic();
	

	double timeend = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
	// UE_LOG(LogTemp, Warning, TEXT("TIME: %f"), (float)(timeend-timestart));
}

void AEditorPlayerController::AlwaysSim()
{
	simOn = !simOn;
}

void AEditorPlayerController::SaveVerilog()
{
	auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
	gameState->SaveVerilog();
}

void AEditorPlayerController::Refresh()
{
	auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
	gameState->SyncAllInput();
}

void AEditorPlayerController::Record()
{
	if (recordOn == true)
	{
		//????????????
		//??????????????????ui
		LOGWARNING("???????????? ???%d?????????", record->StatusArray.Num())
		if (UClass* MyWidgetClass = LoadClass<UMyUserWidget>(
			NULL, TEXT("WidgetBlueprint'/Game/Blueprints/NewWidgetBlueprint.NewWidgetBlueprint_C'")))
		{
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				WidgetInstance = CreateWidget<UMyUserWidget>(PC, MyWidgetClass);
				if (WidgetInstance)
				{
					WidgetInstance->SetRecord(record->StatusArray.Num()-1);
					WidgetInstance->AddToViewport();
					simOn=false;
					LOGWARNING("???????????????")
				}
			}
		}
	}
	//????????????????????????
	if(recordOn==false && WidgetInstance)
	{
		WidgetInstance->RemoveFromViewport();
		WidgetInstance=nullptr;
		delete record;
		record=nullptr;
		return;
	}
	if(recordOn==false)
	{
		LOGWARNING("????????????")
	}
	recordOn = !recordOn;
}

void AEditorPlayerController::PerformanceTest()
{
	int blockNum=1000;
	// 	for(int a=0;a<blockNum;++a)
	// 	{
	// 		for(int types=0;types<=2;++types)
	// 		{
	// 			GetWorld()->SpawnActor<ABaseBlockActor
	//        >(PutList[types], FVector(-100), FRotator(0), FActorSpawnParameters());
	// 		}
	// 	}
	// //???????????? ??????record
	// auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());
	// Recorder* recordx=new Recorder();
	// recordx->RecordOnce(gameState);
	// TArray<BlockStatus>& StatusArray=recordx->StatusArray.HeapTop();
	TArray<BlockStatus>StatusArray;
	for(int xx=0;xx<blockNum;++xx)
	{
		
	}
	auto gameState = Cast<AEditorStateBase>(GetWorld()->GetGameState());

	double timestart = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();

	for(int timesx=0;timesx<100;++timesx)
	{
		gameState->OnlySyncInput();
	}
	// //100???
	// for(int timesx=0;timesx<100;++timesx)
	// {
	// 	for(auto& status:StatusArray)
	// 	{
	// 		if(status.blockType==0)
	// 		{
	// 			status.nodeStatus=(status.nodeStatus&1)&(status.nodeStatus>>1&1);
	// 		}else if(status.blockType==1)
	// 		{
	// 			status.nodeStatus=(status.nodeStatus&1)|(status.nodeStatus>>1&1);
	// 		}else if(status.blockType==2)
	// 		{
	// 			status.nodeStatus=~(status.nodeStatus&1);
	// 		}
	// 	}
	// }
	double timeend = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
	UE_LOG(LogTemp, Warning, TEXT("TIMEfor OnlySyncInput: %f ms"), (float)(timeend-timestart));

	timestart = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();

	for(int timesx=0;timesx<100;++timesx)
	{
		gameState->SolveTickLogic();
	}
	timeend = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
	UE_LOG(LogTemp, Warning, TEXT("TIME for SolveTickLogic: %f ms"), (float)(timeend-timestart));
	
}

void AEditorPlayerController::ChangeMesh(FVector start, FVector end, ALinkStaticMeshActor* mesh)
{
	float distance = FVector::Distance(end, start);
	mesh->SetActorScale3D(FVector(0.25, 0.25, distance / 100));
	lastEndPosition = end;
	FVector centerPos = (end + start) / 2;
	mesh->SetActorLocation(centerPos);
}
