// FILE: first_phase.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: header file for the first phase of the compiler

#ifndef FIRST_PHASE_H
#define FIRST_PHASE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token_buffer.h"
#include "scanner.h"
#include "shared.h"


int first_phase(T_TOKEN_BUFFER *token_buffer);

#endif // FIRST_PHASE_H