"""
    @file arguments.py
    @brief Provides parsing of arguments from command line
    @autor Marek Miček, xmicek08
    @date 4.3.2021
"""

import getopt
import sys

from .errors import *

class ArgumentParser():

    def __init__(self):
        self.source = ''
        self.input = ''

    def parseArgs(self):
        try:
            opts, _  = getopt.getopt(sys.argv[1:], "", ['help', 'source=', 'input='])
        except:
            ErrorHandler.putError("Error: Forbidden usage of arguments !!!\n", 10)

        if (len(opts) == 0):
            ErrorHandler.putError("Error: Forbidden usage of arguments !!!\n", 10)              # there must be some paramether
    
        for opt, arg in opts:
            if (opt in ['--help']):
                if (len(opts) > 1):
                    ErrorHandler.putError("Error: Forbidden usage of arguments !!!\n", 10)      # help can not be set with any other paramether
                else:   
                    self.printHelp()

            elif (opt in ['--source']):
                self.source = arg

            else:
                self.input = arg

    def printHelp(self):
        sys.stdout.write("Script interpret.py takes XML representation of IPPcode21 and interprets it's instructions\n")
        sys.stdout.write("--usage of paramethers:\n")
        sys.stdout.write("--help: prints this hint and exit the program\n")
        sys.stdout.write("--source: XML representation of IPPcode21\n")
        sys.stdout.write("--input: file with input data for interpretation\n")
        sys.exit(0)