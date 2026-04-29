// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventorySlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryWidget.generated.h"

class UInventoryItemData;
class UInventoryComponent;

inline constexpr int32 kColumns{5};

USTRUCT(BlueprintType)
struct FInventoryWidgetCategory {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    TArray<TObjectPtr<UInventorySlotWidget>> Widgets;

    void UnselectAll();
    void SetSelected(int32 Index, bool bSelected);
};

UCLASS()
class SEETHE_API UInventoryWidget : public UUserWidget {
    GENERATED_BODY()
    friend class ASeetheCharacter;

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SuppliesGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> ToolsGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> WeaponsGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> NotesGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> InventoryCategoryText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemDescText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ItemDescBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemManufacturerLogo;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> CategoryWidgetSwitcher;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SuppliesCategoryButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ToolsCategoryButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> WeaponsCategoryButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NotesCategoryButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SuppliesIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ToolsIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> WeaponsIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> NotesIcon;

    UFUNCTION(BlueprintPure, Category="Inventory")
    UInventoryComponent* GetInventoryComponent();

    UFUNCTION(BlueprintCallable, Category="Inventory")
    TArray<FInventorySlot>& GetInventoryCategoryItems(const EInventoryCategory& Category) const;

    UPROPERTY(EditAnywhere, Category="Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

protected:
    UFUNCTION()
    void OnInventoryUpdate();

    virtual void NativeConstruct() override;
    virtual void NativePreConstruct() override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    void MoveSelection(int32 RowDelta, int32 ColDelta);

private:
    float LookX, LookY;
    EInventoryCategory CurrentCategory{EInventoryCategory::EIC_Supplies};

    void UpdateLookAxes(float X, float Y);
    void InitializeWidget(UInventoryComponent* InInventory);

    UPROPERTY()
    TObjectPtr<UInventorySlotWidget> CurrentlySelectedSlot;

    UPROPERTY()
    TMap<EInventoryCategory, FInventoryWidgetCategory> SlotWidgets;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> LinkedInventory;

    UFUNCTION()
    void OnSuppliesCategoryPressed();

    UFUNCTION()
    void OnToolsCategoryPressed();

    UFUNCTION()
    void OnWeaponsCategoryPressed();

    UFUNCTION()
    void OnNotesCategoryPressed();

    UFUNCTION()
    void OnSlotClicked(UInventorySlotWidget* SlotWidget);

    void OnCategoryChanged(const EInventoryCategory& Category);
    UUniformGridPanel* GetCategoryGrid(const EInventoryCategory& Category) const;

    static FButtonStyle GetCategoryButtonStyle();
};