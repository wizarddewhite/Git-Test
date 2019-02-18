/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20180105 (64-bit version)
 * Copyright (c) 2000 - 2018 Intel Corporation
 * 
 * Disassembly of NFIT.dat, Wed Oct 10 00:03:21 2018
 *
 * ACPI Data Table [NFIT]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue
 */

[000h 0000   4]                    Signature : "NFIT"    [NVDIMM Firmware Interface Table]
[004h 0004   4]                 Table Length : 000000E0
[008h 0008   1]                     Revision : 01
[009h 0009   1]                     Checksum : BF
[00Ah 0010   6]                       Oem ID : "BOCHS "
[010h 0016   8]                 Oem Table ID : "BXPCNFIT"
[018h 0024   4]                 Oem Revision : 00000001
[01Ch 0028   4]              Asl Compiler ID : "BXPC"
[020h 0032   4]        Asl Compiler Revision : 00000001

[024h 0036   4]                     Reserved : 00000000

[028h 0040   2]                Subtable Type : 0000 [System Physical Address Range]
[02Ah 0042   2]                       Length : 0038

[02Ch 0044   2]                  Range Index : 0002
[02Eh 0046   2]        Flags (decoded below) : 0003
                   Add/Online Operation Only : 1
                      Proximity Domain Valid : 1
[030h 0048   4]                     Reserved : 00000000
[034h 0052   4]             Proximity Domain : 00000000
[038h 0056  16]           Address Range GUID : 66F0D379-B4F3-4074-AC43-0D3318B78CDB
[048h 0072   8]           Address Range Base : 00000001C0000000
[050h 0080   8]         Address Range Length : 0000000278000000
[058h 0088   8]         Memory Map Attribute : 0000000000008008

[060h 0096   2]                Subtable Type : 0001 [Memory Range Map]
[062h 0098   2]                       Length : 0030

[064h 0100   4]                Device Handle : 00000001
[068h 0104   2]                  Physical Id : 0000
[06Ah 0106   2]                    Region Id : 0000
[06Ch 0108   2]                  Range Index : 0002
[06Eh 0110   2]         Control Region Index : 0003
[070h 0112   8]                  Region Size : 0000000278000000
[078h 0120   8]                Region Offset : 0000000000000000
[080h 0128   8]          Address Region Base : 0000000000000000
[088h 0136   2]             Interleave Index : 0000
[08Ah 0138   2]              Interleave Ways : 0001
[08Ch 0140   2]                        Flags : 0000
                       Save to device failed : 0
                  Restore from device failed : 0
                       Platform flush failed : 0
                            Device not armed : 0
                      Health events observed : 0
                       Health events enabled : 0
                              Mapping failed : 0
[08Eh 0142   2]                     Reserved : 0000

[090h 0144   2]                Subtable Type : 0004 [NVDIMM Control Region]
[092h 0146   2]                       Length : 0050

[094h 0148   2]                 Region Index : 0003
[096h 0150   2]                    Vendor Id : 8086
[098h 0152   2]                    Device Id : 0001
[09Ah 0154   2]                  Revision Id : 0001
[09Ch 0156   2]          Subsystem Vendor Id : 0000
[09Eh 0158   2]          Subsystem Device Id : 0000
[0A0h 0160   2]        Subsystem Revision Id : 0000
[0A2h 0162   1]                 Valid Fields : 00
[0A3h 0163   1]       Manufacturing Location : 00
[0A4h 0164   2]           Manufacturing Date : 0000
[0A6h 0166   2]                     Reserved : 0000
[0A8h 0168   4]                Serial Number : 00123456
[0ACh 0172   2]                         Code : 0301
[0AEh 0174   2]                 Window Count : 0000
[0B0h 0176   8]                  Window Size : 0000000000000000
[0B8h 0184   8]               Command Offset : 0000000000000000
[0C0h 0192   8]                 Command Size : 0000000000000000
[0C8h 0200   8]                Status Offset : 0000000000000000
[0D0h 0208   8]                  Status Size : 0000000000000000
[0D8h 0216   2]                        Flags : 0000
                            Windows buffered : 0
[0DAh 0218   6]                    Reserved1 : 000000000000

Raw Table Data: Length 224 (0xE0)

  0000: 4E 46 49 54 E0 00 00 00 01 BF 42 4F 43 48 53 20  // NFIT......BOCHS 
  0010: 42 58 50 43 4E 46 49 54 01 00 00 00 42 58 50 43  // BXPCNFIT....BXPC
  0020: 01 00 00 00 00 00 00 00 00 00 38 00 02 00 03 00  // ..........8.....
  0030: 00 00 00 00 00 00 00 00 79 D3 F0 66 F3 B4 74 40  // ........y..f..t@
  0040: AC 43 0D 33 18 B7 8C DB 00 00 00 C0 01 00 00 00  // .C.3............
  0050: 00 00 00 78 02 00 00 00 08 80 00 00 00 00 00 00  // ...x............
  0060: 01 00 30 00 01 00 00 00 00 00 00 00 02 00 03 00  // ..0.............
  0070: 00 00 00 78 02 00 00 00 00 00 00 00 00 00 00 00  // ...x............
  0080: 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00  // ................
  0090: 04 00 50 00 03 00 86 80 01 00 01 00 00 00 00 00  // ..P.............
  00A0: 00 00 00 00 00 00 00 00 56 34 12 00 01 03 00 00  // ........V4......
  00B0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  // ................
  00C0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  // ................
  00D0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  // ................
