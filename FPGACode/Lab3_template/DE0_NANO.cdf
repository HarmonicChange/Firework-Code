/* Quartus II 64-Bit Version 15.0.0 Build 145 04/22/2015 Patches 0.01we SJ Web Edition */
JedecChain;
	FileRevision(JESD32A);
	DefaultMfr(6E);

	P ActionCode(Ign)
		Device PartName(EP4CE22F17) MfrSpec(OpMask(0) FullPath("D:/Documents/Classes/Semester_5/ECE_3400/Firework-Code/FPGACode/Lab3_template/DE0_NANO.sof"));
	P ActionCode(Cfg)
		Device PartName(EP4CE22) Path("D:/Documents/Classes/Semester_5/ECE_3400/Firework-Code/FPGACode/Lab3_template/") File("my_generated_jic.jic") MfrSpec(OpMask(1) SEC_Device(EPCS64) Child_OpMask(1 128));

ChainEnd;

AlteraBegin;
	ChainType(JTAG);
AlteraEnd;
