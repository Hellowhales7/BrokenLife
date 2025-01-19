// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BrokenHUD.generated.h"

class UBrokenOverlay;
/**
 * 
 */
UCLASS()
class BROKENLIFE_API ABrokenHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Broken)
	TSubclassOf<UBrokenOverlay> BrokenOverlayClass;

	UBrokenOverlay* BrokenOverlay;

public:
	FORCEINLINE UBrokenOverlay* GetBrokenOverlay() const { return BrokenOverlay; }
};
