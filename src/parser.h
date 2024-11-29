// FILE: parser.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Header file for the parser

#ifndef H_PARSER
#define H_PARSER

#include <stdio.h>
#include <stdlib.h>
#include "token_buffer.h"

//-------------- PUBLIC FUNCTION PROTOTYPES -----------------//

int run_parser(T_TOKEN_BUFFER *token_buffer);

#endif // H_PARSER
