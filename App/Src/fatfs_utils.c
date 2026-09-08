#include "fatfs_utils.h"
#include <string.h>

UAL_FATFS_Status_t UAL_FATFS_UTILITY_MountDrive(FATFS *fatfs) {
	return f_mount(fatfs, "", 1) == FR_OK ?
			UAL_FATFS_STATUS_OK : UAL_FATFS_STATUS_ERROR;
}

UAL_FATFS_Status_t UAL_FATFS_UTILITY_UnmountDrive() {
	return f_mount(NULL, "", 0) == FR_OK ?
			UAL_FATFS_STATUS_OK : UAL_FATFS_STATUS_ERROR;
}
