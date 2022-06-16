"""
    @file interpret.py
    @brief Takes XML representation of IPPcode21 and interprets it's instructions
    @autor Marek Miček, xmicek08
    @date 4.3.2021
"""

from modules import arguments, instructions, xmlParser, stacks, errors
import sys

def main():
    argParser = arguments.ArgumentParser()
    argParser.parseArgs()                                       # check command line arguments

    insList = instructions.InstructionList()                 
    xml = xmlParser.XMLParser(argParser.source, insList)
    xml.checkXML()                                              # parse input XML file
    interpret(insList, argParser.input)                         # interpret instuctions

"""
    @brief Interprets instructions stored in InstructionList
    @param insList Object of InstructionList class which stores all instructions read from XML file
    @param input_file File which contains input data from user for interpretation
"""
def interpret(insList, input_file):
    dataStack = stacks.DataStack()
    frameStack = stacks.FrameStack()

    finish = False
    while 1:
        if (insList.order_index == len(insList.order_List)):    # in case last instruction of xml was CALL, we would be out of range
            break

        insList.instruction_done = int(insList.order_List[insList.order_index])     # store order of current instruction
        insList.interpreted_ins += 1                                                # num of interpreted ins, for BREAK instruction and it's stats
        if (insList.instruction_done == int(insList.order_List[-1])):
            finish = True
        
        instruction = insList.instruction_List[insList.instruction_done]       # get another instruction from list of all instructions
        insList.order_index += 1

        if (instruction.opcode == 'CREATEFRAME'):
            frameStack.create_frame()

        elif (instruction.opcode == 'PUSHFRAME'):
            frameStack.push_frame()

        elif (instruction.opcode == 'POPFRAME'):
            frameStack.pop_frame()

        elif (instruction.opcode == 'DEFVAR'):
            frameStack.def_var(instruction.arg1)

        elif (instruction.opcode == 'MOVE'):
            frameStack.move(instruction)

        elif (instruction.opcode == 'CALL'):
            insList.call()
            insList.jump(instruction.arg1['text'])

        elif (instruction.opcode == 'RETURN'):
            insList.return_call()

        elif (instruction.opcode == 'PUSHS'):
            dataStack.pushs(instruction, frameStack)

        elif (instruction.opcode == 'POPS'):
            sym_type, sym_val = dataStack.popVal()                          # pop type and value of symbol from data stack 
            frameStack.assign_var(instruction.arg1, sym_type, sym_val)      # assign value from data stack to var

        elif (instruction.opcode in ['ADD', 'SUB', 'MUL', 'IDIV']): 
            frameStack.arithmetic_instructions(instruction)

        elif (instruction.opcode in ['EQ', 'LT', 'GT']):
            frameStack.relation_instruction(instruction)

        elif (instruction.opcode in ['AND', 'OR']):
            frameStack.and_or_instructions(instruction)

        elif (instruction.opcode == 'NOT'):
            frameStack.not_instruction(instruction)

        elif (instruction.opcode == 'INT2CHAR'):
            frameStack.int2char(instruction)

        elif (instruction.opcode == 'STRI2INT'):
            frameStack.str2int(instruction)

        elif (instruction.opcode == 'CONCAT'):
            frameStack.concat(instruction)

        elif (instruction.opcode == 'STRLEN'):
            frameStack.strlen(instruction)

        elif (instruction.opcode == 'GETCHAR'):
            frameStack.getchar(instruction)

        elif (instruction.opcode == 'SETCHAR'):
            frameStack.setchar(instruction)

        elif (instruction.opcode == 'TYPE'):
            frameStack.type_instruction(instruction)

        # we already stored name of label
        elif (instruction.opcode == 'LABEL'):
            continue

        elif (instruction.opcode == 'JUMP'):
            insList.jump(instruction.arg1['text'])

        elif (instruction.opcode in ['JUMPIFEQ', 'JUMPIFNEQ']):
            frameStack.conditional_jumps(instruction, insList)

        elif (instruction.opcode == 'EXIT'):
            frameStack.exit(instruction)

        elif (instruction.opcode in ['WRITE', 'DPRINT']):
            frameStack.print_instructions(instruction)

        elif (instruction.opcode == 'READ'):
            frameStack.read_instruction(instruction, input_file)

        elif (instruction.opcode == 'BREAK'):
            sys.stderr.write('Position in code: {}\n'.format(insList.instruction_done))
            sys.stderr.write('Interpreted instructions so far: {}\n'.format(insList.interpreted_ins - 1))
            sys.stderr.write('Call stack: {}\n'.format(insList.call_List))
            sys.stderr.write('Data stack: {}\n'.format(dataStack.dataStack))
            sys.stderr.write('Frame stack: {}\n'.format(frameStack.frameStack))
            sys.stderr.write('Global frame: {}\n'.format(frameStack.GF))
            sys.stderr.write('Temporary frame: {}\n'.format(frameStack.TF))

            if (len(frameStack.frameStack) > 0):
                sys.stderr.write('Local frame: {}\n'.format(frameStack.frameStack[-1]))
            else:
                sys.stdout.write('Local frame: undefined\n')

        if (finish == True):
            break


if __name__ == '__main__':
    main()