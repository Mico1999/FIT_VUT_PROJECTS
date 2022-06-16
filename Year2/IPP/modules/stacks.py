"""
    @file stacks.py
    @brief Stacks for data and for frames including operations with stacks
    @autor Marek Miček, xmicek08
    @date 7.3.2021
"""

from .errors import *

class DataStack():

    def __init__(self):
        self.dataStack = []

    """@brief Provides pushing symbol to data stack """
    def pushVal(self, symbol_type, symbol_val):
        self.dataStack.append((symbol_type, symbol_val))

    """@brief Provides poping symbol from data stack """
    def popVal(self):
        if (len(self.dataStack) > 0):
            return self.dataStack.pop()

        else:
            ErrorHandler.putError("Error: Missing value on data stack !!!\n",56)

    """ @brief Provides instruction PUSHS 
        @param instruction PUSHS instruction with operand
        @param frameStack Object of class FrameStack
    """
    def pushs(self, instruction, frameStack):
        if (instruction.arg1['type'] == 'var'):                             # operand is variable
            var_frame, var_name = instruction.arg1['text'].split('@', 1)
            var_frame = frameStack.check_frame(var_frame)                   # check if frame of var is defined

            if (var_name not in var_frame):                                 # check if variable to be pushed is defined
                ErrorHandler.putError("Error: Tried to access not existing variable !!!\n", 54)
            else:
                self.pushVal('var', var_frame[var_name]['value'])
        # operand is symbol
        else:
            self.pushVal(instruction.arg1['type'], instruction.arg1['text'])


class FrameStack():

    def __init__(self):
        self.frameStack = []
        self.GF = {}
        self.TF = {}
        self.TF_defined = False         # indicates that TF is defined
        self.Type_called = False        # indicates that TYPE instruction is called


    """ @brief Checks if frame of symbol is defined 
        @param frame Current frame to be checked
    """
    def check_frame(self, frame):
        if (frame == 'GF'):
            return self.GF

        elif (frame == 'LF'):
            if (len(self.frameStack) > 0):          # frame stack can't be empty, otherwise error
                return self.frameStack[-1]
            else:
                ErrorHandler.putError("Error: Tried to work with variable from undefined frame !!!\n", 55)
        #TF
        else:
            if (self.TF_defined == True):           # TF must be defined, otherwise error
                return self.TF
            else:
               ErrorHandler.putError("Error: Tried to work with variable from undefined frame !!!\n", 55)


    """ @brief Assigns symbol to variable 
        @param operand First operand of instruction where we copy
        @param symbol_type, symbol_val Type and value of symbol to be copied
    """
    def assign_var(self, operand, symbol_type, symbol_val):
        var_frame, var_name = operand['text'].split('@', 1)
        var_frame = self.check_frame(var_frame)     # check if frame of var is defined

        if (var_name not in var_frame):             # check if var where we copy exists
            ErrorHandler.putError("Error: Tried to move value to not existing variable !!!\n", 54)
        else:
            var_frame[var_name]['type'] = symbol_type
            var_frame[var_name]['value'] = symbol_val


    """ @brief Interprets instruction PUSHFRAME """
    def push_frame(self):
        if (self.TF_defined == True):               # push TF if it is defined
            self.frameStack.append(self.TF)
            self.TF_defined = False
        else:
            ErrorHandler.putError("Error: Tried to push undefined TF !!!\n", 55)


    """ @brief Interprets instruction POPFRAME """
    def pop_frame(self):
        if (len(self.frameStack) > 0):             # can pop only if frame stack not empty             
            self.TF = self.frameStack.pop()
            self.TF_defined = True
        else:
            ErrorHandler.putError("Error: Tried to pop frame from empty stack of frames !!!\n", 55)


    """ @brief Interprets instruction CREATEFRAME """
    def create_frame(self):
        self.TF = {}
        self.TF_defined = True


    """ @brief Interprets instruction DEFVAR <var>
        @param var Current var to be defined
    """
    def def_var(self, var):
        var_frame, var_name = var['text'].split('@', 1)
        var_frame = self.check_frame(var_frame)                         # check if frame of var is defined

        if (var_name not in var_frame):                                 # check if var is not already defined
            var_frame[var_name] = {'type': None, 'value': None}
        else:
            ErrorHandler.putError("Error: Redefinition of variable !!!\n", 52)


    """ @brief Interprets instruction MOVE <var> <symb>
        @param instruction MOVE instruction with operands
    """
    def move(self, instruction):
        if (instruction.arg2['type'] == 'var'):                             # second operand of MOVE is variable
            var_frame, var_name = instruction.arg2['text'].split('@', 1)
            var_frame = self.check_frame(var_frame)                         # check if frame of var is defined

            if (var_name not in var_frame):                                 # check if variable to be copied is defined
                ErrorHandler.putError("Error: Tried to access not existing variable !!!\n", 54)
            else:
                self.assign_var(instruction.arg1, 'var', var_frame[var_name]['value'])
        
        # second operand is symbol
        else:
            self.assign_var(instruction.arg1, instruction.arg2['type'], instruction.arg2['text'])


    """ @brief Checks correctness of var operand of arithemtic, relation, logic instructions """
    def check_operands(self, arg_name):
        var_frame, var_name = arg_name.split('@', 1)
        var_frame = self.check_frame(var_frame)

        if (var_name not in var_frame):     # check if variable is defined
            ErrorHandler.putError("Error: Tried to access not existing variable !!!\n", 54)

        elif (var_frame[var_name]['value'] == None and self.Type_called == False):    # can not read uninitionalized var in other instruction than TYPE
            ErrorHandler.putError("Error: Tried to read from uninitionalized variable !!!\n", 56)
        
        else:
            return (var_frame[var_name]['type'], var_frame[var_name]['value'])


    """ @brief Checks type and value instruction's operand """
    """ @param arg_type, arg_value Type and value of current operand """
    def get_type_value(self, arg_type, arg_value):
        if (arg_type == 'var'):
            return self.check_operands(arg_value)       # operand is var, check it's frame and difinition
        else:
            return arg_type, arg_value                  # operand is symbol, nothing to check


    """ @brief Provides arithmetic instructions ADD, SUB, MUL, IDIV <var> <symb1> <symb2>
        @param instruction Current arithmetic instruction with operands
    """
    def arithmetic_instructions(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])
        try:
            sym_val1 = int(sym_val1)
            sym_val2 = int(sym_val2)
        except:
            ErrorHandler.putError("Error: Invalid type of operand in arithmetic instruction !!!\n", 53)

        # choose operation on the basis of opcode
        if (instruction.opcode == 'ADD'):
            self.assign_var(instruction.arg1, 'int', str(sym_val1 + sym_val2))
        elif (instruction.opcode == 'SUB'):
            self.assign_var(instruction.arg1, 'int', str(sym_val1 - sym_val2))
        elif (instruction.opcode == 'MUL'):
            self.assign_var(instruction.arg1, 'int', str(sym_val1 * sym_val2))
        else:
            if (int(sym_val2) != 0):
                self.assign_var(instruction.arg1, 'int', str(sym_val1 // sym_val2))
            else:
                ErrorHandler.putError("Error: Division by zero !!!\n", 57)

    """ @brief Interprets instructions EQ,LT,GT <var> <symb1> <symb2>
        @param instruction EQ/LT/GT instruction with operands
    """  
    def relation_instruction(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == sym_type2):
            # choose operation on the basis of opcode
            if (instruction.opcode == 'EQ'):
                is_equal = 'true' if sym_val1 == sym_val2 else 'false'
                self.assign_var(instruction.arg1, 'bool', is_equal)

            elif (instruction.opcode == "LT"):
                if (sym_type1 == 'int'):
                    is_less = 'true' if int(sym_val1) < int(sym_val2) else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_less)
                    
                elif (sym_type1 == 'string'):
                    is_less = 'true' if sym_val1 < sym_val2 else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_less)

                elif (sym_type1 == 'bool'):
                    is_less = 'true' if sym_val1 == 'false' and sym_val2 == 'true' else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_less)
                #nil can be only in EQ
                else:
                    ErrorHandler.putError("Error: Invalid type of operand in relation instruction !!!\n", 53)
            # GT
            else:
                if (sym_type1 == 'int'):
                    is_greater = 'true' if int(sym_val1) > int(sym_val2) else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_greater)
                    
                elif (sym_type1 == 'string'):
                    is_less = 'true' if sym_val1 > sym_val2 else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_greater)

                elif (sym_type1 == 'bool'):
                    is_less = 'true' if sym_val1 == 'true' and sym_val2 == 'false' else 'false'
                    self.assign_var(instruction.arg1, 'bool', is_greater)
                #nil can be only in EQ
                else:
                    ErrorHandler.putError("Error: Invalid type of operand in relation instruction !!!\n", 53)
            
        elif(sym_type1 == 'nil' or sym_type2 == 'nil'):
            # only EQ can work with nil
            if (instruction.opcode == 'EQ'):
                self.assign_var(instruction.arg1, 'bool', 'false')      # nil differs from any other type
            else:
                ErrorHandler.putError("Error: Invalid type of operand in relation instruction !!!\n", 53)

        # only nil type can be combined with other type
        else:
            ErrorHandler.putError("Error: Invalid type of operand in relation instruction !!!\n", 53)

    """ @brief Interprets instructions AND, OR <var> <symb1> <symb2>
        @param instruction AND/OR instruction with operands
    """
    def and_or_instructions(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == sym_type2 == 'bool'):
            if (instruction.opcode == 'AND'):
                and_result = 'true' if sym_val1 == sym_val2 == 'true' else 'false'
                self.assign_var(instruction.arg1, 'bool', and_result)
            #OR
            else:
                or_result = 'true' if sym_val1 == 'true' or sym_val2 == 'true' else 'false'
                self.assign_var(instruction.arg1, 'bool', or_result)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in logic instruction !!!\n", 53)

    """ @brief Interprets instruction NOT <var> <symb>
        @param instruction NOT instruction with operands
    """
    def not_instruction(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])

        if (sym_type1 == 'bool'):
            not_result = 'true' if sym_val1 == 'false' else 'false'
            self.assign_var(instruction.arg1, 'bool', not_result)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in logic instruction !!!\n", 53)

    """ @brief Interprets instruction INT2CHAR <var> <symb>
        @param instruction INT2CHAR instruction with operands
    """
    def int2char(self, instruction):
        char = None
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])

        if (sym_type1 == 'int'):
            try:
                char = chr(int(sym_val1))
            except:
                ErrorHandler.putError("Error: INT2CHAR requires int value !!!\n", 58)
            self.assign_var(instruction.arg1, 'string', char)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    """ @brief Interprets instruction STR2INT <var> <symb1> <symb2>
        @param instruction STR2INT instruction with operands
    """
    def str2int(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == 'string' and sym_type2 == 'int'):              # first operand string, second int
            if (0 <= int(sym_val2) <= len(sym_val1)-1):                 # index to string has to be in range
                ord_value = ord(sym_val1[int(sym_val2)])
                self.assign_var(instruction.arg1, 'int', str(ord_value))
            else:
                ErrorHandler.putError("Error: Invalid index in STR2INT !!!\n", 58)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    """ @brief Interprets instruction CONCAT <var> <symb1> <symb2>
        @param instruction CONCAT instruction with operands
    """
    def concat(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == sym_type2 == 'string'):            # both concat operands have to be string
            self.assign_var(instruction.arg1, 'string', sym_val1 + sym_val2)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    """ @brief Interprets instruction STRLEN <var> <symb>
        @param instruction STRLEN instruction with operand
    """
    def strlen(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])

        if (sym_type1 == 'string'):                         # operand has to be string
            self.assign_var(instruction.arg1, 'int', str(len(sym_val1)))
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    """ @brief Interprets instruction GETCHAR <var> <symb1> <symb2>
        @param instruction GETCHAR instruction with operands
    """
    def getchar(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == 'string' and sym_type2 == 'int'):              # first operand string, second int
            if (0 <= int(sym_val2) <= len(sym_val1)-1):                 # index to string has to be in range
                self.assign_var(instruction.arg1, 'string', sym_val1[int(sym_val2)])
            else:
                ErrorHandler.putError("Error: Invalid index in GETCHAR !!!\n", 58)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    """ @brief Interprets instruction SETCHAR <var> <symb1> <symb2>
        @param instruction SETCHAR instruction with operands
    """
    def setchar(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg1['type'], instruction.arg1['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type3, sym_val3 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == 'string' and sym_type2 == 'int' and sym_type3 == 'string'):
            if (0 <= int(sym_val2) <= len(sym_val3)-1 and sym_val3 != ''):  # index in range and last operand not empty string
                sym_val1 = list(sym_val1)
                sym_val1[int(sym_val2)] = sym_val3[0]
                sym_val1 = ''.join(sym_val1)
                self.assign_var(instruction.arg1, 'string', sym_val1)
            else:
                ErrorHandler.putError("Error: String error in SETCHAR !!!\n", 58)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in string instruction !!!\n", 53)

    
    """ @brief Interprets instruction TYPE <symb>
        @param instruction Type instruction with operand
    """
    def type_instruction(self, instruction):
        self.Type_called = True
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        if (sym_type1 == None):
            sym_type1 = ''          # uninitionalized variable => we assigned empty string
        
        self.assign_var(instruction.arg1, 'string', sym_type1)
        self.Type_called = False

    """ @brief Interprets instructions JUMPIFEQ, JUMPIFNEQ <labe> <symb1> <symb2>
        @param instruction JUMPIF(N)EQ instruction with operands
    """
    def conditional_jumps(self, instruction, insList):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        sym_type2, sym_val2 = self.get_type_value(instruction.arg3['type'], instruction.arg3['text'])

        if (sym_type1 == sym_type2 or sym_type1 == 'nil' or sym_type2 == 'nil'):
            # JUMPIFEQ
            if (instruction.opcode == 'JUMPIFEQ' and sym_val1 == sym_val2):
                insList.jump(instruction.arg1['text'])
            # JUMPIFNEQ
            elif (instruction.opcode == 'JUMPIFNEQ' and sym_val1 != sym_val2):
                insList.jump(instruction.arg1['text'])
            # ignore instruction
            else:
                pass
        else:
            ErrorHandler.putError("Error: Invalid type of operand in conditional instruction !!!\n", 53)

    """ @brief Interprets instruction EXIT <symb>
        @param instruction EXIT instruction with operand
    """
    def exit(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg1['type'], instruction.arg1['text'])

        if (sym_type1 == 'int'):
            if (0 <= int(sym_val1) <= 49):
                sys.exit(int(sym_val1))
            else:
                ErrorHandler.putError("Error: Invalid return value of EXIT instruction !!!\n", 57)
        else:
            ErrorHandler.putError("Error: Invalid type of operand in EXIT instruction !!!\n", 53)

    """ @brief Interprets instructions WRITE, DPRINT <symb>
        @param instruction WRITE/DPRINT instruction with operand
    """
    def print_instructions(self, instruction):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg1['type'], instruction.arg1['text'])

        if (sym_type1 == 'nil'):    # nil type will be print as empty string
            sym_val1 = ''

        if (instruction.opcode == 'WRITE'):
            sys.stdout.write(sym_val1)
        # DPRINT
        else:
            sys.stderr.write(sym_val1)


    """ @brief Interprets instruction READ <var> <symb>
        @param instruction READ instruction with operand
        @param input_file Contains input data from user
    """
    def read_instruction(self, instruction, input_file):
        sym_type1, sym_val1 = self.get_type_value(instruction.arg2['type'], instruction.arg2['text'])
        
        try:
            # user entered input file
            if (input_file != ''):
                try:
                    f = open(input_file, "r")
                    usr_data = f.read()
                    f.close()
                except:
                    ErrorHandler.putError("Error: Unable to open input file !!!\n", 11)

            else:
                usr_data = input()

            if (sym_val1 == 'int'):
                try:
                    usr_data = int(usr_data)
                    self.assign_var(instruction.arg1, 'int', str(usr_data))
                except:
                    self.assign_var(instruction.arg1, 'nil', 'nil')

            elif (sym_val1 == 'bool'):
                if (usr_data.upper() == 'TRUE'):
                    self.assign_var(instruction.arg1, 'bool', 'true')
                else:
                    self.assign_var(instruction.arg1, 'bool', 'false')
            # string
            else:
                self.assign_var(instruction.arg1, 'string', usr_data)

        except:
            self.assign_var(instruction.arg1, 'nil', 'nil')  