#include "basicFunctions.h"

int32 dAtoi	(const char*pString){
	return static_cast<int32>(dAtoll(pString));
}

uint32 dAtoui(const char*pString){
	if(!pString) return 0;
	return static_cast<uint32>(strtoul(pString,nullptr,0));
}

long dAtol(const char*pString){
	return static_cast<long>(dAtoll(pString));
}
 
double dAtof(const char*pString){
	if(!pString) return 0.0f;
	return atof(pString);
}
 
int64 dAtoll(const char*pString){
	if(!pString)return 0;
#ifdef WIN32
	return _atoi64(pString);
#else//WIN32
	return atoll(pString);
#endif//WIN32
}

uint64 dAtoull(const char*pString){
	if(!pString) return 0;
#ifdef WIN32
	return _strtoui64(pString,nullptr,10);
#else//WIN32
	return strtoull(pString,nullptr,10);
#endif//WIN32
}

uint8 getNumberBit(uint32 uNumber){
	uint8 uBit = 0;
	while (uNumber != 0){
		uBit++;
		uNumber /= 10;
	}
	return uBit;
}

uint32 getAddValue(uint32 uFrom,uint32 uValue,uint32 nMax){
	uint32 uSum = uFrom + uValue;
	if (uFrom > uSum || uSum >= nMax)
		return nMax;
	return uSum;
}

uint32 addValue(uint32 uValue,int nValue,uint32 nMax){
	if (nValue < 0 && uValue < uint32(-1 * nValue))
		uValue = 0;
	else
		uValue += nValue;

	if (uValue > nMax)
		uValue = nMax;

	return uValue;
}

bool setInt(int32& iOld,int32 iNew,int32 iMax,int32 iMin){
	if(iMax < iMin) iMax = iMin;
	if(iOld == iNew) return false;

	iOld = iNew;
	if(iOld < iMin) iOld = iMin;
	if(iOld > iMax) iOld = iMax;
	return true;
}

bool setInt8(int8&iOld,int8 iNew,int8 iMax,int8 iMin){
	if(iMax < iMin) iMax = iMin;
	if(iOld == iNew) return false;
	
	iOld = iNew;
	if(iOld < iMin) iOld = iMin;
	if(iOld > iMax) iOld = iMax;
	return true;
}

bool canUpdateInt64(int64 iValue,int64 iUpdate,int64 iMax,int64 iMin){
    if(iMax < iMin) iMax = iMin;
    if(iUpdate == 0) return false;

    if(iValue < 0 && iUpdate < 0){
        iValue +=iUpdate;
		if (iValue > 0 || iValue < iMin) return false;
    }else if(iValue > 0 && iUpdate > 0){
        iValue +=iUpdate;
        if(iValue < 0 || iValue > iMax) return false;
    } else{
        return true;
    }
    return true;
}

bool updateInt64(int64&iValue,int64 iUpdate,int64 iMax,int64 iMin){
    if(iMax < iMin) iMax = iMin;
    if(iUpdate == 0) return false;
    if(iValue < 0 && iUpdate < 0){
        iValue +=iUpdate;
        if(iValue > 0)iValue = iMin;
    }else if(iValue > 0 && iUpdate > 0){
        iValue +=iUpdate;
        if(iValue < 0) iValue = iMax;
    }else{
        iValue +=iUpdate;
    }
    if(iValue > iMax) iValue = iMax;
    if(iValue < iMin) iValue = iMin;
    return true;
}

bool updateInt(int32&iValue,int32 iUpdate,int32 iMax,int32 iMin){
	if(iMax < iMin)iMax = iMin;

	if(iUpdate == 0)return false;

	if(iValue < 0 && iUpdate < 0){
		iValue +=iUpdate;
		if(iValue > 0) iValue = iMin;
	}else if(iValue > 0 && iUpdate > 0){
		iValue +=iUpdate;
		if(iValue < 0)iValue = iMax;
	}else{
		iValue +=iUpdate;
	}

	if(iValue > iMax)iValue = iMax;
	if(iValue < iMin)iValue = iMin;
	return true;
}

bool updateUint32(uint32&uValue,int32 iUpdate,uint32 uMax){
	if(iUpdate < 0){
		if(uValue < uint32(iUpdate * -1))
			uValue = 0;
		else
			uValue += iUpdate;
	}else{
		uint32 uTemp = uValue + iUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint32(iUpdate))
			uValue = uMax;
		else
			uValue = uTemp;
	}
	return true;
}

bool canUpdateUint32(uint32 uValue,int32 iUpdate,uint32 uMax){
	if(iUpdate == 0) return true;

	if(iUpdate < 0){
		if(uValue < uint32(iUpdate * -1)) return false;
	}else{
		uint32 uTemp = uValue + iUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint32(iUpdate))
			return false;
	}
	return true;
}

bool canUpdateUint32(uint32 uValue,uint32 uUpdate,bool bAdd,uint32 uMax){
	if(uUpdate == 0)return true;
	if(bAdd){
		uint32 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint32(uUpdate))
			return false;
	}else{
		if(uValue < uUpdate) return false;	
	}
	return true;
}

bool updateUint32(uint32&uValue,uint32&uUpdate,bool bAdd,uint32 uMax){
	if(bAdd){
		uint32 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uUpdate)
			uTemp = uMax;

		uUpdate -= (uTemp - uValue);
		uValue	= uTemp;
	}else{
		uint32 uTemp = 0;
		if(uValue >= uUpdate)
			uTemp = uValue - uUpdate;

		uUpdate -=(uValue - uTemp);
		uValue	= uTemp;
	}
	return true;
}

bool updateUint32_(uint32&uValue,uint32 uUpdate,bool bAdd,uint32 uMax){
	if(bAdd){
		uint32 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uUpdate)
			uTemp = uMax;

		uUpdate -= (uTemp - uValue);
		uValue	= uTemp;
	}else{
		uint32 uTemp = 0;
		if(uValue >= uUpdate)
			uTemp = uValue - uUpdate;

		uUpdate -=(uValue - uTemp);
		uValue	= uTemp;
	}
	return true;
}

bool updateUint64(uint64&uValue,int64 iUpdate,uint64 uMax/* = 0xFFFFFFFFFFFFFFFF*/){
	if(iUpdate < 0){
		if(uValue < uint64(iUpdate * -1))
			uValue = 0;
		else
			uValue += iUpdate;
	}else{
		uint64 uTemp = uValue + iUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint64(iUpdate))
			uValue = uMax;
		else
			uValue = uTemp;
	}
	return true;
}

bool canUpdateUint64(uint64 uValue,int64 iUpdate,uint64 uMax/* = 0xFFFFFFFFFFFFFFFF*/){
	if(iUpdate == 0)return true;
	if(iUpdate < 0){
		if(uValue < uint64(iUpdate * -1)) return false;
	}else{
		uint64 uTemp = uValue + iUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint64(iUpdate)) return false;
	}
	return true;
}

bool canUpdateUint64(uint64 uValue,uint64 uUpdate,bool bAdd,uint64 uMax/* = 0xFFFFFFFFFFFFFFFF*/){
	if(uUpdate == 0)return true;
	if(bAdd){
		uint64 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uint64(uUpdate)) return false;
	}else{
		if(uValue < uUpdate) return false;	
	}
	return true;
}

bool updateUint64(uint64&uValue,uint64&uUpdate,bool bAdd,uint64 uMax/* = 0xFFFFFFFFFFFFFFFF*/){
	if(bAdd){
		uint64 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uUpdate)
			uTemp = uMax;

		uUpdate -= (uTemp - uValue);
		uValue	= uTemp;
	}else{
		uint64 uTemp = 0;
		if(uValue >= uUpdate)
			uTemp = uValue - uUpdate;

		uUpdate -=(uValue - uTemp);
		uValue	= uTemp;
	}
	return true;
}

bool updateUint64_(uint64&uValue,uint64 uUpdate,bool bAdd,uint64 uMax/* = 0xFFFFFFFFFFFFFFFF*/){
	if(bAdd){
		uint64 uTemp = uValue + uUpdate;
		if(uTemp > uMax || uTemp < uValue || uTemp < uUpdate)
			uTemp = uMax;

		uUpdate -= (uTemp - uValue);
		uValue	= uTemp;
	}else{
		uint64 uTemp = 0;
		if(uValue >= uUpdate)
			uTemp = uValue - uUpdate;

		uUpdate -=(uValue - uTemp);
		uValue	= uTemp;
	}
	return true;
}

uint32 getUint32Proportion(uint32 uValue,float32 fRatio,bool bRoundedUp){
	float32 fReturn = uValue * fRatio;
	uint32	uReturn = uint32(fReturn);
	if(bRoundedUp){
		if(float32(uReturn) < fReturn) uReturn++;
	}
	return uReturn;
}

uint32 dDivisionUint32(uint64 _molecular,uint32 _denominator,bool _floor){
	if(!_molecular || !_denominator) return 0;
	uint32 _return = uint32(_molecular / _denominator);
	if(!_floor && (_molecular % _denominator) > 0)
		++_return;
	return _return;
}

uint64 dDivisionUint64(uint64 _molecular,uint64 _denominator,bool _floor){
	if(!_molecular || !_denominator) return 0;
	uint64 _return = _molecular / _denominator;
	if(!_floor && (_molecular % _denominator) > 0)
		++_return;
	return _return;
}

bool canMultiplicationUint16(uint16 _multiplier, uint16 _multiplicand, uint16 uMax){
	uint32 uProduct = (uint32)_multiplier *  (uint32)_multiplicand;
	if(uProduct > uMax) return false;
	return true;
}

bool canMultiplicationUint32(uint32 _multiplier, uint32 _multiplicand, uint32 uMax){
	uint64 uProduct = (uint64)_multiplier *  (uint64)_multiplicand;
	if(uProduct > uMax) return false;
	return true;
}

void multiplicationUint32(uint32& _multiplier, float _multiplicand, uint32 uMax){
	uint64 uProduct = uint64(_multiplier * _multiplicand);
	if(uProduct > uMax)
		_multiplier = uMax;
	else
		_multiplier = (uint32)uProduct;
}
