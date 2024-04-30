// Fill out your copyright notice in the Description page of Project Settings.


#include "KismetMultiFPSLibrary.h"

void UKismetMultiFPSLibrary::SortValues(UPARAM(ref)TArray<FDeathMatchPlayerData>& Values)
{
	Values.Sort([](const FDeathMatchPlayerData& a, const FDeathMatchPlayerData& b) {return a.PlayerScore > b.PlayerScore; });
	//Qsort(Values, 0, Values.Num());
}

void UKismetMultiFPSLibrary::Qsort(UPARAM(ref)TArray<FDeathMatchPlayerData>& Values, int32 L, int32 R)
{
	if (L + 1 >= R) return;
	int32 First = L;
	int32 Last = R - 1;
	FDeathMatchPlayerData Key = Values[First];

	while (First < Last)
	{
		while (First < Last && Values[Last].PlayerScore >= Key.PlayerScore) --Last;
		Values[First] = Values[Last];
		while (First < Last && Values[First].PlayerScore <= Key.PlayerScore) ++First;
		Values[Last] = Values[First];
	}

	Values[First] = Key;
	Qsort(Values, L, First);
	Qsort(Values, First + 1, R);
}
