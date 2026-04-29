// Copyright (C) 2026 Jake Rieger


#include "InventoryWidget.h"

#include "HUDBase.h"
#include "InputActionValue.h"
#include "InventoryItem3dPreview.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "Seethe/SeetheCharacter.h"
#include "Seethe/Inventory/InventoryItem.h"
#include "Seethe/Inventory/InventoryComponent.h"

void UInventoryWidget::UpdateLookAxes(const float X, const float Y) {
    LookX = X;
    LookY = Y;
}

void FInventoryWidgetCategory::UnselectAll() {
    for (const auto& Widget : Widgets) {
        Widget->SetSelected(false);
    }
}

void FInventoryWidgetCategory::SetSelected(const int32 Index, const bool bSelected) {
    if (Widgets.IsValidIndex(Index)) {
        Widgets[Index]->SetSelected(bSelected);
    }
}

UInventoryComponent* UInventoryWidget::GetInventoryComponent() {
    return LinkedInventory;
}

TArray<FInventorySlot>& UInventoryWidget::GetInventoryCategoryItems(const EInventoryCategory& Category) const {
    return LinkedInventory->GetSlots(Category);
}

void UInventoryWidget::OnInventoryUpdate() {
    if (!LinkedInventory || !SlotWidgetClass) { return; }

    for (auto i = 0; i < static_cast<int32>(EInventoryCategory::NUM_CATEGORIES); i++) {
        EInventoryCategory Category = static_cast<EInventoryCategory>(i);
        auto& Slots                 = GetInventoryCategoryItems(Category);

        for (auto k = 0; k < kInventorySize; k++) {
            auto& CurrentSlot             = Slots[k];
            auto* InventoryWidgetCategory = SlotWidgets.Find(Category);
            if (InventoryWidgetCategory) {
                if (CurrentSlot.ItemData && CurrentSlot.Quantity > 0) {
                    InventoryWidgetCategory->Widgets[k]->SetupSlot(CurrentSlot, k);
                } else {
                    InventoryWidgetCategory->Widgets[k]->SetupAsEmptySlot();
                }
            }
        }
    }
}

void UInventoryWidget::NativeConstruct() {
    Super::NativeConstruct();

    SuppliesCategoryButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnSuppliesCategoryPressed);
    ToolsCategoryButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnToolsCategoryPressed);
    WeaponsCategoryButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnWeaponsCategoryPressed);
    NotesCategoryButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnNotesCategoryPressed);

    ItemDescBorder->SetVisibility(ESlateVisibility::Hidden);
    OnCategoryChanged(EInventoryCategory::EIC_Supplies);

    // Create inventory slots
    if (!SlotWidgetClass) { return; }
    for (auto i = 0; i < static_cast<int32>(EInventoryCategory::NUM_CATEGORIES); i++) {
        EInventoryCategory Category = static_cast<EInventoryCategory>(i);
        auto& [Widgets]             = SlotWidgets.Add(Category);

        auto* Grid = GetCategoryGrid(Category);
        Grid->ClearChildren();

        for (auto k = 0; k < kInventorySize; k++) {
            UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
            if (NewSlot) {
                NewSlot->SetupAsEmptySlot();
                NewSlot->OnSlotClicked.AddDynamic(this, &UInventoryWidget::OnSlotClicked);
                Widgets.Add(NewSlot);

                UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(NewSlot);
                if (GridSlot) {
                    const auto Row = k / kColumns;
                    const auto Col = k % kColumns;
                    GridSlot->SetRow(Row);
                    GridSlot->SetColumn(Col);
                }
            }
        }
    }
}

void UInventoryWidget::NativePreConstruct() {
    Super::NativePreConstruct();
    ItemNameText->SetText(FText());
    ItemDescText->SetText(FText());

    KeyStates.Add(EKeys::Tab, false);
    KeyStates.Add(EKeys::Gamepad_Special_Right, false);
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
    const auto Key = InKeyEvent.GetKey();

    if (Key == EKeys::Tab || Key == EKeys::Gamepad_Special_Right) {
        const bool bIsDown = KeyStates.FindRef(Key);
        if (bIsDown) { return FReply::Handled(); }

        if (const auto* HUD = Cast<AHUDBase>(GetOwningPlayer()->GetHUD())) {
            HUD->GetPreviewActor()->SetVisible(false);
        }

        if (auto* SC = Cast<ASeetheCharacter>(GetOwningPlayerPawn())) {
            const FInputActionValue Value(Key);
            SC->OnToggleInventory(Value);
        }

        KeyStates[Key] = true;

        return FReply::Handled();
    }

    if (Key == EKeys::X || Key == EKeys::Gamepad_FaceButton_Right) {
        if (CurrentlySelectedSlot) {
            UInventoryItem* Item = CurrentlySelectedSlot->Item.ItemData;
            if (Item) {
                const int32 Index                 = CurrentlySelectedSlot->ItemIndex;
                const EInventoryCategory Category = Item->ItemCategory;
                if (auto* SC = Cast<ASeetheCharacter>(GetOwningPlayerPawn())) {
                    Item->Drop(SC, Index, Category);
                }
            }
        }
    }

    // Navigate categories with gamepad
    if (Key == EKeys::Gamepad_LeftShoulder || Key == EKeys::Gamepad_RightShoulder
        || Key == EKeys::A || Key == EKeys::D) {
        int32 CurrentIndex       = CategoryWidgetSwitcher->GetActiveWidgetIndex();
        constexpr int32 MaxIndex = static_cast<int32>(EInventoryCategory::NUM_CATEGORIES) - 1;

        if (Key == EKeys::Gamepad_LeftShoulder || Key == EKeys::A) {
            CurrentIndex = (CurrentIndex <= 0) ? MaxIndex : CurrentIndex - 1;
        } else {
            CurrentIndex = (CurrentIndex >= MaxIndex) ? 0 : CurrentIndex + 1;
        }

        OnCategoryChanged(static_cast<EInventoryCategory>(CurrentIndex));
        return FReply::Handled();
    }

    if (Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Up) {
        MoveSelection(-1, 0);
        return FReply::Handled();
    }
    if (Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Down) {
        MoveSelection(1, 0);
        return FReply::Handled();
    }
    if (Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Left) {
        MoveSelection(0, -1);
        return FReply::Handled();
    }
    if (Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Right) {
        MoveSelection(0, 1);
        return FReply::Handled();
    }

    if (Key == EKeys::Gamepad_FaceButton_Bottom) {
        if (CurrentlySelectedSlot) {
            CurrentlySelectedSlot->OnUse();
            return FReply::Handled();
        }
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UInventoryWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
    const auto Key = InKeyEvent.GetKey();

    if (Key == EKeys::Tab || Key == EKeys::Gamepad_Special_Right) {
        KeyStates[Key] = false;
    }

    return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UInventoryWidget::MoveSelection(const int32 RowDelta, const int32 ColDelta) {
    if (!CurrentlySelectedSlot) {
        const auto& First = SlotWidgets[CurrentCategory].Widgets[0];
        if (First->bEmptySlot) {
            return;
        }
        OnSlotClicked(First);
        return;
    }

    // This SHOULDN'T be possible, but just in case.
    if (CurrentlySelectedSlot->bEmptySlot) {
        return;
    }

    const int32 CurrentIdx = CurrentlySelectedSlot->ItemIndex;
    const int32 CurrentRow = CurrentIdx / kColumns;
    const int32 CurrentCol = CurrentIdx % kColumns;

    const int32 NewRow = FMath::Clamp(CurrentRow + RowDelta, 0, (kInventorySize / kColumns) - 1);
    const int32 NewCol = FMath::Clamp(CurrentCol + ColDelta, 0, kColumns - 1);
    const int32 NewIdx = (NewRow * kColumns) + NewCol;

    UInventorySlotWidget* TargetSlot = SlotWidgets[CurrentCategory].Widgets[NewIdx];
    if (TargetSlot) {
        OnSlotClicked(TargetSlot);
    }
}

void UInventoryWidget::InitializeWidget(UInventoryComponent* InInventory) {
    if (InInventory) {
        LinkedInventory = InInventory;
        LinkedInventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryUpdate);
    }
}

// ReSharper disable CppMemberFunctionMayBeConst
void UInventoryWidget::OnSuppliesCategoryPressed() {
    OnCategoryChanged(EInventoryCategory::EIC_Supplies);
}

void UInventoryWidget::OnToolsCategoryPressed() {
    OnCategoryChanged(EInventoryCategory::EIC_Tool);
}

void UInventoryWidget::OnWeaponsCategoryPressed() {
    OnCategoryChanged(EInventoryCategory::EIC_Weapon);
}

void UInventoryWidget::OnNotesCategoryPressed() {
    OnCategoryChanged(EInventoryCategory::EIC_Note);
}

void UInventoryWidget::OnSlotClicked(UInventorySlotWidget* SlotWidget) {
    if (!SlotWidget) { return; }

    for (auto& [_, WidgetCategory] : SlotWidgets) {
        WidgetCategory.UnselectAll();
    }

    CurrentlySelectedSlot = SlotWidget;
    CurrentlySelectedSlot->SetSelected(true);

    ItemDescBorder->SetVisibility(ESlateVisibility::Visible);
    if (const UInventoryItem* ItemData = CurrentlySelectedSlot->Item.ItemData) {
        ItemNameText->SetText(ItemData->ItemName);
        ItemDescText->SetText(ItemData->ItemDescription);

        const auto LogoX = ItemData->ItemManufacturer->ManufacturerLogo->GetSizeX();
        const auto LogoY = ItemData->ItemManufacturer->ManufacturerLogo->GetSizeY();
        ItemManufacturerLogo->SetDesiredSizeOverride(FVector2D(LogoX, LogoY));
        ItemManufacturerLogo->SetBrushFromTexture(ItemData->ItemManufacturer->ManufacturerLogo);

        if (const auto* PC = GetOwningPlayer()) {
            if (const auto* HUD = Cast<AHUDBase>(PC->GetHUD())) {
                HUD->GetPreviewActor()->SetupPreview(ItemData->ItemPreviewMesh,
                                                     ItemData->ItemPreviewTransform);
                HUD->GetPreviewActor()->SetVisible(true);
            }
        }
    }
}

void UInventoryWidget::OnCategoryChanged(const EInventoryCategory& Category) {
    CurrentCategory = Category;
    CategoryWidgetSwitcher->SetActiveWidgetIndex(static_cast<int32>(Category));

    const auto OtherButtonStyle = GetCategoryButtonStyle();
    auto SelectedButtonStyle    = OtherButtonStyle;
    SelectedButtonStyle.Normal  = OtherButtonStyle.Hovered;

    TStaticArray<UButton*, 3> OtherButtons {nullptr, nullptr, nullptr};
    TStaticArray<UImage*, 3> OtherIcons {nullptr, nullptr, nullptr};

    switch (Category) {
        case EInventoryCategory::EIC_Supplies: {
            InventoryCategoryText->SetText(NSLOCTEXT("UI", "Inventory", "Supplies"));
            SuppliesCategoryButton->SetStyle(SelectedButtonStyle);
            SuppliesIcon->SetColorAndOpacity(FLinearColor(1.f, 0.79f, 0.3f, 1.f));

            OtherButtons[0] = ToolsCategoryButton;
            OtherButtons[1] = WeaponsCategoryButton;
            OtherButtons[2] = NotesCategoryButton;
            OtherIcons[0]   = ToolsIcon;
            OtherIcons[1]   = WeaponsIcon;
            OtherIcons[2]   = NotesIcon;

            break;
        }
        case EInventoryCategory::EIC_Tool: {
            InventoryCategoryText->SetText(NSLOCTEXT("UI", "Inventory", "Tools"));
            ToolsCategoryButton->SetStyle(SelectedButtonStyle);
            ToolsIcon->SetColorAndOpacity(FLinearColor(1.f, 0.79f, 0.3f, 1.f));

            OtherButtons[0] = SuppliesCategoryButton;
            OtherButtons[1] = WeaponsCategoryButton;
            OtherButtons[2] = NotesCategoryButton;
            OtherIcons[0]   = SuppliesIcon;
            OtherIcons[1]   = WeaponsIcon;
            OtherIcons[2]   = NotesIcon;

            break;
        }
        case EInventoryCategory::EIC_Weapon: {
            InventoryCategoryText->SetText(NSLOCTEXT("UI", "Inventory", "Weapons"));
            WeaponsCategoryButton->SetStyle(SelectedButtonStyle);
            WeaponsIcon->SetColorAndOpacity(FLinearColor(1.f, 0.79f, 0.3f, 1.f));

            OtherButtons[0] = ToolsCategoryButton;
            OtherButtons[1] = SuppliesCategoryButton;
            OtherButtons[2] = NotesCategoryButton;
            OtherIcons[0]   = ToolsIcon;
            OtherIcons[1]   = SuppliesIcon;
            OtherIcons[2]   = NotesIcon;

            break;
        }
        case EInventoryCategory::EIC_Note: {
            InventoryCategoryText->SetText(NSLOCTEXT("UI", "Inventory", "Notes"));
            NotesCategoryButton->SetStyle(SelectedButtonStyle);
            NotesIcon->SetColorAndOpacity(FLinearColor(1.f, 0.79f, 0.3f, 1.f));

            OtherButtons[0] = ToolsCategoryButton;
            OtherButtons[1] = WeaponsCategoryButton;
            OtherButtons[2] = SuppliesCategoryButton;
            OtherIcons[0]   = ToolsIcon;
            OtherIcons[1]   = WeaponsIcon;
            OtherIcons[2]   = SuppliesIcon;

            break;
        }
        default:
            return;
    }

    for (auto* Button : OtherButtons) {
        Button->SetStyle(OtherButtonStyle);
    }

    for (auto* Icon : OtherIcons) {
        Icon->SetColorAndOpacity(FLinearColor::White.CopyWithNewOpacity(0.3f));
    }
}

UUniformGridPanel* UInventoryWidget::GetCategoryGrid(const EInventoryCategory& Category) const {
    switch (Category) {
        case EInventoryCategory::EIC_Supplies:
            return SuppliesGrid;
        case EInventoryCategory::EIC_Tool:
            return ToolsGrid;
        case EInventoryCategory::EIC_Weapon:
            return WeaponsGrid;
        case EInventoryCategory::EIC_Note:
            return NotesGrid;
        default:
            return nullptr;
    }
}

FButtonStyle UInventoryWidget::GetCategoryButtonStyle() {
    FSlateBrush NormalBrush;
    NormalBrush.TintColor = FSlateColor(FLinearColor::Black.CopyWithNewOpacity(0.0f));
    NormalBrush.DrawAs    = ESlateBrushDrawType::Box;
    NormalBrush.ImageSize = FVector2D {80.f, 80.f};
    NormalBrush.SetResourceObject(nullptr);

    FSlateBrush HoveredBrush = NormalBrush;
    HoveredBrush.TintColor   = FSlateColor(FLinearColor::Black.CopyWithNewOpacity(0.4f));

    FSlateBrush PressedBrush = NormalBrush;
    PressedBrush.TintColor   = FSlateColor(FLinearColor::Black.CopyWithNewOpacity(0.8f));

    FButtonStyle Style;
    Style.Normal  = NormalBrush;
    Style.Hovered = HoveredBrush;
    Style.Pressed = PressedBrush;

    return Style;
}