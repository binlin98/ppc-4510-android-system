/*
 * =============================================================================
 *
 *       Filename:  wifi_adapter.c
 *
 *    Description:  
 *
 *         Author:  Nick Lau
 *          Email:  liudeping@norco.com.cn
 *        Company:  Shenzhen Norco Intelligent Technology Co., Ltd.
 *
 *        Version:  1.0
 *        Created:  04/13/2016 02:17:06 PM
 *        License:  Copyright (c) 2016, Nick Lau.
 *                  This program is free software; you can redistribute it
 *                  and/or modify it under the terms of the GNU General Public
 *                  License as published by the Free Software Foundation,
 *                  version 2 of the License.
 *                  This program is distributed in the hope that it will be
 *                  useful, but WITHOUT ANY WARRANTY; without even the implied
 *                  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                  PURPOSE.
 *                  See the GNU General Public License version 2 for more
 *                  details.
 *       Revision:  none
 * =============================================================================
 */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <cutils/properties.h>

struct Products {
	const char* idVendor;
	const char* idProduct;
	const char* driverName;
	const char* driverPath;
	const char* driverArgs;
};

struct Products list_products[] = {
	//8192CU
	{"0bda", "8176", "8192cu", "/system/lib/modules/8192cu.ko", "ifname=wlan0 if2name=p2p0"},
	{"0bda", "018a", "8192cu", "/system/lib/modules/8192cu.ko", "ifname=wlan0 if2name=p2p0"},
	{"0bda", "8191", "8192cu", "/system/lib/modules/8192cu.ko", "ifname=wlan0 if2name=p2p0"},
	//8188ETV
	{"0bda", "8179", "8188eu", "/system/lib/modules/8188eu.ko", "ifname=wlan0 if2name=p2p0"},
	//8723BU
	{"0bda", "b720", "8723bu", "/system/lib/modules/8723bu.ko", "ifname=wlan0 if2name=p2p0"},
};

#define MAX_PATH 2
const char* find_paths[MAX_PATH] = {
	"/sys/devices/soc0/soc.0/2100000.aips-bus/2184000.usb/ci_hdrc.0",
	"/sys/devices/soc0/soc.0/2100000.aips-bus/2184200.usb/ci_hdrc.1"
};

#define PRODUCT_NUM	(sizeof(list_products) / sizeof(struct Products))

static int match = 0;

void match_product(void)
{
	int fd;
	char idVendor[5];
	char idProduct[5];

	fd = open("idVendor", O_RDONLY);
	if (fd >= 0) {
		read(fd, idVendor, sizeof(idVendor));
		idVendor[4]='\0';
		close(fd);
	} else {
		return;
	}

	fd = open("idProduct", O_RDONLY);
	if (fd >= 0) {
		read(fd, idProduct, sizeof(idProduct));
		idProduct[4]='\0';
		close(fd);
	} else {
		return;
	}

	for (unsigned int i = 0; i < PRODUCT_NUM; i++) {
		if (!strcmp(list_products[i].idProduct, idProduct)
				&& !strcmp(list_products[i].idVendor, idVendor)) {
			property_set("wlan.driver.name", list_products[i].driverName);
			property_set("wlan.driver.path", list_products[i].driverPath);
			property_set("wlan.driver.arg", list_products[i].driverArgs);
			printf("%s %s %s\n", list_products[i].driverName,
					list_products[i].driverPath,
					list_products[i].driverArgs);
			match = 1;
		}
	}

	return;
}

void printdir(const char* dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if ((dp = opendir(dir)) == NULL) {
		fprintf(stderr, "Can't open directory %s\n", dir);
		return;
	}

	chdir(dir);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 ||
					strcmp(entry->d_name, "..") == 0)
				continue;
			printdir(entry->d_name, depth+4);
			if(match == 1)
				return ;
		} else if (strstr(entry->d_name, "idVendor") != NULL ||
				strstr(entry->d_name, "idProduct") != NULL) {
			match_product();
		}
	}
	chdir("..");
	closedir(dp);
}


/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  main
 *  Description:  
 * =============================================================================
 */
	int
main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	char value[PROPERTY_VALUE_MAX];

        while(1)
        {
		// 检查是否成功适配
		if (property_get("wlan.driver.name", value, NULL) == 0) {
			// 这里表示还没成功适配
		} else {
			exit(0); // 成功之后不再适配直接退出
		}

		match = 0;
		for (unsigned int i = 0; i < MAX_PATH; i++) {
			printdir(find_paths[i], 0);
		}
		sleep(2); 
        }


	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */





