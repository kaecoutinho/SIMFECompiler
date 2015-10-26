# SIMFECompiler

**SIMFECompiler**'s repository

Created by Kaê Angeli Coutinho and Ricardo Oliete Ogata

_MIT license_

## Introduction

**SIMFECompiler** is a _Compilers_ class project that's capable of front-end compile any source code written in _SIM_.

The front-end compiler is mostly written in _C++_ and uses [_Boost_](http://www.boost.org) and [_DFA_](https://github.com/kaiky25/DFA) libraries as main dependencies.

Check out [_SIM_'s grammar](https://github.com/kaiky25/SIMFECompiler/blob/master/SIMGrammar.md) to better understand its compiler.

## Usage

#### Building yourself

<ol>
  <li>Open the <i><a href="https://github.com/kaiky25/SIMFECompiler/tree/master/Source%20Code">Source Code</a></i> folder</li>
  <li>Run <i>make</i> command</li>
  <li>Optionally run <i>make clean</i> command to clean unnecessary files</li>
  <li>Execute the <i>SIMFECompiler</i>(.exe, for Windows users) binary</li>
</ol>

#### Executing the generated binary (_SIMFECompiler_)

<ol>
  <li>Open a command-line app and navigate to the <i><a href="https://github.com/kaiky25/SIMFECompiler/tree/master/Source%20Code">Source Code</a></i> folder located on your machine</li>
  <li>Execute the <i>SIMFECompiler</i> binary passing the path of the desired <i>SIM</i> file to be compiled as argument directly from the command line</li>
  <li>It's done, check the compiler's message to see if the compilation succeed or failed</li>
</ol>

For more helpful information, execute the _SIMFECompiler_ binary without any arguments.

## Changelog

#### Version 1.2

<ul>
  <li>Semantical analysis</li>
  <li>Fixed bugs</li>
  <li>Full front-end compiler</li>
</ul>

#### Version 1.1

<ul>
  <li>Syntactic analysis</li>
  <li>Fixed bugs</li>
  <li>Better workflow</li>
  <li>Updated grammar file</li>
</ul>

#### Version 1.0

<ul>
  <li>Raw SIM front-end compiler</li>
  <li>Lexical analysis only</li>
  <li>JSON recognizers</li>
</ul>