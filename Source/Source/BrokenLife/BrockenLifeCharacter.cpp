// Fill out your copyright notice in the Description page of Project Settings.


#include "BrockenLifeCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "../Public/Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/BrokenHUD.h"
#include "HUD/BrokenOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

// Sets default values
ABrockenLifeCharacter::ABrockenLifeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}


void ABrockenLifeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	InitializeBrokenOverlay(PlayerController);
	

	Tags.Add(FName("EngageableTarget"));
}

void ABrockenLifeCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	const FVector2D MoveVec = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MoveVec.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MoveVec.X);
}

void ABrockenLifeCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVec = Value.Get<FVector2D>();

	AddControllerPitchInput(LookVec.Y);
	AddControllerYawInput(LookVec.X);

}

void ABrockenLifeCharacter::Equip(const FInputActionValue& Value)
{
	AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
	const bool bValue = Value.Get<bool>();
	if (!bValue) return;


	//UE_LOG(LogTemp, Warning, TEXT("quip"));
	if (Weapon)
	{
		if (EquipedWeapon)
		{
			EquipedWeapon->Destroy();
		}
		Weapon->SetOwner(this);
		Weapon->SetInstigator(this);
		EquipWeapon(Weapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
		
	}
}

void ABrockenLifeCharacter::Attack(const FInputActionValue& Value)
{
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}
void ABrockenLifeCharacter::Dance(const FInputActionValue& Value)
{
	if (DanceMontage)
	{
		PlayDanceMonatage();
	}

}
void ABrockenLifeCharacter::Dodge(const FInputActionValue& Value)
{
	if (IsOccupied() || !HasEnoughStamina(Attributes->GetDodgeCost())) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && BrokenOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		UE_LOG(LogTemp, Warning, TEXT("%f"), Attributes->GetStamina());

		BrokenOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
	
}
bool ABrockenLifeCharacter::HasStamina()
{
	return Attributes && Attributes->GetStaminaPercent() > 0.f;
}																																																																																																																																																																																	
bool ABrockenLifeCharacter::HasEnoughStamina(float StaminaCost)
{
	return Attributes && Attributes->GetStamina() > StaminaCost;																																																														
}
bool ABrockenLifeCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}
bool ABrockenLifeCharacter::CanAttack()
{
	const bool bCanAttack = (ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped);
	return bCanAttack;
}

bool ABrockenLifeCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ABrockenLifeCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquipedWeapon;
}

void ABrockenLifeCharacter::Disarm()
{
	PlayEquipMonatage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ABrockenLifeCharacter::Arm()
{
	PlayEquipMonatage(FName("Equip"));
	CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ABrockenLifeCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void ABrockenLifeCharacter::AttackWeaponToBack()
{
	if (EquipedWeapon)
	{
		EquipedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ABrockenLifeCharacter::AttachWeaponToHand()
{
	if (EquipedWeapon)
	{
		EquipedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ABrockenLifeCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABrockenLifeCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}



void ABrockenLifeCharacter::PlayEquipMonatage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ABrockenLifeCharacter::PlayDanceMonatage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DanceMontage) {
		AnimInstance->Montage_Play(DanceMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Dance1");
			break;
		case 1:
			SectionName = FName("Dance2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DanceMontage);
	}
}

void ABrockenLifeCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquipedWeapon = Weapon;
}

void ABrockenLifeCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABrockenLifeCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}


void ABrockenLifeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Attributes && BrokenOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		BrokenOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ABrockenLifeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABrockenLifeCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABrockenLifeCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABrockenLifeCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ABrockenLifeCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ABrockenLifeCharacter::Attack);
		EnhancedInputComponent->BindAction(DanceAction, ETriggerEvent::Started, this, &ABrockenLifeCharacter::Dance);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ABrockenLifeCharacter::Dodge);
	}
}

float ABrockenLifeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Handledamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ABrockenLifeCharacter::Jump()
{
	if (IsUnoccupied()) {
		Super::Jump();
	}
}

bool ABrockenLifeCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ABrockenLifeCharacter::SetHUDHealth()
{
	if (BrokenOverlay && Attributes)
	{
		BrokenOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ABrockenLifeCharacter::Hit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::Hit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if(Attributes && Attributes->GetHealthPercent() >0.f)
		ActionState = EActionState::EAS_HitReaction;
}

void ABrockenLifeCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ABrockenLifeCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && BrokenOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		BrokenOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ABrockenLifeCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && BrokenOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		BrokenOverlay->SetGold(Attributes->GetGold());
	}
}


void ABrockenLifeCharacter::InitializeBrokenOverlay(APlayerController* PlayerController)
{
	ABrokenHUD* BrokenHUD = Cast<ABrokenHUD>(PlayerController->GetHUD());
	if (BrokenHUD)
	{
		BrokenOverlay = BrokenHUD->GetBrokenOverlay();
		if (BrokenOverlay)
		{
			BrokenOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			BrokenOverlay->SetStaminaBarPercent(1.f);
			BrokenOverlay->SetGold(0);
			BrokenOverlay->SetSouls(0);
		}
	}
}




