// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorPlayerController.h"


#include "BaseBlockActor.h"
#include "DrawDebugHelpers.h"
#include "EditorStateBase.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerInput.h"
#include "LinkObject.h"

AEditorPlayerController::AEditorPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	mouseState.Append("release");
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/AndGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/OrGate")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/Input")).Class);
	PutList.Add(ConstructorHelpers::FClassFinder<ABaseBlockActor>(TEXT("/Game/Blueprints/Output")).Class);
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
	InputComponent->BindAction("Put_Menu", IE_Pressed, this, &AEditorPlayerController::MenuOn);
	InputComponent->BindAction("Next", IE_Pressed, this, &AEditorPlayerController::Next);
	InputComponent->BindAction("Start_Sim", IE_Pressed, this, &AEditorPlayerController::StartSim);
	InputComponent->BindAction("Always_Sim", IE_Pressed, this, &AEditorPlayerController::AlwaysSim);
}

void AEditorPlayerController::ChangeNodeState(UNodeStaticMeshComponent* component, FVector startNormal)
{
	auto location = component->GetComponentTransform().GetLocation();
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *(location.ToString()));
	static TArray<UNodeStaticMeshComponent*> componentArray;
	if (mouseState.Equals("linking"))
	{
		//挂上事件
		mouseState = "release";

		//压入终点
		componentArray.Add(component);
		UE_LOG(LogTemp,Warning,TEXT("压入终点 %s"),*component->GetOuter()->GetName());
		auto endPosition = component->GetComponentTransform().GetLocation();
		linkingPositionTemp.Add(endPosition);
		//todo-节点连接完成 保存节点信息
		//n个柱子 n+1个position
		//最后一段连接强制修正到终点
		if(linkingMeshTemp.Num()>1)
		{
			auto& mesh=linkingMeshTemp.Last();
			auto& endPos=linkingPositionTemp.Last();
			auto& startPos=linkingPositionTemp.Last(1);
			if (linkDirection.X == 0)
			{
				startPos.X = endPos.X;
			}
			else
			{
				startPos.Y=endPos.Y;
			}
			ChangeMesh(startPos,endPos,mesh);
			auto& mesh1=linkingMeshTemp.Last(1);
			auto& startPos2=linkingPositionTemp.Last(2);
			ChangeMesh(startPos2,startPos,mesh1);
		}else
		{
			
		}
		auto gameState=Cast<AEditorStateBase>(GetWorld()->GetGameState());
		auto lineObject=NewObject<ULinkObject>();
		lineObject->init(linkingPositionTemp,linkingMeshTemp,componentArray.HeapTop(),componentArray.Last());
		gameState->lineArray.Add(lineObject);
		componentArray.HeapTop()->lineArray.Add(lineObject);
		componentArray.Last()->lineArray.Add(lineObject);
		linkingMeshTemp.Reset();
		linkingPositionTemp.Reset();
	}
	else if (mouseState.Equals("release"))
	{
		lock=true;
		linkDirection = startNormal;
		//节点起点
		mouseState = "linking";
		componentArray.Reset();
		componentArray.Add(component);
		UE_LOG(LogTemp,Warning,TEXT("压入起点 %s"),*component->GetOuter()->GetName());

		auto startPosition = component->GetComponentTransform().GetLocation();
		linkingPositionTemp.Add(startPosition);
		//初始化第一次的圆柱
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
	if(simOn)
	{
		StartSim();
	}
	lock=false;
	FVector2D mouseDelta;
	GetInputMouseDelta(mouseDelta.X, mouseDelta.Y);
	static FVector2D lastMousePosition;

	if (mouseDelta.GetAbsMax() > 0)
	{
		mouseDelta*=-50;
		if (UWorld* World = GetWorld())
		{

			for (FConstCameraActorIterator Iterator = World->GetAutoActivateCameraIterator(); Iterator; ++Iterator)
			{
				ACameraActor* PlayerController = Iterator->Get();
				PlayerController->SetActorLocation(PlayerController->GetActorLocation()+FVector(mouseDelta.Y,mouseDelta.X,0));

			}
		}
	}

	FVector2D mousePosition;
	if (GetMousePosition(mousePosition.X, mousePosition.Y))
	{
		if (!lastMousePosition.Equals(mousePosition))
		{
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *mousePosition.ToString())
			lastMousePosition = mousePosition;
			OnMouseMove(mousePosition);
			if(menuOn)
			{
				ReFreshPut();
			}
			
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
	//根据鼠标位置 计算出朝向和视角起点的世界坐标
	auto bDirectionGet = DeprojectMousePositionToWorld(Start, Dir); //获取初始位置和方向
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
			pos.Y=startPosition.Y;
			mesh->SetActorRotation(FRotator(0, 90, 90));
		}
		ChangeMesh(startPosition,pos,mesh);
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

	if (mouseState.Equals("linking") && lock==false)
	{
		float temp=linkDirection.X;
		linkDirection.X = linkDirection.Y;
		linkDirection.Y = temp;
		//节点起点
		
		linkingPositionTemp.Add(lastEndPosition);
		//初始化第一次的圆柱
		auto mesh = createLinkingMesh(lastEndPosition);
		linkingMeshTemp.Add(mesh);
		FVector2D mousePosition;
		if (GetMousePosition(mousePosition.X, mousePosition.Y))
		{
			ChangeLinkShape(mousePosition);
		}
	}
	if(menuOn)
	{
		menuOn=false;
		actorShow=nullptr;
	}
}

void AEditorPlayerController::MouseRightClick()
{
	if(mouseState.Equals("linking"))
	{
		float temp=linkDirection.X;
		linkDirection.X = linkDirection.Y;
		linkDirection.Y = temp;
		linkingPositionTemp.Pop();
		linkingMeshTemp.Pop()->Destroy();
		if(linkingMeshTemp.Num()==0)
		{
			mouseState="release";
			return;
		}
		FVector2D mousePosition;
		if (GetMousePosition(mousePosition.X, mousePosition.Y))
		{
			ChangeLinkShape(mousePosition);
		}
	}else
	{
		FVector Start, Dir, End;
		if (DeprojectMousePositionToWorld(Start, Dir))
		{
			// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 10, 1, 10);
			End = (Dir * 1e9f);
			End+=Start;
			//根据鼠标位置 计算出朝向和视角起点的世界坐标
   
			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
			auto actor=HitResult.Actor.Get();
			if(actor)
			{
				UClass* ca=actor->GetClass();
				UClass* cb= ABaseBlockActor::StaticClass();
				
				
					auto block=Cast<ABaseBlockActor>(actor);
					if(block && block->GateType=="Input")
					{
						TArray<UNodeStaticMeshComponent*, TInlineAllocator<8>> componentArray;
						actor->GetComponents<UNodeStaticMeshComponent, TInlineAllocator<8>>(componentArray);
						auto component=componentArray.HeapTop();
						component->nowactivate=!component->nowactivate;
					}
				
			}

		}

	}
}

void AEditorPlayerController::MenuOn()
{
	menuOn=!menuOn;
	if(menuOn==false)
	{
		if(actorShow)
		{
			actorShow->Destroy();
			auto gameState=Cast<AEditorStateBase>(GetWorld()->GetGameState());
			gameState->blockArray.Remove(actorShow);
		}
	}
	Next(-1);
}

void AEditorPlayerController::Next(int idDelta)
{
	if(menuOn==false)return;
	static int buildId=0;
	buildId+=idDelta;
	buildId++;
	buildId%=PutList.Num();
	FVector Start, Dir, End;
	if (DeprojectMousePositionToWorld(Start, Dir))
	{
		if(actorShow)
		{
			actorShow->Destroy();
			auto gameState=Cast<AEditorStateBase>(GetWorld()->GetGameState());
			gameState->blockArray.Remove(actorShow);
		}
		
		//根据鼠标位置 计算出朝向和视角起点的世界坐标
		End = Start + (Dir * 1e9f);
		FVector pos = FMath::LinePlaneIntersection(Start, End,
                                                   FVector(0,0,80),
                                                   FVector(0, 0, 1));
		actorShow=GetWorld()->SpawnActor<ABaseBlockActor>(PutList[buildId], pos, FRotator(0), FActorSpawnParameters());
		actorShow->SetActorLocation(pos);
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
		
		//根据鼠标位置 计算出朝向和视角起点的世界坐标
		End = Start + (Dir * 8e9f);
		FVector pos = FMath::LinePlaneIntersection(Start, End,
                                                   FVector(0,0,80),
                                                   FVector(0, 0, 1));
		// UE_LOG(LogTemp, Warning, TEXT("%s"), *pos.ToString())

		actorShow->SetActorLocation(pos);
	}
}

void AEditorPlayerController::StartSim()
{
	auto gameState=Cast<AEditorStateBase>(GetWorld()->GetGameState());
	gameState->SolveTickLogic();
}

void AEditorPlayerController::AlwaysSim()
{
	
	simOn=!simOn;
	
}

void AEditorPlayerController::ChangeMesh(FVector start, FVector end, ALinkStaticMeshActor* mesh)
{
	float distance = FVector::Distance(end, start);
	mesh->SetActorScale3D(FVector(0.25, 0.25, distance / 100));
	lastEndPosition=end;
	FVector centerPos = (end + start) / 2;
	mesh->SetActorLocation(centerPos);
}
