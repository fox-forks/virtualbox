/* $Id$ */
/** @file
 * PDM IOMMU - All Contexts.
 */

/*
 * Copyright (C) 2021 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#define LOG_GROUP LOG_GROUP_PDM
#define PDMPCIDEV_INCLUDE_PRIVATE  /* Hack to get pdmpcidevint.h included at the right point. */
#include "PDMInternal.h"

#include <VBox/vmm/vmcc.h>
#ifdef IN_RING3
# include <iprt/mem.h>
#endif


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/
/**
 * Gets the PDM IOMMU for the current context from the PDM device instance.
 */
#ifdef IN_RING0
#define PDMDEVINS_TO_IOMMU(a_pDevIns)   &(a_pDevIns)->Internal.s.pGVM->pdmr0.s.aIommus[0];
#else
#define PDMDEVINS_TO_IOMMU(a_pDevIns)   &(a_pDevIns)->Internal.s.pVMR3->pdm.s.aIommus[0];
#endif


/**
 * Gets the PCI device ID (Bus:Dev:Fn) for the given PCI device.
 *
 * @returns PCI device ID.
 * @param   pDevIns     The device instance.
 * @param   pPciDev     The PCI device structure. Cannot be NULL.
 */
DECL_FORCE_INLINE(uint16_t) pdmIommuGetPciDeviceId(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev)
{
    uint8_t const idxBus = pPciDev->Int.s.idxPdmBus;
#if defined(IN_RING0)
    PGVM pGVM = pDevIns->Internal.s.pGVM;
    Assert(idxBus < RT_ELEMENTS(pGVM->pdmr0.s.aPciBuses));
    PCPDMPCIBUSR0 pBus = &pGVM->pdmr0.s.aPciBuses[idxBus];
#elif defined(IN_RING3)
    PVM pVM = pDevIns->Internal.s.pVMR3;
    Assert(idxBus < RT_ELEMENTS(pVM->pdm.s.aPciBuses));
    PCPDMPCIBUS pBus = &pVM->pdm.s.aPciBuses[idxBus];
#endif
    return PCIBDF_MAKE(pBus->iBus, pPciDev->uDevFn);
}


/**
 * Bus master physical memory read after translating the physical address using the
 * IOMMU.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns     The device instance.
 * @param   pPciDev     The PCI device. Cannot be NULL.
 * @param   GCPhys      The guest-physical address to read.
 * @param   pvBuf       Where to put the data read.
 * @param   cbRead      How many bytes to read.
 * @param   fFlags      Combination of PDM_DEVHLP_PHYS_RW_F_XXX.
 *
 * @thread  Any.
 */
int pdmIommuMemAccessRead(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, RTGCPHYS GCPhys, void *pvBuf, size_t cbRead, uint32_t fFlags)
{
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        int rc = VINF_SUCCESS;
        while (cbRead > 0)
        {
            RTGCPHYS GCPhysOut;
            size_t   cbContig;
            rc = pIommu->pfnMemAccess(pDevInsIommu, uDeviceId, GCPhys, cbRead, PDMIOMMU_MEM_F_READ, &GCPhysOut, &cbContig);
            if (RT_SUCCESS(rc))
            {
                /** @todo Handle strict return codes from PGMPhysRead. */
                rc = pDevIns->CTX_SUFF(pHlp)->pfnPhysRead(pDevIns, GCPhysOut, pvBuf, cbRead, fFlags);
                if (RT_SUCCESS(rc))
                {
                    cbRead -= cbContig;
                    pvBuf   = (void *)((uintptr_t)pvBuf + cbContig);
                    GCPhys += cbContig;
                }
                else
                    break;
            }
            else
            {
                LogFunc(("IOMMU memory read failed. uDeviceId=%#x GCPhys=%#RGp cb=%zu rc=%Rrc\n", uDeviceId, GCPhys, cbRead, rc));
                break;
            }
        }
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
}


/**
 * Bus master physical memory write after translating the physical address using the
 * IOMMU.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns     The device instance.
 * @param   pPciDev     The PCI device structure. Cannot be NULL.
 * @param   GCPhys      The guest-physical address to write.
 * @param   pvBuf       The data to write.
 * @param   cbWrite     How many bytes to write.
 * @param   fFlags      Combination of PDM_DEVHLP_PHYS_RW_F_XXX.
 *
 * @thread  Any.
 */
int pdmIommuMemAccessWrite(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, RTGCPHYS GCPhys, const void *pvBuf, size_t cbWrite,
                           uint32_t fFlags)
{
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        int rc = VINF_SUCCESS;
        while (cbWrite > 0)
        {
            RTGCPHYS GCPhysOut;
            size_t   cbContig;
            rc = pIommu->pfnMemAccess(pDevInsIommu, uDeviceId, GCPhys, cbWrite, PDMIOMMU_MEM_F_WRITE, &GCPhysOut, &cbContig);
            if (RT_SUCCESS(rc))
            {
                /** @todo Handle strict return codes from PGMPhysWrite. */
                rc = pDevIns->CTX_SUFF(pHlp)->pfnPhysWrite(pDevIns, GCPhysOut, pvBuf, cbWrite, fFlags);
                if (RT_SUCCESS(rc))
                {
                    cbWrite -= cbContig;
                    pvBuf    = (const void *)((uintptr_t)pvBuf + cbContig);
                    GCPhys  += cbContig;
                }
                else
                    break;
            }
            else
            {
                LogFunc(("IOMMU memory write failed. uDeviceId=%#x GCPhys=%#RGp cb=%zu rc=%Rrc\n", uDeviceId, GCPhys, cbWrite,
                         rc));
                break;
            }
        }
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
}


/**
 * Requests the mapping of a guest page into ring-3 in preparation for a bus master
 * physical memory read operation.
 *
 * Refer pfnPhysGCPhys2CCPtrReadOnly() for further details.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns     The device instance.
 * @param   pPciDev     The PCI device structure. Cannot be NULL.
 * @param   GCPhys      The guest physical address of the page that should be
 *                      mapped.
 * @param   fFlags      Flags reserved for future use, MBZ.
 * @param   ppv         Where to store the address corresponding to GCPhys.
 * @param   pLock       Where to store the lock information that
 *                      pfnPhysReleasePageMappingLock needs.
 */
int pdmIommuMemAccessReadCCPtr(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, RTGCPHYS GCPhys, uint32_t fFlags, void const **ppv,
                               PPGMPAGEMAPLOCK pLock)
{
#if defined(IN_RING3)
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        size_t   cbContig  = 0;
        RTGCPHYS GCPhysOut = NIL_RTGCPHYS;
        int rc = pIommu->pfnMemAccess(pDevInsIommu, uDeviceId, GCPhys & X86_PAGE_BASE_MASK, X86_PAGE_SIZE, PDMIOMMU_MEM_F_READ,
                                      &GCPhysOut, &cbContig);
        if (RT_SUCCESS(rc))
        {
            Assert(GCPhysOut != NIL_RTGCPHYS);
            Assert(cbContig == X86_PAGE_SIZE);
            return pDevIns->pHlpR3->pfnPhysGCPhys2CCPtrReadOnly(pDevIns, GCPhysOut, fFlags, ppv, pLock);
        }

        LogFunc(("IOMMU memory read for pointer access failed. uDeviceId=%#x GCPhys=%#RGp rc=%Rrc\n", uDeviceId, GCPhys, rc));
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
#else
    RT_NOREF6(pDevIns, pPciDev, GCPhys, fFlags, ppv, pLock);
    return VERR_NOT_IMPLEMENTED;
#endif
}


/**
 * Requests the mapping of a guest page into ring-3 in preparation for a bus master
 * physical memory write operation.
 *
 * Refer pfnPhysGCPhys2CCPtr() for further details.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns     The device instance.
 * @param   pPciDev     The PCI device structure. Cannot be NULL.
 * @param   GCPhys      The guest physical address of the page that should be
 *                      mapped.
 * @param   fFlags      Flags reserved for future use, MBZ.
 * @param   ppv         Where to store the address corresponding to GCPhys.
 * @param   pLock       Where to store the lock information that
 *                      pfnPhysReleasePageMappingLock needs.
 */
int pdmIommuMemAccessWriteCCPtr(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, RTGCPHYS GCPhys, uint32_t fFlags, void **ppv,
                                PPGMPAGEMAPLOCK pLock)
{
#if defined(IN_RING3)
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        size_t   cbContig  = 0;
        RTGCPHYS GCPhysOut = NIL_RTGCPHYS;
        int rc = pIommu->pfnMemAccess(pDevInsIommu, uDeviceId, GCPhys & X86_PAGE_BASE_MASK, X86_PAGE_SIZE, PDMIOMMU_MEM_F_WRITE,
                                      &GCPhysOut, &cbContig);
        if (RT_SUCCESS(rc))
        {
            Assert(GCPhysOut != NIL_RTGCPHYS);
            Assert(cbContig == X86_PAGE_SIZE);
            return pDevIns->pHlpR3->pfnPhysGCPhys2CCPtr(pDevIns, GCPhysOut, fFlags, ppv, pLock);
        }

        LogFunc(("IOMMU memory write for pointer access failed. uDeviceId=%#x GCPhys=%#RGp rc=%Rrc\n", uDeviceId, GCPhys, rc));
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
#else
    RT_NOREF6(pDevIns, pPciDev, GCPhys, fFlags, ppv, pLock);
    return VERR_NOT_IMPLEMENTED;
#endif
}


/**
 * Requests the mapping of multiple guest pages into ring-3 in prepartion for a bus
 * master physical memory read operation.
 *
 * Refer pfnPhysBulkGCPhys2CCPtrReadOnly() for further details.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns             The device instance.
 * @param   pPciDev             The PCI device structure. Cannot be NULL.
 * @param   cPages              Number of pages to lock.
 * @param   paGCPhysPages       The guest physical address of the pages that
 *                              should be mapped (@a cPages entries).
 * @param   fFlags              Flags reserved for future use, MBZ.
 * @param   papvPages           Where to store the ring-3 mapping addresses
 *                              corresponding to @a paGCPhysPages.
 * @param   paLocks             Where to store the locking information that
 *                              pfnPhysBulkReleasePageMappingLock needs (@a cPages
 *                              in length).
 */
int pdmIommuMemAccessBulkReadCCPtr(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t cPages, PCRTGCPHYS paGCPhysPages,
                                   uint32_t fFlags, const void **papvPages, PPGMPAGEMAPLOCK paLocks)
{
#ifdef IN_RING3
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        /* Allocate space for translated addresses. */
        size_t const cbIovas  = cPages * sizeof(uint64_t);
        PRTGCPHYS paGCPhysOut = (PRTGCPHYS)RTMemAllocZ(cbIovas);
        if (paGCPhysOut)
        { /* likely */ }
        else
        {
            LogFunc(("caller='%s'/%d: returns %Rrc - Failed to alloc %zu bytes for IOVA addresses\n",
                     pDevIns->pReg->szName, pDevIns->iInstance, VERR_NO_MEMORY, cbIovas));
            return VERR_NO_MEMORY;
        }

        /* Ask the IOMMU for corresponding translated physical addresses. */
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        AssertCompile(sizeof(RTGCPHYS) == sizeof(uint64_t));
        int rc = pIommu->pfnMemBulkAccess(pDevInsIommu, uDeviceId, cPages, (uint64_t const *)paGCPhysPages, PDMIOMMU_MEM_F_READ,
                                          paGCPhysOut);
        if (RT_SUCCESS(rc))
        {
            /* Perform the bulk mapping but with the translated addresses. */
            rc = pDevIns->pHlpR3->pfnPhysBulkGCPhys2CCPtrReadOnly(pDevIns, cPages, paGCPhysOut, fFlags, papvPages, paLocks);
            if (RT_FAILURE(rc))
                LogFunc(("Bulk mapping for read access failed. cPages=%zu fFlags=%#x rc=%Rrc\n", rc, cPages, fFlags));
        }
        else
            LogFunc(("Bulk translation for read access failed. uDeviceId=%#x cPages=%zu rc=%Rrc\n", uDeviceId, cPages, rc));

        RTMemFree(paGCPhysOut);
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
#else
    RT_NOREF7(pDevIns, pPciDev, cPages, paGCPhysPages, fFlags, papvPages, paLocks);
    return VERR_NOT_IMPLEMENTED;
#endif
}


/**
 * Requests the mapping of multiple guest pages into ring-3 in prepartion for a bus
 * master physical memory write operation.
 *
 * Refer pfnPhysBulkGCPhys2CCPtr() for further details.
 *
 * @returns VBox status code.
 * @retval  VERR_IOMMU_NOT_PRESENT if an IOMMU is not present.
 *
 * @param   pDevIns             The device instance.
 * @param   pPciDev             The PCI device structure. Cannot be NULL.
 * @param   cPages              Number of pages to lock.
 * @param   paGCPhysPages       The guest physical address of the pages that
 *                              should be mapped (@a cPages entries).
 * @param   fFlags              Flags reserved for future use, MBZ.
 * @param   papvPages           Where to store the ring-3 mapping addresses
 *                              corresponding to @a paGCPhysPages.
 * @param   paLocks             Where to store the locking information that
 *                              pfnPhysBulkReleasePageMappingLock needs (@a cPages
 *                              in length).
 */
int pdmIommuMemAccessBulkWriteCCPtr(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t cPages, PCRTGCPHYS paGCPhysPages,
                                    uint32_t fFlags, void **papvPages, PPGMPAGEMAPLOCK paLocks)
{
#ifdef IN_RING3
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        /* Allocate space for translated addresses. */
        size_t const cbIovas  = cPages * sizeof(uint64_t);
        PRTGCPHYS paGCPhysOut = (PRTGCPHYS)RTMemAllocZ(cbIovas);
        if (paGCPhysOut)
        { /* likely */ }
        else
        {
            LogFunc(("caller='%s'/%d: returns %Rrc - Failed to alloc %zu bytes for IOVA addresses\n",
                     pDevIns->pReg->szName, pDevIns->iInstance, VERR_NO_MEMORY, cbIovas));
            return VERR_NO_MEMORY;
        }

        /* Ask the IOMMU for corresponding translated physical addresses. */
        uint16_t const uDeviceId = pdmIommuGetPciDeviceId(pDevIns, pPciDev);
        AssertCompile(sizeof(RTGCPHYS) == sizeof(uint64_t));
        int rc = pIommu->pfnMemBulkAccess(pDevInsIommu, uDeviceId, cPages, (uint64_t const *)paGCPhysPages, PDMIOMMU_MEM_F_WRITE,
                                          paGCPhysOut);
        if (RT_SUCCESS(rc))
        {
            /* Perform the bulk mapping but with the translated addresses. */
            rc = pDevIns->pHlpR3->pfnPhysBulkGCPhys2CCPtr(pDevIns, cPages, paGCPhysOut, fFlags, papvPages, paLocks);
            if (RT_FAILURE(rc))
                LogFunc(("Bulk mapping of addresses failed. cPages=%zu fFlags=%#x rc=%Rrc\n", rc, cPages, fFlags));
        }
        else
            LogFunc(("IOMMU bulk translation failed. uDeviceId=%#x cPages=%zu rc=%Rrc\n", uDeviceId, cPages, rc));

        RTMemFree(paGCPhysOut);
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
#else
    RT_NOREF7(pDevIns, pPciDev, cPages, paGCPhysPages, fFlags, papvPages, paLocks);
    return VERR_NOT_IMPLEMENTED;
#endif
}


/** @copydoc PDMIOMMUREGR3::pfnMsiRemap */
int pdmIommuMsiRemap(PPDMDEVINS pDevIns, uint16_t uDeviceId, PCMSIMSG pMsiIn, PMSIMSG pMsiOut)
{
    PPDMIOMMU  pIommu       = PDMDEVINS_TO_IOMMU(pDevIns);
    PPDMDEVINS pDevInsIommu = pIommu->CTX_SUFF(pDevIns);
    if (   pDevInsIommu
        && pDevInsIommu != pDevIns)
    {
        int rc = pIommu->pfnMsiRemap(pDevInsIommu, uDeviceId, pMsiIn, pMsiOut);
        if (RT_FAILURE(rc))
        {
            LogFunc(("MSI remap failed. uDeviceId=%#x pMsiIn=(%#RX64, %#RU32) rc=%Rrc\n", uDeviceId, pMsiIn->Addr.u64,
                     pMsiIn->Data.u32, rc));
        }
        return rc;
    }
    return VERR_IOMMU_NOT_PRESENT;
}

