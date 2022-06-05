/*
 *  Parser for assembly source code.
 */

#pragma once

#include <stdint.h>

/*
 * An ASM declaration is either a label like (MY_LABEL) or an instruction. This
 * is a separate enum from just instruction types so we can remove labels later
 * on after we replace them with memory locations.
 */
enum asm_declaration_type {
	ASM_DECL_LABEL,
	ASM_DECL_INSTRUCTION,
};

/*
 * An ASM instruction is either an A or C instruction.
 */
enum asm_instruction_type {
	ASM_INST_A,
	ASM_INST_C,
};

/*
 * An A instruction in ASM source code either points to a label (@hello) or an
 * address (@12345678901234).
 */
enum asm_a_instruction_type {
	ASM_A_INST_LABEL,
	ASM_A_INST_ADDRESS,
};


struct asm_a_instruction {
	enum asm_a_instruction_type type;
	union {
		char *label;
		uint16_t address;
	};
};

/*
 * The `dest` (ddd) part of a C instruction represented in assembler source.
 */
enum asm_c_dest {
	ASM_C_DEST_NULL,
	ASM_C_DEST_M,
	ASM_C_DEST_D,
	ASM_C_DEST_MD,
	ASM_C_DEST_A,
	ASM_C_DEST_AM,
	ASM_C_DEST_AD,
	ASM_C_DEST_AMD,
};

/*
 * The `a` and `comp` (a cccccc) part of a C instruction represented in
 * assembler source.
 */
enum asm_c_a_comp {
	ASM_C_A_COMP_ZERO,    ///< 0
	ASM_C_A_COMP_ONE,     ///< 1
	ASM_C_A_COMP_NEG_ONE, ///< -1

	ASM_C_A_COMP_D, ///< D
	ASM_C_A_COMP_A, ///< A
	ASM_C_A_COMP_M, ///< M

	ASM_C_A_COMP_NOT_D, ///< !D
	ASM_C_A_COMP_NOT_A, ///< !A
	ASM_C_A_COMP_NOT_M, ///< !M

	ASM_C_A_COMP_NEG_D, ///< -D
	ASM_C_A_COMP_NEG_A, ///< -A
	ASM_C_A_COMP_NEG_M, ///< -M

	ASM_C_A_COMP_D_PLUS_ONE, ///< D+1
	ASM_C_A_COMP_A_PLUS_ONE, ///< A+1
	ASM_C_A_COMP_M_PLUS_ONE, ///< M+1

	ASM_C_A_COMP_D_MINUS_ONE, ///< D-1
	ASM_C_A_COMP_A_MINUS_ONE, ///< A-1
	ASM_C_A_COMP_M_MINUS_ONE, ///< M-1

	ASM_C_A_COMP_D_PLUS_A, ///< D+A
	ASM_C_A_COMP_D_PLUS_M, ///< D+M

	ASM_C_A_COMP_D_MINUS_A, ///< D-A
	ASM_C_A_COMP_D_MINUS_M, ///< D-M

	ASM_C_A_COMP_A_MINUS_D, ///< A-D
	ASM_C_A_COMP_M_MINUS_D, ///< M-D

	ASM_C_A_COMP_D_AND_A, ///< D&A
	ASM_C_A_COMP_D_AND_M, ///< D&M

	ASM_C_A_COMP_D_OR_A, ///< D|A
	ASM_C_A_COMP_D_OR_M, ///< D|M
};

/*
 * The `jmp` (jjj) part of a C instruction represented in assembler source.
 */
enum asm_c_jump {
	ASM_C_JUMP_NULL, ///< No jump
	ASM_C_JUMP_JGT, ///< if comp > 0 jump
	ASM_C_JUMP_JEQ, ///< if comp == 0 jump
	ASM_C_JUMP_JGE, ///< if comp >= 0 jump
	ASM_C_JUMP_JLT, ///< if comp < 0 jump
	ASM_C_JUMP_JNE, ///< if comp != 0 jump
	ASM_C_JUMP_JLE, ///< if comp <= 0 jump
	ASM_C_JUMP_JMP, ///< unconditional jump
};

/*
 * A C instruction in ASM source is `dest = comp; jump`
 */
struct asm_c_instruction {
	enum asm_c_dest dest;
	enum asm_c_a_comp a_comp;
	enum asm_c_jump jump;
};

struct asm_instruction {
	enum asm_instruction_type type;
	union {
		struct asm_a_instruction a_instruction;
		struct asm_c_instruction c_instruction;
	};
};

struct asm_declaration {
	enum asm_declaration_type type;
	union {
		char *label;
		struct asm_instruction instruction;
	};
};

/*
 * Frees an `asm_instruction`. The components of the instruction must have been
 * allocated with malloc.
 */
void asm_declaration_destroy(struct asm_declaration);
