#!/usr/bin/env python

import sys

if len(sys.argv) < 4 :
	print "Usage: %s %s %s %s"%(sys.argv[0],"<outputFile.V>","<pseudoOutputFile.V>","<pseudo_expected.V>")
	exit(-1)
else:
	outputFile = sys.argv[1]
	pseudoOutFile = sys.argv[2]
	asmFile = sys.argv[3]


cregs = "MR1|MR2|CR1|CR2|SSP"
regs = "R0|R1|R2|R3|R4|R5|R6|R7|R8|R9|RA|RB|RC|BP|SP|FLGS"
# word accessible registers
regs_wa = "R0|R1|R2|R3|R4|R5|R6|R7"
regs16 = "R0L|R0H|R1L|R1H|R2L|R2H|R3L|R3H|R4L|R4H|R5L|R5H|R6L|R6H|R7L|R7H"
regs8 = "R0LL|R0LH|R0HL|R0HH|R1LL|R1LH|R1HL|R1HH|R2LL|R2LH|R2HL|R2HH|R3LL|R3LH|R3HL|R3HH"
imm12 = "0x0FFF|0x0023"
imm16 = "0xF099|0x00AB"
imm32 = "0xFFFFFFFF|@0x02"
identifier = "_ident"

# All OPs that take only registers
reg_reg_reg_ops = "ADD|AND|DIV|OR|MOD|MUL|SAR|SDIV|SHL|SHR|SMUL|SUB|XOR"
reg_reg_ops = "IN|NOT|OUT|XCHG|MOV"
creg_reg_ops = "MOVTC"
reg_creg_ops = "MOVFC|XCHGC"
reg16_reg16_ops = "MOV"
reg_reg16_ops = "SEX"
reg8_reg8_ops = "MOV"
reg_reg8_ops = "SEX"
reg_ops = "TEST|JMP|CALL|LOADAS|STOREAS"


# All OPs that take immediate 12bit data somewhere
reg_reg_imm12_ops = "ADD|AND|DIV|OR|MOD|MUL|SAR|SDIV|SHL|SHR|SMUL|SUB|XOR|STOREID"
reg_reg16_imm12_ops = "STOREID"
reg_reg8_imm12_ops = "STOREID"
reg_imm12_ops = "CMP"
reg16_reg_imm12_ops = "LOADID"
reg8_reg_imm12_ops = "LOADID"

# All Ops that take immediate 16bit data somewhere
reg_imm16_ops = "LOADMA|STOREMA"
reg16_imm16_ops = "LOADI"
reg8_imm16_ops = "LOADI"

# All Ops that take only immediate data
imm16_ops = "CALL|FAIL|INT|JMP|TEST"

# Ops that take no data
no_arg_ops = "STOP|IRET"

# Pseudo Ops
pseudo_reg32_ops = "PUSH|POP"
pseudo_reg32_imm32_ops = "LOAD32"
pseudo_reg32_identifier_ops = "LOADADR|LDJMP|LDCALL"

# special VASM Syntax
reg32_regrange_ops = "LOADMA|STOREMA"

register_emit_map = {
	"R0": 0,
	"R1": 1,
	"R2": 2,
	"R3": 3,
	"R4": 4,
	"R5": 5,
	"R6": 6,
	"R7": 7,
	"R8": 8,
	"R9": 9,
	"RA": 10,
	"RB": 11,
	"RC": 12,
	"BP": 13,
	"SP": 14,
	"FLGS": 15
}

reg_types = { "reg" : regs, "reg16" : regs16, "reg8" : regs8, "creg" : cregs, "reg_wa" : regs_wa }
imm_types = { "imm16" : imm16, "imm12" : imm12, "imm32" : imm32 }


def splitData(data):
	return data.split("|")

def genOp(op, nextGenFunc):
	op = op + "\t"
	lines = [op]
	nextGenF = nextGenFunc.pop(0)
	nextGenF(lines, nextGenFunc)

def genericGenRegArg(lines, nextGenFunc, regType):
	registers = reg_types[regType]
	registers = registers.split("|")
	newLines = []
	for line in lines:
		for reg in registers:
			newLines.append(line + reg + ",\t")
			
	nextGenF = nextGenFunc.pop(0)
	nextGenF(newLines, nextGenFunc)

def genRegArg(lines, nextGenFunc):
	genericGenRegArg(lines, nextGenFunc, "reg")

def genRegWaArg(lines, nextGenFunc):
	genericGenRegArg(lines, nextGenFunc, "reg_wa")
	
def genCregArg(lines, nextGenFunc):
	genericGenRegArg(lines, nextGenFunc, "creg")

def genReg16Arg(lines, nextGenFunc):
	genericGenRegArg(lines, nextGenFunc, "reg16")
	
def genReg8Arg(lines, nextGenFunc):
	genericGenRegArg(lines, nextGenFunc, "reg8")

def genericGenImmArg(lines, nextGenFunc, immType):
	immediateValues = imm_types[immType]
	immediateValues = splitData(immediateValues)
	newLines = []
	for line in lines:
		for immediateValue in immediateValues:
			newLines.append(line + immediateValue + ",\t")
	nextGenF = nextGenFunc.pop(0)
	nextGenF(newLines, nextGenFunc)

def genImm12Arg(lines, nextGenFunc):
	genericGenImmArg(lines, nextGenFunc, "imm12")

def genImm16Arg(lines, nextGenFunc):
	genericGenImmArg(lines, nextGenFunc, "imm16")

def genImm32Arg(lines, nextGenFunc):
	genericGenImmArg(lines, nextGenFunc, "imm32")
	
def genIdentifierArg(lines, nextGenFunc):
	identifiers = splitData(identifier)
	newLines = []
	for line in lines:
		for ident in identifiers:
			newLines.append(line + ident + ",\t")
	nextGenF = nextGenFunc.pop(0)
	nextGenF(newLines, nextGenFunc)

def genRegRangeArg(lines, nextGenFunc):
	registers = splitData(regs)
	newLines = []
	for line in lines:
		for reg1 in registers:
			for reg2 in registers:
				newLines.append(line + reg1 + ":" + reg2 + ",\t")
	nextGenF = nextGenFunc.pop(0)
	nextGenF(newLines, nextGenFunc)

def genFinish(lines, nextGenFunc):
	for line in lines:
		if line.endswith(",\t"):
			line = line.rstrip(',\t')
		line += "\n"
		
		writeAssemblerFromPseudoLine(line)
		
		f.write(line)

def writeAssemblerFromPseudoLine(line) :
	op = line.split("\t")[0]
	if op in pseudo_op_map:
		pseudo_op_map[op](line)

def genLines(operands, nextFuncs):
	for op in operands :
		genOp(op, nextFuncs[:])
		
def translateLOADMA(instr):
	# LOADMA          RC,     0x002b
	parts = instr.split("\t")
	arg2 = parts[2]
	regRange = arg2.split(":")
	if regRange[0] in register_emit_map:
		
		startReg = int(register_emit_map[regRange[0]]) << 4
		endReg = int(register_emit_map[regRange[1].rstrip("\n")])
		regRangeVal = startReg | endReg
		regRangeVal = "0x%04x" % regRangeVal
		asmF.write(parts[0] + "\t" + parts[1] + "\t" + regRangeVal + "\n");

def translateSTOREMA(instr):
	translateLOADMA(instr)

def translatePUSH(instr):
	# PUSH	RX -> STOREMA	SP,	RX,	RX
	parts = instr.split("\t")
	reg = parts[1].rstrip("\n")
	storema = "STOREMA\tSP,\t"+reg+":"+reg+"\n"
	translateSTOREMA(storema)

def translatePOP(instr):
	# POP	RX -> LOADMA	SP,	RX,	RX
	parts = instr.split("\t")
	reg = parts[1].rstrip("\n")
	loadma = "LOADMA\tSP,\t"+reg+":"+reg+"\n"
	translateLOADMA(loadma)

def translateLOAD32(instr):
	# LOAD32	RX,	@0x0XX
	parts = instr.split("\t")
	regValue = register_emit_map[parts[1].rstrip(",")]
	hexValue = parts[2].lower()
	if hexValue.startswith("@"):
		hexValue = hexValue[3:-1]
	else:
		hexValue = hexValue[2:-1]
	padding = (8-len(hexValue))*"0"
	hexValue = padding + hexValue
	hword = hexValue[0:4]
	lword = hexValue[4:8]
	if regValue < 8 :
		asmF.write("LOADI\t" + parts[1].rstrip(",") + "L,\t0x" + lword + "\n");
		asmF.write("LOADI\t" + parts[1].rstrip(",") + "H,\t0x" + hword + "\n");
	else:
		asmF.write("XCHG\tR7," + parts[1].rstrip(",") + "\n")
		asmF.write("LOADI\tR7L,\t0x" + lword + "\n");
		asmF.write("LOADI\tR7H,\t0x" + hword + "\n");
		asmF.write("XCHG\tR7," + parts[1].rstrip(",") + "\n")

def translateLOADADR(instr):
	# LOADADR	RX,	_ident -> 2x LOADI	RX,	ADDR
	parts = instr.split("\t")
	asmF.write("LOADI\t" + parts[1].rstrip(",") + "L,\t0x1000\n");
	asmF.write("LOADI\t" + parts[1].rstrip(",") + "H,\t0x0000\n");
	
def translateLDJMP(instr):
	# LDJMP		RX,	_ident -> 1x LOADADR, 1x CALL RX
	translateLOADADR(instr);
	parts = instr.split("\t")
	asmF.write("JMP\t" + parts[1].rstrip(",") + "\n");

def translateLDCALL(instr):
	# LDCALL	RX,	_ident -> 1x LOADADR, 1x JMP RX
	translateLOADADR(instr);
	parts = instr.split("\t")
	asmF.write("CALL\t" + parts[1].rstrip(",") + "\n");

pseudo_op_map = { 
	"LOADMA": translateLOADMA, 
	"STOREMA": translateSTOREMA, 
	"PUSH": translatePUSH, 
	"POP": translatePOP, 
	"LOAD32": translateLOAD32,
	"LOADADR": translateLOADADR,
	"LDJMP": translateLDJMP,
	"LDCALL": translateLDCALL
}

reg_reg_reg_ops = splitData(reg_reg_reg_ops)
reg_reg_ops = splitData(reg_reg_ops)
reg_reg_imm12_ops = splitData(reg_reg_imm12_ops)
reg_reg16_imm12_ops = splitData(reg_reg16_imm12_ops)
reg16_reg_imm12_ops = splitData(reg16_reg_imm12_ops)
reg8_reg_imm12_ops = splitData(reg8_reg_imm12_ops)
reg_reg8_imm12_ops = splitData(reg_reg8_imm12_ops)
reg_imm16_ops = splitData(reg_imm16_ops)
reg16_imm16_ops = splitData(reg16_imm16_ops)
reg8_imm16_ops = splitData(reg8_imm16_ops)
reg_imm12_ops = splitData(reg_imm12_ops)
reg_ops = splitData(reg_ops)
imm16_ops = splitData(imm16_ops)
no_arg_ops = splitData(no_arg_ops)
creg_reg_ops = splitData(creg_reg_ops)
reg_creg_ops = splitData(reg_creg_ops)
reg16_reg16_ops = splitData(reg16_reg16_ops)
reg_reg16_ops = splitData(reg_reg16_ops)
reg8_reg8_ops = splitData(reg8_reg8_ops)
reg_reg8_ops = splitData(reg_reg8_ops)
pseudo_reg32_ops = splitData(pseudo_reg32_ops)
pseudo_reg32_identifier_ops = splitData(pseudo_reg32_identifier_ops)
reg32_regrange_ops = splitData(reg32_regrange_ops)
pseudo_reg32_imm32_ops = splitData(pseudo_reg32_imm32_ops)

f = open(outputFile,'w')

genLines(reg_reg_reg_ops, [genRegArg, genRegArg, genRegArg, genFinish])
genLines(reg_reg_ops, [genRegArg, genRegArg, genFinish])
genLines(reg_reg_imm12_ops, [genRegArg, genRegArg, genImm12Arg, genFinish])
genLines(reg_reg16_imm12_ops, [genRegArg, genReg16Arg, genImm12Arg, genFinish])
genLines(reg16_reg_imm12_ops, [genReg16Arg, genRegArg, genImm12Arg, genFinish])
genLines(reg8_reg_imm12_ops, [genReg8Arg, genRegArg, genImm12Arg, genFinish])
genLines(reg_reg8_imm12_ops, [genRegArg, genReg8Arg, genImm12Arg, genFinish])
genLines(reg_ops, [genRegArg, genFinish])
genLines(reg_imm12_ops, [genRegArg, genImm12Arg ,genFinish])
genLines(reg_imm16_ops, [genRegArg, genImm16Arg, genFinish])
genLines(imm16_ops, [genImm16Arg, genFinish])
genLines(no_arg_ops, [genFinish])
genLines(creg_reg_ops, [genCregArg, genRegArg, genFinish])
genLines(reg_creg_ops, [genRegArg, genCregArg, genFinish])
genLines(reg16_reg16_ops, [genReg16Arg, genReg16Arg, genFinish])
genLines(reg_reg16_ops, [genRegArg, genReg16Arg, genFinish])
genLines(reg8_reg8_ops, [genReg8Arg, genReg8Arg, genFinish])
genLines(reg_reg8_ops, [genRegArg, genReg8Arg, genFinish])

f.close()

f = open(pseudoOutFile, 'w')
asmF = open(asmFile, 'w')

f.write(".label " + identifier + "\n");
genLines(pseudo_reg32_ops, [genRegArg, genFinish])
genLines(pseudo_reg32_identifier_ops, [genRegWaArg, genIdentifierArg, genFinish])
genLines(reg32_regrange_ops, [genRegArg, genRegRangeArg, genFinish])
genLines(pseudo_reg32_imm32_ops, [genRegArg, genImm32Arg, genFinish])

asmF.close()
f.close()

exit(0)