/*
 * Copyright (C) 2010 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */
#ifndef ___VBoxVideoVdma_h___
#define ___VBoxVideoVdma_h___

#include <iprt/cdefs.h>
#include <VBox/VBoxVideo.h>
#include "../VBoxVideo.h"

#if 0
typedef DECLCALLBACK(int) FNVBOXVDMASUBMIT(struct _DEVICE_EXTENSION* pDevExt, struct VBOXVDMAINFO * pInfo, HGSMIOFFSET offDr, PVOID pvContext);
typedef FNVBOXVDMASUBMIT *PFNVBOXVDMASUBMIT;

typedef struct VBOXVDMASUBMIT
{
    PFNVBOXVDMASUBMIT pfnSubmit;
    PVOID pvContext;
} VBOXVDMASUBMIT, *PVBOXVDMASUBMIT;
#endif

/* DMA commands are currently submitted over HGSMI */
typedef struct VBOXVDMAINFO
{
    HGSMIHEAP CmdHeap;
    UINT      uLastCompletedPagingBufferCmdFenceId;
    BOOL      fEnabled;
#if 0
    VBOXVDMASUBMIT Submitter;
#endif
} VBOXVDMAINFO, *PVBOXVDMAINFO;

int vboxVdmaCreate (struct _DEVICE_EXTENSION* pDevExt, VBOXVDMAINFO *pInfo, ULONG offBuffer, ULONG cbBuffer
#if 0
        , PFNVBOXVDMASUBMIT pfnSubmit, PVOID pvContext
#endif
        );
int vboxVdmaDisable (struct _DEVICE_EXTENSION* pDevExt, PVBOXVDMAINFO pInfo);
int vboxVdmaEnable (struct _DEVICE_EXTENSION* pDevExt, PVBOXVDMAINFO pInfo);
int vboxVdmaFlush (struct _DEVICE_EXTENSION* pDevExt, PVBOXVDMAINFO pInfo);
int vboxVdmaDestroy (struct _DEVICE_EXTENSION* pDevExt, PVBOXVDMAINFO pInfo);
int vboxVdmaCBufDrSubmit (struct _DEVICE_EXTENSION* pDevExt, PVBOXVDMAINFO pInfo, PVBOXVDMACBUF_DR pDr);
struct VBOXVDMACBUF_DR* vboxVdmaCBufDrCreate (PVBOXVDMAINFO pInfo, uint32_t cbTrailingData);
void vboxVdmaCBufDrFree (PVBOXVDMAINFO pInfo, struct VBOXVDMACBUF_DR* pDr);

#define VBOXVDMACBUF_DR_DATA_OFFSET() (sizeof (VBOXVDMACBUF_DR))
#define VBOXVDMACBUF_DR_SIZE(_cbData) (VBOXVDMACBUF_DR_DATA_OFFSET() + (_cbData))
#define VBOXVDMACBUF_DR_DATA(_pDr) ( ((uint8_t*)(_pDr)) + VBOXVDMACBUF_DR_DATA_OFFSET() )
#endif /* #ifndef ___VBoxVideoVdma_h___ */
