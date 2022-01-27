/** @file
 * VM - The Virtual Machine Monitor, VTable ring-3 API, Definition Template.
 *
 * This is used by the vmmr3vtable.h header and the VMMR3VTable.cpp source file
 * that implements it.
 */

/*
 * Copyright (C) 2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */


/** @name VMM
 * @{ */
VTABLE_ENTRY(VMMGetSvnRev)
VTABLE_ENTRY(VMMGetCpu)

VTABLE_RESERVED(pfnVMMR3Reserved1)
VTABLE_RESERVED(pfnVMMR3Reserved2)
VTABLE_RESERVED(pfnVMMR3Reserved3)
VTABLE_RESERVED(pfnVMMR3Reserved4)
VTABLE_RESERVED(pfnVMMR3Reserved5)
/** @} */

/** @name VM
 * @{
 */
VTABLE_ENTRY(VMR3Create)
VTABLE_ENTRY(VMR3PowerOn)
VTABLE_ENTRY(VMR3Suspend)
VTABLE_ENTRY(VMR3GetSuspendReason)
VTABLE_ENTRY(VMR3Resume)
VTABLE_ENTRY(VMR3GetResumeReason)
VTABLE_ENTRY(VMR3Reset)
VTABLE_ENTRY(VMR3Save)
VTABLE_ENTRY(VMR3Teleport)
VTABLE_ENTRY(VMR3LoadFromFile)
VTABLE_ENTRY(VMR3LoadFromStream)
VTABLE_ENTRY(VMR3PowerOff)
VTABLE_ENTRY(VMR3Destroy)

VTABLE_ENTRY(VMR3GetVM)
VTABLE_ENTRY(VMR3GetUVM)
VTABLE_ENTRY(VMR3RetainUVM)
VTABLE_ENTRY(VMR3ReleaseUVM)
VTABLE_ENTRY(VMR3GetName)
VTABLE_ENTRY(VMR3GetUuid)
VTABLE_ENTRY(VMR3GetState)
VTABLE_ENTRY(VMR3GetStateU)
VTABLE_ENTRY(VMR3GetStateName)
VTABLE_ENTRY(VMR3AtStateRegister)
VTABLE_ENTRY(VMR3AtStateDeregister)
VTABLE_ENTRY(VMR3AtErrorRegister)
VTABLE_ENTRY(VMR3AtErrorDeregister)
VTABLE_ENTRY(VMR3SetError)
VTABLE_ENTRY(VMR3SetErrorV)
VTABLE_ENTRY(VMR3AtRuntimeErrorRegister)
VTABLE_ENTRY(VMR3AtRuntimeErrorDeregister)

VTABLE_ENTRY(VMR3ReqCallU)
VTABLE_ENTRY(VMR3ReqCallVU)
VTABLE_ENTRY(VMR3ReqCallWaitU)
VTABLE_ENTRY(VMR3ReqCallNoWait)
VTABLE_ENTRY(VMR3ReqCallNoWaitU)
VTABLE_ENTRY(VMR3ReqCallVoidWaitU)
VTABLE_ENTRY(VMR3ReqCallVoidNoWait)
VTABLE_ENTRY(VMR3ReqPriorityCallWait)
VTABLE_ENTRY(VMR3ReqPriorityCallWaitU)
VTABLE_ENTRY(VMR3ReqPriorityCallVoidWaitU)
VTABLE_ENTRY(VMR3ReqAlloc)
VTABLE_ENTRY(VMR3ReqFree)
VTABLE_ENTRY(VMR3ReqQueue)
VTABLE_ENTRY(VMR3ReqWait)

VTABLE_ENTRY(VMR3NotifyCpuDeviceReady)
VTABLE_ENTRY(VMR3WaitForDeviceReady)
VTABLE_ENTRY(VMR3GetVMCPUThread)
VTABLE_ENTRY(VMR3GetVMCPUNativeThread)
VTABLE_ENTRY(VMR3GetVMCPUNativeThreadU)
VTABLE_ENTRY(VMR3GetCpuCoreAndPackageIdFromCpuId)
VTABLE_ENTRY(VMR3HotUnplugCpu)
VTABLE_ENTRY(VMR3HotPlugCpu)
VTABLE_ENTRY(VMR3SetCpuExecutionCap)
VTABLE_ENTRY(VMR3SetPowerOffInsteadOfReset)

VTABLE_RESERVED(pfnVMR3Reserved1)
VTABLE_RESERVED(pfnVMR3Reserved2)
VTABLE_RESERVED(pfnVMR3Reserved3)
VTABLE_RESERVED(pfnVMR3Reserved4)
VTABLE_RESERVED(pfnVMR3Reserved5)
/** @} */

/** @name CFGM
 * @{ */
VTABLE_ENTRY(CFGMR3Init)
VTABLE_ENTRY(CFGMR3Term)
VTABLE_ENTRY(CFGMR3ConstructDefaultTree)

VTABLE_ENTRY(CFGMR3CreateTree)
VTABLE_ENTRY(CFGMR3DestroyTree)
VTABLE_ENTRY(CFGMR3Dump)
VTABLE_ENTRY(CFGMR3DuplicateSubTree)
VTABLE_ENTRY(CFGMR3ReplaceSubTree)
VTABLE_ENTRY(CFGMR3InsertSubTree)
VTABLE_ENTRY(CFGMR3InsertNode)
VTABLE_ENTRY(CFGMR3InsertNodeF)
VTABLE_ENTRY(CFGMR3InsertNodeFV)
VTABLE_ENTRY(CFGMR3SetRestrictedRoot)
VTABLE_ENTRY(CFGMR3RemoveNode)
VTABLE_ENTRY(CFGMR3InsertInteger)
VTABLE_ENTRY(CFGMR3InsertString)
VTABLE_ENTRY(CFGMR3InsertStringN)
VTABLE_ENTRY(CFGMR3InsertStringF)
VTABLE_ENTRY(CFGMR3InsertStringFV)
VTABLE_ENTRY(CFGMR3InsertStringW)
VTABLE_ENTRY(CFGMR3InsertBytes)
VTABLE_ENTRY(CFGMR3InsertPassword)
VTABLE_ENTRY(CFGMR3InsertPasswordN)
VTABLE_ENTRY(CFGMR3InsertValue)
VTABLE_ENTRY(CFGMR3RemoveValue)
VTABLE_ENTRY(CFGMR3CopyTree)

VTABLE_ENTRY(CFGMR3Exists)
VTABLE_ENTRY(CFGMR3QueryType)
VTABLE_ENTRY(CFGMR3QuerySize)
VTABLE_ENTRY(CFGMR3QueryInteger)
VTABLE_ENTRY(CFGMR3QueryIntegerDef)
VTABLE_ENTRY(CFGMR3QueryString)
VTABLE_ENTRY(CFGMR3QueryStringDef)
VTABLE_ENTRY(CFGMR3QueryPassword)
VTABLE_ENTRY(CFGMR3QueryPasswordDef)
VTABLE_ENTRY(CFGMR3QueryBytes)

VTABLE_ENTRY(CFGMR3QueryU64)
VTABLE_ENTRY(CFGMR3QueryU64Def)
VTABLE_ENTRY(CFGMR3QueryS64)
VTABLE_ENTRY(CFGMR3QueryS64Def)
VTABLE_ENTRY(CFGMR3QueryU32)
VTABLE_ENTRY(CFGMR3QueryU32Def)
VTABLE_ENTRY(CFGMR3QueryS32)
VTABLE_ENTRY(CFGMR3QueryS32Def)
VTABLE_ENTRY(CFGMR3QueryU16)
VTABLE_ENTRY(CFGMR3QueryU16Def)
VTABLE_ENTRY(CFGMR3QueryS16)
VTABLE_ENTRY(CFGMR3QueryS16Def)
VTABLE_ENTRY(CFGMR3QueryU8)
VTABLE_ENTRY(CFGMR3QueryU8Def)
VTABLE_ENTRY(CFGMR3QueryS8)
VTABLE_ENTRY(CFGMR3QueryS8Def)
VTABLE_ENTRY(CFGMR3QueryBool)
VTABLE_ENTRY(CFGMR3QueryBoolDef)
VTABLE_ENTRY(CFGMR3QueryPort)
VTABLE_ENTRY(CFGMR3QueryPortDef)
VTABLE_ENTRY(CFGMR3QueryUInt)
VTABLE_ENTRY(CFGMR3QueryUIntDef)
VTABLE_ENTRY(CFGMR3QuerySInt)
VTABLE_ENTRY(CFGMR3QuerySIntDef)
VTABLE_ENTRY(CFGMR3QueryPtr)
VTABLE_ENTRY(CFGMR3QueryPtrDef)
VTABLE_ENTRY(CFGMR3QueryGCPtr)
VTABLE_ENTRY(CFGMR3QueryGCPtrDef)
VTABLE_ENTRY(CFGMR3QueryGCPtrU)
VTABLE_ENTRY(CFGMR3QueryGCPtrUDef)
VTABLE_ENTRY(CFGMR3QueryGCPtrS)
VTABLE_ENTRY(CFGMR3QueryGCPtrSDef)
VTABLE_ENTRY(CFGMR3QueryStringAlloc)
VTABLE_ENTRY(CFGMR3QueryStringAllocDef)

VTABLE_ENTRY(CFGMR3GetRoot)
VTABLE_ENTRY(CFGMR3GetRootU)
VTABLE_ENTRY(CFGMR3GetParent)
VTABLE_ENTRY(CFGMR3GetParentEx)
VTABLE_ENTRY(CFGMR3GetChild)
VTABLE_ENTRY(CFGMR3GetChildF)
VTABLE_ENTRY(CFGMR3GetChildFV)
VTABLE_ENTRY(CFGMR3GetFirstChild)
VTABLE_ENTRY(CFGMR3GetNextChild)
VTABLE_ENTRY(CFGMR3GetName)
VTABLE_ENTRY(CFGMR3GetNameLen)
VTABLE_ENTRY(CFGMR3AreChildrenValid)
VTABLE_ENTRY(CFGMR3GetFirstValue)
VTABLE_ENTRY(CFGMR3GetNextValue)
VTABLE_ENTRY(CFGMR3GetValueName)
VTABLE_ENTRY(CFGMR3GetValueNameLen)
VTABLE_ENTRY(CFGMR3GetValueType)
VTABLE_ENTRY(CFGMR3AreValuesValid)
VTABLE_ENTRY(CFGMR3ValidateConfig)

VTABLE_RESERVED(pfnCFMGR3Reserved1)
VTABLE_RESERVED(pfnCFMGR3Reserved2)
VTABLE_RESERVED(pfnCFMGR3Reserved3)
VTABLE_RESERVED(pfnCFMGR3Reserved4)
VTABLE_RESERVED(pfnCFMGR3Reserved5)
/** @} */

/** @name SSM
 * @{ */
VTABLE_ENTRY(SSMR3Term)
VTABLE_ENTRY(SSMR3RegisterInternal)
VTABLE_ENTRY(SSMR3RegisterExternal)
VTABLE_ENTRY(SSMR3RegisterStub)
VTABLE_ENTRY(SSMR3DeregisterInternal)
VTABLE_ENTRY(SSMR3DeregisterExternal)
VTABLE_ENTRY(SSMR3Save)
VTABLE_ENTRY(SSMR3Load)
VTABLE_ENTRY(SSMR3ValidateFile)
VTABLE_ENTRY(SSMR3Open)
VTABLE_ENTRY(SSMR3Close)
VTABLE_ENTRY(SSMR3Seek)
VTABLE_ENTRY(SSMR3HandleGetStatus)
VTABLE_ENTRY(SSMR3HandleSetStatus)
VTABLE_ENTRY(SSMR3HandleGetAfter)
VTABLE_ENTRY(SSMR3HandleIsLiveSave)
VTABLE_ENTRY(SSMR3HandleMaxDowntime)
VTABLE_ENTRY(SSMR3HandleHostBits)
VTABLE_ENTRY(SSMR3HandleRevision)
VTABLE_ENTRY(SSMR3HandleVersion)
VTABLE_ENTRY(SSMR3HandleHostOSAndArch)
VTABLE_ENTRY(SSMR3HandleReportLivePercent)
VTABLE_ENTRY(SSMR3Cancel)

VTABLE_ENTRY(SSMR3PutStruct)
VTABLE_ENTRY(SSMR3PutStructEx)
VTABLE_ENTRY(SSMR3PutBool)
VTABLE_ENTRY(SSMR3PutU8)
VTABLE_ENTRY(SSMR3PutS8)
VTABLE_ENTRY(SSMR3PutU16)
VTABLE_ENTRY(SSMR3PutS16)
VTABLE_ENTRY(SSMR3PutU32)
VTABLE_ENTRY(SSMR3PutS32)
VTABLE_ENTRY(SSMR3PutU64)
VTABLE_ENTRY(SSMR3PutS64)
VTABLE_ENTRY(SSMR3PutU128)
VTABLE_ENTRY(SSMR3PutS128)
VTABLE_ENTRY(SSMR3PutUInt)
VTABLE_ENTRY(SSMR3PutSInt)
VTABLE_ENTRY(SSMR3PutGCUInt)
VTABLE_ENTRY(SSMR3PutGCUIntReg)
VTABLE_ENTRY(SSMR3PutGCPhys32)
VTABLE_ENTRY(SSMR3PutGCPhys64)
VTABLE_ENTRY(SSMR3PutGCPhys)
VTABLE_ENTRY(SSMR3PutGCPtr)
VTABLE_ENTRY(SSMR3PutGCUIntPtr)
VTABLE_ENTRY(SSMR3PutRCPtr)
VTABLE_ENTRY(SSMR3PutIOPort)
VTABLE_ENTRY(SSMR3PutSel)
VTABLE_ENTRY(SSMR3PutMem)
VTABLE_ENTRY(SSMR3PutStrZ)

VTABLE_ENTRY(SSMR3GetStruct)
VTABLE_ENTRY(SSMR3GetStructEx)
VTABLE_ENTRY(SSMR3GetBool)
VTABLE_ENTRY(SSMR3GetBoolV)
VTABLE_ENTRY(SSMR3GetU8)
VTABLE_ENTRY(SSMR3GetU8V)
VTABLE_ENTRY(SSMR3GetS8)
VTABLE_ENTRY(SSMR3GetS8V)
VTABLE_ENTRY(SSMR3GetU16)
VTABLE_ENTRY(SSMR3GetU16V)
VTABLE_ENTRY(SSMR3GetS16)
VTABLE_ENTRY(SSMR3GetS16V)
VTABLE_ENTRY(SSMR3GetU32)
VTABLE_ENTRY(SSMR3GetU32V)
VTABLE_ENTRY(SSMR3GetS32)
VTABLE_ENTRY(SSMR3GetS32V)
VTABLE_ENTRY(SSMR3GetU64)
VTABLE_ENTRY(SSMR3GetU64V)
VTABLE_ENTRY(SSMR3GetS64)
VTABLE_ENTRY(SSMR3GetS64V)
VTABLE_ENTRY(SSMR3GetU128)
VTABLE_ENTRY(SSMR3GetU128V)
VTABLE_ENTRY(SSMR3GetS128)
VTABLE_ENTRY(SSMR3GetS128V)
VTABLE_ENTRY(SSMR3GetGCPhys32)
VTABLE_ENTRY(SSMR3GetGCPhys32V)
VTABLE_ENTRY(SSMR3GetGCPhys64)
VTABLE_ENTRY(SSMR3GetGCPhys64V)
VTABLE_ENTRY(SSMR3GetGCPhys)
VTABLE_ENTRY(SSMR3GetGCPhysV)
VTABLE_ENTRY(SSMR3GetUInt)
VTABLE_ENTRY(SSMR3GetSInt)
VTABLE_ENTRY(SSMR3GetGCUInt)
VTABLE_ENTRY(SSMR3GetGCUIntReg)
VTABLE_ENTRY(SSMR3GetGCPtr)
VTABLE_ENTRY(SSMR3GetGCUIntPtr)
VTABLE_ENTRY(SSMR3GetRCPtr)
VTABLE_ENTRY(SSMR3GetIOPort)
VTABLE_ENTRY(SSMR3GetSel)
VTABLE_ENTRY(SSMR3GetMem)
VTABLE_ENTRY(SSMR3GetStrZ)
VTABLE_ENTRY(SSMR3GetStrZEx)
VTABLE_ENTRY(SSMR3Skip)
VTABLE_ENTRY(SSMR3SkipToEndOfUnit)
VTABLE_ENTRY(SSMR3SetLoadError)
VTABLE_ENTRY(SSMR3SetLoadErrorV)
VTABLE_ENTRY(SSMR3SetCfgError)
VTABLE_ENTRY(SSMR3SetCfgErrorV)

VTABLE_RESERVED(pfnSSMR3Reserved1)
VTABLE_RESERVED(pfnSSMR3Reserved2)
VTABLE_RESERVED(pfnSSMR3Reserved3)
VTABLE_RESERVED(pfnSSMR3Reserved4)
VTABLE_RESERVED(pfnSSMR3Reserved5)
/** @} */

/** @name STAM
 * @{ */
VTABLE_ENTRY(STAMR3InitUVM)
VTABLE_ENTRY(STAMR3TermUVM)
VTABLE_ENTRY(STAMR3RegisterU)
VTABLE_ENTRY(STAMR3Register)
VTABLE_ENTRY(STAMR3RegisterFU)
VTABLE_ENTRY(STAMR3RegisterF)
VTABLE_ENTRY(STAMR3RegisterVU)
VTABLE_ENTRY(STAMR3RegisterV)
VTABLE_ENTRY(STAMR3RegisterCallback)
VTABLE_ENTRY(STAMR3RegisterCallbackV)
VTABLE_ENTRY(STAMR3RegisterRefresh)
VTABLE_ENTRY(STAMR3RegisterRefreshV)
VTABLE_ENTRY(STAMR3Deregister)
VTABLE_ENTRY(STAMR3DeregisterF)
VTABLE_ENTRY(STAMR3DeregisterV)
VTABLE_ENTRY(STAMR3DeregisterByPrefix)
VTABLE_ENTRY(STAMR3DeregisterByAddr)
VTABLE_ENTRY(STAMR3Reset)
VTABLE_ENTRY(STAMR3Snapshot)
VTABLE_ENTRY(STAMR3SnapshotFree)
VTABLE_ENTRY(STAMR3Dump)
VTABLE_ENTRY(STAMR3DumpToReleaseLog)
VTABLE_ENTRY(STAMR3Print)
VTABLE_ENTRY(STAMR3Enum)
VTABLE_ENTRY(STAMR3GetUnit)
VTABLE_ENTRY(STAMR3GetUnit1)
VTABLE_ENTRY(STAMR3GetUnit2)

VTABLE_RESERVED(pfnSTAMR3Reserved1)
VTABLE_RESERVED(pfnSTAMR3Reserved2)
VTABLE_RESERVED(pfnSTAMR3Reserved3)
VTABLE_RESERVED(pfnSTAMR3Reserved4)
VTABLE_RESERVED(pfnSTAMR3Reserved5)
/** @} */

/** @name CPUM
 * @{ */
VTABLE_ENTRY(CPUMGetHostCpuVendor)
VTABLE_ENTRY(CPUMGetHostMicroarch)

VTABLE_RESERVED(pfnCPUMR3Reserved1)
VTABLE_RESERVED(pfnCPUMR3Reserved2)
VTABLE_RESERVED(pfnCPUMR3Reserved3)
VTABLE_RESERVED(pfnCPUMR3Reserved4)
VTABLE_RESERVED(pfnCPUMR3Reserved5)
/** @} */

/** @name DBGC
 * @{ */
VTABLE_ENTRY(DBGCCreate)

VTABLE_RESERVED(pfnDBGCR3Reserved1)
VTABLE_RESERVED(pfnDBGCR3Reserved2)
VTABLE_RESERVED(pfnDBGCR3Reserved3)
VTABLE_RESERVED(pfnDBGCR3Reserved4)
VTABLE_RESERVED(pfnDBGCR3Reserved5)
/** @} */

/** @name DBGF
 * @{ */
VTABLE_ENTRY(DBGFR3BpClear)
VTABLE_ENTRY(DBGFR3BpDisable)
VTABLE_ENTRY(DBGFR3BpEnable)
VTABLE_ENTRY(DBGFR3BpOwnerCreate)
VTABLE_ENTRY(DBGFR3BpOwnerDestroy)
VTABLE_ENTRY(DBGFR3BpSetInt3)
VTABLE_ENTRY(DBGFR3BpSetInt3Ex)
VTABLE_ENTRY(DBGFR3BpSetMmio)
VTABLE_ENTRY(DBGFR3BpSetMmioEx)
VTABLE_ENTRY(DBGFR3BpSetPortIo)
VTABLE_ENTRY(DBGFR3BpSetPortIoEx)
VTABLE_ENTRY(DBGFR3BpSetReg)
VTABLE_ENTRY(DBGFR3BpSetRegEx)
VTABLE_ENTRY(DBGFR3BpSetREM)
VTABLE_ENTRY(DBGFR3CoreWrite)
VTABLE_ENTRY(DBGFR3Info)
VTABLE_ENTRY(DBGFR3InfoRegisterExternal)
VTABLE_ENTRY(DBGFR3InfoDeregisterExternal)
VTABLE_ENTRY(DBGFR3InfoGenericGetOptError)
VTABLE_ENTRY(DBGFR3InjectNMI)
VTABLE_ENTRY(DBGFR3LogModifyDestinations)
VTABLE_ENTRY(DBGFR3LogModifyFlags)
VTABLE_ENTRY(DBGFR3LogModifyGroups)
VTABLE_ENTRY(DBGFR3OSDetect)
VTABLE_ENTRY(DBGFR3OSQueryNameAndVersion)
VTABLE_ENTRY(DBGFR3RegCpuQueryU8)
VTABLE_ENTRY(DBGFR3RegCpuQueryU16)
VTABLE_ENTRY(DBGFR3RegCpuQueryU32)
VTABLE_ENTRY(DBGFR3RegCpuQueryU64)
VTABLE_ENTRY(DBGFR3RegCpuQueryXdtr)
VTABLE_ENTRY(DBGFR3RegFormatValue)
VTABLE_ENTRY(DBGFR3RegNmQuery)
VTABLE_ENTRY(DBGFR3RegNmQueryAll)
VTABLE_ENTRY(DBGFR3RegNmQueryAllCount)
VTABLE_ENTRY(DBGFR3RegNmSetBatch)
VTABLE_ENTRY(DBGFR3OSDeregister)
VTABLE_ENTRY(DBGFR3OSRegister)
VTABLE_ENTRY(DBGFR3OSQueryInterface)
VTABLE_ENTRY(DBGFR3MemReadString)
VTABLE_ENTRY(DBGFR3MemRead)
VTABLE_ENTRY(DBGFR3MemScan)
VTABLE_ENTRY(DBGFR3ModInMem)
VTABLE_ENTRY(DBGFR3AddrFromFlat)
VTABLE_ENTRY(DBGFR3AsSymbolByName)
VTABLE_ENTRY(DBGFR3AsResolveAndRetain)
VTABLE_ENTRY(DBGFR3AsSetAlias)
VTABLE_ENTRY(DBGFR3AddrAdd)
VTABLE_ENTRY(DBGFR3AddrSub)
VTABLE_ENTRY(DBGFR3AsGetConfig)
VTABLE_ENTRY(DBGFR3CpuGetCount)
VTABLE_ENTRY(DBGFR3CpuGetMode)
VTABLE_ENTRY(DBGFR3CpuGetState)
VTABLE_ENTRY(DBGFR3AddrFromSelOff)
VTABLE_ENTRY(DBGFR3FlowCreate)
VTABLE_ENTRY(DBGFR3FlowRetain)
VTABLE_ENTRY(DBGFR3FlowRelease)
VTABLE_ENTRY(DBGFR3FlowQueryStartBb)
VTABLE_ENTRY(DBGFR3FlowQueryBbByAddress)
VTABLE_ENTRY(DBGFR3FlowQueryBranchTblByAddress)
VTABLE_ENTRY(DBGFR3FlowGetBbCount)
VTABLE_ENTRY(DBGFR3FlowGetBranchTblCount)
VTABLE_ENTRY(DBGFR3FlowGetCallInsnCount)
VTABLE_ENTRY(DBGFR3FlowBbRetain)
VTABLE_ENTRY(DBGFR3FlowBbRelease)
VTABLE_ENTRY(DBGFR3FlowBbGetStartAddress)
VTABLE_ENTRY(DBGFR3FlowBbGetEndAddress)
VTABLE_ENTRY(DBGFR3FlowBbGetBranchAddress)
VTABLE_ENTRY(DBGFR3FlowBbGetFollowingAddress)
VTABLE_ENTRY(DBGFR3FlowBbGetType)
VTABLE_ENTRY(DBGFR3FlowBbGetInstrCount)
VTABLE_ENTRY(DBGFR3FlowBbGetFlags)
VTABLE_ENTRY(DBGFR3FlowBbQueryBranchTbl)
VTABLE_ENTRY(DBGFR3FlowBbQueryError)
VTABLE_ENTRY(DBGFR3FlowBbQueryInstr)
VTABLE_ENTRY(DBGFR3FlowBbQuerySuccessors)
VTABLE_ENTRY(DBGFR3FlowBbGetRefBbCount)
VTABLE_ENTRY(DBGFR3FlowBbGetRefBb)
VTABLE_ENTRY(DBGFR3FlowBranchTblRetain)
VTABLE_ENTRY(DBGFR3FlowBranchTblRelease)
VTABLE_ENTRY(DBGFR3FlowBranchTblGetSlots)
VTABLE_ENTRY(DBGFR3FlowBranchTblGetStartAddress)
VTABLE_ENTRY(DBGFR3FlowBranchTblGetAddrAtSlot)
VTABLE_ENTRY(DBGFR3FlowBranchTblQueryAddresses)
VTABLE_ENTRY(DBGFR3FlowItCreate)
VTABLE_ENTRY(DBGFR3FlowItDestroy)
VTABLE_ENTRY(DBGFR3FlowItNext)
VTABLE_ENTRY(DBGFR3FlowItReset)
VTABLE_ENTRY(DBGFR3FlowBranchTblItCreate)
VTABLE_ENTRY(DBGFR3FlowBranchTblItDestroy)
VTABLE_ENTRY(DBGFR3FlowBranchTblItNext)
VTABLE_ENTRY(DBGFR3FlowBranchTblItReset)
VTABLE_ENTRY(DBGFR3FlowTraceModCreate)
VTABLE_ENTRY(DBGFR3FlowTraceModCreateFromFlowGraph)
VTABLE_ENTRY(DBGFR3FlowTraceModRetain)
VTABLE_ENTRY(DBGFR3FlowTraceModRelease)
VTABLE_ENTRY(DBGFR3FlowTraceModEnable)
VTABLE_ENTRY(DBGFR3FlowTraceModDisable)
VTABLE_ENTRY(DBGFR3FlowTraceModQueryReport)
VTABLE_ENTRY(DBGFR3FlowTraceModClear)
VTABLE_ENTRY(DBGFR3FlowTraceModAddProbe)
VTABLE_ENTRY(DBGFR3FlowTraceProbeCreate)
VTABLE_ENTRY(DBGFR3FlowTraceProbeRetain)
VTABLE_ENTRY(DBGFR3FlowTraceProbeRelease)
VTABLE_ENTRY(DBGFR3FlowTraceProbeEntriesAdd)
VTABLE_ENTRY(DBGFR3FlowTraceReportRetain)
VTABLE_ENTRY(DBGFR3FlowTraceReportRelease)
VTABLE_ENTRY(DBGFR3FlowTraceReportGetRecordCount)
VTABLE_ENTRY(DBGFR3FlowTraceReportQueryRecord)
VTABLE_ENTRY(DBGFR3FlowTraceReportQueryFiltered)
VTABLE_ENTRY(DBGFR3FlowTraceReportEnumRecords)
VTABLE_ENTRY(DBGFR3FlowTraceRecordRetain)
VTABLE_ENTRY(DBGFR3FlowTraceRecordRelease)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetSeqNo)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetTimestamp)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetAddr)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetProbe)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetValCount)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetVals)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetValsCommon)
VTABLE_ENTRY(DBGFR3FlowTraceRecordGetCpuId)
VTABLE_ENTRY(DBGFR3PlugInLoad)
VTABLE_ENTRY(DBGFR3PlugInUnload)
VTABLE_ENTRY(DBGFR3PlugInLoadAll)
VTABLE_ENTRY(DBGFR3PlugInUnloadAll)
VTABLE_ENTRY(DBGFR3SampleReportCreate)
VTABLE_ENTRY(DBGFR3SampleReportRetain)
VTABLE_ENTRY(DBGFR3SampleReportRelease)
VTABLE_ENTRY(DBGFR3SampleReportStart)
VTABLE_ENTRY(DBGFR3SampleReportStop)
VTABLE_ENTRY(DBGFR3SampleReportDumpToFile)
VTABLE_ENTRY(DBGFR3SelQueryInfo)
VTABLE_ENTRY(DBGFR3StackWalkBegin)
VTABLE_ENTRY(DBGFR3StackWalkNext)
VTABLE_ENTRY(DBGFR3StackWalkEnd)
VTABLE_ENTRY(DBGFR3TypeDeregister)
VTABLE_ENTRY(DBGFR3TypeDumpEx)
VTABLE_ENTRY(DBGFR3TypeQueryReg)
VTABLE_ENTRY(DBGFR3TypeQuerySize)
VTABLE_ENTRY(DBGFR3TypeQueryValByType)
VTABLE_ENTRY(DBGFR3TypeRegister)
VTABLE_ENTRY(DBGFR3TypeSetSize)
VTABLE_ENTRY(DBGFR3TypeValFree)
VTABLE_ENTRY(DBGFR3TypeValDumpEx)

VTABLE_RESERVED(pfnDBGFR3Reserved1)
VTABLE_RESERVED(pfnDBGFR3Reserved2)
VTABLE_RESERVED(pfnDBGFR3Reserved3)
VTABLE_RESERVED(pfnDBGFR3Reserved4)
VTABLE_RESERVED(pfnDBGFR3Reserved5)
/** @} */

/** @name EM
 * @{ */
VTABLE_ENTRY(EMR3QueryExecutionPolicy)
VTABLE_ENTRY(EMR3QueryMainExecutionEngine)
VTABLE_ENTRY(EMR3SetExecutionPolicy)

VTABLE_RESERVED(pfnEMR3Reserved1)
VTABLE_RESERVED(pfnEMR3Reserved2)
VTABLE_RESERVED(pfnEMR3Reserved3)
VTABLE_RESERVED(pfnEMR3Reserved4)
VTABLE_RESERVED(pfnEMR3Reserved5)
/** @} */


/** @name HM
 * @{ */
VTABLE_ENTRY(HMR3IsEnabled)
VTABLE_ENTRY(HMR3IsNestedPagingActive)
VTABLE_ENTRY(HMR3IsUXActive)
VTABLE_ENTRY(HMR3IsVpidActive)

VTABLE_RESERVED(pfnHMR3Reserved1)
VTABLE_RESERVED(pfnHMR3Reserved2)
VTABLE_RESERVED(pfnHMR3Reserved3)
VTABLE_RESERVED(pfnHMR3Reserved4)
VTABLE_RESERVED(pfnHMR3Reserved5)
/** @} */

/** @name PDM
 * @{ */
VTABLE_ENTRY(PDMR3AsyncCompletionBwMgrSetMaxForFile)
VTABLE_ENTRY(PDMR3DeviceAttach)
VTABLE_ENTRY(PDMR3DeviceDetach)
VTABLE_ENTRY(PDMR3DriverAttach)
VTABLE_ENTRY(PDMR3DriverDetach)
VTABLE_ENTRY(PDMR3NsBwGroupSetLimit)
VTABLE_ENTRY(PDMR3QueryDeviceLun)
VTABLE_ENTRY(PDMR3QueryDriverOnLun)
VTABLE_ENTRY(PDMR3QueryLun)

VTABLE_ENTRY(PDMCritSectEnter)
VTABLE_ENTRY(PDMCritSectEnterDebug)
VTABLE_ENTRY(PDMCritSectTryEnter)
VTABLE_ENTRY(PDMCritSectTryEnterDebug)
VTABLE_ENTRY(PDMR3CritSectEnterEx)
VTABLE_ENTRY(PDMCritSectLeave)
VTABLE_ENTRY(PDMCritSectIsOwner)
VTABLE_ENTRY(PDMCritSectIsOwnerEx)
VTABLE_ENTRY(PDMCritSectIsInitialized)
VTABLE_ENTRY(PDMCritSectHasWaiters)
VTABLE_ENTRY(PDMCritSectGetRecursion)
VTABLE_ENTRY(PDMR3CritSectYield)
VTABLE_ENTRY(PDMR3CritSectName)
VTABLE_ENTRY(PDMR3CritSectDelete)

VTABLE_ENTRY(PDMQueueAlloc)
VTABLE_ENTRY(PDMQueueInsert)
VTABLE_ENTRY(PDMQueueInsertEx)

VTABLE_ENTRY(PDMR3ThreadDestroy)
VTABLE_ENTRY(PDMR3ThreadIAmRunning)
VTABLE_ENTRY(PDMR3ThreadIAmSuspending)
VTABLE_ENTRY(PDMR3ThreadResume)
VTABLE_ENTRY(PDMR3ThreadSleep)
VTABLE_ENTRY(PDMR3ThreadSuspend)

VTABLE_ENTRY(PDMR3UsbCreateEmulatedDevice)
VTABLE_ENTRY(PDMR3UsbCreateProxyDevice)
VTABLE_ENTRY(PDMR3UsbDetachDevice)
VTABLE_ENTRY(PDMR3UsbHasHub)
VTABLE_ENTRY(PDMR3UsbDriverAttach)
VTABLE_ENTRY(PDMR3UsbDriverDetach)
VTABLE_ENTRY(PDMR3UsbQueryLun)
VTABLE_ENTRY(PDMR3UsbQueryDriverOnLun)

VTABLE_RESERVED(pfnPDMR3Reserved1)
VTABLE_RESERVED(pfnPDMR3Reserved2)
VTABLE_RESERVED(pfnPDMR3Reserved3)
VTABLE_RESERVED(pfnPDMR3Reserved4)
VTABLE_RESERVED(pfnPDMR3Reserved5)
VTABLE_RESERVED(pfnPDMR3Reserved6)
VTABLE_RESERVED(pfnPDMR3Reserved7)
VTABLE_RESERVED(pfnPDMR3Reserved8)
VTABLE_RESERVED(pfnPDMR3Reserved9)
VTABLE_RESERVED(pfnPDMR3Reserved10)
/** @} */

/** @name PGM
 * @{ */
VTABLE_ENTRY(PGMHandlerPhysicalPageTempOff)
VTABLE_ENTRY(PGMPhysReadGCPtr)
VTABLE_ENTRY(PGMPhysSimpleDirtyWriteGCPtr)
VTABLE_ENTRY(PGMPhysSimpleReadGCPtr)
VTABLE_ENTRY(PGMPhysSimpleWriteGCPhys)
VTABLE_ENTRY(PGMPhysSimpleWriteGCPtr)
VTABLE_ENTRY(PGMPhysWriteGCPtr)
VTABLE_ENTRY(PGMShwMakePageWritable)
VTABLE_ENTRY(PGMR3QueryGlobalMemoryStats)
VTABLE_ENTRY(PGMR3QueryMemoryStats)

VTABLE_RESERVED(pfnPGMR3Reserved1)
VTABLE_RESERVED(pfnPGMR3Reserved2)
VTABLE_RESERVED(pfnPGMR3Reserved3)
VTABLE_RESERVED(pfnPGMR3Reserved4)
VTABLE_RESERVED(pfnPGMR3Reserved5)
/** @} */

/** @name TM
 * @{ */

VTABLE_ENTRY(TMR3GetCpuLoadPercents)
VTABLE_ENTRY(TMR3TimerSetCritSect)
VTABLE_ENTRY(TMR3TimerLoad)
VTABLE_ENTRY(TMR3TimerSave)
VTABLE_ENTRY(TMR3TimerSkip)
VTABLE_ENTRY(TMR3TimerDestroy)
VTABLE_ENTRY(TMTimerFromMicro)
VTABLE_ENTRY(TMTimerFromMilli)
VTABLE_ENTRY(TMTimerFromNano)
VTABLE_ENTRY(TMTimerGet)
VTABLE_ENTRY(TMTimerGetFreq)
VTABLE_ENTRY(TMTimerGetMicro)
VTABLE_ENTRY(TMTimerGetMilli)
VTABLE_ENTRY(TMTimerGetNano)
VTABLE_ENTRY(TMTimerIsActive)
VTABLE_ENTRY(TMTimerIsLockOwner)
VTABLE_ENTRY(TMTimerLock)
VTABLE_ENTRY(TMTimerSet)
VTABLE_ENTRY(TMTimerSetFrequencyHint)
VTABLE_ENTRY(TMTimerSetMicro)
VTABLE_ENTRY(TMTimerSetMillies)
VTABLE_ENTRY(TMTimerSetNano)
VTABLE_ENTRY(TMTimerSetRelative)
VTABLE_ENTRY(TMTimerStop)
VTABLE_ENTRY(TMTimerToMicro)
VTABLE_ENTRY(TMTimerToMilli)
VTABLE_ENTRY(TMTimerToNano)
VTABLE_ENTRY(TMTimerUnlock)
VTABLE_ENTRY(TMR3GetWarpDrive)
VTABLE_ENTRY(TMR3SetWarpDrive)
VTABLE_ENTRY(TMR3TimeVirtGet)
VTABLE_ENTRY(TMR3TimeVirtGetMicro)
VTABLE_ENTRY(TMR3TimeVirtGetMilli)
VTABLE_ENTRY(TMR3TimeVirtGetNano)

VTABLE_RESERVED(pfnTMR3Reserved1)
VTABLE_RESERVED(pfnTMR3Reserved2)
VTABLE_RESERVED(pfnTMR3Reserved3)
VTABLE_RESERVED(pfnTMR3Reserved4)
VTABLE_RESERVED(pfnTMR3Reserved5)
/** @} */