// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#define SURFACE_FLESHDEFAULT	SurfaceType1
#define SURFACE_FLESHVULNERABLE SurfaceType2
#define COLLISION_WEAPON		ECC_GameTraceChannel1
//用于打印的记录器
class COOPGAME_API DDRecord
{
private:
	//自身单例指针
	static TSharedPtr<DDRecord>RecordInst;

	//字符串缓存
	FString RecordInfo;

	//显示时间
	float ShowTime;
	
public:
	//输出模式
	uint8 PatternID;
	
public:
	void InitParams(float showTime=3000.f);
	
	//获取指向单例的指针
	static TSharedPtr<DDRecord>Get();
	
	//输出字符串
	inline void Out()
	{
		switch (PatternID)
		{
		case 0:
			GEngine->AddOnScreenDebugMessage(-1,ShowTime,FColor::Yellow,RecordInfo);break;
		case 1:
			UE_LOG(LogTemp,Log,TEXT("%s"),*RecordInfo);break;
		case 2:
			UE_LOG(LogTemp,Warning,TEXT("%s"),*RecordInfo);break;
		case 3:
			UE_LOG(LogTemp,Error,TEXT("%s"),*RecordInfo);break;
		}
		RecordInfo.Empty();
	}
	
	inline DDRecord&operator<<(FString Info){RecordInfo.Append(Info);return *this;}
	inline DDRecord&operator<<(const char* Info){RecordInfo+=Info;return *this;}
	inline DDRecord&operator<<(const char Info){RecordInfo.AppendChar(Info);return *this;}
	inline DDRecord&operator<<(int32 Info){RecordInfo.Append(FString::FromInt(Info));return *this;}
	inline DDRecord&operator<<(float Info){RecordInfo.Append(FString::SanitizeFloat(Info));return *this;}
	inline DDRecord&operator<<(bool Info){RecordInfo.Append(Info?FString("Ture"):FString("False"));return *this;}
	
	inline DDRecord&operator<<(FName Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FText Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FVector Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FRotator Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FQuat Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FMatrix Info){RecordInfo.Append(Info.ToString());return *this;}
	inline DDRecord&operator<<(FColor Info){RecordInfo.Append(Info.ToString());return *this;}
	
	inline void operator<<(TWeakPtr<DDRecord>Record){Out();ShowTime=3000.f;}
};

namespace DDH
{
	FORCEINLINE DDRecord& Debug(float ShowTime=3000.f)
	{
		DDRecord::Get()->PatternID=0;
		DDRecord::Get()->InitParams(ShowTime);
		return *DDRecord::Get();
	}
	
	FORCEINLINE DDRecord& Log()
	{
		DDRecord::Get()->PatternID=1;
		return *DDRecord::Get();
	}
	
	FORCEINLINE DDRecord& Warning()
	{
		DDRecord::Get()->PatternID=2;
		return *DDRecord::Get();
	}
	
	FORCEINLINE DDRecord& Error()
	{
		DDRecord::Get()->PatternID=3;
		return *DDRecord::Get();
	}
	
	FORCEINLINE TWeakPtr<DDRecord> Endl()
	{
		return DDRecord::Get();
	}

	//枚举转字符串
	template<typename TEnum>
	FORCEINLINE FString GetEnumValueAsString(const FString& Name,TEnum Value)
	{
		const UEnum *EnumPtr=FindObject<UEnum>(ANY_PACKAGE,*Name,true);
		if (!EnumPtr)
		{
			return FString("InValid");
		}
		//DDH::Debug()<<(int32)Value<<DDH::Endl();
		return EnumPtr->GetNameStringByIndex((int32)Value);
	}
	
	template<typename TEnum>
	FORCEINLINE FName GetEnumValueAsName(const FString& Name,TEnum Value)
	{
		const UEnum *EnumPtr=FindObject<UEnum>(ANY_PACKAGE,*Name,true);
		if (!EnumPtr)
		{
			return FName("InValid");
		}
		return FName(*EnumPtr->GetNameStringByIndex((int32)Value));
	}

	template<typename TEnum>
	FORCEINLINE TEnum GetEnumValueFromName(const FString& Name,FName Value)
	{
		const UEnum *EnumPtr=FindObject<UEnum>(ANY_PACKAGE,*Name,true);
		if (!EnumPtr)
		{
			return TEnum(0);
		}
		return (TEnum)EnumPtr->GetIndexByName(Value);
	}

	FORCEINLINE int32 GetEnumIndexFromName(const FString& Name,FName Value)
	{
		const UEnum *EnumPtr=FindObject<UEnum>(ANY_PACKAGE,*Name,true);
		if (!EnumPtr)
		{
			return -1;
		}
		return EnumPtr->GetIndexByName(Value);
	}
}

