#include "BarrierCommon.h"


void LittleEndianToBigEndianBarrierParameters(BarrierParameters *ptr)
{
    if (ptr)
    {
        ptr->record_id = LittleEndianToBigEndianLongLong(ptr->record_id);
        ptr->latitude = LittleEndianToBigEndianDouble(ptr->latitude);
        ptr->longitude = LittleEndianToBigEndianDouble(ptr->longitude);
        ptr->altitude = LittleEndianToBigEndianInt(ptr->altitude);
        ptr->height = LittleEndianToBigEndianInt(ptr->height);
    }
}
void PrintfBarrierParameters(BarrierParameters *ptr)
{
    if (ptr)
    {
        printf("索引ID号:%lld\n", ptr->record_id);
        printf("类型:%s\n", ptr->obsType);
        printf("发光类型:%s\n", ptr->lightingType);
        printf("纬度:%lf\n", ptr->latitude);
        printf("经度:%lf\n", ptr->longitude);
        printf("海拔:%d\n", ptr->altitude);
        printf("高度:%d\n", ptr->height);
    }
}
