// Fill out your copyright notice in the Description page of Project Settings.


#include "KismetMultiFPSLibrary.h"

void UKismetMultiFPSLibrary::SortValues(UPARAM(ref)TArray<FDeathMatchPlayerData>& Values)
{
	//Values.Sort([](const FDeathMatchPlayerData& a, const FDeathMatchPlayerData& b) {return a.PlayerScore > b.PlayerScore; });
	Qsort(Values, 0, Values.Num() - 1);
}

TArray<FDeathMatchPlayerData>& UKismetMultiFPSLibrary::Qsort(UPARAM(ref)TArray<FDeathMatchPlayerData>& Values, int l, int r)
{
	if (l >= r)
	{
		return Values;
	}
	int i = l;
	int j = r;
	FDeathMatchPlayerData mid = Values[(l + r) / 2];
	FDeathMatchPlayerData temp;
	do
	{
		while (Values[i].PlayerScore > mid.PlayerScore)i++;
		while (Values[j].PlayerScore < mid.PlayerScore)j--;
		if (i <= j)
		{
			temp = Values[i];
			Values[i] = Values[j];
			Values[j] = temp;
			i++;
			j--;
		}
	} while (i <= j);
	if (l < j) Qsort(Values, l, j);
	if (i < r) Qsort(Values, i, r);

	return Values;
}
