"""
    @file xmlParser.py
    @brief Provides parsing of xml file from input
    @autor Marek Miček, xmicek08
    @date 5.3.2021
"""

import xml.etree.ElementTree as ET
import re
from .errors import *
from .instructions import *

class XMLParser():

    def __init__(self, source, insList):
        self.source = source                # source XML file to be parsed
        self.insList = insList              # instance of InstructionList class for storing data of ins.
        self.argCounter = 0                 # counts arguments of current instruction

    """
        @brief Checks format of XML file
    """
    def checkXML(self):
        try:
            if (self.source != ''):
                self.tree = ET.parse(self.source)
            else:
                self.tree = ET.parse(sys.stdin)

            self.root = self.tree.getroot()

        except FileNotFoundError:
            ErrorHandler.putError("Error: Unable to open input file !!!\n", 11)

        except Exception:
            ErrorHandler.putError("Error: Invalid XML format in input file !!!\n", 31)            

        if (self.root.tag != 'program' or 'language' not in self.root.attrib or self.root.attrib['language'].upper() != 'IPPCODE21'):
            ErrorHandler.putError("Error: Unexpcted structure of program element !!!\n", 32)
        else:
            for atr in self.root.attrib:
                if (atr not in ['language', 'description', 'name']):        # program element can't have other atribute
                    ErrorHandler.putError("Error: Unexpcted attribute in program element !!!\n", 32)

        # iterate throught each child of root element
        for instruction in self.root:
            for atr in instruction.attrib:
                if (atr not in ['order', 'opcode']):                     # instruction elemenet can't have other atribute
                    ErrorHandler.putError("Error: Unexpcted attribute in instruction element !!!\n", 32)

            try:
                order = int(instruction.attrib['order'])
            except:
                ErrorHandler.putError("Error: Invalid value of order attribute !!!\n", 32)

            if (instruction.tag != 'instruction' or 'order' not in instruction.attrib or 'opcode' not in instruction.attrib):
                ErrorHandler.putError("Error: Expected instruction element with atributes order and opcode !!!\n", 32)

            if (instruction.attrib['order'] in self.insList.order_List or order <= 0):    # duplicit or negative order forbidden
                ErrorHandler.putError("Error: Invalid order of instructions !!!\n", 32)

            # save instruction order    
            self.insList.order_List.append(instruction.attrib['order'])

            # iterate throught each argument of instruction and check format
            self.argCounter = 0
            for argument in instruction:
                self.argCounter += 1
                if (argument.tag != 'arg' + str(self.argCounter) or 'type' not in argument.attrib or len(argument.attrib) != 1):
                    ErrorHandler.putError("Error: Unexpected structure of arg element !!!\n", 32)
                
                if (argument.attrib['type'] not in ['int', 'bool', 'string', 'nil', 'label', 'type', 'var']):
                    ErrorHandler.putError("Error: Unexpected attribute of arg element !!!\n", 32)

            self.checkSyntax(instruction)

    """
        @brief Checks lexical and syntactic correctness of each instruction
        @param instruction Current instruction to be checked
    """        
    def checkSyntax(self, instruction):

        try:
            instruction.attrib['opcode'] = str(instruction.attrib['opcode']).upper()
        except:
            ErrorHandler.putError("Error: Unxpected opcode !!!\n", 32)      # invalid opcode

        if (instruction.attrib['opcode'] in ['CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'RETURN', 'BREAK']):
            if (self.argCounter != 0):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                               # no arguments for these opcodes
                current_ins = Instruction(instruction.attrib['opcode'])                         # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins   # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['MOVE', 'NOT', 'INT2CHAR', 'STRLEN', 'TYPE']):
            if (self.argCounter != 2):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # arguments <var> <symb>
                self.checkVar(instruction[0])
                self.checkSymbol(instruction[1])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0], arg2=instruction[1])   # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                       # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['DEFVAR', 'POPS']):
            if (self.argCounter != 1):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # argument <var>
                self.checkVar(instruction[0])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0])                        # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                       # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['CALL', 'JUMP', 'LABEL']):
            if (self.argCounter != 1):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)

            if (instruction.attrib['opcode'] == 'LABEL'):
                if (instruction[0].text in self.insList.labels_List):                                               # check for redefinition of label
                    ErrorHandler.putError("Error: Redefinition of label !!!\n", 52)
                else:
                    self.insList.labels_List[instruction[0].text] = int(self.insList.order_List[-1])                # store new label, key is name, value is order of instruction

            self.checkLabel(instruction[0])
            current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0])                            # new instance of actual instruction read from XML
            self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                           # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['PUSHS', 'WRITE', 'EXIT', 'DPRINT']):
            if (self.argCounter != 1):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # argument <symb>
                self.checkSymbol(instruction[0])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0])                        # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                       # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND', 'OR', 'STRI2INT', 'CONCAT', 'GETCHAR', 'SETCHAR']):
            if (self.argCounter != 3):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # argument <var> <symb1> <symb2>
                self.checkVar(instruction[0])
                self.checkSymbol(instruction[1])
                self.checkSymbol(instruction[2])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0], arg2=instruction[1], arg3=instruction[2])   # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                                            # insert to list of all instructions

        elif (instruction.attrib['opcode'] in ['READ']):
            if (self.argCounter != 2):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # argument <var> <type>
                self.checkVar(instruction[0])
                self.checkType(instruction[1])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0], arg2=instruction[1])   # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                       # insert to list of all instruction

        elif (instruction.attrib['opcode'] in ['JUMPIFEQ', 'JUMPIFNEQ']):
            if (self.argCounter != 3):
                ErrorHandler.putError("Error: Invalid number of operands for instruction !!!\n", 32)
            else:                                                                                                   # argument <label> <symb1> <symb2>
                self.checkLabel(instruction[0])
                self.checkSymbol(instruction[1])
                self.checkSymbol(instruction[2])
                current_ins = Instruction(instruction.attrib['opcode'], arg1=instruction[0], arg2=instruction[1], arg3=instruction[2])   # new instance of actual instruction read from XML
                self.insList.instruction_List[int(self.insList.order_List[-1])] = current_ins                                            # insert to list of all instructions    
    
        else:
            ErrorHandler.putError("Error: Unexpected opcode !!!\n", 32)      # invalid opcode
    """
        @brief Checks lexical and syntactic correcness of each variable
        @param var Current variable to be checked
    """
    def checkVar(self, var):
        if (var.attrib['type'] != 'var' or var.text is None):                           # check type name and var name existance
            ErrorHandler.putError("Error: Unexpected structure of arg element for variable !!!\n", 32)
        
        elif (not re.search('^(GF|LF|TF)@([A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])([0-9]|[A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])*$', var.text)):
            ErrorHandler.putError("Error: Unexpected structure of arg element for variable !!!\n", 32)

    """
        @brief Checks lexical and syntactic correcness of each symbol
        @param symbol Current symbol to be checked
    """
    def checkSymbol(self, symbol):
        if (symbol.attrib['type'] == 'var'):        # symbol is variable
            self.checkVar(symbol)

        elif (symbol.attrib['type'] in ['int', 'bool', 'string', 'nil']):
            if (symbol.attrib['type'] == 'int'):
                if (symbol.text is None):                                               # there must be value of int
                    ErrorHandler.putError("Error: Unexpected structure of arg element for int symbol !!!\n", 32)

                elif (not re.search('^([+-]?[1-9][0-9]*|[+-]?[0-9])$', symbol.text)):
                    ErrorHandler.putError("Error: Unexpected structure of arg element for int symbol !!!\n", 32)
            
            elif (symbol.attrib['type'] == 'bool'):
                if (symbol.text is None or symbol.text not in ['true', 'false']):       # there must be bool type and value of bool
                    ErrorHandler.putError("Error: Unexpected structure of arg element for bool symbol !!!\n", 32)

            elif (symbol.attrib['type'] == 'nil'):
                if (symbol.text is None or symbol.text not in ['nil']):                 # there must be nil type and value of nil
                    ErrorHandler.putError("Error: Unexpected structure of arg element for nil symbol !!!\n", 32)
            #string
            else:
                if (symbol.text is None):
                    symbol.text = ''                                                    # string without value is valid
                elif (not re.search('^(\\\\[0-9]{3}|[^\s\\\\#])*$', symbol.text)):
                    ErrorHandler.putError("Error: Unexpected structure of arg element for string symbol !!!\n", 32)
                #remove escape sequences
                else:
                    escape_sequence = re.findall('(\\\\[0-9]{3})', symbol.text)
                    for i in escape_sequence:
                        symbol.text = symbol.text.replace(i, chr(int(i[1:])))
        else:   
            ErrorHandler.putError("Error: Unexpected structure of arg element for symbol !!!\n", 32)

    """
        @brief Checks lexical and syntactic correcness of each label
        @param label Current label to be checked
    """
    def checkLabel(self, label):
        if (label.attrib['type'] != 'label' or label.text is None): # check type name and existance of label name
            ErrorHandler.putError("Error: Unexpected structure of arg element for label !!!\n", 32)
        
        elif (not re.search('^([A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])([0-9]|[A-Z]|[a-z]|[\_\-\$\&\%\*\!\?])*$', label.text)):
            ErrorHandler.putError("Error: Unexpected structure of arg element for label !!!\n", 32)

    """
        @brief Checks lexical and syntactic correctness of type argument
        @param var Current variable to be checked
    """
    def checkType(self, type_val):
        if (type_val.attrib['type'] != 'type' or type_val.text is None or type_val.text not in ['int', 'bool', 'string']):      # check type name and type value existance
            ErrorHandler.putError("Error: Unexpected structure of arg element for type !!!\n", 32)
        
