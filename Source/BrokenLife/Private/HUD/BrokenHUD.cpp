// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BrokenHUD.h"
#include "HUD/BrokenOverlay.h"

void ABrokenHUD::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && BrokenOverlayClass)
		{
			BrokenOverlay = CreateWidget<UBrokenOverlay>(Controller, BrokenOverlayClass);
			BrokenOverlay->AddToViewport();
		}
	}
}
