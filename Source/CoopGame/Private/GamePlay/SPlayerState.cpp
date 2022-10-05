// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/SPlayerState.h"

void ASPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}
