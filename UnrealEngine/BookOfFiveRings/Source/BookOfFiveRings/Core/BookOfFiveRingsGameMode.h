#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/ScrollDataAsset.h"
#include "Data/CharacterDataAsset.h"
#include "Characters/FighterActor.h"
#include "Core/PresentationManager.h"
#include "BookOfFiveRingsGameMode.generated.h"

/**
 * ABookOfFiveRingsGameMode
 * Spawns and wires together all runtime actors.
 * Equivalent to Unity's GameBootstrapper — creates all scroll and
 * character data as runtime UObjects, spawns the FighterActor and
 * PresentationManager, then passes references so the presentation runs.
 *
 * Set this class as the Game Mode in DefaultEngine.ini (already done).
 */
UCLASS()
class BOOKOFFIVERINGS_API ABookOfFiveRingsGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABookOfFiveRingsGameMode();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TSubclassOf<class UPresentationWidget> WidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TSubclassOf<AFighterActor> FighterClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TSubclassOf<APresentationManager> ManagerClass;

protected:
    virtual void BeginPlay() override;

private:
    TArray<UScrollDataAsset*>    BuildScrollData();
    TArray<UCharacterDataAsset*> BuildCharacterData();

    static FSwordplayMove MakeMove(const FString& Name, const FString& Desc,
                                   const FLinearColor& Color, float Duration, EMoveType Type);
};
