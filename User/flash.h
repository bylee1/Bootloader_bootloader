#ifndef FLASH_H__
#define FLASH_H__ 1

#include "main.h"

// #define COMPILE_BOOTLOADER  /*!< 부트로더 컴파일용 */

#define FLASH_BOOT_ADDRESS_START 0x08000000U        /*!< 부트로더 시작 주소. 크기 12K */
#define FLASH_APPLECATION_ADDRESS_START 0x08003000U /*!< 어플리케이션 펌웨어 시작 주소. 크기 56K */
#define FLASH_FW_ADDRESS_START 0x08011000U          /*!< 업데이트 펌웨어 시작 주소. 크기 56K */
#define FLASH_FW_ADDRESS_END 0x0801F000U            /*!< 업데이트 펌웨어 끝 주소 */
#define FLASH_FWINFO_ADDRESS_START 0x0801F000U      /*!< 업데이트 펌웨어 정보 시작 주소. 크기 2K */

#define FLASH_APP_PAGE_START ((FLASH_APPLECATION_ADDRESS_START - FLASH_BOOT_ADDRESS_START) / FLASH_PAGE_SIZE) /*!< 업데이트 펌웨어 시작 페이지 */
#define FLASH_APP_PAGE_END ((FLASH_FW_ADDRESS_START - FLASH_BOOT_ADDRESS_START) / FLASH_PAGE_SIZE)            /*!< 업데이트 펌웨어 끝 페이지 */
#define FLASH_APP_PAGE_SIZE (FLASH_APP_PAGE_END - FLASH_APP_PAGE_START)

#define FLASH_FW_PAGE_START ((FLASH_FW_ADDRESS_START - FLASH_BOOT_ADDRESS_START) / FLASH_PAGE_SIZE) /*!< 업데이트 펌웨어 시작 페이지 */
#define FLASH_FW_PAGE_END ((FLASH_FW_ADDRESS_END - FLASH_BOOT_ADDRESS_START) / FLASH_PAGE_SIZE)      /*!< 업데이트 펌웨어 끝 페이지 */
#define FLASH_FW_PAGE_SIZE (FLASH_FW_PAGE_END - FLASH_FW_PAGE_START)

#define FLASH_FWINFO_PAGE ((FLASH_FWINFO_ADDRESS_START - FLASH_BOOT_ADDRESS_START) / FLASH_PAGE_SIZE) /*!< 업데이트 정보 페이지 */

void procFirmwareUpdate();
void bootloader(void);

#endif /* FLASH_H__ */
