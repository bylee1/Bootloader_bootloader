/**
******************************************************************************
* @file    flash.c
* @author  정두원
* @date    2024-05-20
* @brief   STM32 FLASH Ctrl
* @details

*/

#include <stdio.h>
#include <string.h>
#include "flash.h"

/** @defgroup FLASH 제어 함수
 * @brief 내부 Flash 제어 및 펌웨어 업데이트
 * @{
 */

/** @defgroup FLASH_Variable FLASH 변수
 * @{
 */

struct CONFIG_BOOT
{
    unsigned char Idx[4];
    uint32_t size;
    uint32_t flag;
};

struct CONFIG_BOOT Boot_Cfg;

/**
 * @}
 */

/**
 * @brief 펌웨어 저장 영역 삭제
 *
 */
void Flash_FwErase(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; /* PAGE 단위 지우기 */
    EraseInitStruct.Page = FLASH_FW_PAGE_START;        /* 삭제 할 시작 페이지 */
    EraseInitStruct.NbPages = FLASH_FW_PAGE_SIZE;      /* 삭제 할 페이지 수 */

    HAL_FLASH_Unlock();
    uint32_t PageError = 0; /*!< 0xFFFFFFFF 값이면 정상 삭제됨 */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        /* Write Error */
    }
}

/**
 * @brief 펌웨어 업데이트 정보 삭제
 *
 * @return int
 */
static int Update_Config_Erase(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; /* PAGE 단위 지우기 */
    EraseInitStruct.Page = FLASH_FWINFO_PAGE;          /* 삭제 할 페이지 시작 주소 */
    EraseInitStruct.NbPages = 1;                       /* 삭제 할 페이지 수 */

    HAL_FLASH_Unlock();
    uint32_t PageError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief 펌웨어 업데이트 정보 쓰기
 *
 * @param firmwareSize
 */
static void Update_Config_Write(uint32_t firmwareSize)
{
    uint32_t Address = FLASH_FWINFO_ADDRESS_START;
    uint64_t tmpFlashReadData[ (sizeof(struct CONFIG_BOOT) / 8) + 1 ] = {
        0U,
    };

    memcpy(Boot_Cfg.Idx, "SBAN", 4);
    Boot_Cfg.size = firmwareSize; // 패킷 펌웨어 사이즈
    Boot_Cfg.flag = 1;

    memcpy((void *)tmpFlashReadData, (void *)&Boot_Cfg, sizeof(struct CONFIG_BOOT));

    for (int flashAddrIndex = 0; flashAddrIndex < (sizeof(struct CONFIG_BOOT) / 8) + 1; flashAddrIndex++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, tmpFlashReadData[flashAddrIndex]);
        Address += 8;
    }
}

/**
 * @brief 펌웨어 업데이트 프로세스
 *
 * @param firmwareData
 */
uint16_t firmNumLast = 0;
void procFirmwareUpdate()
{
    // 펌웨어 업데이트 테스트
    (void)Update_Config_Erase();
    Update_Config_Write(0xE000);
    printf("OK");
    // NVIC_SystemReset();
}

/*****************************************부트로더 코드 ********************************************/
typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

//--------------------------------------------------------
ErrorStatus updateFirmware(void)
{
    /* 어플리케이션 영역 삭제 */
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; /* PAGE 단위 지우기 */
    EraseInitStruct.Page = FLASH_APP_PAGE_START;       /* 삭제 할 시작주소 */
    EraseInitStruct.NbPages = FLASH_APP_PAGE_SIZE;     /* 삭제 할 페이지 수 */

    HAL_FLASH_Unlock();
    uint32_t PageError = 0U; /*!< 0xFFFFFFFF 값이면 정상 삭제됨 */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        /* Erase Error */
        return ERROR;
    }

    /* 복사 */
    uint32_t Address_Application = FLASH_APPLECATION_ADDRESS_START;
    uint32_t Address_Fiwmare = FLASH_FW_ADDRESS_START;
    uint32_t flashAddrIndexEnd = ((Boot_Cfg.size + 7) / 8);

    HAL_FLASH_Unlock();
    for (uint32_t flashAddrIndex = 0U; flashAddrIndex < flashAddrIndexEnd; flashAddrIndex++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address_Application, *(__IO uint64_t *)Address_Fiwmare) != HAL_OK)
        {
            /* error 처리 */
            return ERROR;
        }
        Address_Application += 8U;
        Address_Fiwmare += 8U;
    }

    return SUCCESS;
}

//--------------------------------------------------------
void bootloader(void)
{
    memcpy(&Boot_Cfg, (unsigned char *)FLASH_FWINFO_ADDRESS_START, sizeof(struct CONFIG_BOOT));

    if (memcmp(&Boot_Cfg.Idx[0], "SBAN", 4) == 0)
    {
        if (Boot_Cfg.flag && Boot_Cfg.size && Boot_Cfg.size <= 0xE000)
        {
            if (updateFirmware() == SUCCESS)
            {
                Flash_FwErase();
            }
        }
        Update_Config_Erase();
    }

    /* Jump to user application */
    JumpAddress = *(__IO uint32_t *)(FLASH_APPLECATION_ADDRESS_START + 4);
    Jump_To_Application = (pFunction)JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)FLASH_APPLECATION_ADDRESS_START);
    Jump_To_Application();
}

/**
 * @}
 */
