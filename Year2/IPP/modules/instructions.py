"""
    @file instructions.py
    @brief Stores information about instructions from xml input file
    @autor Marek Miček, xmicek08
    @date 5.3.2021
"""

from .errors import *

""" @brief Provides storage of data of each instruction which will be used for interpretation """
class InstructionList():

    def __init__(self):
        self.instruction_List = {}
        self.labels_List = {}
        self.call_List = []
        self.order_List = []
        self.instruction_done = 0           # current number of interpreted ins, reduced when poping from call stack
        self.interpreted_ins = 0            # number of all interpreted ins, not reduced when poping from call stack
        self.order_index = 0                # index to order_List, useful when jump instructions called

    """
        @brief Jumps to specific label
        @param label Name of label we want to jump
    """
    def jump(self, label):
        if (label in self.labels_List):
            new_instruction_order = self.labels_List[label]
            new_instruction_order = self.order_List.index(str(new_instruction_order)) + 1
            self.order_index = new_instruction_order
        else:
            ErrorHandler.putError("Error: Tried to jump to not defined label !!!\n", 52)

    """ @brief Provides storing position of next instruction to call stack """
    def call(self):
        self.call_List.append(self.order_index)             # store position of next ins after current


    """ @brief Provides RETURN, assigns position of next instruction from call stack """
    def return_call(self):
        if (len(self.call_List) > 0):                           # call stack can not be empty
            self.order_index = self.call_List.pop()             # go back to instruction number from call stack
        else:
            ErrorHandler.putError("Error: Missing value in call stack !!!\n", 56)


""" @brief Provides interface of each instruction read from XML file """
class Instruction():

    def __init__(self, opcode, arg1 = None, arg2 = None, arg3 = None):
        self.opcode = opcode
        if (arg1 != None):
            self.arg1 = {'type': arg1.attrib['type'], 'text': arg1.text}
        if (arg2 != None):
            self.arg2 = {'type': arg2.attrib['type'], 'text': arg2.text}
        if (arg3 != None):
            self.arg3 = {'type': arg3.attrib['type'], 'text': arg3.text}