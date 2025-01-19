// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "AbilitySystemInterface.h"
#include "RuneAttributeSet.h"
#include "InputActionValue.h"
#include "Public/Items/Item.h"
#include "BaseCharacter.h"
#include "Public/CharacterType.h"
#include "Public/Interfaces/PickupInterface.h"
#include "BrockenLifeCharacter.generated.h"




class UGroomComponent;
class UAnimMontage;
class AWeapon;
class ASoul;
class UBrokenOverlay;
class ATreasure;
UCLASS(config=Game)
class BROKENLIFE_API ABrockenLifeCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ABrockenLifeCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;


	virtual void Hit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DanceAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;
	/*
	* Input 
	*/


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void Dance(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);

	/*
	*  Play Montage Function
	*/
	void PlayEquipMonatage(FName SectionName);
	void PlayDanceMonatage();

	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;																																																																																														
	virtual void DodgeEnd() override;

	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	virtual void Die_Implementation() override;
	bool HasStamina();
	bool HasEnoughStamina(float StaminaCost);


	bool IsOccupied();
	UFUNCTION(BlueprintCallable)
	void AttackWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
private:

	bool IsUnoccupied();
	void InitializeBrokenOverlay(APlayerController* PlayerController);
	void SetHUDHealth();
	// Character Components 
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;
	UPROPERTY(VisibleAnywhere, Category ="Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;


	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DanceMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "ture"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	UBrokenOverlay* BrokenOverlay;
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};
