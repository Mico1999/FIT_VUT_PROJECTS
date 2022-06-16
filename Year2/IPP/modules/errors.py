"""
    @file errors.py
    @brief Provides notification of errors
    @autor Marek Miček, xmicek08
    @date 4.3.2021
"""

import sys

class ErrorHandler():

    def __init__(self):
        pass

    @staticmethod
    def putError(errorMessage, errorCode):
        sys.stderr.write(errorMessage)
        sys.exit(errorCode)