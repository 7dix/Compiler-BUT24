# IFJ24 Compiler

![License](https://img.shields.io/badge/license-GPL3-0)

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Directory Structure](#directory-structure)
- [Code Snippets](#code-snippets)
  - [Lexical Analysis](#lexical-analysis)
  - [Syntax Analysis](#syntax-analysis)
  - [Semantic Analysis](#semantic-analysis)
  - [Code Generation](#code-generation)
- [Authors](#authors)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction

IFJ24 is a project for the course IFJ at FIT. This project aims to develop a compiler for the IFJ24 language. The compiler is divided into two phases:
1. **First Phase**: Gathers function signatures to the symbol table and checks that the main function exists. All read tokens are stored in a buffer.
2. **Second Phase**: Uses the buffer to run full syntax-based compilation.

## Features

- Lexical Analysis
- Syntax Analysis
- Semantic Analysis
- Code Generation
- Error Handling

## Installation

To install the project, clone the repository and compile the source code:
```sh
git clone https://github.com/7dix/IFJ24.git
cd IFJ24
make
```

## Usage
To run the project, use the following command:
```sh
./ifj24
```

## Authors
- [Marek Tenora](https://github.com/7dix)
- [Otasek Kočí](https://github.com/Otas02CZ)
- [Kryštof Valenta](https://github.com/chadmee)
- [Martin Zůbek](https://github.com/MiroSantona)

## Project Motivation
Our primary [motivation](https://youtu.be/K4XqeVWG7uo?si=wObYDwwN5HvTuQaw) while building this project.

## License
This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.
